# NUSA UV-K5 V1 TNC KISS

> ⚠️ **EXPERIMENTAL / UNTESTED FIRMWARE**
>
> REV1A has been compile/link/pack verified, but it has **not yet been tested end-to-end on real UV-K5 V1 hardware with external KISS host software**.
> This release is intentionally public so UV-K5 V1 users can help test it. Please report both successful and failed tests, including host software, operating system, UART adapter/interface, radio frequency, and whether RX and TX worked.

## Purpose

NUSA UV-K5 TNC KISS turns a **Quansheng UV-K5 V1** into a Bell 202 / AX.25 1200-baud radio TNC controlled by external software through a **standard serial KISS interface**.

It is based on the APRS RX/TX modem engine used in the NUSA REV1AB firmware family.

The TNC firmware is deliberately different from the NUSA iGATE UART protocol:

- no `A5 5A 4E 55` proprietary framing;
- no TNC2 ASCII stream;
- no autonomous digipeating;
- no autonomous APRS-IS gating;
- no autonomous beaconing while KISS TNC mode is active.

The host application owns packet transmission.

## Firmware variants

### Normal

`NUSA_UVK5_TNC_KISS_REV1A_NORMAL.packed.bin`

- UV-K5 still behaves as a normal radio outside TNC mode.
- **Long F2** enters KISS TNC mode.
- **EXIT** leaves KISS TNC mode.
- While KISS mode is active, the UART is owned by the KISS parser instead of the normal CPS/CAT serial parser.

### Standalone

`NUSA_UVK5_TNC_KISS_REV1A_STANDALONE.packed.bin`

- Power-on automatically enters KISS TNC mode.
- Intended for a UV-K5 dedicated as a host-controlled packet/APRS modem.
- Uses VFO A / the stored APRS standalone frequency setting as its RF channel.
- The host controls packet TX through KISS.
- KISS RETURN resets the KISS session; the dedicated Standalone unit remains a TNC appliance.

## Serial interface

Default serial settings:

```text
38400 baud
8 data bits
No parity
1 stop bit
No hardware flow control
```

The DP32G030 UART driver uses the calibrated divider already used by the NUSA UV-K5 firmware:

```c
UART1->BAUD = Frequency / 39053U;
```

This is intended to produce the normal ~38400 baud wire rate after MCU RC-clock calibration.

Use a UART interface appropriate for the UV-K5 V1 programming/data connector. **Do not connect true ±RS-232 voltage levels directly to the radio or MCU UART.** Verify the connector pinout and logic-level interface used by your specific cable/hardware before connecting it.

## Standard KISS protocol

REV1A uses normal KISS framing:

```text
FEND  = C0
FESC  = DB
TFEND = DC
TFESC = DD
```

KISS command byte:

```text
high nibble = port number
low nibble  = command
```

Only physical **port 0** is implemented.

Implemented commands:

| Command | Code | Meaning |
|---|---:|---|
| DATA | `0x00` | Raw AX.25 frame |
| TXDELAY | `0x01` | PTT/TX lead delay, 10 ms/unit |
| PERSIST | `0x02` | p-persistence value 0–255 |
| SLOTTIME | `0x03` | CSMA slot time, 10 ms/unit |
| TXTAIL | `0x04` | TX tail, 10 ms/unit |
| FULLDUPLEX | `0x05` | 0 = half duplex, non-zero = full duplex |
| RETURN | `0xFF` | Return/reset KISS session behavior |

Unknown KISS commands are ignored.

## AX.25 and FCS behavior

### Host → UV-K5 → RF

The host sends a standard KISS DATA frame:

```text
C0 00 [raw AX.25 frame without FCS] C0
```

The UV-K5:

1. unescapes KISS data;
2. validates that the frame fits the internal APRS/TNC buffer;
3. computes the AX.25 FCS internally;
4. adds the FCS;
5. creates the HDLC/NRZI bitstream;
6. transmits Bell 202 AFSK over RF.

### RF → UV-K5 → Host

The UV-K5:

1. receives Bell 202 / HDLC from RF;
2. decodes NRZI/HDLC and bit stuffing;
3. verifies the received AX.25 FCS;
4. removes the two FCS bytes;
5. KISS-escapes the remaining raw AX.25 frame;
6. sends it to the host as:

```text
C0 00 [raw AX.25 frame without FCS] C0
```

This matches normal KISS TNC behavior: **FCS is handled inside the TNC and is not carried in KISS DATA.**

## Default KISS timing

Until changed by the host:

```text
TXDELAY   = 50  = 500 ms
PERSIST   = 63
SLOTTIME  = 10  = 100 ms
TXTAIL    = 3   = 30 ms
FULLDUPLEX= 0
```

The firmware implements a small transmit queue and p-persistence/slot-time channel access for half-duplex operation.

## APRS-oriented limits

REV1A is designed primarily for **APRS / AX.25 UI traffic at 1200 baud**, not as a full replacement for every historical packet-radio TNC feature.

Current KISS TX frame storage is optimized for typical APRS packet sizes. Very large generic AX.25 frames are not the primary target of this first revision.

## What is intentionally disabled in KISS mode

To keep the TNC predictable and compatible with host software, KISS mode does not autonomously:

- digipeat packets;
- send position beacons;
- gate RF packets to APRS-IS;
- gate APRS-IS packets to RF;
- insert NUSA proprietary UART headers.

The host application decides what packets are sent.

## Host compatibility goal

The interface is intended to work with software that supports a **serial KISS TNC** on Windows, Linux/Raspberry Pi, or other systems.

Examples of useful test environments include:

- Linux AX.25 tools such as `kissattach` / `kissparms`;
- APRS applications that can open a serial KISS TNC;
- custom host software that follows standard KISS framing.

Compatibility with a specific application must be confirmed by field testing; this REV1A release does not claim that every program has already been tested.

## First test procedure

See **[TESTING.md](./TESTING.md)**.

Recommended order:

1. flash the correct Normal or Standalone build;
2. select a legal APRS/packet frequency for your region;
3. connect the verified UART interface;
4. configure the host for serial KISS at 38400 8N1;
5. test **RF RX → KISS host** first;
6. test **KISS host → UV-K5 RF TX** second;
7. test TXDELAY/PERSIST/SLOTTIME/TXTAIL settings;
8. report results.

## Build verification

The published files were checked before release for:

- successful compile and link;
- NUSA firmware identity;
- KISS constants and KISS RX/TX code present in the build source;
- calibrated UART divider retained;
- packed firmware CRC;
- packed/deobfuscated firmware round-trip matching the raw build.

These checks verify the build artifact. They **do not replace real-hardware testing**.

## SHA256 reference

These values are documentation only; no separate checksum asset is published.

```text
NORMAL
b68bf4649d86d4cd06ddf363944a7834ac830be120f2e86c2def49db4829ad0e

STANDALONE
cba26650328280b06e8c1efe4ac256a9b55754290b5e86c23ecdcc57d02520a7
```

## Testing feedback requested

Please open a GitHub Issue and include:

- UV-K5 hardware version;
- Normal or Standalone firmware;
- PC / Raspberry Pi / other host;
- operating system;
- UART adapter/interface used;
- host application;
- RX result;
- TX result;
- KISS parameter result;
- any decoded packet examples or logs;
- photos/screenshots where useful.

Negative results are just as useful as successful tests.

## Warning

This is experimental firmware for UV-K5 V1. Flashing custom firmware always carries risk. Keep a known-good firmware image available so the radio can be restored if required.

Do not flash this build to a different hardware generation such as UV-K5 V3 unless a build is explicitly released for that hardware.
