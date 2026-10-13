#include "nusa_gps_autobaud.h"
static uint32_t since_switch, last_valid;
static uint16_t seen;
static uint8_t idx, locked, confirmations;

void NUSA_GPS_AutoBaud_Reset(uint32_t now, uint16_t valid_checksums)
{
    since_switch = now;
    last_valid = now;
    seen = valid_checksums;
    idx = 0;
    locked = 0;
    confirmations = 0;
}
uint8_t NUSA_GPS_AutoBaud_Active(void) { return idx; }
uint8_t NUSA_GPS_AutoBaud_Locked(void) { return locked; }
int NUSA_GPS_AutoBaud_Update(uint32_t now, uint16_t good)
{
    if (good != seen) {
        uint16_t delta = (uint16_t)(good - seen);
        seen = good;
        last_valid = now;
        if (!locked) {
            confirmations = (uint8_t)(confirmations + (delta > 2u ? 2u : delta));
            if (confirmations >= 2u) locked = 1u;
        }
    }
    if (locked) {
        if ((uint32_t)(now - last_valid) < 3000u) return -1;
        // Module disconnected or baud changed: do not remain locked forever.
        locked = 0u;
    } else if ((uint32_t)(now - since_switch) < 500u) {
        return -1;
    }
    idx = (uint8_t)((idx + 1u) % 3u);
    since_switch = now;
    last_valid = now;
    confirmations = 0u;
    return (int)idx;
}
