# NUSA UV-K5 TNC KISS REV1A — Community Test Guide

> This firmware is currently **UNTESTED on real hardware end-to-end**. The goal of this guide is to collect reproducible results.

## Minimum test setup

- Quansheng UV-K5 V1
- REV1A Normal or Standalone firmware
- verified UART connection between UV-K5 and host/interface
- PC, Raspberry Pi, or other KISS-capable host
- second APRS/packet radio or known-good TNC for over-the-air verification
- legal packet/APRS frequency for your region

## Serial settings

```text
38400 baud
8N1
No hardware flow control
KISS port 0
```

## Test A — Enter TNC mode

Normal:

1. boot normally;
2. Long F2;
3. confirm the screen shows `NUSA TNC KISS` / `KISS READY`.

Standalone:

1. power on;
2. confirm it enters TNC mode automatically.

Record whether the radio remains stable for at least 5 minutes.

## Test B — RF receive to KISS host

1. Start the host application in serial KISS mode.
2. Transmit a known-good APRS packet from another radio/TNC.
3. Confirm the UV-K5 receives/decodes it.
4. Confirm the host receives a KISS DATA frame.
5. Confirm the host decodes the correct source/destination/path/info.

Expected KISS form:

```text
C0 00 [AX.25 bytes without FCS] C0
```

The FCS must not appear in the host payload.

## Test C — KISS host to RF transmit

1. Make the host send one short APRS UI packet through KISS.
2. Confirm UV-K5 PTT/TX occurs.
3. Decode the RF transmission using a known-good receiver/TNC.
4. Check source, destination, path, info, and FCS.

## Test D — Escaping

Send a test AX.25 payload containing byte values that require KISS escaping if your host tool permits binary payload testing:

- `C0` must be represented over KISS as `DB DC`
- `DB` must be represented over KISS as `DB DD`

Confirm the RF AX.25 data contains the original bytes after unescaping.

## Test E — KISS parameters

Try changing:

- TXDELAY
- PERSIST
- SLOTTIME
- TXTAIL
- FULLDUPLEX

Confirm there is no crash/reboot and that timing changes are observable where practical.

## Test F — Repeated packets

Send and receive multiple packets for at least 10–15 minutes.

Check for:

- missed frames;
- duplicate frames caused by the TNC itself;
- UART lockup;
- radio reboot;
- stuck PTT;
- stuck RX LED;
- failure to return to RX after TX.

## Suggested Linux/Raspberry Pi check

If using Linux AX.25 tools, configure the serial device as a KISS TNC with the appropriate `kissattach` / `kissparms` workflow for your distribution. Device names and package setup vary by Linux distribution.

Do not assume an example device such as `/dev/ttyUSB0`; use the actual serial device assigned by your system.

## Report template

Open a GitHub Issue with:

```text
Hardware:
UV-K5 board/version:
Firmware: Normal / Standalone
Firmware file:
Host:
Operating system:
UART interface:
Host software:
Frequency:

Enter KISS mode: PASS/FAIL
RF -> KISS RX: PASS/FAIL
KISS -> RF TX: PASS/FAIL
TXDELAY: PASS/FAIL/NOT TESTED
PERSIST/SLOTTIME: PASS/FAIL/NOT TESTED
TXTAIL: PASS/FAIL/NOT TESTED
Long-duration test: PASS/FAIL/NOT TESTED

Observed packet:
Expected packet:

Notes:
Logs/screenshots:
```

Please include failures. A repeatable failure is especially valuable because it helps isolate firmware, UART, RF modem, or host-software problems.
