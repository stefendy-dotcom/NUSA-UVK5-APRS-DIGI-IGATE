# Persistence Fix1 — 23 September 2026

This maintenance release fixes APRS configuration persistence across radio power cycles for all four current UV-K5 variants.

## Fixed firmware

- DIGI Normal REV1G — `rev1g-fix1`
- DIGI Standalone REV1H — `rev1h-standalone-fix1`
- iGATE Normal REV1A — `igate-rev1a-fix1`
- iGATE Standalone REV1B — `igate-rev1b-standalone-fix1`

## Fixes

### Normal and Standalone variants

APRS EEPROM data is now read as two 8-byte pages, matching the UV-K5 EEPROM page layout. Position coordinates now include an integrity check stored in otherwise-unused coordinate bits. Legacy coordinates from the previous firmware remain readable; after the coordinates are saved once with Fix1, corrupted coordinate data is rejected instead of being accepted as a different valid-looking position.

### Standalone Object Name / Comment

The previous Standalone implementation built a 40-byte metadata record but called the UV-K5 `EEPROM_WriteBuffer()` routine only once. That routine writes one 8-byte page, so only the header, symbol fields, and the first two Object Name characters were actually saved. The Comment was never stored.

Fix1 writes the metadata as five 8-byte EEPROM pages and adds CRC16 protection. The full 9-character Object Name, full 16-character Comment, symbol table, and symbol code now survive power cycles.

Old truncated custom Object Name/Comment values cannot be recovered because those bytes were never written by the previous firmware. After upgrading Standalone, enter and save Object Name and Comment once.

## Validation

The four builds compile with `-Werror`, packed firmware CRC-XMODEM was verified, encode/decode round-trip tests passed, and persistence behavior was subsequently tested successfully on real UV-K5 hardware.

Public GitHub releases contain only the firmware files needed by users.
