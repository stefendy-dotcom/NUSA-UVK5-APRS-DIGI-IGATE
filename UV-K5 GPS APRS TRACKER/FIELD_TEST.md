# REV1D field test — 13 October 2026

## Observed, on an actual UV-K5 V1

- REV1C with the GPS UART TX line connected: red TX LED blinked repeatedly, and the radio actually transmitted unexpectedly.
- Disconnecting **GPS TX only** (leaving ground connected): red LED stopped blinking and the unintended RF TX stopped.
- REV1D enables the GPS tracker-specific application PTT Guard: the operator confirmed that with GPS connected, the **red LED no longer blinked except during intentional APRS beacons**.
- GPS position was obtained; the operator configured callsign `YB3DE-3` and automatic interval **1 minute**, with the station **stationary at home**.
- The operator provided **35 APRS-IS received packet records** for the same coordinates (displayed times 19:33:31–20:22:29, October 13). Most adjacent received packets are ~60–64 seconds apart; there are also longer 2–4-minute gaps. This is a reception log, **not** an exhaustive RF TX log.
- Captured sample:

```text
YB3DE-3>APZUAG,WIDE2-2,qAR,YH3NPX-1:!0714.65S/11246.38E>000/000UV-K5 APRS
```

Interpretation:
- `qAR,YH3NPX-1` indicates the packet was gated to APRS-IS by iGate `YH3NPX-1`.
- `WIDE2-2` is unchanged in the reported APRS-IS path (consistent with direct reception rather than an explicitly marked RF digipeat).
- Position `0714.65S/11246.38E`; symbol `>` (vehicle); `000/000` course/speed.
- Raw packet payload and comment render correctly with `APZUAG`.

## Tests completed in software before this on-air verification

- Clean ARM GCC 14.2.1 build for DP32G030 V1; raw flash image **60,488 B**, within 61,440-B build limit.
- Packed binary **60,506 B**, independently verified obfuscation and XMODEM CRC against the upstream packer.
- Native NMEA RMC/GGA, invalid/stale fixes, coordinates, speed/heading, SmartBeaconing arithmetic: **PASS**.
- Native auto-baud scanning 9600 / 38400 / 115200, checksum lock, timeout and counter rollover: **PASS**.
- Static verification: application PTT physical polling compiled out for GPS builds; startup PTT checks bypassed; software APRS RF-keying path retained.

## Not yet proven

- SmartBeaconing on moving vehicles, turning/corner pegging, and 30-minute stationary Smart interval.
- Every GPS M10/M8/NEO-6 module and every factory baud configuration.
- Long-duration RF operation in changing battery, temperature and GPS quality.
- Compatibility with all third-party Bell 202 hardware TNCs.
- Electrical safety of every jack/cable configuration; exact cable design needs board-specific confirmation.
- Automatic boot into tracker mode **while GPS is already powered**; the early bootloader can still see pulses before REV1D runs.
- Ordinary voice PTT is deliberately **disabled**.

**Status:** field-tested for stationary fixed-interval APRS RF→IS beacon and unintended-TX suppression on reported hardware; other use cases remain experimental.
