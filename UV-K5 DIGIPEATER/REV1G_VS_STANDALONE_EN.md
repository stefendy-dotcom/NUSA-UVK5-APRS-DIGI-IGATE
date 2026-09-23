# REV1G vs REV1G Standalone

This document explains the differences between the first two NUSA UV-K5
APRS DIGI firmware variants.

---

## 1. REV1G Normal

REV1G is the project's **proven RF baseline**.

Firmware:

```text
NUSA_UVK5_APRS_DIGI_REV1G.packed.bin
```

REV1G is intended for users who still want normal UV-K5 flexibility and
prefer to enter APRS mode manually.

### Entering APRS

APRS does not automatically start after a normal boot.

Enter APRS using:

```text
Long F2
```

when the F2 Long function is configured for APRS.

Leave APRS using:

```text
EXIT
```

### Main REV1G features

- Bell 202 APRS at 1200 baud
- approximately 1200 Hz / 2200 Hz tones
- AX.25 / HDLC
- hardware `TIMER_BASE0` TX bit pacing
- APRS digipeater
- WIDE2 n-N
- duplicate suppression of approximately 20 seconds
- fixed/manual position beacon
- configurable `DgDly`
- configurable `DgTail`
- weak-signal APRS RX configuration
- manual beacon using `*`

### REV1G field-test status

REV1G has been confirmed in real RF testing:

- transmitted packets were decoded by SoundModem,
- APRS packets from a UV-5RH were received,
- UV-5RH packets were successfully digipeated,
- a `WIDE2-2` path was correctly rewritten using n-N rules.

Example:

```text
RX:
YF9UAG-7>APZUAG,WIDE2-2:...

DIGI TX:
YF9UAG-7>APZUAG,YF9UAG-3*,WIDE2-1:...
```

For this reason, REV1G is treated as the **stable RF engine baseline** for
later variants.

---

## 2. REV1G Standalone

REV1G Standalone is designed to turn a UV-K5 into a **dedicated permanent
APRS digipeater appliance** rather than a general-purpose handheld radio.

Firmware:

```text
NUSA_UVK5_APRS_DIGI_REV1G_STANDALONE.packed.bin
```

The Bell 202 / AX.25 / hardware timer engine from REV1G is intentionally
retained.

### Automatic startup

After power-on or reset:

```text
POWER ON
   ↓
Load configuration
   ↓
Select VFO A
   ↓
Dual Watch OFF
Cross Band OFF
   ↓
n-N / WIDE2
   ↓
Start APRS automatically
   ↓
DIGIPEATER ACTIVE
```

No Long-F2 action is required.

This behavior is intended for unattended installations. If power is lost,
the battery drops, or the radio resets, the unit returns to APRS operation
automatically.

### Single-VFO operation

Standalone uses only:

```text
RX VFO = A
TX VFO = A
```

and disables:

```text
Dual Watch
Cross Band
```

VFO B remains allocated internally by the firmware architecture for
structural safety, but it is **not selected for APRS runtime operation**.

### Direct APRS frequency entry

Standalone adds:

```text
APRFq
```

Examples:

```text
14439000 = 144.39000 MHz
14480000 = 144.80000 MHz
14464000 = 144.64000 MHz
```

Press `MENU` to save.

While entering digits:

```text
EXIT = backspace
```

The `APRFq` value is stored in EEPROM and restored after restart.

### Standalone menu

```text
TxPwr
APRFq
DgCall
DgSSID
DgDly
DgTail
PosBcn
BLat
LatNS
BLon
LonEW
```

`DgPeat` is not shown because Standalone always forces:

```text
n-N + WIDE2
```

### Keys during APRS operation

```text
MENU  = configuration
*     = manual beacon
EXIT  = cannot disable APRS
PTT   = not used for normal voice TX
```

Normal handheld-radio operation is intentionally restricted.

---

# Comparison Table

| Feature | REV1G Normal | REV1G Standalone (Legacy) | REV1H Standalone (Latest) |
|---|---|---|---|
| APRS digipeater | Yes | Yes | Yes |
| Bell 202 1200/2200 | Yes | Yes, REV1G engine | Yes, REV1G engine |
| AX.25 / HDLC | Yes | Yes | Yes |
| Hardware TIMER_BASE0 TX | Yes | Yes | Yes |
| WIDE2 n-N | Yes | Always enabled | Always enabled |
| Duplicate suppression | Yes | Yes | Yes |
| Position beacon | Yes | Yes | Yes |
| Beacon Type | Station only | Station only | Station or APRS Object (`BType`) |
| Editable Object Name | No | No | Yes, 9 characters (`ObjNam`) |
| Editable Comment Suffix | No | No | Yes, 16 characters (`BComnt`) |
| Transmitted Comment Prefix | `NUSA DIGI` | `NUSA DIGI` | `NUSA DIGI` (mandatory) |
| Editable APRS Symbol | No | No | Yes, Table + Code (`SymTbl`, `Symbol`) |
| EEPROM Persistence | Fix1 2-page | Single page (truncated) | Fix1 5-page CRC16 NAP3 |
| Manual `*` beacon | Yes | Yes | Yes |
| APRS starts after boot | No | Yes | Yes |
| Long F2 required | Yes | No | No |
| EXIT can leave APRS | Yes | No | No |
| Normal VFO A/B flexibility | Available | VFO A only for APRS | VFO A only for APRS |
| Dual Watch | Radio setting | OFF | OFF |
| Cross Band | Radio setting | OFF | OFF |
| `APRFq` menu | No | Yes | Yes |
| Dedicated stored APRS frequency | No | Yes | Yes |
| Normal voice-radio use | More flexible | Intentionally restricted | Intentionally restricted |
| Primary purpose | HT + APRS digi | Dedicated APRS digi | Dedicated APRS digi appliance |
| Unattended-site use | Possible | Specifically designed | Specifically designed |

---

# Which One Should I Use?

Use **REV1G Normal** when:

- the UV-K5 will still be used as a handheld radio,
- APRS is only needed occasionally,
- you want to enter and leave APRS manually.

Use **REV1H Standalone** when:

- the radio will be permanently installed as an unattended digipeater,
- APRS must recover automatically after a power interruption,
- you want custom APRS Object Name, custom symbols, and comment suffixes,
- only one APRS frequency is required,
- direct keypad APRS frequency configuration (`APRFq`) is desired.

*(REV1G Standalone is retained as legacy reference; all new standalone installations should use REV1H Standalone Fix1).*

---

# RF Design Note

Standalone is not a replacement APRS modem engine.

Its architecture is:

```text
REV1G proven RF engine
        +
autostart
        +
single VFO A
        +
APRFq EEPROM menu
        +
Object / Symbol / Comment Controls (REV1H)
        +
Fix1 5-page CRC16 NAP3 EEPROM Persistence
```

The standalone changes are intentionally kept outside the field-proven
REV1G Bell 202 / AX.25 / hardware timer RF path.
