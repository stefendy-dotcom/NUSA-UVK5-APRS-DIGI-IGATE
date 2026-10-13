# NUSA UV-K5 GPS APRS Tracker — REV1D

**Standalone APRS position tracker for the Quansheng UV-K5 / K6 Board V1 (DP32G030 + BK4819), using an external GPS/GNSS module.**

> **FIELD-TESTED FOR STATIONARY 1-MINUTE BEACONS (13 October 2026).** REV1D successfully sent RF position packets into APRS-IS, and the GPS serial input no longer caused unintended RF TX in the reported hardware test. This is **not** a claim that SmartBeaconing in motion, all GPS hardware, or long-term reliability have been validated. Not for UV-K5 Board V3/PY32F071.

## Download counter

[![GPS Tracker Downloads](https://img.shields.io/endpoint?url=https%3A%2F%2Fraw.githubusercontent.com%2Fstefendy-dotcom%2FNUSA-UVK5-APRS-DIGI-IGATE%2Fmain%2Fbadges%2Fgps-tracker.json&style=for-the-badge&logo=github)](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/gps-tracker-rev1d)

> The counter tracks **cumulative GitHub Release downloads of GPS Tracker firmware BIN files**, not source ZIPs, raw.githubusercontent.com, or regular GitHub repository file views. The workflow refreshes badges periodically.

## Download

- **Firmware (counted):** [Download GPS Tracker REV1D from GitHub Release](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/download/gps-tracker-rev1d/NUSA_UVK5_GPS_APRS_TRACKER_REV1D_PTT_GUARD_PRETEST.packed.bin) — packed UV-K5 flasher file, **60,506 bytes**.
- **GitHub Release page:** [GPS Tracker REV1D — field-tested fixed 1-minute beacon](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/gps-tracker-rev1d).
- **Repository firmware mirror (not counted):** [same REV1D BIN](firmware/NUSA_UVK5_GPS_APRS_TRACKER_REV1D_PTT_GUARD_PRETEST.packed.bin).
- **Complete source and tests:** [REV1D source ZIP](source/NUSA_UVK5_GPS_APRS_TRACKER_REV1D_PTT_GUARD_PRETEST_SOURCE.zip)
- [Browsable source patch](source/nusa_uvk5_gps_rev1d.patch), [GPS NMEA parser](source/app/nusa_gps_core.c), [GPS auto-baud](source/app/nusa_gps_autobaud.c), and [build instructions](SOURCE_BUILD.md)
- [Recorded on-air test and known limitations](FIELD_TEST.md)
- [SHA-256 hashes](SHA256SUMS.txt)

**REV1D keeps the exact packed binary from the successful hardware test; only its GitHub presentation and documentation have been updated.**

## Features

- Standalone GPS APRS position beacon, **no ESP32 or RP2040** between the GPS and UV-K5.
- GPS UART NMEA 0183 **8N1 auto-baud: 9600, 38400, 115200**. Scans until two checksum-valid NMEA sentences lock the detected rate; re-scans if valid input is absent for 30 seconds.
- RMC positioning, GGA satellite count, compatible GP/GN NMEA talkers. UART **TTL 3.3 V-compatible** signals required.
- **Interval Beacon** configurable **1–60 minutes** (field-tested at 1 minute).
- **SmartBeaconing** (`Intv = SMART`): speed-dependent interval plus corner pegging (**not yet field-tested**).
- Manual TX: **MENU → BEACON → MENU**; requires valid callsign and a fresh GPS position.
- Callsign editable by UP/DOWN, short MENU advances cursor, **long MENU saves**, SSID configured separately.
- APRS destination `APZUAG`, path `WIDE2-2`, car symbol `>`.
- REV1D **PTT Guard** blocks unintended TX due to the shared GPS-UART/PTT jack signal. The ordinary **physical voice PTT is disabled** in this tracker firmware; RF APRS beacons are keyed independently by software.

## Important radio safety and electrical limitations

**This is a dedicated tracker image, not a general voice-HT firmware. Pressing the radio's physical PTT will not transmit voice on REV1D.** Reflash suitable radio firmware to restore normal voice PTT behavior.

The radio's external serial input and external PTT detection share connector circuitry. Earlier REV1C caused repeated uncommanded RF transmission when NMEA TX was connected; removing GPS TX stopped it. REV1D suppresses *application* PTT polling and successfully stopped the uncommanded RF in the reported hardware test.

**The bootloader operates before REV1D and is not covered by PTT Guard.** Turn on the radio with the external GPS module powered **off**; power the GPS only after the radio reaches its normal application screen. Disconnect GPS when flashing. Do **not** use live hot-plugging of the connector. Verify the physical jack pinout, module power and 3.3 V TTL levels for the actual device before connecting it. Do not feed the GPS from the radio connector. Do not use RS-232 ±12 V on UART pins.

For safe initial tests, use a correctly rated **50-ohm dummy load**, not an open RF output. Set `APRS=OFF` and watch for unwanted TX first; note that **manual BEACON still transmits** when invoked with valid GPS/callsign even when automatic scheduling is off.

## Getting started / Cara pakai

1. Confirm the radio is **UV-K5/K6 V1 DP32G030 + BK4819**, and keep a compatible recovery image. Flash the packed REV1D BIN with the GPS disconnected.
2. Wire the externally powered GPS/GNSS UART TX and a common ground through a **verified** compatible interface. GPS serial output must provide NMEA RMC/GGA on one of the three supported speeds.
3. Radio on first; **GPS power on after radio boot**. In MENU → `Loc` (display mode, not editor), wait for a valid position.
4. Set `Call` and `SSID`, then the legal local APRS frequency and desired comment. Callsign is not auto-generated.
5. Select `Intv = 1 min` for initial test, or `SMART` only for later SmartBeaconing testing. Enable `APRS=ON` for **automatic** beaconing.
6. For one deliberate manual beacon, select `BEACON` and press `MENU`.
7. Confirm with a nearby SDR/TNC or iGate; check the actual RF packets and check that there is no transmission between scheduled beacons.

## Validated on-air result

The operator reported **35 APRS-IS observations** for `YB3DE-3` on 13 Oct 2026, from the iGate `YH3NPX-1`, with `Intv = 1 min` and the tracker stationary:

```text
YB3DE-3>APZUAG,WIDE2-2,qAR,YH3NPX-1:!0714.65S/11246.38E>000/000UV-K5 APRS
```

Most recorded reception intervals were approximately 60–64 seconds. Longer gaps in *received* APRS-IS records do not establish whether frames were missed over RF or not sent; only an RF-side log can determine that. The operator separately confirmed: **the red TX LED no longer flashes except when an APRS beacon is transmitted**.

**Caution:** the underlying TX implementation documents FFSK tones around **1200/1800 Hz**, not guaranteed 1200/2200 Hz Bell 202 interoperability with every hardware TNC. Successful RF→IS decoding at one iGate is confirmed, but broad modem compatibility remains untested.

## Firmware provenance / licensing

Derived from [bcanata/uv-k5-firmware-ta1js](https://github.com/bcanata/uv-k5-firmware-ta1js), pinned to commit `6320cdac7a2dce46f88027d220ff67e47eb0e3fb`. The source ZIP and [source patch](source/nusa_uvk5_gps_rev1d.patch) include changes for GPS serial, display/menu, and PTT guard, plus standalone GPS parser/auto-baud files and native tests. Source retains upstream notices; see the repository [Apache-2.0 LICENSE](../LICENSE). No changes were made to the main Digipeater, iGATE, or KISS firmware.
