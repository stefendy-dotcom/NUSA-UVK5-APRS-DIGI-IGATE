# Persistence Fix1 source patches

These patches document the settings.c changes used to build the field-tested Persistence Fix1 binaries.

They cover:
- 8-byte EEPROM page-aligned APRS reads
- coordinate integrity checking with legacy-read compatibility
- Standalone 5 x 8-byte metadata writes
- CRC16-protected Standalone metadata

See ../../PERSISTENCE_FIX1.md for release details and SHA256 values.
