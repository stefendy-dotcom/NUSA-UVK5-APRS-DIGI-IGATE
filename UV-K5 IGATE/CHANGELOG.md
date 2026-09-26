# Changelog

## REV1U — 2026-09-25

Field-tested UV-K5 firmware update for REV1U Normal and REV1U Standalone (release `rev1u`).

- Adopts the field-tested REV1T multi-device RX engine (aggregated BK4819 fragments, 480-byte buffer, decode after about 120 ms FSK inactivity).
- Corrected LED behavior and NUSA boot branding.
- Persistence Fix1 remains included.
- iGATE UART protocol and ESP32 side are unchanged; no new ESP32 firmware is required.

## Persistence Fix1 — 2026-09-23

Field-tested EEPROM persistence maintenance release for REV1A Normal and REV1B Standalone.

Fixed:

- APRS settings are read as two 8-byte EEPROM pages.
- coordinates saved by Fix1 include an integrity check in unused coordinate bits while retaining legacy-read compatibility.
- REV1B Standalone Object Name / Comment metadata is written as five 8-byte EEPROM pages instead of a single page.
- Standalone metadata uses CRC16-protected NAP3 format.
- full 9-character Object Name, full 16-character Comment, symbol table and symbol code persist across power cycles.

Validation:

- compile verified with `-Werror`
- packed CRC-XMODEM verified
- persistence round-trip/corruption tests passed
- power-cycle persistence tested successfully on real UV-K5 hardware

## REV1B Standalone

Standalone-only UV-K5 revision retaining the REV1A UART/iGate engine while adding dedicated APRS beacon controls, Object Name, comment suffix and editable symbol settings.

## REV1A

Normal UV-K5 iGate baseline. RF → ESP32 → APRS-IS has been field-tested successfully.
