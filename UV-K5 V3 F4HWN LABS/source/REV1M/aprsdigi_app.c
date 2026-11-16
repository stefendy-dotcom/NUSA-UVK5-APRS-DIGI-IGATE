/* Copyright 2026 Armel F4HWN
 * Licensed under the Apache License, Version 2.0.
 */

/* NUSA DIGI - traceable WIDEn-N / TRACEn-N APRS digipeater.
 *
 * The receiver is the APRS RX Bell 202 software demodulator. A valid AX.25 UI
 * frame is eligible when the first unused hop matches WIDEn-N / TRACEn-N
 * or the boot callsign. The APRS Edit SSID applies to the digipeater
 * address and own position beacon.  FCS is rebuilt before APRS TX. Logical packet hashes suppress duplicates
 * for 30 seconds, ignoring path and FCS.
 *
 * Set a non-zero squelch: transmission waits for 100-227 ms of closed squelch
 * and is abandoned after 12 seconds. EXIT stops the app. The loader restores
 * the resident radio configuration on return.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../app_api.h"
#include "aprsdigi_assets.h"

/* PA4 audio capture and the 48 MHz cycle clock are resident (API level 3);
 * the capture holds PA4 at mid-scale (ADC code 2048). */
#define BIAS_CODE      2048u
#define FS             9600u
#define CYC_PER_SAMPLE (48000000u / FS)
#define CYC_PER_BIT    (48000000u / 1200u)
#define HOUSE_EVERY    480u
#define BUSY_MAX       60u
#define REFRESH_SLOTS  20u

#define BP_B0          5356
#define BP_A1        (-10714)
#define BP_A2          5673
#define AGC_ATT        5
#define AGC_DEC        10
#define PK_MIN         16
#define PLL_STEP       8192
#define PLL_CTR        36864
#define PLL_SHIFT      2
#define NSL            3u
#define PRE_BITS       24u

#define FRAME_MAX      330u
#define FRAME_MIN      18u
#define FCS_GOOD       0xF0B8u

#define DUPE_SLOTS     8u
#define DUPE_MS        30000u
#define HOLD_FREE_MIN  100u
#define HOLD_MAX_MS    12000u
#define BEACON_INTERVAL_MS 1800000u
#define SLICER_DUP_MS  350u

#define REG_51         0x51u
#define REG_70         0x70u
#define REG_71         0x71u
#define REG_0C         0x0Cu
#define REG_33         0x33u  /* BK4819 GPIO output state; bit0 = red LED */
#define F_MARK         12389u
#define F_SPACE        22714u
#define TONE_LEVEL     66u
#define TXDELAY_FLAGS  40u
#define TAIL_FLAGS     3u

enum { ST_WAIT=0, ST_HOLD, ST_TX };

typedef struct {
    int32_t dc, x1, x2, y1, y2, im, qm, is, qs, pm, ps;
    uint32_t r, ks;
    int16_t ring[8][4];
    int8_t cm[8], cs[48];
} dem_t;

_Static_assert(COS2200==COS1200+COS1200_LEN &&
               offsetof(dem_t,cm)==DEMOD_INIT_LEN &&
               COS1200==DEMOD_INIT+DEMOD_INIT_LEN &&
               sizeof(dem_t)==DEMOD_INIT_LEN+COS1200_LEN+COS2200_LEN,
               "demodulator assets must match dem_t");

typedef struct {
    int32_t dprev, phase;
    uint16_t n, crc;
    uint8_t wa, wb, last, sr, ones, byte, nb, inframe, hdr, bits;
    bool pre;
    uint8_t buf[FRAME_MAX];
} sl_t;

typedef struct { uint32_t hash, at; } dupe_t;

/* Keep byte data, halfwords, then words in the short Thumb offset ranges. */
static struct {
    uint8_t own[7];
    char call[10];
    uint8_t prevKey, state, refresh, dupeNext;
    bool running, resetRx, space, beaconPending, rxLed;
    uint8_t diagPage;
    uint8_t ones;
    /* RSSI display removed to make room for APRS Edit profile reader. */
    uint16_t pendingLen, queuedLen, holdMs;
    union {struct {
        uint16_t rx, digi, dupes, drops, beaconTx;
        uint16_t skipped, queueDrops, timeouts;
        uint16_t skipNoHop, skipAlias, skipOwn, canceled;
    }; uint16_t count[12];};
    const app_api_t *A;
    uint8_t *pendingBuf, *queuedBuf;
    uint32_t pendingAt, freeAt, pendingHash, queuedHash, lastHash, lastAt, next, beaconDue;
    dupe_t seen[DUPE_SLOTS];
} g;

