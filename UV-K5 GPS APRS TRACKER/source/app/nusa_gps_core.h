/* NUSA UV-K5 GPS tracker core. Pure C, no radio hardware dependency.
 * Compatible with NEO-6/6M, NEO-M8N, ATGM336H: NMEA 0183 RMC/GGA.
 * GPS TX output must be 3.3V UART TTL (not RS-232).
 */
#ifndef NUSA_GPS_CORE_H
#define NUSA_GPS_CORE_H
#include <stdint.h>
#include <stdbool.h>
typedef struct {
    int32_t latitude_udeg, longitude_udeg;
    uint16_t speed_kmh10, heading_deg;
    uint8_t satellites, valid, seen_rmc, sent_once;
    uint32_t fix_tick, first_fix_tick, last_tx_tick;
    uint16_t good_sentences, bad_sentences;
} NUSA_GPS_Fix;
extern NUSA_GPS_Fix nusa_gps;
void NUSA_GPS_Reset(void);
void NUSA_GPS_ResetStream(void);
void NUSA_GPS_Feed(uint8_t ch, uint32_t tick10ms);
bool NUSA_GPS_Fresh(uint32_t tick10ms);
bool NUSA_GPS_NeedsBeacon(uint32_t tick10ms, uint8_t interval_min);
void NUSA_GPS_MarkSent(uint32_t tick10ms);
#endif
