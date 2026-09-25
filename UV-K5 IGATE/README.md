# NUSA UV-K5 iGATE

## Latest UV-K5 firmware: REV1U

REV1U is the current UV-K5 iGATE firmware for both Normal and Standalone. It keeps the existing UART protocol and ESP32 firmware while adopting the field-tested REV1T multi-device RX engine plus the REV1U LED/branding fix.

- Standby LED OFF / RX green / TX red.
- Multiple BK4819 RX fragments are aggregated before AX.25 decode.
- Persistence Fix1 is retained.
- Boot/version identity is compiled as NUSA.
- **No new ESP32 firmware is required.**

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1U_NORMAL.packed.bin
firmware/NUSA_UVK5_IGATE_REV1U_STANDALONE.packed.bin
```

Release: **[rev1u](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1u)**


[![iGate REV1A Downloads](https://img.shields.io/github/downloads/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/igate-rev1a/total?style=for-the-badge&logo=github&label=iGate%20REV1A%20Downloads)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/igate-rev1a-fix1)

**NUSA UV-K5 iGATE** turns a Quansheng UV-K5/UV-5K into the RF modem/radio side of a Wi-Fi APRS iGate. REV1U is the current Normal and Standalone UV-K5 firmware family. An ESP32 DevKit/WROOM-32 or ESP32-C3 handles Wi-Fi, APRS-IS connectivity, filtering, and the web dashboard.

Two UV-K5 firmware variants are provided:

1. **REV1U Normal** — APRS/iGate is entered manually, normally with Long F2.
2. **REV1U Standalone** — dedicated iGate mode starts automatically after power-on.

Both UV-K5 variants use the **same UART protocol**. Separate compiled ESP32 packages are provided for classic ESP32/WROOM-32 and ESP32-C3.

## Latest Standalone: REV1B

> **REV1A Normal remains the field-tested iGate baseline. REV1B is a Standalone-only feature revision.**

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1B_STANDALONE.packed.bin
```

Release: **[igate-rev1b-standalone](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/igate-rev1b-standalone-fix1)**

REV1B Standalone retains the existing UV-K5 ↔ ESP32 UART/iGate engine and adds dedicated APRS beacon controls:

```text
TxPwr
APRFq
IS2RF
IgCall
IgSSID
TxDly
TxTail
PosBcn
BType
ObjNam
BComnt
SymTbl
Symbol
BLat
LatNS
BLon
LonEW
```

- `BType`: Station or APRS Object beacon.
- `ObjNam`: editable Object Name, maximum 9 characters. Default: `NUSAIGATE`.
- `BComnt`: editable user comment suffix, maximum 16 characters.
- `SymTbl`: APRS primary `/` or alternate `\` symbol table.
- `Symbol`: editable printable APRS symbol code.
- Object beacon uses alive marker `*` and pseudo timestamp `111111z`.
- Every beacon comment always begins with **`NUSA IGATE`**; the menu edits only the suffix.

The same ESP32 REV1A package/protocol is retained; no new ESP32 firmware is required for this UV-K5 Standalone revision.

**Validation status:** build verified and packed-CRC verified; the Fix1 EEPROM persistence changes have been tested successfully on real UV-K5 hardware.

SHA256:

```text
0080f7ea35f2e84489c341c3987aeabc9eea04cc00e1c8edbe9dd5c68b098460
```

## Persistence Fix1 — 23 September 2026

The current REV1A Normal and REV1B Standalone UV-K5 binaries include the power-cycle persistence fix:

- APRS EEPROM data is read as two 8-byte pages.
- saved coordinates include an integrity check while remaining compatible with legacy coordinate data.
- REV1B Standalone Object Name and Comment are now stored as five 8-byte EEPROM pages with CRC16 protection.
- full 9-character Object Name and full 16-character Comment persist after radio OFF/ON.

After upgrading REV1B Standalone, enter and save `ObjNam` and `BComnt` once. The old firmware did not write the missing bytes, so previously truncated custom text cannot be recovered.

SHA256 of the Fix1 UV-K5 binaries:

```text
675b32bbda58a71d333af5b0fa4e8ec7d8eb664f25e672712c701573a74855a7  NUSA_UVK5_IGATE_REV1A.packed.bin
0080f7ea35f2e84489c341c3987aeabc9eea04cc00e1c8edbe9dd5c68b098460  NUSA_UVK5_IGATE_REV1B_STANDALONE.packed.bin
```

See **[Persistence Fix1 details](../PERSISTENCE_FIX1.md)**.

## Connection Diagram

![NUSA UV-K5 iGATE Connection Diagram](Connection%20Diagram.png)

## Architecture

```text
APRS RF station
      |
      v
Quansheng UV-K5
Bell 202 / AX.25
      |
      | UART 38400 8N1
      v
ESP32 DevKit / WROOM-32
      |
      | Wi-Fi / TCP
      v
APRS-IS
```

The return path is implemented for eligible APRS messages:

```text
APRS-IS message
      |
      v
ESP32 eligibility / anti-loop checks
      |
      | UART
      v
UV-K5 Bell 202 TX
      |
      v