void *memset(void *d,int c,size_t n){
    uint8_t *p=d; while(n--) *p++=(uint8_t)c; return d;
}
void *memcpy(void *d,const void *s,size_t n){
    uint8_t *p=d; const uint8_t *q=s; while(n--) *p++=*q++; return d;
}

static void inc(uint16_t *v){ if(*v<9999u) (*v)++; }

/* The resident cycle clock, for the TX bit timing (the receiver holds the
 * service in a register). */
static uint32_t clkCyc(void){ return g.A->cycles_48mhz(); }

__attribute__((noinline))
static uint16_t crcByte(uint16_t c,uint8_t v){
    c^=v;
    for(uint8_t i=0;i<8u;i++) c=(c&1u)?(uint16_t)((c>>1)^0x8408u):(uint16_t)(c>>1);
    return c;
}

static bool callChar(uint8_t c){
    return (c>='A'&&c<='Z')||(c>='0'&&c<='9')||c==' ';
}

/* The caller already guarantees FRAME_MIN and a valid FCS. */
__attribute__((noinline)) static uint16_t uiEnd(const uint8_t *f,uint16_t n){
    uint16_t end=(uint16_t)(n-2u), e=13u;
    while(!(f[e]&1u) && e+7u<end) e+=7u;
    if(!(f[e]&1u)||e+2u>=end||f[e+1u]!=0x03u||f[e+2u]!=0xF0u) return 0;
    return e;
}

static bool sameCall(const uint8_t *a,const uint8_t *b){
    for(uint8_t i=0;i<6u;i++) if(a[i]!=b[i]) return false;
    return (a[6]&0x1Eu)==(b[6]&0x1Eu);
}

/* Returns remaining hop count for the first unused WIDEn-N or TRACEn-N
 * path entry (or our exact callsign). No user-selectable filter modes. */
static uint8_t pathHops(const uint8_t *a){
    if(sameCall(a,g.own)) return 1u;
    bool wide = a[0]==('W'<<1) && a[1]==('I'<<1) &&
                a[2]==('D'<<1) && a[3]==('E'<<1) &&
                a[5]==(' '<<1);
    bool trace = a[0]==('T'<<1) && a[1]==('R'<<1) &&
                 a[2]==('A'<<1) && a[3]==('C'<<1) &&
                 a[4]==('E'<<1);
    if(!wide&&!trace) return 0;
    uint8_t digit=(uint8_t)(a[wide?4u:5u]>>1);
    if(digit<'1'||digit>'7') return 0;
    uint8_t n=(uint8_t)(digit-'0');
    uint8_t remain=(uint8_t)((a[6]>>1)&15u);
    if(remain==0u||remain>n) return 0;
    return remain;
}

/* Hash destination, source, control/PID and info; path and FCS are ignored. */
__attribute__((noinline)) static uint32_t logicalHash(const uint8_t *f,uint16_t n,uint16_t e){
    uint32_t h=2166136261u;
    /* The AX.25 source address extension bit changes when a path is
     * inserted/removed; it is not part of the packet identity. */
    for(uint16_t i=0;i<14u;i++){
        uint8_t b=f[i];
        if(i==13u)b&=0xFEu;
        h^=b;h*=16777619u;
    }
    for(uint16_t i=(uint16_t)(e+1u);i<n-2u;i++){ h^=f[i]; h*=16777619u; }
    return h?h:1u;
}

static dupe_t *duplicate(uint32_t h,uint32_t now){
    for(uint8_t i=0;i<DUPE_SLOTS;i++)
        if(g.seen[i].hash==h && now-g.seen[i].at<DUPE_MS) return &g.seen[i];
    return NULL;
}
static void remember(uint32_t h,uint32_t now){
    g.seen[g.dupeNext].hash=h;
    g.seen[g.dupeNext].at=now;
    g.dupeNext=(uint8_t)((g.dupeNext+1u)%DUPE_SLOTS);
}

