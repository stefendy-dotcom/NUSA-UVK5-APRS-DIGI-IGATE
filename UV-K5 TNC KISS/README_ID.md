# NUSA UV-K5 V1 TNC KISS — Bahasa Indonesia

> ⚠️ **FIRMWARE EKSPERIMENTAL / BELUM DIUJI**
>
> REV1A sudah lolos compile, link, packing CRC, dan verifikasi file, tetapi **belum diuji end-to-end pada UV-K5 V1 nyata dengan software KISS eksternal**.
> Firmware ini sengaja dipublikasikan agar pengguna UV-K5 V1 dapat ikut menguji. Mohon laporkan hasil berhasil maupun gagal beserta software host, sistem operasi, interface UART, frekuensi, hasil RX dan hasil TX.

## Tujuan

Firmware ini mengubah **Quansheng UV-K5 V1** menjadi radio + modem TNC Bell 202 / AX.25 1200 baud yang dikendalikan software eksternal melalui **KISS serial standar**.

Basis modem RX/TX berasal dari keluarga firmware NUSA REV1AB.

KISS TNC ini tidak memakai protokol UART proprietary NUSA iGATE:

- tidak memakai header `A5 5A 4E 55`;
- tidak memakai stream TNC2 ASCII;
- tidak mendigipeat sendiri;
- tidak melakukan iGate sendiri;
- tidak beacon sendiri ketika KISS TNC aktif.

Software host menentukan packet yang akan dipancarkan.

## Varian firmware

### Normal

`NUSA_UVK5_TNC_KISS_REV1A_NORMAL.packed.bin`

- Di luar TNC mode, UV-K5 tetap dapat digunakan sebagai radio normal.
- **Long F2** masuk ke KISS TNC mode.
- **EXIT** keluar dari KISS TNC mode.
- Ketika KISS aktif, UART digunakan oleh parser KISS dan tidak dipakai parser CPS/CAT normal.

### Standalone

`NUSA_UVK5_TNC_KISS_REV1A_STANDALONE.packed.bin`

- Setelah power-on langsung masuk KISS TNC.
- Ditujukan untuk UV-K5 yang didedikasikan sebagai modem packet/APRS.
- Menggunakan VFO A / frekuensi APRS standalone yang tersimpan.
- Semua packet TX dikendalikan host melalui KISS.
- KISS RETURN mereset sesi KISS; unit Standalone tetap berfungsi sebagai perangkat TNC.

## UART

```text
38400 baud
8 data bit
No parity
1 stop bit
Tanpa hardware flow control
```

Driver DP32G030 tetap memakai pembagi UART terkalibrasi:

```c
UART1->BAUD = Frequency / 39053U;
```

Gunakan interface UART yang benar untuk konektor programming/data UV-K5 V1. **Jangan menghubungkan level tegangan RS-232 ± langsung ke UART radio/MCU.** Pastikan pinout konektor dan level logic kabel/interface yang digunakan sudah benar.

## KISS standar

```text
FEND  = C0
FESC  = DB
TFEND = DC
TFESC = DD
```

Command byte:

```text
nibble atas  = nomor port
nibble bawah = command
```

Firmware saat ini memakai **port 0**.

| Command | Code | Fungsi |
|---|---:|---|
| DATA | `0x00` | Raw AX.25 |
| TXDELAY | `0x01` | Delay awal TX/PTT, 10 ms/unit |
| PERSIST | `0x02` | Nilai p-persistence 0–255 |
| SLOTTIME | `0x03` | Slot time CSMA, 10 ms/unit |
| TXTAIL | `0x04` | TX tail, 10 ms/unit |
| FULLDUPLEX | `0x05` | 0 = half duplex, selain 0 = full duplex |
| RETURN | `0xFF` | Return/reset sesi KISS |

Command lain yang belum didukung akan diabaikan.

## Alur AX.25 dan FCS

Host mengirim:

```text
C0 00 [raw AX.25 tanpa FCS] C0
```

