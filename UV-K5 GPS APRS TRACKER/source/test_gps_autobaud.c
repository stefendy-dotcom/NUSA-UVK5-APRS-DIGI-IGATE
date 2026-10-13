#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "base/app/nusa_gps_autobaud.h"
#include "base/app/nusa_gps_core.h"
static void feed(const char *body, uint32_t tick) {
    uint8_t sum=0;
    for (const char *p=body; *p; ++p) sum^=(uint8_t)*p;
    char buf[200]; snprintf(buf,sizeof(buf),"$%s*%02X\r\n",body,sum);
    for (const char *p=buf; *p; ++p) NUSA_GPS_Feed((uint8_t)*p,tick);
}
int main(void) {
    NUSA_GPS_Reset();
    NUSA_GPS_AutoBaud_Reset(1000, nusa_gps.good_sentences);
    assert(NUSA_GPS_AutoBaud_Active()==0);
    assert(!NUSA_GPS_AutoBaud_Locked());
    assert(NUSA_GPS_AutoBaud_Update(1499,0)==-1);
    assert(NUSA_GPS_AutoBaud_Update(1500,0)==1); // 9600 -> 38400
    NUSA_GPS_ResetStream();
    assert(NUSA_GPS_AutoBaud_Update(1999,0)==-1);
    assert(NUSA_GPS_AutoBaud_Update(2000,0)==2); // 38400 -> 115200
    NUSA_GPS_ResetStream();
    // Two checksum-valid messages confirm active baud, even if GPS has no fix.
    feed("GNGGA,123520,,,,,0,00,99.9,,,,,,",2001);
    assert(NUSA_GPS_AutoBaud_Update(2001,nusa_gps.good_sentences)==-1);
    assert(!NUSA_GPS_AutoBaud_Locked());
    feed("GNRMC,123520,V,,,,,,,230394,,,N",2002);
    assert(NUSA_GPS_AutoBaud_Update(2002,nusa_gps.good_sentences)==-1);
    assert(NUSA_GPS_AutoBaud_Locked());
    // invalid NMEA checksum doesn't count toward lock.
    assert(NUSA_GPS_AutoBaud_Update(5001,nusa_gps.good_sentences)==-1);
    assert(NUSA_GPS_AutoBaud_Update(5002,nusa_gps.good_sentences)==0); // lost GPS: 30s => 9600
    assert(!NUSA_GPS_AutoBaud_Locked());
    assert(NUSA_GPS_AutoBaud_Update(5502,nusa_gps.good_sentences)==1);
    assert(NUSA_GPS_AutoBaud_Update(6002,nusa_gps.good_sentences)==2);
    assert(NUSA_GPS_AutoBaud_Update(6502,nusa_gps.good_sentences)==0);
    NUSA_GPS_AutoBaud_Reset(UINT32_MAX - 100,65535u);
    assert(NUSA_GPS_AutoBaud_Update(400,65535u)==1); // overflow-safe ticks
    assert(NUSA_GPS_AutoBaud_Update(401,1u)==-1); // checksum counter wrap
    assert(NUSA_GPS_AutoBaud_Locked()); // delta=2
    puts("NUSA REV1B AUTObaud unit tests: PASS (9600/38400/115200; checksum; 30s recovery; rollover)");
    return 0;
}