/* One additional, independent packet buffer avoids dropping a valid frame
 * simply because the channel has not yet cleared for the previous frame.
 * Promotion re-arms random hold and preserves the 30-s dedupe history. */
static void promoteQueue(uint32_t now){
    if(g.queuedLen){
        memcpy(g.pendingBuf,g.queuedBuf,g.queuedLen);
        g.pendingLen=g.queuedLen;g.queuedLen=0;
        g.pendingHash=g.queuedHash;g.queuedHash=0;
        g.pendingAt=now;
        g.holdMs=(uint16_t)(HOLD_FREE_MIN+(g.A->rand32()&127u));
        g.freeAt=0;g.beaconPending=false;g.state=ST_HOLD;
    }else{
        g.pendingHash=0;g.queuedHash=0;
        g.freeAt=0;g.state=ST_WAIT;
    }
}

/* Queue any eligible WIDEn-N path (including WIDE2-1 and WIDE2-2). */
__attribute__((noinline)) static void accept(const sl_t *m){
    const uint8_t *f=m->buf;
    uint16_t e=uiEnd(f,m->n);
    if(!e) return;
    uint32_t now=g.A->ticks_ms();
    uint32_t h=logicalHash(f,m->n,e);
    /* The three slicers often decode the same on-air frame. Count and process
     * it once; this short filter is separate from network duplicate history. */
    if(g.lastHash==h && now-g.lastAt<SLICER_DUP_MS) return;
    g.lastHash=h; g.lastAt=now;
    inc(&g.rx);
    /* Do not retransmit APRS with an invalid or missing operator ID. */
    if(!g.own[0]) {inc(&g.skipped);return;}

    uint16_t target=0;
    bool ownUsed=false,usedHop=false;
    for(uint16_t p=20u;p<=e;p+=7u){
        const uint8_t *a=f+p-6u;
        if(a[6]&0x80u){
            usedHop=true;
            if(sameCall(a,g.own)) ownUsed=true;
        }
        if(!(a[6]&0x80u) && !target) target=(uint16_t)(p-6u);
    }
    uint8_t hops=target?pathHops(f+target):0u;
    bool eligible=hops&&!ownUsed;
    /* A path-changing repeat by another digipeater has the same logical hash.
     * Cancel our queued copy. Another copy of the original WIDE1-1 packet is
     * only suppressed: it does not prove that a digipeater already sent it. */
    dupe_t *dupe=duplicate(h,now);
    if(dupe){
        /* A retransmission seen from another digi can make a queued
         * copy redundant. Cancel only the matching hash, not the
         * other waiting frame nor an unrelated position beacon. */
        if(!eligible&&usedHop&&g.state==ST_HOLD){
            if(g.queuedLen&&h==g.queuedHash){
                g.queuedLen=0;g.queuedHash=0;inc(&g.canceled);
            }else if(!g.beaconPending&&h==g.pendingHash){
                promoteQueue(now);inc(&g.canceled);
            }
        }
        inc(&g.dupes);
        return;
    }
    if(ownUsed || sameCall(f+7u,g.own)){
        inc(&g.skipped);inc(&g.skipOwn);return;
    }
    if(!target){inc(&g.skipped);inc(&g.skipNoHop);return;}
    if(!hops){inc(&g.skipped);inc(&g.skipAlias);return;}

    if(g.state==ST_HOLD && g.queuedLen){
        inc(&g.drops);inc(&g.queueDrops);return;
    }
    /* Only eight AX.25 repeater addresses fit. Make room for one
     * trace callsign when an n-N alias still has hops remaining. */
    if(hops>1u&&(m->n+7u>FRAME_MAX||e>=69u)){
        inc(&g.drops); return;
    }

    remember(h,now);
    const bool queued=g.state==ST_HOLD;
    uint8_t *out=queued?g.queuedBuf:g.pendingBuf;
    uint16_t outLen;
    if(hops>1u){
        /* MYCALL-SSID* is inserted BEFORE the alias. The alias's SSID
         * counts down: WIDE2-2 => MYCALL-SSID*,WIDE2-1. */
        memcpy(out,f,target);
        memcpy(out+target+7u,f+target,m->n-target);
        memcpy(out+target,g.own,6u);
        out[target+6u]=(uint8_t)(0xE0u|(g.own[6]&0x1Eu));
        uint8_t *alias=out+target+7u;
        alias[6]=(uint8_t)((alias[6]&~0x1Eu)|((hops-1u)<<1));
        outLen=(uint16_t)(m->n+7u);
    }else{
        memcpy(out,f,m->n);
        memcpy(out+target,g.own,6u);
        out[target+6u]=(uint8_t)((f[target+6u]&1u)|0xE0u|(g.own[6]&0x1Eu));
        outLen=m->n;
    }
    uint16_t c=0xFFFFu;
    for(uint16_t i=0;i<outLen-2u;i++) c=crcByte(c,out[i]);
    c^=0xFFFFu;
    out[outLen-2u]=(uint8_t)c;
    out[outLen-1u]=(uint8_t)(c>>8);
    if(queued){g.queuedLen=outLen;g.queuedHash=h;}
    else{
        g.pendingLen=outLen;g.pendingHash=h;g.pendingAt=now;
        g.holdMs=(uint16_t)(HOLD_FREE_MIN+(g.A->rand32()&127u));
        g.freeAt=0;g.state=ST_HOLD;
    }
    g.refresh=REFRESH_SLOTS;
}

