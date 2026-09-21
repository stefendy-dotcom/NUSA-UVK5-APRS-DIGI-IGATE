# Changelog

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