Local RF station
```

## Firmware variants

| Feature | REV1A Normal | REV1B Standalone |
|---|---|---|
| APRS/iGate startup | Manual, normally Long F2 | Automatic at power-on |
| VFO used | Selected VFO when APRS is entered | VFO A |
| Dual Watch while APRS is active | OFF | OFF |
| Cross Band | Normal radio setting outside APRS | OFF in standalone runtime |
| Voice PTT while APRS is active | Blocked | Blocked |
| EXIT from APRS | Leaves APRS | Does not disable iGate |
| MENU from APRS | Use normal radio/menu flow | Opens configuration menu |
| UART protocol | 38400 8N1 | Same |
| ESP32 firmware | Same | Same |
| RF -> APRS-IS | Same | Same |
| APRS-IS -> RF message gating | Same | Same |
| Dedicated APRS frequency menu | No | Yes |
| Station / APRS Object beacon | Station | Selectable |
| Editable Object Name | No | Yes, 9 chars |
| Editable comment suffix | No | Yes, 16 chars |
| Mandatory comment prefix | `NUSA IGATE` | `NUSA IGATE` |
| Editable APRS symbol | No | Yes, table + code |

### REV1A Normal

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
```

Select the APRS frequency/VFO, then enter APRS/iGate mode manually.

### REV1B Standalone

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1B_STANDALONE.packed.bin
```

Standalone behavior:

```text
POWER ON
   |
   v
Load EEPROM / VFO settings
   |
   v
Force VFO A
Dual Watch OFF
Cross Band OFF
   |
   v
Use dedicated stored APRFq (first boot inherits VFO-A)
Force simplex APRS runtime
   |
   v
Enter APRS/iGate automatically
   |
   v
IGATE ACTIVE
```

The standalone build keeps the same Bell-202 engine, AX.25 handling, UART framing, RF -> APRS-IS export, and IS -> RF message queue used by REV1A Normal.

The ESP32 does **not** need a different firmware.

## Main features

- Bell 202 / AX.25 APRS at 1200 baud.
- RF -> APRS-IS gating.
- APRS-IS -> RF restricted to APRS messages.
- IS -> RF target must have been heard directly on RF within the last 30 minutes.
- Exact callsign + SSID target matching.
- Anti-loop checks for NOGATE, RFONLY, TCPXX, Internet-heard stations, duplicates and third-party handling.
- Web dashboard at `http://192.168.4.1/`.
- Default ESP32 AP: `NUSA-IGATE` / `12345678`.
- UART: **38400 baud, 8N1**.
- Position beacon TOCALL: `APZUAG`.
- Position path: `WIDE2-1`.
- Position comment always begins with `NUSA IGATE`.
- REV1B Standalone can select Station or APRS Object beacon.
- REV1B Standalone provides editable 9-character Object Name, 16-character comment suffix, symbol table and symbol code.

## Validation status

- REV1A Normal UV-K5 firmware: compile verified, packed CRC verified.
- Corrected UART interface: **field-tested successfully**.
- REV1A Normal RF -> ESP32 -> APRS-IS: **field-tested successfully**.
- REV1B Standalone UV-K5 firmware: **compile verified and packed CRC verified**.
- REV1B Standalone Fix1 EEPROM persistence: **field-tested successfully on hardware**.
- IS -> RF APRS message path: implemented; pending on-air field validation.
- ESP32-C3 package: **compile verified** with Arduino-ESP32 core 3.3.11 (`esp32:esp32:esp32c3`); hardware field test pending.

## Release files

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
firmware/NUSA_UVK5_IGATE_REV1B_STANDALONE.packed.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip
esp32/NUSA_UVK5_ESP32C3_IGATE_REV1A_BIN.zip
FLASHING.md
SHA256SUMS.txt
```

The classic ESP32 ZIP contains exactly four compiled binary files:

```text
NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

The ESP32-C3 ZIP also contains exactly four compiled binary files:

```text
NUSA_UVK5_ESP32C3_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32C3_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32C3_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32C3_IGATE_REV1A_firmware.bin
```

See **[FLASHING.md](FLASHING.md)** for the exact offsets. ESP32-C3 uses bootloader offset **0x0**.

## Correct UART connection

```text
UV-K5 2.5 mm RING   (UART TX) -- 1 kΩ --> ESP32 GPIO16 / RX2
UV-K5 2.5 mm SLEEVE (GND) -------------- ESP32 GND
ESP32 GPIO17 / TX2 -- 1 kΩ --> UV-K5 3.5 mm SLEEVE (MIC-/PTT/UART RX)
```

ESP32-C3:

```text
UV-K5 2.5 mm RING   (UART TX) -- 1 kΩ --> ESP32-C3 GPIO4 / RX
UV-K5 2.5 mm SLEEVE (GND) -------------- ESP32-C3 GND
ESP32-C3 GPIO5 / TX -- 1 kΩ --> UV-K5 3.5 mm SLEEVE (MIC-/PTT/UART RX)
```

Do not connect these to the ESP32:

```text
UV-K5 2.5 mm TIP  = SPK+
UV-K5 3.5 mm RING = MIC+
UV-K5 3.5 mm TIP  = radio V+
```

The 3.5 mm TIP carries radio supply voltage and must not be connected to an ESP32 GPIO or ground.

## Credits

```text
NUSA UV-K5 iGATE
YF9UAG
Indonesia
```

## Safety / regulatory note

Verify wiring with a multimeter before applying power. Never rely on cable wire colors to identify TIP/RING/SLEEVE. The operator is responsible for lawful frequency, power, callsign and unattended operation.