__attribute__((noinline))
static void hdlcBit(sl_t *m,uint8_t b){
    if(m->bits<255u) m->bits++;
    m->sr=(uint8_t)((m->sr>>1)|(b<<7));
    if(m->sr==0x7Eu){
        bool ok=m->inframe&&m->n>=FRAME_MIN&&m->crc==FCS_GOOD;
        m->pre=m->bits==8u;
        m->bits=0;
        if(ok) accept(m);
        m->inframe=1; m->hdr=1; m->n=0; m->crc=0xFFFFu;
        m->ones=m->byte=m->nb=0;
        return;
    }
    if(b){
        if(++m->ones>6u){ m->ones=7u; m->inframe=0; return; }
    }else{
        if(m->ones==5u){ m->ones=0; return; }
        m->ones=0;
    }
    if(!m->inframe) return;
    m->byte|=(uint8_t)(b<<m->nb);
    if(++m->nb==8u){
        if(m->n<FRAME_MAX){
            m->crc=crcByte(m->crc,m->byte);
            if(m->n<6u&&((m->byte&1u)||!callChar((uint8_t)(m->byte>>1)))) m->hdr=0;
            m->buf[m->n++]=m->byte;
        }else m->inframe=0;
        m->byte=m->nb=0;
    }
}

static bool busy(const sl_t *m){
    return (m->pre&&m->bits<PRE_BITS)||(m->inframe&&m->n&&m->hdr);
}

static int32_t mag(int32_t i,int32_t q){
    if(i<0) i=-i;
    if(q<0) q=-q;
    if(i<q){ int32_t t=i; i=q; q=t; }
    return i+(q>>2)+(q>>3);
}

__attribute__((noinline))
static int32_t tone(int32_t i,int32_t q,int32_t *pk){
    int32_t mt=mag(i,q),p=*pk;
    p+=mt>p?(mt-p)>>AGC_ATT:-(p>>AGC_DEC);
    if(p<PK_MIN) p=PK_MIN;
    *pk=p;
    return mt;
}

