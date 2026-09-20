# NUSA UV-K5 APRS Digipeater

Custom APRS digipeater firmware for the **Quansheng UV-K5**, developed and field-tested for Bell 202 / AX.25 APRS operation.

This first public release contains two firmware variants:

1. **REV1G — Normal APRS Digipeater**
2. **REV1G Standalone — Dedicated Autostart APRS Digipeater**

> **Important:** Flash only the `.packed.bin` file intended for the UV-K5 firmware updater. Keep a known-good firmware available for recovery before experimenting with custom firmware.

---

## Firmware Variants

| Feature | REV1G Normal | REV1G Standalone |
|---|---:|---:|
| APRS digipeater | Yes | Yes |
| Bell 202 / AX.25 | Yes | Yes |
| WIDE2 n-N | Yes | Yes |
| SoundModem-tested TX | Yes | Same proven REV1G engine |
| Manual position beacon | Yes | Yes |
| Automatic position beacon | Yes | Yes |
| Enter APRS manually | Yes | No |
| APRS autostart after reboot | No | Yes |
| Dedicated single-frequency operation | No | Yes |
| Keypad APRS frequency setting | No | Yes |
| VFO B used during APRS runtime | Possible in normal radio operation | No |
| Best use | General testing / flexible HT use | Permanent unattended digi |

Recommended firmware filenames:

```text
firmware/
├── NUSA_UVK5_APRS_DIGI_REV1G.packed.bin
└── NUSA_UVK5_APRS_DIGI_REV1G_STANDALONE.packed.bin
```

---

# REV1G Normal

Firmware:

```text
NUSA_UVK5_APRS_DIGI_REV1G.packed.bin
```

REV1G is the **stable RF baseline**.

Field testing confirmed:

- Bell 202 AFSK with approximately **1200 Hz / 2200 Hz** tones
- APRS at 1200 baud
- SoundModem successfully decodes transmitted packets
- APRS packets from a UV-5RH are received and digipeated
- WIDE2 n-N path rewriting works correctly
- Fixed/manual position beacon works
- Hardware-timer-based TX bit pacing is used

Example received packet:

```text
YF9UAG-7>APZUAG,WIDE2-2:...
```

Example after digipeating by `YF9UAG-3`:

```text
YF9UAG-7>APZUAG,YF9UAG-3*,WIDE2-1:...
```

## Entering APRS Mode

Configure Long F2 as APRS if required by the current EEPROM settings, then:

```text
Long F2
```

opens the APRS screen and starts the APRS modem.

`EXIT` leaves the APRS screen.

## REV1G APRS Menu

```text
DgPeat
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

Recommended starting settings:

```text
DgPeat = n-N
DgDly  = 900 ms
DgTail = 60 ms
```

WIDE operation is intentionally fixed to **WIDE2**.

### Callsign example

For:

```text
YF9UAG-3
```

set:

```text
DgCall = YF9UAG
DgSSID = 3
```

---

# REV1G Standalone

Firmware:

```text
NUSA_UVK5_APRS_DIGI_REV1G_STANDALONE.packed.bin
```

This build is intended to turn the UV-K5 into a **dedicated APRS digipeater appliance**.

After power-on or reset:

```text
POWER ON
   ↓
Load stored configuration
   ↓
Force VFO A
   ↓
Dual Watch OFF
Cross Band OFF
   ↓
Force n-N / WIDE2
   ↓
Start APRS automatically
   ↓
