/* NUSA GPS NMEA parser / SmartBeaconing: bounded, no malloc, no floats.
 * RMC is the source of location validity; GGA only supplies satellites.
 * Bad NMEA does not update a fix; status V revokes it immediately.
 */
#include "nusa_gps_core.h"
#include <string.h>
NUSA_GPS_Fix nusa_gps;
static char line[100];
static uint8_t len, collecting;
static uint16_t last_tx_heading;

static int digit(char c) { return c >= '0' && c <= '9' ? c - '0' : -1; }
static int nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}
static uint32_t uint_part(const char *s, uint8_t maxdigits) {
    uint32_t v = 0;
    while (*s && maxdigits-- && digit(*s) >= 0) { v = v * 10u + (uint32_t)digit(*s++); }
    return v;
}
static bool coord(const char *s, char hemi, bool lon, int32_t *result) {
    const uint8_t degree_digits = lon ? 3u : 2u;
    uint32_t d = 0, m, fraction = 0, mult = 100000u;
    if (!s || !result) return false;
    for (uint8_t i = 0; i < degree_digits; ++i) {
        const int n = digit(s[i]); if (n < 0) return false;
        d = d * 10u + (uint32_t)n;
    }
    if (digit(s[degree_digits]) < 0 || digit(s[degree_digits+1]) < 0) return false;
    m = (uint32_t)digit(s[degree_digits])*10u + (uint32_t)digit(s[degree_digits+1]);
    s += degree_digits + 2u;
    if (*s == '.') {
        ++s;
        while (digit(*s) >= 0) {
            if (mult) { fraction += (uint32_t)digit(*s) * mult; mult /= 10u; }
            ++s;
        }
    }
    if (*s || m >= 60u || d > (lon ? 180u : 90u) ||
        (d == (lon ? 180u : 90u) && (m || fraction))) return false;
    uint32_t micro = d * 1000000u + (m * 1000000u + fraction + 30u) / 60u;
    if ((lon && hemi == 'W') || (!lon && hemi == 'S')) *result = -(int32_t)micro;
    else if ((lon && hemi == 'E') || (!lon && hemi == 'N')) *result = (int32_t)micro;
    else return false;
    return true;
}
static uint32_t scaled100(const char *s) {
    uint32_t whole = 0, frac = 0;
    if (!s) return 0;
    while (digit(*s) >= 0) {
        if (whole > 9999) return 0;
        whole = whole*10u + (uint32_t)digit(*s++);
    }
    if (*s == '.') {
        ++s; if (digit(*s) >= 0) frac = (uint32_t)digit(*s++)*10u;
        if (digit(*s) >= 0) frac += (uint32_t)digit(*s);
    }
    return whole * 100u + frac;
}
static void parse(char *body, uint32_t tick) {
    char *f[18] = {0};
    uint8_t count = 0;
    f[count++] = body;
    for (char *s = body; *s && count < 18u; ++s) {
        if (*s == ',') { *s = 0; f[count++] = s+1; }
    }
    if (count < 7u || strlen(f[0]) != 5u ||
        f[0][0] != 'G' || (f[0][1] != 'P' && f[0][1] != 'N' &&
        f[0][1] != 'L' && f[0][1] != 'A' && f[0][1] != 'B')) return;
    if (memcmp(f[0]+2, "GGA", 3) == 0 && count >= 8u) {
        uint32_t sat = uint_part(f[7], 2);
        nusa_gps.satellites = sat > 99u ? 99u : (uint8_t)sat;
        return;
    }
    if (memcmp(f[0]+2, "RMC", 3) != 0 || count < 9u) return;
    nusa_gps.seen_rmc = 1u;
    if (f[2][0] != 'A' || f[2][1] != 0) { nusa_gps.valid = 0u; return; }
    int32_t lat, lon;
    if (!coord(f[3], f[4][0], false, &lat) ||
        !coord(f[5], f[6][0], true, &lon)) { nusa_gps.valid = 0u; return; }
    const uint32_t knots100 = scaled100(f[7]);
    const uint32_t course100 = scaled100(f[8]);
    const uint32_t speed10 = (knots100 * 1852u + 5000u)/10000u;
    nusa_gps.latitude_udeg = lat;
    nusa_gps.longitude_udeg = lon;
    nusa_gps.speed_kmh10 = speed10 > 2000u ? 2000u : (uint16_t)speed10;
    nusa_gps.heading_deg = (uint16_t)((course100 / 100u) % 360u);
    if (!nusa_gps.valid) nusa_gps.first_fix_tick = tick;
    nusa_gps.valid = 1u;
    nusa_gps.fix_tick = tick;
}
static void complete(uint32_t tick) {
    if (len < 9u) return;
    line[len] = 0;
    char *star = strchr(line, '*');
    if (!star || !star[1] || !star[2] || star[3] != 0) {
        ++nusa_gps.bad_sentences; return;
    }
    int a = nibble(star[1]), b = nibble(star[2]);
    if (a < 0 || b < 0) { ++nusa_gps.bad_sentences; return; }
    uint8_t x = 0;
    for (char *s = line; s < star; ++s) x ^= (uint8_t)*s;
    if (x != (uint8_t)((a << 4) | b)) { ++nusa_gps.bad_sentences; return; }
    ++nusa_gps.good_sentences;
    *star = 0;
    parse(line, tick);
}
void NUSA_GPS_ResetStream(void) { len = 0u; collecting = 0u; }
void NUSA_GPS_Reset(void) {
    memset(&nusa_gps, 0, sizeof(nusa_gps));
    len = 0u; collecting = 0u; last_tx_heading = 0u;
}
void NUSA_GPS_Feed(uint8_t ch, uint32_t tick) {
    if (ch == '$') { collecting = 1u; len = 0u; return; }
    if (!collecting) return;
    if (ch == '\n') {
        if (len && line[len-1] == '\r') --len;
        complete(tick); collecting = 0u; len = 0u; return;
    }
    if (ch == 13u) return; // NMEA CRLF: ignore CR, process LF
    if (ch < 0x20u || ch > 0x7Eu || len >= sizeof(line)-1u) {
        collecting = 0u; len = 0u; ++nusa_gps.bad_sentences; return;
    }
    line[len++] = (char)ch;
}
bool NUSA_GPS_Fresh(uint32_t tick) {
    return nusa_gps.valid && (uint32_t)(tick - nusa_gps.fix_tick) < 500u;
}
bool NUSA_GPS_NeedsBeacon(uint32_t tick, uint8_t interval_min) {
    if (!NUSA_GPS_Fresh(tick)) return false;
    if (!nusa_gps.sent_once)
        return (uint32_t)(tick - nusa_gps.first_fix_tick) >= 1500u;
    uint32_t elapsed = (uint32_t)(tick - nusa_gps.last_tx_tick);
    if (interval_min > 0u) return elapsed >= (uint32_t)interval_min * 6000u;
    uint32_t period_seconds;
    if (nusa_gps.speed_kmh10 < 50u) period_seconds = 1800u;
    else {
        period_seconds = 42000u / nusa_gps.speed_kmh10;
        if (period_seconds < 60u) period_seconds = 60u;
        if (period_seconds > 1800u) period_seconds = 1800u;
    }
    if (elapsed >= period_seconds * 100u) return true;
    if (elapsed >= 1500u && nusa_gps.speed_kmh10 >= 100u) {
        uint16_t delta = (uint16_t)((nusa_gps.heading_deg + 360u - last_tx_heading) % 360u);
        if (delta > 180u) delta = 360u - delta;
        const uint16_t turn = (uint16_t)(25u + 2500u / nusa_gps.speed_kmh10);
        if (delta >= turn) return true;
    }
    return false;
}
void NUSA_GPS_MarkSent(uint32_t tick) {
    nusa_gps.sent_once = 1u;
    nusa_gps.last_tx_tick = tick;
    last_tx_heading = nusa_gps.heading_deg;
}
