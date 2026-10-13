/* NUSA GPS serial rate discovery. Hardware independent and unit-testable. */
#ifndef NUSA_GPS_AUTOBAUD_H
#define NUSA_GPS_AUTOBAUD_H
#include <stdint.h>
void NUSA_GPS_AutoBaud_Reset(uint32_t now, uint16_t valid_checksums);
// Returns baud index 0..2 when UART should be reconfigured; else -1.
// A baud is locked after two checksum-valid NMEA sentences.
// On loss of valid NMEA for 30s, scanning automatically resumes.
int NUSA_GPS_AutoBaud_Update(uint32_t now, uint16_t valid_checksums);
uint8_t NUSA_GPS_AutoBaud_Active(void);
uint8_t NUSA_GPS_AutoBaud_Locked(void);
#endif
