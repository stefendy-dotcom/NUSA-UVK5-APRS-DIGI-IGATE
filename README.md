# NUSA UV-K5 APRS

[![REV1G Downloads](https://img.shields.io/github/downloads/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/rev1g/total?style=for-the-badge&logo=github&label=REV1G%20Downloads)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1g)
[![iGate REV1A Downloads](https://img.shields.io/github/downloads/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/igate-rev1a/total?style=for-the-badge&logo=github&label=iGate%20REV1A%20Downloads)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/igate-rev1a)

APRS projects for the **Quansheng UV-K5 / UV-5K**, developed by **YF9UAG, Indonesia**.

> There are **two different projects** in this repository.  
> The Digipeater can run on the UV-K5 by itself.  
> The iGate uses a **UV-K5 + ESP32**.

## Choose your project

| Project | Hardware | Purpose | Current version | Documentation |
|---|---|---|---|---|
| **UV-K5 DIGIPEATER** | UV-K5 only | Standalone APRS RF digipeater | REV1G | **[Open DIGIPEATER](./UV-K5%20DIGIPEATER/)** |
| **UV-K5 iGATE** | UV-K5 + ESP32 | RF ↔ APRS-IS gateway | REV1A | **[Open iGATE](./UV-K5%20IGATE/)** |

---

# UV-K5 DIGIPEATER

The Digipeater firmware runs directly on the Quansheng UV-K5 without an ESP32.

Main features:

- Bell 202 / AX.25 APRS 1200 baud
- WIDE2 n-N digipeating
- Normal and Standalone variants
- Position beacon
- TOCALL: `APZUAG`
- REV1G has been field-tested in standalone operation

**Documentation and firmware:**

### [UV-K5 DIGIPEATER →](./UV-K5%20DIGIPEATER/)

**Release:**

### [Download REV1G →](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1g)

---

# UV-K5 + ESP32 iGATE

**NUSA UV-K5 iGATE REV1A** uses the UV-K5 as the Bell 202 / AX.25 RF modem and an ESP32 as the Wi-Fi/APRS-IS gateway.

```text
APRS RF
   |
   v
Quansheng UV-K5
Bell 202 / AX.25 1200 baud
   |
   | UART 38400 8N1
   v
ESP32
   |
   | Wi-Fi / TCP
   v
APRS-IS
```

For eligible APRS messages, the return path is also implemented:

```text
APRS-IS
   |
   v
ESP32 filtering / anti-loop checks
   |
   | UART
   v
UV-K5
   |
   v
Local APRS RF station
```

## What you need for the iGate

- Quansheng UV-K5 / UV-5K
- ESP32 DevKit / WROOM-32, or supported ESP32-C3 build
- Wi-Fi Internet connection
- Correct UV-K5 ↔ ESP32 UART interface
- Valid APRS callsign and APRS-IS passcode

## iGate quick start

### 1. Flash the UV-K5

Choose one of the supplied REV1A firmware variants:

- **REV1A Normal** — enter APRS/iGate mode manually, normally with **Long F2**
- **REV1A Standalone** — dedicated iGate mode starts automatically after power-on

Firmware and detailed instructions are in:

**[UV-K5 iGATE documentation →](./UV-K5%20IGATE/)**

### 2. Flash the ESP32

The GitHub release provides compiled ESP32 ZIP packages. The classic ESP32 package contains the four binaries required for flashing:

```text
bootloader.bin
partitions.bin
boot_app0.bin
firmware.bin
```

Exact flash offsets and ESP32-C3 instructions:

**[FLASHING.md →](./UV-K5%20IGATE/FLASHING.md)**

### 3. Connect the UV-K5 to the ESP32

Classic ESP32 / WROOM-32:

```text
UV-K5 2.5 mm RING   (UART TX) -- 1 kΩ --> ESP32 GPIO16 / RX2
UV-K5 2.5 mm SLEEVE (GND) -------------- ESP32 GND
ESP32 GPIO17 / TX2 -- 1 kΩ --> UV-K5 3.5 mm SLEEVE (MIC-/PTT/UART RX)
```

Do **not** connect:

```text
UV-K5 2.5 mm TIP  = SPK+
UV-K5 3.5 mm RING = MIC+
UV-K5 3.5 mm TIP  = radio V+
```

The 3.5 mm TIP carries radio supply voltage and must not be connected to an ESP32 GPIO or ground.

**Connection diagram:**

### [View Connection Diagram →](./UV-K5%20IGATE/Connection%20Diagram.png)

### 4. Configure the ESP32

After flashing:

1. Connect to Wi-Fi AP **NUSA-IGATE**
2. Password: **12345678**
3. Open **http://192.168.4.1/**
4. Configure:
   - your Wi-Fi
   - iGate callsign and SSID
   - APRS-IS server
   - APRS-IS passcode
   - position / beacon settings as required

### 5. Start the iGate

For **REV1A Normal**:

1. Select the VFO and APRS frequency on the UV-K5.
2. Long-press **F2** to enter APRS/iGate mode.
3. The selected VFO is used and Dual Watch is disabled while APRS mode is active.
4. The ESP32 handles Wi-Fi, APRS-IS connectivity, filtering, and the web dashboard.

For **REV1A Standalone**, the dedicated iGate runtime starts automatically after power-on.

## iGate traffic behavior

### RF → APRS-IS

RF packets decoded by the UV-K5 are sent to the ESP32 and then gated to APRS-IS with duplicate and loop protection.

This path has been **field-tested successfully** on REV1A Normal.

### APRS-IS → RF

The return path is intentionally restricted.

An APRS-IS message may be sent to RF only when its target has been heard **directly on RF within the last 30 minutes**, using exact callsign + SSID matching and additional anti-loop checks.

This path is implemented and is pending full on-air field validation.

## iGate validation status

- REV1A Normal UV-K5 firmware: compile verified and packed CRC verified
- Corrected UV-K5 ↔ ESP32 UART interface: **field-tested successfully**
- REV1A Normal RF → ESP32 → APRS-IS: **field-tested successfully**
- REV1A Standalone: compile verified; hardware field test pending
- IS → RF APRS message path: implemented; on-air field validation pending
- ESP32-C3 package: compile verified; hardware field test pending

## iGate documentation

- **[English README](./UV-K5%20IGATE/README.md)**
- **[README Bahasa Indonesia](./UV-K5%20IGATE/README_ID.md)**
- **[Flashing instructions](./UV-K5%20IGATE/FLASHING.md)**
- **[Connection diagram](./UV-K5%20IGATE/Connection%20Diagram.png)**

**Release:**

### [Download iGate REV1A →](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/igate-rev1a)

---

## Releases

- **Digipeater REV1G:** [rev1g](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1g)
- **iGate REV1A:** [igate-rev1a](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/igate-rev1a)

## License

See the repository-level **LICENSE** file. Project-specific attribution and documentation are stored inside each project folder.
