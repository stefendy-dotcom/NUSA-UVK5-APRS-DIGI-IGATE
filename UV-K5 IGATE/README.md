# NUSA UV-K5 iGATE REV1A

[![iGate REV1A Downloads](https://img.shields.io/github/downloads/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/igate-rev1a/total?style=for-the-badge&logo=github&label=iGate%20REV1A%20Downloads)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/igate-rev1a)

**NUSA UV-K5 iGATE REV1A** turns a Quansheng UV-K5/UV-5K into the RF modem/radio side of a Wi-Fi APRS iGate. An ESP32 DevKit/WROOM-32 handles Wi-Fi, APRS-IS connectivity, filtering, and the web dashboard.

Two UV-K5 firmware variants are provided:

1. **REV1A Normal** — APRS/iGate is entered manually, normally with Long F2.
2. **REV1A Standalone** — dedicated iGate mode starts automatically after power-on.

Both variants use the **same ESP32 firmware and the same UART protocol**.

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

| Feature | REV1A Normal | REV1A Standalone |
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

### REV1A Normal

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
```

Select the APRS frequency/VFO, then enter APRS/iGate mode manually.

### REV1A Standalone

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1A_STANDALONE.packed.bin
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
Use stored VFO-A RX frequency
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
- Position comment: `NUSA IGATE`.

## Validation status

- REV1A Normal UV-K5 firmware: compile verified, packed CRC verified.
- Corrected UART interface: **field-tested successfully**.
- REV1A Normal RF -> ESP32 -> APRS-IS: **field-tested successfully**.
- REV1A Standalone UV-K5 firmware: **compile verified and packed CRC verified**.
- REV1A Standalone: **not yet field-tested on hardware**.
- IS -> RF APRS message path: implemented; pending on-air field validation.

## Release files

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
firmware/NUSA_UVK5_IGATE_REV1A_STANDALONE.packed.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip
FLASHING.md
SHA256SUMS.txt
```

The ESP32 ZIP contains exactly four compiled binary files:

```text
NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

See **[FLASHING.md](FLASHING.md)** for the exact ESP32 offsets and UV-K5 flashing procedure.

## Correct UART connection

```text
UV-K5 2.5 mm RING   (UART TX) -- 1 kΩ --> ESP32 GPIO16 / RX2
UV-K5 2.5 mm SLEEVE (GND) -------------- ESP32 GND
ESP32 GPIO17 / TX2 -- 1 kΩ --> UV-K5 3.5 mm SLEEVE (MIC-/PTT/UART RX)
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