UV-K5 menambahkan FCS sendiri, membuat HDLC/NRZI, lalu memancarkan Bell 202 ke RF.

Pada RX, UV-K5 memeriksa FCS packet RF. Bila FCS valid, dua byte FCS dibuang dan host menerima:

```text
C0 00 [raw AX.25 tanpa FCS] C0
```

Jadi **FCS tidak dibawa di KISS DATA**, sesuai perilaku TNC KISS normal.

## Default parameter

```text
TXDELAY    50  = 500 ms
PERSIST    63
SLOTTIME   10  = 100 ms
TXTAIL     3   = 30 ms
FULLDUPLEX 0
```

Firmware memakai queue TX kecil dan mekanisme p-persistence/slot-time untuk half duplex.

## Fokus REV1A

Firmware ini terutama ditujukan untuk **APRS / AX.25 UI 1200 baud**. REV1A belum dimaksudkan sebagai pengganti seluruh fitur TNC packet-radio klasik dan ukuran frame internal dioptimalkan untuk packet APRS normal.

## Yang tidak dilakukan ketika KISS aktif

- tidak mendigipeat otomatis;
- tidak beacon posisi otomatis;
- tidak RF→APRS-IS;
- tidak APRS-IS→RF;
- tidak menyisipkan framing UART proprietary NUSA.

## Target kompatibilitas

Targetnya adalah software yang mendukung **serial KISS TNC** pada Windows, Linux/Raspberry Pi, dan host lain.

Untuk Linux/Raspberry Pi dapat diuji antara lain dengan stack AX.25 seperti `kissattach` / `kissparms`, atau aplikasi APRS yang mendukung serial KISS.

**Kompatibilitas setiap software belum diklaim sebelum ada hasil pengujian nyata.**

## Cara uji

Lihat **[TESTING.md](./TESTING.md)**.

Urutan yang disarankan:

1. flash firmware Normal atau Standalone;
2. set frekuensi packet/APRS yang legal di wilayah Anda;
3. sambungkan interface UART yang sudah diverifikasi;
4. set host ke Serial KISS 38400 8N1;
5. uji **RF RX → host KISS**;
6. uji **host KISS → UV-K5 TX RF**;
7. uji TXDELAY/PERSIST/SLOTTIME/TXTAIL;
8. laporkan hasilnya.

## Status verifikasi

Sebelum dipublikasikan file sudah diverifikasi:

- compile dan link berhasil;
- identitas firmware NUSA benar;
- fungsi KISS RX/TX dan konstanta KISS ada di source build;
- divider UART terkalibrasi tetap digunakan;
- CRC firmware packed benar;
- hasil deobfuscation packed sama dengan raw build.

Tetapi **ini belum sama dengan field test**. RX/TX KISS tetap harus diuji pada perangkat nyata.

## SHA256

Tidak ada asset checksum terpisah; nilai ini hanya dicantumkan di dokumentasi.

```text
NORMAL
b68bf4649d86d4cd06ddf363944a7834ac830be120f2e86c2def49db4829ad0e

STANDALONE
cba26650328280b06e8c1efe4ac256a9b55754290b5e86c23ecdcc57d02520a7
```

## Mohon bantuan pengujian

Buat GitHub Issue dan sertakan:

- versi hardware UV-K5;
- firmware Normal/Standalone;
- PC / Raspberry Pi / host lain;
- sistem operasi;
- interface UART;
- aplikasi yang dipakai;
- hasil RX;
- hasil TX;
- hasil parameter KISS;
- log/packet yang diterima;
- foto/screenshot bila diperlukan.

Laporan gagal juga sangat berguna untuk pengembangan berikutnya.

## Peringatan

Ini firmware eksperimen untuk **UV-K5 V1**. Simpan firmware yang sudah diketahui bekerja agar radio dapat dipulihkan bila diperlukan.

Jangan flash ke UV-K5 V3 atau hardware lain kecuali ada build yang secara khusus dibuat untuk hardware tersebut.
