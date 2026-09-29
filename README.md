# NUSA UV-K5 APRS

## 📊 Cumulative Firmware Downloads

[![DIGI Normal Downloads](https://img.shields.io/endpoint?url=https%3A%2F%2Fraw.githubusercontent.com%2Fstefendy-dotcom%2FNUSA-UVK5-APRS-DIGI-IGATE%2Fmain%2Fbadges%2Fdigi-normal.json&style=for-the-badge&logo=github)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases)
[![DIGI Standalone Downloads](https://img.shields.io/endpoint?url=https%3A%2F%2Fraw.githubusercontent.com%2Fstefendy-dotcom%2FNUSA-UVK5-APRS-DIGI-IGATE%2Fmain%2Fbadges%2Fdigi-standalone.json&style=for-the-badge&logo=github)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases)

[![iGATE Normal Downloads](https://img.shields.io/endpoint?url=https%3A%2F%2Fraw.githubusercontent.com%2Fstefendy-dotcom%2FNUSA-UVK5-APRS-DIGI-IGATE%2Fmain%2Fbadges%2Figate-normal.json&style=for-the-badge&logo=github)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases)
[![iGATE Standalone Downloads](https://img.shields.io/endpoint?url=https%3A%2F%2Fraw.githubusercontent.com%2Fstefendy-dotcom%2FNUSA-UVK5-APRS-DIGI-IGATE%2Fmain%2Fbadges%2Figate-standalone.json&style=for-the-badge&logo=github)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases)

[![ESP32 iGATE Downloads](https://img.shields.io/endpoint?url=https%3A%2F%2Fraw.githubusercontent.com%2Fstefendy-dotcom%2FNUSA-UVK5-APRS-DIGI-IGATE%2Fmain%2Fbadges%2Fesp32-igate.json&style=for-the-badge&logo=github)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases)
[![ESP32-C3 iGATE Downloads](https://img.shields.io/endpoint?url=https%3A%2F%2Fraw.githubusercontent.com%2Fstefendy-dotcom%2FNUSA-UVK5-APRS-DIGI-IGATE%2Fmain%2Fbadges%2Fesp32c3-igate.json&style=for-the-badge&logo=github)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases)

[![TOTAL Firmware Downloads](https://img.shields.io/endpoint?url=https%3A%2F%2Fraw.githubusercontent.com%2Fstefendy-dotcom%2FNUSA-UVK5-APRS-DIGI-IGATE%2Fmain%2Fbadges%2Ftotal.json&style=for-the-badge&logo=github)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases)

> These counters are **cumulative across all GitHub Release assets**, from the first public release through the latest release. They update automatically after release events and are rechecked every hour. Files such as `SHA256SUMS.txt` are not counted.

APRS projects for the **Quansheng UV-K5 / UV-5K**, developed by **YF9UAG, Indonesia**.

## ✅ Latest Release: REV1AB — 29 September 2026

REV1AB replaces REV1U for the current UV-K5 firmware family.

- **DIGI Normal + Standalone:** includes the field-proven `PATH FULL` fix. The retransmit/path buffer is 160 bytes, normal WIDE2-N trace insertion is retained, and a valid packet is no longer dropped when all AX.25 address slots are occupied.
- The DIGI fix was proven on-air in the REV1Z diagnostic build: the same packet that previously stopped at `D:PATH FULL` successfully reached `D:TX OK` and was confirmed retransmitted over RF.
- **iGATE Normal + Standalone:** keeps the newer RX/decode engine, sends RF→IS UART immediately after a valid AX.25/FCS decode, and restores the calibrated DP32 UART divisor `Frequency / 39053` for the intended ~38400-baud link.
- iGATE Normal hard-binds **Long F2** to APRS/iGATE so stale EEPROM side-key assignments cannot block entry.
- Persistence Fix1, NUSA boot branding, and LED behavior **standby OFF / RX green / TX red** are retained.
- **Validation note:** DIGI forwarding fix is field-proven. REV1AB iGATE is compile/pack verified but its RF→IS path still needs final hardware confirmation after the UART physical-path issue found during diagnostics is corrected.
- Release: **[REV1AB](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1ab)**

> **REV1U is superseded by REV1AB** because REV1U could decode packets but forwarding/digipeating was not reliable in field testing.

> ✅ **Persistence Fix1 — 23 September 2026:** power-cycle persistence fixes for DIGI Normal/Standalone and iGATE Normal/Standalone have been **tested successfully on real hardware**. See **[PERSISTENCE_FIX1.md](./PERSISTENCE_FIX1.md)**.

> There are **two different projects** in this repository.  
> The Digipeater can run on the UV-K5 by itself.  
> The iGate uses a **UV-K5 + ESP32**.

## Choose your project

| Project | Hardware | Purpose | Current version | Documentation |
|---|---|---|---|---|
| **UV-K5 DIGIPEATER** | UV-K5 only | Standalone APRS RF digipeater | **Normal REV1AB / Standalone REV1AB** | **[Open DIGIPEATER](./UV-K5%20DIGIPEATER/)** |
| **UV-K5 iGATE** | UV-K5 + ESP32 | RF ↔ APRS-IS gateway | **Normal REV1AB / Standalone REV1AB** | **[Open iGATE](./UV-K5%20IGATE/)** |

---

# UV-K5 DIGIPEATER

The Digipeater firmware runs directly on the Quansheng UV-K5 without an ESP32.

Main features:

- Bell 202 / AX.25 APRS 1200 baud
- WIDE2 n-N digipeating
- Normal and Standalone variants
- Position beacon
- TOCALL: `APZUAG`
- **REV1U** is the current field-tested DIGI engine for both Normal and Standalone; Persistence Fix1 is retained

**Documentation and firmware:**

### [UV-K5 DIGIPEATER →](./UV-K5%20DIGIPEATER/)

**Release:**

### [Download DIGI Normal + Standalone REV1AB →](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1ab)

---

# UV-K5 + ESP32 iGATE

**NUSA UV-K5 iGATE REV1AB** uses the UV-K5 as the Bell 202 / AX.25 RF modem and an ESP32 as the Wi-Fi/APRS-IS gateway.

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

Choose the required UV-K5 variant:

- **REV1AB Normal** — enter APRS/iGate mode manually, normally with **Long F2**
- **REV1AB Standalone** — dedicated iGate mode starts automatically after power-on and retains editable APRS Object, comment and symbol settings

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

For **REV1AB Normal**:

1. Select the VFO and APRS frequency on the UV-K5.
2. Long-press **F2** to enter APRS/iGate mode.
3. The selected VFO is used and Dual Watch is disabled while APRS mode is active.
4. The ESP32 handles Wi-Fi, APRS-IS connectivity, filtering, and the web dashboard.

For **REV1AB Standalone**, the dedicated iGate runtime starts automatically after power-on.

### Latest Standalone-only APRS beacon controls

Both **DIGI REV1AB Standalone** and **iGATE REV1AB Standalone** provide menu controls for:

- `BType` — Station / Object beacon
- `ObjNam` — editable APRS Object Name, maximum 9 characters
- `BComnt` — editable user comment suffix
- `SymTbl` — APRS symbol table (`/` or `\`)
- `Symbol` — editable APRS symbol code

The identity prefix is mandatory and cannot be removed:

- DIGI always transmits `NUSA DIGI` before the user comment
- iGATE always transmits `NUSA IGATE` before the user comment

These Standalone builds are **build verified, packed-CRC verified, and the Fix1 EEPROM persistence changes have been tested successfully on real UV-K5 hardware**.

## iGate traffic behavior

### RF → APRS-IS

RF packets decoded by the UV-K5 are sent to the ESP32 and then gated to APRS-IS with duplicate and loop protection.

This path has been **field-tested successfully** on REV1A Normal.

### APRS-IS → RF

The return path is intentionally restricted.

An APRS-IS message may be sent to RF only when its target has been heard **directly on RF within the last 30 minutes**, using exact callsign + SSID matching and additional anti-loop checks.

This path is implemented and is pending full on-air field validation.

## iGate validation status

- REV1U Normal UV-K5 firmware: compile verified, packed CRC verified, and based on the field-tested REV1T RX engine
- Corrected UV-K5 ↔ ESP32 UART interface: **field-tested successfully**
- REV1A Normal RF → ESP32 → APRS-IS: **field-tested successfully**
- REV1U Standalone: compile/CRC verified; Fix1 EEPROM persistence and REV1U LED behavior tested successfully on real hardware
- IS → RF APRS message path: implemented; on-air field validation pending
- ESP32-C3 package: compile verified; hardware field test pending

## iGate documentation

- **[English README](./UV-K5%20IGATE/README.md)**
- **[README Bahasa Indonesia](./UV-K5%20IGATE/README_ID.md)**
- **[Flashing instructions](./UV-K5%20IGATE/FLASHING.md)**
- **[Connection diagram](./UV-K5%20IGATE/Connection%20Diagram.png)**

**Release:**

### [Download iGATE Normal + Standalone REV1AB →](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1ab)

---

## Releases

- **Latest — REV1AB family (DIGI + iGATE, Normal + Standalone):** [rev1ab](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1ab)
- **Superseded — REV1U:** [rev1u](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1u)
- **Digipeater Normal REV1G Fix1:** [rev1g-fix1](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1g-fix1)
- **Digipeater Standalone REV1H Fix1:** [rev1h-standalone-fix1](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/rev1h-standalone-fix1)
- **iGate Normal REV1A Fix1:** [igate-rev1a-fix1](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/igate-rev1a-fix1)
- **iGate Standalone REV1B Fix1:** [igate-rev1b-standalone-fix1](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/igate-rev1b-standalone-fix1)

## License

See the repository-level **LICENSE** file. Project-specific attribution and documentation are stored inside each project folder.
