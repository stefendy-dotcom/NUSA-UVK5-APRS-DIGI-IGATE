# NUSA UV-K5 iGATE REV1A

**NUSA UV-K5 iGATE REV1A** turns a Quansheng UV-K5/UV-5K into the RF modem/radio side of a Wi-Fi APRS iGate, with an ESP32 DevKit/WROOM-32 handling APRS-IS connectivity and the web dashboard.

This is the **normal/manual-start** iGate variant. The radio still behaves as a normal handheld after power-on; enter the APRS/iGate screen with the configured APRS action (normally **Long F2**). A future standalone build can use the same RF engine with automatic APRS startup.

## Architecture

```text
APRS RF station
      |
      v
Quansheng UV-K5
Bell 202 / AX.25 decode
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

## Main features

- Dedicated UV-K5 APRS iGate firmware.
- Bell 202 / AX.25 1200 baud RF modem runs inside the UV-K5 firmware.
- ESP32 connects the radio to APRS-IS over Wi-Fi.
- RF -> APRS-IS gating with direct/indirect station handling.
- APRS-IS -> RF is restricted to **APRS messages only**.
- IS -> RF target must have been heard **directly on RF within the last 30 minutes**.
- Exact callsign + SSID matching for the local target.
- Anti-loop checks for NOGATE, RFONLY, TCPXX, Internet-heard stations, duplicates, and third-party handling.
- Web dashboard at `http://192.168.4.1/` while connected to the ESP32 AP.
- Default ESP32 AP: `NUSA-IGATE` / `12345678`.
- UV-K5 and ESP32 UART: **38400 baud, 8N1**.
- Position beacon TOCALL: `APZUAG`.
- Position symbol: alternate table `\` with symbol `&` (`\&`).
- Position path: `WIDE2-1`.
- Position comment: `NUSA IGATE`.
- APRS/iGate runtime is locked to one selected VFO; Dual Watch is disabled while the APRS modem is active so packet reception is not interrupted by VFO switching.

## Current validation status

- UV-K5 firmware: compile verified.
- UV-K5 `.packed.bin`: packed CRC verified.
- ESP32 sketch: compile verified using **Arduino-ESP32 core 3.3.11**, board target `esp32:esp32:esp32` (ESP32 Dev Module / WROOM-32 class hardware).
- Corrected UART interface: **field-tested successfully**.
- RF -> ESP32 -> APRS-IS: **field-tested successfully**.
- IS -> RF APRS message path: implemented; do not treat it as field-proven until it has been tested on-air in the target installation.

## Files

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_FULL.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_APP.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A.ino
docs/NUSA_UVK5_IGATE_REV1A_CONNECTION.png
FLASHING.md
SHA256SUMS.txt
```

For most ESP32 users, use **`NUSA_UVK5_ESP32_IGATE_REV1A_FULL.bin`**. It is the merged 4 MB flash image and is intended to be written at address `0x0`.

The `APP.bin` file is provided for advanced/recovery use and is written at `0x10000` only when a compatible bootloader and partition table are already installed.

## Correct UART connection

Use the connection diagram in this repository. The important electrical connections are:

```text
UV-K5 2.5 mm RING   (UART TX) -- 1 kΩ --> ESP32 GPIO16 / RX2
UV-K5 2.5 mm SLEEVE (GND) -------------- ESP32 GND
ESP32 GPIO17 / TX2 -- 1 kΩ --> UV-K5 3.5 mm SLEEVE (MIC-/PTT/UART RX)
```

Do **not** connect these to the ESP32:

```text
UV-K5 2.5 mm TIP  = SPK+
UV-K5 3.5 mm RING = MIC+
UV-K5 3.5 mm TIP  = radio V+
```

The 3.5 mm TIP carries radio supply voltage and must not be connected to an ESP32 GPIO or ground.

![NUSA UV-K5 iGATE connection](docs/NUSA_UVK5_IGATE_REV1A_CONNECTION.png)

## Basic operation

1. Set the desired APRS frequency on the UV-K5 VFO that will be used by the iGate.
2. Enter the UV-K5 APRS/iGate screen (normally Long F2).
3. Power the ESP32 and connect to AP `NUSA-IGATE`.
4. Open `http://192.168.4.1/` and configure Wi-Fi, iGate callsign/SSID and APRS-IS server settings.
5. Confirm Wi-Fi is connected and APRS-IS shows `VERIFIED`.
6. Transmit an APRS packet from another station.
7. Confirm the UV-K5 decodes the packet and the ESP32 dashboard counters under **RF -> APRS-IS** increase.
8. Confirm the packet reaches APRS-IS / your APRS map.

See **[FLASHING.md](FLASHING.md)** before installing either binary.

## Credits

Development and field testing:

```text
NUSA UV-K5 iGATE
YF9UAG
Indonesia
```

Built on the open-source Quansheng UV-K5 firmware ecosystem and APRS community work.

## Safety / regulatory note

Verify all wiring with a multimeter before applying power. Never assume cable wire colors identify TIP/RING/SLEEVE correctly. Amateur-radio frequency, power, identification and unattended-operation rules vary by jurisdiction; the operator is responsible for compliant operation.