DIGIPEATER ACTIVE
```

No Long-F2 action is required.

The proven REV1G Bell 202 / AX.25 / hardware timer TX engine is intentionally kept unchanged.

## Standalone Menu

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

`DgPeat` is not shown because standalone mode always uses:

```text
n-N + WIDE2
```

## Setting APRS Frequency

Open:

```text
MENU → APRFq
```

Enter the frequency as **8 digits in 10 Hz units**.

Examples:

```text
14439000 = 144.39000 MHz
14480000 = 144.80000 MHz
14464000 = 144.64000 MHz
```

Press:

```text
MENU
```

to save.

While entering the frequency:

```text
EXIT = backspace
```

The stored APRS frequency is restored after a power-cycle.

On the first upgrade boot, if no dedicated `APRFq` value exists yet, the firmware uses the existing **VFO A frequency as the initial value**. It is still recommended to open `APRFq` and explicitly save the desired APRS frequency.

## Single-VFO Behaviour

Standalone uses only VFO A for APRS runtime:

```text
TX VFO      = A
RX VFO      = A
Dual Watch  = OFF
Cross Band  = OFF
```

VFO B remains internally allocated by the firmware architecture, but it is not selected for APRS runtime operation.

RX and TX use the same APRS frequency.

---

# Fixed Position Beacon

Both variants support a fixed/manual APRS position beacon.

## Automatic beacon

Set:

```text
PosBcn = ON
```

Current interval:

```text
10 minutes
```

## Manual beacon

On the APRS screen:

```text
*
```

requests a manual position beacon.

Manual beacon also works when:

```text
PosBcn = OFF
```

## Coordinate format

Coordinates are entered as decimal degrees multiplied by 10,000.

Example:

```text
Latitude  = 2.5515 South
Longitude = 140.7102 East
```

Configure:

```text
BLat  = 25515
LatNS = S
BLon  = 1407102
LonEW = E
```

The firmware converts the coordinates to APRS `DDMM.mm` / `DDDMM.mm` format.

## Beacon format

```text
Destination : APRS
Path        : WIDE2-1
Symbol      : #
Comment     : NUSA DIGI
```

Example:

```text
YF9UAG-3>APRS,WIDE2-1:!0233.09S/14042.61E#NUSA DIGI
```

---

# WIDE2 n-N Digipeating

Example received packet:

```text
YF9UAG-7>APZUAG,WIDE2-2:...
```

After digipeating by `YF9UAG-3`:

```text
YF9UAG-7>APZUAG,YF9UAG-3*,WIDE2-1:...
```

Duplicate suppression is approximately:

```text
20 seconds
```

---

# REV1G Bell 202 TX Engine

REV1G uses a timer-paced tone-generator method for APRS TX.

Important characteristics:

```text
Modulation : Bell 202 AFSK
Tones      : 1200 / 2200 Hz
Baud       : 1200
TX timing  : hardware TIMER_BASE0
Timer load : 820
Tone gain  : 100
```

The AX.25/HDLC writer uses:

- LSB-first HDLC byte transmission
- bit stuffing after five consecutive `1` bits
- NRZI encoding
- AX.25 FCS/CRC

Real RF testing confirmed successful SoundModem decoding after this hardware-timer TX method was introduced.

Earlier experimental builds generated the correct tone pair but had timing that was not stable enough for normal APRS decoders. REV1G therefore should be treated as the **stable RF baseline** for future development.

---

# LED Behaviour

During APRS operation:

```text
Green LED = APRS RX / listening activity
Red LED   = APRS TX
```

---

# Flashing / Cara Flash

## English

The firmware files provided in this repository use the `.packed.bin` format and require a **Quansheng UV-K5 compatible firmware updater**.

Before flashing:

- Use a reliable USB programming cable.
- Make sure the radio battery has sufficient charge.
- Use a firmware updater that supports Quansheng UV-K5 `.packed.bin` files.
- Keep a known-good firmware available for recovery.

Choose the firmware you want to install:

```text
NUSA_UVK5_APRS_DIGI_REV1G.packed.bin
NUSA_UVK5_APRS_DIGI_REV1G_STANDALONE.packed.bin
```

Follow the instructions provided by the UV-K5 firmware updater you choose.

> **Note:** Firmware flashing/updater software is not developed or maintained as part of the NUSA UV-K5 APRS DIGI project. This repository intentionally does **not** redistribute third-party updater installers. Obtain the updater from the original vendor or another trusted distribution source. Quansheng support: https://en.qsfj.com/support/downloads/3268

Flash the:

```text
*.packed.bin
```

file.

Do **not** flash a `.raw.bin` file unless you specifically know why it is needed.

Do not disconnect the programming cable, turn the radio off, or remove power while firmware is being written.

### Recommended first setup for REV1G Standalone

```text
APRFq  = local APRS frequency
DgCall = YOURCALL
DgSSID = desired SSID
DgDly  = 900 ms
DgTail = 60 ms
PosBcn = OFF
```

Then:

1. Power-cycle the UV-K5.
2. Do not press any APRS activation key.
3. Confirm that APRS starts automatically.
4. Transmit from another APRS station using `WIDE2-1` or `WIDE2-2`.
5. Monitor the output with SoundModem, Dire Wolf, another APRS radio/TNC, or an SDR receiver.

---

## Bahasa Indonesia

File firmware pada repository ini menggunakan format `.packed.bin` dan membutuhkan **software firmware updater yang kompatibel dengan Quansheng UV-K5**.

Sebelum melakukan flashing:

- Gunakan kabel programming USB yang baik dan stabil.
- Pastikan baterai radio memiliki daya yang cukup.
- Gunakan firmware updater yang mendukung file `.packed.bin` Quansheng UV-K5.
- Simpan firmware yang diketahui bekerja dengan baik untuk keperluan recovery.

Pilih firmware yang ingin dipasang:

```text
NUSA_UVK5_APRS_DIGI_REV1G.packed.bin
NUSA_UVK5_APRS_DIGI_REV1G_STANDALONE.packed.bin
```

Ikuti prosedur yang diberikan oleh software firmware updater UV-K5 yang digunakan.

> **Catatan:** Software flashing/updater bukan dikembangkan atau dipelihara sebagai bagian dari proyek NUSA UV-K5 APRS DIGI. Repository ini sengaja **tidak mendistribusikan installer updater pihak ketiga**. Ambil updater dari vendor asli atau sumber lain yang benar-benar terpercaya. Dukungan Quansheng: https://en.qsfj.com/support/downloads/3268

Untuk flashing gunakan file:

```text
*.packed.bin
```

Jangan flash file `.raw.bin` kecuali benar-benar memahami kegunaannya.

Jangan mencabut kabel programming, mematikan radio, atau memutus daya selama proses penulisan firmware sedang berlangsung.

### Konfigurasi awal yang disarankan untuk REV1G Standalone

```text
APRFq  = frekuensi APRS setempat
DgCall = CALLSIGN
DgSSID = SSID yang diinginkan
DgDly  = 900 ms
DgTail = 60 ms
PosBcn = OFF
```

Setelah itu:

1. Matikan lalu hidupkan kembali UV-K5.
2. Jangan menekan tombol aktivasi APRS.
3. Pastikan APRS aktif otomatis.
4. Kirim packet dari stasiun APRS lain menggunakan `WIDE2-1` atau `WIDE2-2`.
5. Pantau hasilnya menggunakan SoundModem, Dire Wolf, radio/TNC APRS lain, atau SDR receiver.

---

# Known Design Choices

The standalone firmware intentionally prioritizes unattended APRS operation over normal HT features.

Therefore:

- APRS autostarts after restart
- VFO switching is not part of normal runtime use
- VFO B is not used during APRS operation
- voice PTT operation is intentionally restricted
- the unit should be treated as a dedicated APRS device

If normal handheld-radio flexibility is required, use **REV1G Normal** instead.

---

# RF and Regulatory Notice

Amateur-radio frequencies, power limits, identification requirements, and unattended digipeater rules vary by country and licensing authority.

The operator is responsible for:

- using a permitted frequency
- using an authorized callsign/SSID
- complying with local amateur-radio regulations
- preventing harmful interference
- selecting an appropriate TX power and duty cycle

---

# Hardware Warning

Custom firmware can make a radio unusable if an incompatible image is flashed or if power is lost during programming.

Before flashing:

- use a reliable programming cable
- use a sufficiently charged battery
- keep a recovery procedure and known-good firmware available

Use at your own risk.

---

# Upstream / Technical References

This project was developed from the UV-K5 open-source firmware ecosystem.

Relevant upstream/reference projects include:

- bg7nzl UV-K5 firmware  
  https://github.com/bg7nzl/uv-k5-v1-firmware

- KD8CEC / UV-K5 CEC firmware work  
  https://github.com/phdlee/uvk5cec

When publishing source code or derivative binaries, retain all applicable upstream copyright, attribution, and license notices.

---

# Credits

Development and field testing:

```text
NUSA UV-K5 APRS DIGI
YF9UAG
Indonesia
```

Thanks to the open-source UV-K5 and APRS communities whose work made experimentation on this platform possible.

---

# Release Summary

## REV1G

Stable RF baseline:

- Bell 202 1200/2200 Hz
- TIMER_BASE0 TX pacing
- SoundModem field-tested
- WIDE2 n-N digipeating
- duplicate suppression
- weak-signal APRS RX configuration
- manual and automatic fixed-position beacon
- configurable TX preamble and tail

## REV1G Standalone

Dedicated unattended digipeater variant:

- APRS autostart
- forced n-N / WIDE2
- VFO A only during runtime
- Dual Watch OFF
- Cross Band OFF
- keypad `APRFq`
- EEPROM-stored APRS frequency
- reduced APRS-only configuration menu

---

## Which One Should I Use?

Use **REV1G Normal** when you still want manual APRS entry and more normal HT flexibility.

Use **REV1G Standalone** when the UV-K5 will be installed as a dedicated single-frequency APRS digipeater.
