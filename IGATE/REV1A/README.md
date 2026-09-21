# NUSA UV-K5 iGATE REV1A

**NUSA UV-K5 iGATE REV1A** turns a Quansheng UV-K5/UV-5K into the RF modem/radio side of a Wi-Fi APRS iGate, with an ESP32 DevKit/WROOM-32 handling APRS-IS connectivity and the web dashboard.

This is the **normal/manual-start** iGate variant. After power-on the radio still behaves as a normal handheld; enter the APRS/iGate screen using the configured APRS action, normally **Long F2**.

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
- Anti-loop checks for NOGATE, RFONLY, TCPXX, Internet-heard stations, duplicates and third-party handling.
- Web dashboard at `http://192.168.4.1/`.
- Default ESP32 AP: `NUSA-IGATE` / `12345678`.
- UART: **38400 baud, 8N1**.
- Position beacon TOCALL: `APZUAG`.
- Position path: `WIDE2-1`.
- Position comment: `NUSA IGATE`.
- Dual Watch is disabled while the APRS modem is active so packets are not interrupted by VFO switching.

## Validation status

- UV-K5 firmware: compile verified.
- UV-K5 `.packed.bin`: packed CRC verified.
- ESP32 binary package: compile verified using **Arduino-ESP32 core 3.3.11** for `esp32:esp32:esp32`.
- Corrected UART interface: **field-tested successfully**.
- RF -> ESP32 -> APRS-IS: **field-tested successfully**.
- IS -> RF APRS message path: implemented; pending on-air field validation.

## Release files

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip
FLASHING.md
SHA256SUMS.txt
```

The ESP32 source `.ino` is **not distributed** in this release. The ESP32 is supplied as a compiled ZIP package containing exactly four binary files.

After extracting `NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip`:

```text
NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

Flash offsets:

```text
0x1000  bootloader
0x8000  partitions
0xE000  boot_app0
0x10000 firmware
```

See **[FLASHING.md](FLASHING.md)** for the exact command and procedure.

## Correct UART connection

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

## Basic operation

1. Set the desired APRS frequency on the UV-K5 VFO used by the iGate.
2. Enter the UV-K5 APRS/iGate screen, normally Long F2.
3. Power the ESP32 and connect to AP `NUSA-IGATE`.
4. Open `http://192.168.4.1/` and configure Wi-Fi, iGate callsign/SSID and APRS-IS.
5. Confirm Wi-Fi is connected and APRS-IS shows `VERIFIED`.
6. Send an APRS packet from another RF station.
7. Confirm the UV-K5 decodes it and the ESP32 **RF -> APRS-IS** counters increase.
8. Confirm the packet reaches APRS-IS.

## Credits

```text
NUSA UV-K5 iGATE
YF9UAG
Indonesia
```

## Safety / regulatory note

Verify all wiring with a multimeter before applying power. Never rely on cable wire colors to identify TIP/RING/SLEEVE. The operator is responsible for lawful frequency, power, callsign and unattended operation.
