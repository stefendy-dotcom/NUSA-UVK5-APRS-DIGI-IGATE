# Firmware

Official NUSA UV-K5 APRS DIGI firmware binaries.

## Current

- `NUSA_UVK5_APRS_DIGI_REV1U_NORMAL.packed.bin` — Normal, current field-tested release.
- `NUSA_UVK5_APRS_DIGI_REV1U_STANDALONE.packed.bin` — Standalone, current field-tested release.

REV1U keeps Persistence Fix1 and adds field-tested multi-device RX improvements (aggregated BK4819 RX fragments, 480-byte RX buffer, decode after about 120 ms FSK inactivity), corrected LED behavior (standby OFF, RX green, TX red), and NUSA boot branding.

## Previous

- `NUSA_UVK5_APRS_DIGI_REV1G.packed.bin` — Normal, field-tested RF baseline.
- `NUSA_UVK5_APRS_DIGI_REV1H_STANDALONE.packed.bin` — previous dedicated Standalone revision.

## Legacy Standalone

- `NUSA_UVK5_APRS_DIGI_REV1G_STANDALONE.packed.bin` — previous Standalone build retained for reference.

REV1H Standalone adds Station/Object beacon selection, editable 9-character Object Name, editable comment suffix with mandatory `NUSA DIGI` prefix, and editable APRS symbol table/code.
