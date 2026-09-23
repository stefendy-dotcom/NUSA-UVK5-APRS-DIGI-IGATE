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

## SHA256

```text
51b89ff9bb9bdbf141b44f0ffc362a2e0cd9007d4ece376c5d863209a3befa5f  NUSA_UVK5_APRS_DIGI_REV1G.packed.bin
d567603ced9ef646109e2049218ab65d71028ec7572b09f33d12ca27e87e47b2  NUSA_UVK5_APRS_DIGI_REV1H_STANDALONE.packed.bin
675b32bbda58a71d333af5b0fa4e8ec7d8eb664f25e672712c701573a74855a7  NUSA_UVK5_IGATE_REV1A.packed.bin
0080f7ea35f2e84489c341c3987aeabc9eea04cc00e1c8edbe9dd5c68b098460  NUSA_UVK5_IGATE_REV1B_STANDALONE.packed.bin
```

Source-level `settings.c` patches for each baseline are stored under `source-patches/PERSISTENCE_FIX1/`.
