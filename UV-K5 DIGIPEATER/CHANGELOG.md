# Changelog

## Persistence Fix1 — 2026-09-23

Field-tested EEPROM persistence maintenance release for REV1G Normal and REV1H Standalone.

Fixed:

- APRS settings are now read as two 8-byte EEPROM pages instead of one 16-byte read.
- coordinates saved by Fix1 include an integrity check in unused coordinate bits while retaining legacy-read compatibility.
- REV1H Standalone Object Name / Comment persistence bug fixed: metadata is now written as five 8-byte pages instead of a single 8-byte page.
- Standalone metadata upgraded to CRC16-protected NAP3 format.
- full 9-character Object Name, full 16-character Comment, symbol table and symbol code now persist across power cycles.

Validation:

- all builds compile with `-Werror`
- packed CRC-XMODEM verified
- persistence round-trip/corruption tests passed
- power-cycle persistence tested successfully on real UV-K5 hardware


## REV1H Standalone

Standalone-only feature revision based on the proven REV1G Bell 202 / AX.25 RF engine.

Added:

- dedicated APRS-only menu
- editable standalone `APRFq`
- selectable Station / APRS Object beacon
- editable APRS Object Name, maximum 9 characters
- editable user comment suffix, maximum 16 characters
- mandatory transmitted comment prefix `NUSA DIGI`
- editable APRS symbol table (`/` or `\\`)
- editable APRS symbol code
- APRS Object alive marker `*` with pseudo timestamp `111111z`
- non-APRS HT features removed from the dedicated build to preserve flash space

Validation:

- compile verified with `-Werror`
- packed CRC-XMODEM verified
- hardware/on-air field test of REV1H pending

## REV1G

- Bell 202 AFSK TX using 1200 Hz / 2200 Hz tones.
- Hardware `TIMER_BASE0` for deterministic APRS bit pacing.
- Default TX preamble approximately 900 ms.
- Configurable `DgDly` and `DgTail`.
- WIDE2 n-N digipeater support.
- Duplicate suppression approximately 20 seconds.
- Fixed/manual position beacon.
- Position beacon TOCALL changed from `APRS` to `APZUAG` for REV1G Normal and Standalone.
- Manual `*` beacon.
- APRS weak-RX configuration.
- SoundModem RF decode confirmed in field testing.
- UV-5RH APRS digipeating confirmed in field testing.

## REV1G Standalone

Based on the proven REV1G RF engine.

Added:

- automatic APRS startup after reset/power-cycle
- forced n-N / WIDE2
- VFO A runtime lock
- RX VFO A + TX VFO A
- Dual Watch disabled
- Cross Band disabled
- `APRFq` keypad frequency menu
- dedicated APRS frequency stored in EEPROM
- simplified APRS-only configuration menu

The REV1G Bell 202 / `TIMER_BASE0` TX engine is intentionally retained.