static void front(dem_t *m,int32_t adc,int32_t *a,int32_t *b){
    m->dc+=((adc<<4)-m->dc)>>6;
    int32_t x=adc-(m->dc>>4);
    int32_t y=(BP_B0*(x-m->x2)-BP_A1*m->y1-BP_A2*m->y2)>>14;
    m->x2=m->x1; m->x1=x; m->y2=m->y1; m->y1=y;
    int16_t *o=m->ring[m->r];
    uint32_t k=m->ks+12u; if(k>=48u) k-=48u;
    int32_t p;
    p=(y*m->cm[m->r])>>8;         m->im+=p-o[0]; o[0]=(int16_t)p;
    p=(y*m->cm[(m->r+6u)&7u])>>8; m->qm+=p-o[1]; o[1]=(int16_t)p;
    p=(y*m->cs[m->ks])>>8;        m->is+=p-o[2]; o[2]=(int16_t)p;
    p=(y*m->cs[k])>>8;            m->qs+=p-o[3]; o[3]=(int16_t)p;
    m->r=(m->r+1u)&7u;
    if(++m->ks>=48u) m->ks=0;
    int32_t mm=tone(m->im,m->qm,&m->pm),ms=tone(m->is,m->qs,&m->ps);
    *a=mm*m->ps; *b=ms*m->pm;
}

static void slice(sl_t *m,int32_t a,int32_t b){
    int32_t d=a*m->wa-b*m->wb;
    m->phase+=PLL_STEP;
    if((d>0)!=(m->dprev>0)) m->phase+=(PLL_CTR-m->phase)>>PLL_SHIFT;
    m->dprev=d;
    if(m->phase>=65536){
        m->phase-=65536;
        uint8_t lv=(uint8_t)(d>0);
        hdlcBit(m,(uint8_t)(lv==m->last));
        m->last=lv;
    }
}

static void setTone(void){
    g.A->bk_write(REG_71,(uint16_t)(g.space?F_SPACE:F_MARK));
    g.A->bk_write(REG_70,(uint16_t)(0x8000u|(TONE_LEVEL<<8)));
}
static void sendBit(uint8_t b){
    while((int32_t)(clkCyc()-g.next)<0){}
    if(!b){ g.space=!g.space; setTone(); }
    g.next+=CYC_PER_BIT;
}
__attribute__((noinline))
static void sendByte(uint8_t v,bool stuff){
    for(uint8_t i=0;i<8u;i++){
        uint8_t b=v&1u; v>>=1; sendBit(b);
        if(stuff){
            if(!b) g.ones=0;
            else if(++g.ones==5u){ sendBit(0); g.ones=0; }
        }
    }
}


/* Build a position packet using the resident read-only APRS TX profile
 * service. This preserves the 4 KiB DIGI overlay code/RAM budget. */
static void queueBeacon(uint32_t now){
    if(g.state!=ST_WAIT || !g.A->beacon_build) return;
    uint16_t n=g.A->beacon_build(g.pendingBuf, FRAME_MAX,g.own);
    if(!n) return;
    g.pendingLen=n;
    g.pendingHash=0;g.pendingAt=now;
    g.holdMs=(uint16_t)(HOLD_FREE_MIN+(g.A->rand32()&127u));
    g.freeAt=0u; g.state=ST_HOLD; g.beaconPending=true;
    g.refresh=REFRESH_SLOTS;
}

/* Update green LED only on a change, to avoid SPI writes in each sample. */
static void greenLed(bool on){
    if(g.rxLed!=on){g.rxLed=on;g.A->led(on);}
}
__attribute__((noinline)) static void send(void){
    const app_api_t *A=g.A;
    if(A->tx_state()){
        if(!g.beaconPending)inc(&g.drops);
        g.beaconPending=false;
        promoteQueue(A->ticks_ms());
        return;
    }
    g.state=ST_TX;
    greenLed(false);
    A->tx_set_params();
    A->bk_write(REG_51,0);
    A->tx_tone(1200);
    /* Red LED is BK4819 GPIO5, register 0x33 bit0.
     * RADIO_SetupRegisters in tx_end explicitly turns it off. */
    A->bk_write(REG_33,(uint16_t)(A->bk_read(REG_33)|1u));
    g.space=false; setTone();
    g.next=clkCyc()+CYC_PER_BIT;
    for(uint8_t i=0;i<TXDELAY_FLAGS;i++) sendByte(0x7Eu,false);
    g.ones=0;
    for(uint16_t i=0;i<g.pendingLen;i++) sendByte(g.pendingBuf[i],true);
    for(uint8_t i=0;i<TAIL_FLAGS;i++) sendByte(0x7Eu,false);
    while((int32_t)(clkCyc()-g.next)<0){}
    A->tx_mute(true);
    A->tx_end();
    /* Let the restored RX synthesizer and discriminator settle before decode. */
    A->set_af(APP_AF_FM);
    A->delay_ms(50);
    if(g.beaconPending) inc(&g.beaconTx);
    else inc(&g.digi);
    promoteQueue(A->ticks_ms());
    g.resetRx=true;
}

