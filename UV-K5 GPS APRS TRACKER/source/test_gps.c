#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "base/app/nusa_gps_core.h"
static void feed(const char *body, uint32_t tick, int bad) {
    uint8_t sum=0;
    for (const char *p=body;*p;p++) sum^=(uint8_t)*p;
    if (bad) sum^=1u;
    char buf[160]; snprintf(buf,sizeof(buf),"$%s*%02X\r\n",body,sum);
    for (const char *p=buf;*p;p++) NUSA_GPS_Feed((uint8_t)*p,tick);
}
int main(void) {
    NUSA_GPS_Reset();
    assert(!NUSA_GPS_Fresh(0));
    feed("GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,,",100,0);
    assert(NUSA_GPS_Fresh(100));
    assert(nusa_gps.latitude_udeg==48117300);
    assert(nusa_gps.longitude_udeg==11516667);
    assert(nusa_gps.speed_kmh10>=414 && nusa_gps.speed_kmh10<=416);
    assert(nusa_gps.heading_deg==84);
    assert(!NUSA_GPS_NeedsBeacon(1599,0));
    feed("GPGGA,123520,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,",1600,0);
    feed("GPRMC,123520,A,4807.038,N,01131.000,E,022.4,084.4,230394,,",1600,0);
    assert(nusa_gps.satellites==8u);
    assert(NUSA_GPS_NeedsBeacon(1600,0));
    NUSA_GPS_MarkSent(1600);
    assert(!NUSA_GPS_NeedsBeacon(1601,0));
    // invalid checksum must not alter coordinates
    feed("GNRMC,123530,A,5000.000,N,02000.000,E,020.0,090.0,230394,,A",1800,1);
    assert(nusa_gps.latitude_udeg==48117300 && nusa_gps.bad_sentences>0);
    assert(!NUSA_GPS_Fresh(2301));
    feed("GNRMC,123535,A,0421.454,S,14011.100,E,022.4,220.0,230394,,A",3200,0);
    assert(nusa_gps.latitude_udeg==-4357567 && nusa_gps.longitude_udeg==140185000);
    assert(nusa_gps.heading_deg==220);
    assert(NUSA_GPS_NeedsBeacon(3200,0)); // corner pegging
    NUSA_GPS_MarkSent(3200);
    assert(!NUSA_GPS_NeedsBeacon(3201,0));
    // explicit void status cancels fix immediately
    feed("GPRMC,123536,V,,,,,,,230394,,",3300,0);
    assert(!NUSA_GPS_Fresh(3300));
    assert(!NUSA_GPS_NeedsBeacon(999999,1));
    // zero-degree location is valid, not interpreted as missing
    feed("GPRMC,123537,A,0000.000,N,00000.000,E,000.0,000.0,230394,,",3400,0);
    assert(NUSA_GPS_Fresh(3400));
    assert(nusa_gps.latitude_udeg==0 && nusa_gps.longitude_udeg==0);
    // no automatic TX before interval ends
    NUSA_GPS_MarkSent(3400);
    feed("GPRMC,123540,A,0000.000,N,00000.000,E,000.0,000.0,230394,,",9400,0);
    assert(NUSA_GPS_NeedsBeacon(9400,1));
    puts("PASS: NEO-6/6M + GN NMEA checksums, coordinates, speed, heading, GGA sats, first-fix, corner, fixed, stale, invalid");
    return 0;
}
