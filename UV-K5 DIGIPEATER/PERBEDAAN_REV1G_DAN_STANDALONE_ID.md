# Perbedaan REV1G dan REV1G Standalone

Dokumen ini menjelaskan perbedaan antara dua firmware awal NUSA UV-K5
APRS DIGI.

---

## 1. REV1G Normal

REV1G adalah **baseline stabil RF** proyek ini.

Firmware:

```text
NUSA_UVK5_APRS_DIGI_REV1G.packed.bin
```

REV1G cocok untuk pengguna yang masih ingin mempertahankan fleksibilitas
UV-K5 dan masuk ke mode APRS secara manual.

### Cara masuk APRS

APRS tidak otomatis aktif setelah boot.

Masuk ke APRS dengan:

```text
Long F2
```

jika fungsi F2 Long sudah diset ke APRS.

Keluar dari APRS:

```text
EXIT
```

### Fungsi utama REV1G

- APRS Bell 202 1200 baud
- tone 1200 Hz / 2200 Hz
- AX.25 / HDLC
- TX bit timing menggunakan hardware `TIMER_BASE0`
- APRS digipeater
- WIDE2 n-N
- duplicate suppression sekitar 20 detik
- fixed/manual position beacon
- `DgDly`
- `DgTail`
- weak-signal APRS RX configuration
- manual beacon dengan tombol `*`

### Status pengujian REV1G

REV1G telah berhasil diuji di RF nyata:

- packet TX berhasil didecode SoundModem,
- packet APRS dari UV-5RH berhasil diterima,
- packet UV-5RH berhasil didigipeat,
- path `WIDE2-2` berhasil diubah menjadi path n-N yang benar.

Contoh:

```text
RX:
YF9UAG-7>APZUAG,WIDE2-2:...

DIGI TX:
YF9UAG-7>APZUAG,YF9UAG-3*,WIDE2-1:...
```

Karena itu REV1G diperlakukan sebagai **engine RF baseline** untuk
pengembangan berikutnya.

---

## 2. REV1G Standalone

REV1G Standalone dibuat untuk menjadikan UV-K5 sebagai **perangkat APRS
digipeater permanen**, bukan sebagai HT normal.

Firmware:

```text
NUSA_UVK5_APRS_DIGI_REV1G_STANDALONE.packed.bin
```

Engine Bell 202 / AX.25 / hardware timer berasal dari REV1G dan sengaja
dipertahankan.

### Autostart

Setelah radio hidup atau restart:

```text
POWER ON
   ↓
Load konfigurasi
   ↓
Pilih VFO A
   ↓
Dual Watch OFF
Cross Band OFF
   ↓
n-N / WIDE2
   ↓
APRS aktif otomatis
   ↓
DIGIPEATER ACTIVE
```

Tidak perlu lagi Long F2.

Ini penting untuk lokasi digipeater unattended. Jika baterai drop,
power terputus, atau radio restart, perangkat kembali ke APRS secara otomatis.

### Single VFO

Standalone hanya menggunakan:

```text
RX VFO = A
TX VFO = A
```

dan mematikan:

```text
Dual Watch
Cross Band
```

VFO B masih ada di struktur internal firmware agar arsitektur firmware
tetap aman, tetapi **tidak dipilih untuk operasi APRS**.

### Frekuensi APRS langsung dari keypad

Standalone menambahkan menu:

```text
APRFq
```

Contoh:

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

Nilai `APRFq` disimpan di EEPROM dan otomatis digunakan lagi setelah restart.

### Menu Standalone

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

`DgPeat` tidak ditampilkan karena Standalone selalu dipaksa:

```text
n-N + WIDE2
```

### Tombol saat APRS berjalan

```text
MENU  = konfigurasi
*     = manual beacon
EXIT  = tidak mematikan APRS
PTT   = tidak digunakan untuk voice TX
```

Fungsi HT biasa sengaja dibatasi.

---

# Tabel Perbandingan

| Fitur | REV1G Normal | REV1G Standalone (Legacy) | REV1H Standalone (Terbaru) |
|---|---|---|---|
| APRS Digipeater | Ya | Ya | Ya |
| Bell 202 1200/2200 | Ya | Ya, engine REV1G | Ya, engine REV1G |
| AX.25 / HDLC | Ya | Ya | Ya |
| Hardware TIMER_BASE0 TX | Ya | Ya | Ya |
| WIDE2 n-N | Ya | Selalu aktif | Selalu aktif |
| Duplicate suppression | Ya | Ya | Ya |
| Position beacon | Ya | Ya | Ya |
| Tipe Beacon | Station saja | Station saja | Station atau APRS Object (`BType`) |
| Object Name Editable | Tidak | Tidak | Ya, 9 karakter (`ObjNam`) |
| Suffix Comment Editable | Tidak | Tidak | Ya, 16 karakter (`BComnt`) |
| Prefix Comment TX | `NUSA DIGI` | `NUSA DIGI` | `NUSA DIGI` (wajib) |
| Symbol APRS Editable | Tidak | Tidak | Ya, Tabel + Kode (`SymTbl`, `Symbol`) |
| Persistence EEPROM | Fix1 2-page | 1 page (terpotong) | Fix1 5-page CRC16 NAP3 |
| Manual beacon `*` | Ya | Ya | Ya |
| APRS otomatis setelah boot | Tidak | Ya | Ya |
| Perlu Long F2 | Ya | Tidak | Tidak |
| Bisa keluar APRS dengan EXIT | Ya | Tidak | Tidak |
| VFO A/B normal | Masih tersedia | Hanya VFO A untuk APRS | Hanya VFO A untuk APRS |
| Dual Watch | Mengikuti radio | OFF | OFF |
| Cross Band | Mengikuti radio | OFF | OFF |
| Menu `APRFq` | Tidak | Ya | Ya |
| Frekuensi APRS tersimpan khusus | Tidak | Ya | Ya |
| Voice HT normal | Masih lebih fleksibel | Sengaja dibatasi | Sengaja dibatasi |
| Tujuan utama | HT + APRS digi | Dedicated APRS digi | Appliance dedicated APRS digi |
| Cocok untuk unattended site | Bisa | Dirancang khusus | Dirancang khusus |

---

# Pilih yang Mana?

Gunakan **REV1G Normal** jika:

- UV-K5 masih ingin dipakai sebagai HT fleksibel,
- APRS hanya digunakan saat diperlukan,
- ingin bisa masuk/keluar mode APRS secara manual.

Gunakan **REV1H Standalone** jika:

- radio akan dipasang permanen sebagai digipeater tanpa pengawasan,
- setelah power-cycle/listrik padam, radio harus otomatis kembali aktif sebagai digipeater,
- ingin mengatur Object Name, Symbol kustom, dan suffix komentar,
- hanya membutuhkan satu frekuensi APRS khusus,
- ingin mengubah frekuensi APRS langsung dari keypad (`APRFq`).

*(REV1G Standalone disimpan sebagai referensi lama; seluruh instalasi baru disarankan menggunakan REV1H Standalone Fix1).*

---

# Catatan RF

Standalone bukan engine APRS baru.

Arsitekturnya adalah:

```text
REV1G proven RF engine
        +
autostart
        +
single VFO A
        +
APRFq EEPROM menu
        +
Kontrol Object / Symbol / Comment (REV1H)
        +
Fix1 5-page CRC16 NAP3 EEPROM Persistence
```

Karena itu perubahan standalone sengaja tidak mengubah bagian RF REV1G
yang sudah berhasil diuji menggunakan SoundModem dan UV-5RH.