/* The resident formatter (API level 3). One local stub keeps every call a
 * plain bl: the service lies past the ldr immediate range of the table. */
__attribute__((noinline))
static char *puti(char *o,int32_t v){ return g.A->append_i32(o,v); }
#define putu puti
static char *text(uint16_t off,char *s){ g.A->asset_read(off,s,TEXT_MAX); return s; }
static void field(uint8_t x,uint8_t line,uint16_t label,uint16_t value,char *s){
    char *o=s; text(label,s); while(*o) o++;
    char *v=o; o=putu(o,value); *o='\0';
    g.A->print_normal(s,x,0,line);
    g.A->print_bold(v,(uint8_t)(x+(uint8_t)(v-s)*7u),0,line);
}
static void draw(void){
    const app_api_t *A=g.A;
    char s[TEXT_MAX+5u];
    A->display_clear(); A->status_clear();
    A->print_inverse(text(T_TITLE,s),2,0,true,true,(uint8_t)(2u+T_TITLE_CHARS*4u));
    /* Show only TX state. The DIGI accepts WIDE1-1/WIDE2-1/WIDE2-2
     * automatically; W1 and WN modes no longer exist. */
    static const uint8_t states[]={T_WAIT,T_HOLD,T_TX};
    text(states[g.state],s);
    A->print_inverse(s,43,0,true,true,63u);
    A->draw_battery();
    A->print_bold(g.call,0,0,0);
    /* This table renders the same diagnostic pages in fewer instructions. */
    static const uint8_t fld[3][8]={
        {T_RX,0,T_DIGI,1,T_DUPE,2,T_DROP,3},
        {T_SKIP,5,T_QFULL,6,T_TIME,7,T_DROP,3},
        {T_NOHP,8,T_ALIS,9,T_OWN,10,T_CANC,11}
    };
    for(uint8_t i=0;i<4u;i++)
        field((uint8_t)((i&1u)?65u:0u),(uint8_t)(1u+(i>>1)),
              fld[g.diagPage][i*2u],g.count[fld[g.diagPage][i*2u+1u]],s);
    field(72,0,T_BCN,g.beaconTx,s);
    g.A->draw_frequency(A->rx_freq(),0,4);   /* the main screen's big digits, lines 4-5 */
    A->blit_status(); A->blit_full();
}

static void house(void){
    const app_api_t *A=g.A;
    uint8_t key=A->get_key();
    if(key!=APP_KEY_INVALID&&key!=g.prevKey){
        A->backlight_on();
        if(key==APP_KEY_EXIT) g.running=false;
        else if(key==APP_KEY_2){
            if(++g.diagPage>2u)g.diagPage=0u;
            g.refresh=REFRESH_SLOTS;
        }
        else if(key==APP_KEY_3 && g.state==ST_WAIT && g.own[0]){
            uint32_t now=A->ticks_ms();
            queueBeacon(now);             /* immediate test without waiting 30 min */
            g.beaconDue=now+BEACON_INTERVAL_MS;
        }
    }
    g.prevKey=key;
    if(!g.running) return;

    uint32_t now=A->ticks_ms();
    if(g.state==ST_HOLD){
        if(now-g.pendingAt>=HOLD_MAX_MS){
            if(g.beaconPending) g.beaconPending=false;
            else{inc(&g.drops);inc(&g.timeouts);}
            promoteQueue(now);
        }else if(!(A->bk_read(REG_0C)&2u)){
            if(!g.freeAt) g.freeAt=now;
            else if(now-g.freeAt>=g.holdMs) send();
        }else g.freeAt=0;
    }

    if(g.own[0] && g.state==ST_WAIT && (int32_t)(now-g.beaconDue)>=0){
        g.beaconDue=now+BEACON_INTERVAL_MS;
        queueBeacon(now);
    }

    A->battery_sample();         /* yields the capture's ADC itself */
    if(++g.refresh>=REFRESH_SLOTS){ g.refresh=0; draw(); }
    A->backlight_update();
}

