# NUSA UV-K5 APRS Digipeater — Bahasa Indonesia

Firmware APRS digipeater untuk **Quansheng UV-K5**.

Rilis awal GitHub berisi dua firmware:

1. **REV1G Normal**
2. **REV1G Standalone Digipeater**

## REV1G Normal

Gunakan versi ini jika UV-K5 masih ingin dipakai lebih fleksibel dan APRS diaktifkan secara manual.

Firmware:

```text
NUSA_UVK5_APRS_DIGI_REV1G.packed.bin
```

Fitur penting:

- Bell 202 1200/2200 Hz
- APRS 1200 baud
- AX.25 / HDLC
- WIDE2 n-N digipeater
- duplicate suppression sekitar 20 detik
- beacon posisi otomatis dan manual
- `DgDly` dan `DgTail`
- TX hardware timer `TIMER_BASE0`
- sudah berhasil didecode SoundModem pada pengujian RF nyata
- sudah berhasil mendigi packet APRS dari UV-5RH

Konfigurasi dasar:

```text
DgPeat = n-N
DgCall = CALLSIGN tanpa SSID
DgSSID = SSID
DgDly  = 900 ms
DgTail = 60 ms
```

Untuk masuk APRS gunakan Long F2 jika action F2 Long sudah diatur ke APRS.

## REV1G Standalone

Gunakan versi ini jika UV-K5 akan dijadikan **APRS digipeater permanen**.

Firmware:

```text
NUSA_UVK5_APRS_DIGI_REV1G_STANDALONE.packed.bin
```

Setelah radio dinyalakan:

```text
POWER ON
   ↓
VFO A aktif
   ↓
Dual Watch OFF
Cross Band OFF
   ↓
n-N + WIDE2
   ↓
APRS aktif otomatis
```

Tidak perlu Long F2.

### Menu standalone

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

`DgPeat` tidak ditampilkan karena standalone selalu menggunakan:

```text
n-N + WIDE2
```

### Mengatur frekuensi

Buka:

```text
MENU → APRFq
```

Masukkan 8 digit:

```text
14439000 = 144.39000 MHz
14480000 = 144.80000 MHz
14464000 = 144.64000 MHz
```

Tekan `MENU` untuk menyimpan.

Saat mengetik:

```text
EXIT = backspace
```

Frekuensi disimpan di EEPROM dan dipakai kembali setelah radio mati/hidup.

### VFO Standalone

```text
TX VFO = A
RX VFO = A
Dual Watch = OFF
Cross Band = OFF
```

VFO B tetap ada dalam struktur internal firmware, tetapi tidak dipilih saat APRS bekerja.

## Beacon Posisi

Beacon otomatis:

```text
PosBcn = ON
```

Interval sekitar 10 menit.

Beacon manual:

```text
*
```

Beacon manual tetap dapat dikirim walaupun:

```text
PosBcn = OFF
```

Contoh posisi:

```text
2.5515 S
140.7102 E
```

diisi:

```text
BLat  = 25515
LatNS = S
BLon  = 1407102
LonEW = E
```

## Contoh Digipeat

Packet masuk:

```text
YF9UAG-7>APZUAG,WIDE2-2:...
```

setelah didigi oleh `YF9UAG-3`:

```text
YF9UAG-7>APZUAG,YF9UAG-3*,WIDE2-1:...
```

## Catatan Flashing

Gunakan file:

```text
*.packed.bin
```

Jangan flash `.raw.bin` kecuali benar-benar memahami penggunaannya.

REV1G adalah baseline RF yang sudah terbukti pada pengujian nyata. Pada standalone, engine Bell 202 / AX.25 / TIMER_BASE0 REV1G dipertahankan; perubahan utamanya ada pada autostart, menu frekuensi, penyimpanan EEPROM, dan penggunaan VFO A saja.

Operator tetap bertanggung jawab menggunakan frekuensi, callsign, power, dan operasi unattended sesuai peraturan radio amatir yang berlaku.