__attribute__((noinline))
static void listen(void){
    dem_t d;
    sl_t sl[NSL];
reset:
    memset(sl,0,sizeof sl);
    g.A->asset_read(DEMOD_INIT,&d,sizeof d);
    sl[0].wa=2; sl[0].wb=3;
    sl[1].wa=1; sl[1].wb=1;
    sl[2].wa=3; sl[2].wb=2;
    g.resetRx=false;
    /* The two per-sample services, held in registers: their table slots lie
     * past the ldr immediate range. */
    uint32_t (*const clk)(void)=g.A->cycles_48mhz;
    uint16_t (*const sample)(void)=g.A->af_capture_sample;
    uint32_t next=clk();
    uint16_t cnt=0;
    uint8_t busyFor=0;
    while(g.running){
        while((int32_t)(clk()-next)<0){}
        next+=CYC_PER_SAMPLE;
        int32_t a,b;
        front(&d,sample(),&a,&b);
        for(uint8_t k=0;k<NSL;k++) slice(&sl[k],a,b);
        if(++cnt<HOUSE_EVERY) continue;
        cnt=0;
        bool bz=false;
        for(uint8_t k=0;k<NSL;k++) bz|=busy(&sl[k]);
        greenLed(bz || ((g.A->bk_read(REG_0C)&2u)!=0u));
        if(bz&&++busyFor<BUSY_MAX) continue;
        busyFor=0;
        house();
        if(g.resetRx) goto reset;
        next=clk();
    }
}

/* API4 on unmodified F4HWN Labs REV1E has sys_storage_read.
 * Read the APRS TX asset profile (APC7) saved by APRS Edit.
 * No operator callsign is ever hardcoded in this app. */
__attribute__((noinline)) static void makeOwn(const app_api_t *A){
    uint8_t *p=g.pendingBuf;
    uint32_t tag[2];
    for(uint8_t slot=0;slot<48u;slot++){
        uint32_t base=0x102000u+((uint32_t)slot<<13);
        A->sys_storage_read(base+20u,tag,8u);
        if(tag[0]!=0x53525041u||tag[1]!=0x00585420u)continue;
        A->sys_storage_read(base+256u,p,117u);
        if(p[0]!='A'||p[1]!='P'||p[2]!='C'||p[3]!='7'||
           p[110]>6u||(p[5]>>4)>3u)goto invalid;
        uint8_t n=p[110];
        if(n)memcpy(g.call,p+111u,n);
        else {A->boot_callsign(g.call,sizeof g.call);
              while(n<7u&&g.call[n])n++;}
        if(!n||n>6u)goto invalid;
        g.call[n]=0;
        for(uint8_t i=0;i<n;i++)
            if(!((g.call[i]>='A'&&g.call[i]<='Z')||
                 (g.call[i]>='0'&&g.call[i]<='9')))goto invalid;
        for(uint8_t i=0;i<6u;i++)g.own[i]=(uint8_t)((i<n?g.call[i]:' ')<<1);
        uint8_t ss=(uint8_t)(p[5]&15u);
        g.own[6]=(uint8_t)(0x60u|(ss<<1));
        if(ss){g.call[n++]='-';g.call[n]=0;
               *A->append_i32(g.call+n,ss)=0;}
        else g.call[n]=0;
        return;
    }
invalid:
    g.own[0]=0;
    memcpy(g.call,"NO CALL",8u);
}

__attribute__((section(".text.entry"),used))
void app_main(const app_api_t *api){
    uint8_t pending[FRAME_MAX], queued[FRAME_MAX];
    g.A=api;g.pendingBuf=pending;g.queuedBuf=queued;
    g.prevKey=APP_KEY_INVALID;
    api->af_capture_open();      /* PA4 bias and ADC channel 4, restored by close */
    api->set_af(APP_AF_FM);
    api->delay_ms(50);
    api->backlight_on();
    g.running=true;
    g.beaconDue=api->ticks_ms()+BEACON_INTERVAL_MS;
    makeOwn(api);
    g.state=ST_WAIT; draw(); listen();
    api->af_capture_close();
}
