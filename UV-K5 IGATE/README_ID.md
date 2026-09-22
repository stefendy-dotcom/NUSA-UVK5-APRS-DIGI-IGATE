# NUSA UV-K5 iGATE — Bahasa Indonesia

**NUSA UV-K5 iGATE** menjadikan Quansheng UV-K5/UV-5K sebagai sisi radio/modem RF untuk APRS iGate. REV1A adalah baseline Normal dan REV1B adalah revisi khusus Standalone. ESP32 DevKit/WROOM-32 atau ESP32-C3 menangani Wi-Fi, APRS-IS, filtering, dan dashboard web.

Tersedia dua varian firmware UV-K5:

1. **REV1A Normal** — APRS/iGate masuk secara manual, biasanya Long F2.
2. **REV1B Standalone** — mode iGate dedicated aktif otomatis setelah radio dinyalakan.

Kedua varian UV-K5 menggunakan **protokol UART yang sama**. Paket binary terpisah tersedia untuk ESP32 klasik/WROOM-32 dan ESP32-C3.

## Terbaru khusus Standalone: REV1B

**REV1A Normal tetap menjadi baseline iGate yang sudah berhasil diuji RF → ESP32 → APRS-IS. REV1B hanya merevisi firmware UV-K5 Standalone.**

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1B_STANDALONE.packed.bin
```

Release: [igate-rev1b-standalone](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/igate-rev1b-standalone)

Menu Standalone REV1B:

```text
TxPwr
APRFq
IS2RF
IgCall
IgSSID
TxDly
TxTail
PosBcn
BType
ObjNam
BComnt
SymTbl
Symbol
BLat
LatNS
BLon
LonEW
```

Fitur baru:

- `BType`: Station atau APRS Object.
- `ObjNam`: Object Name editable, maksimum 9 karakter; default `NUSAIGATE`.
- `BComnt`: suffix comment editable, maksimum 16 karakter.
- `SymTbl`: symbol table `/` atau `\`.
- `Symbol`: kode symbol APRS editable.
- Beacon Object memakai `*` dan pseudo timestamp `111111z`.
- Comment TX **selalu diawali `NUSA IGATE`**; user hanya mengedit suffix-nya.

Protokol UART UV-K5 ↔ ESP32 tetap sama. **ESP32 tidak memerlukan firmware baru** untuk revisi UV-K5 Standalone ini.

Status REV1B Standalone: **build verified + packed CRC verified**, hardware/on-air field test masih pending.

SHA256:

```text
d3b69b7c1cddd886248a6afceaca68987a30007ecca2aadba0ea0da7dfcb8c21
```

## Diagram Koneksi

![Diagram Koneksi NUSA UV-K5 iGATE](Connection%20Diagram.png)

## Arsitektur

```text
Stasiun APRS RF
      |
      v
Quansheng UV-K5
Bell 202 / AX.25
      |
      | UART 38400 8N1
      v
ESP32 DevKit / WROOM-32
      |
      | Wi-Fi / TCP
      v
APRS-IS
```

Jalur balik untuk APRS message:

```text
APRS-IS message
      |
      v
ESP32: eligibility + anti-loop
      |
      | UART
      v
UV-K5 Bell 202 TX
      |
      v
Stasiun RF lokal
```

## Varian firmware

| Fungsi | REV1A Normal | REV1B Standalone |
|---|---|---|
| Start APRS/iGate | Manual, biasanya Long F2 | Otomatis saat power-on |
| VFO yang digunakan | VFO yang dipilih saat masuk APRS | VFO A |
| Dual Watch saat APRS | OFF | OFF |
| Cross Band | Normal di luar APRS | OFF saat standalone |
| Voice PTT saat APRS | Diblokir | Diblokir |
| EXIT dari APRS | Keluar APRS | Tidak mematikan iGate |
| MENU dari APRS | Mengikuti alur radio normal | Membuka menu konfigurasi |
| Protokol UART | 38400 8N1 | Sama |
| Firmware ESP32 | Sama | Sama |
| RF -> APRS-IS | Sama | Sama |
| APRS-IS -> RF message | Sama | Sama |
| Menu frekuensi APRS khusus | Tidak | Ya |
| Beacon Station / APRS Object | Station | Bisa dipilih |
| Object Name editable | Tidak | Ya, 9 karakter |
| Suffix comment editable | Tidak | Ya, 16 karakter |
| Prefix comment wajib | `NUSA IGATE` | `NUSA IGATE` |
| Symbol APRS editable | Tidak | Ya, table + code |

### REV1A Normal

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
```

Pilih VFO/frekuensi APRS, kemudian masuk APRS/iGate secara manual.

### REV1B Standalone

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1B_STANDALONE.packed.bin
```

Alur standalone:

```text
POWER ON
   |
   v
Load EEPROM / VFO
   |
   v
Paksa VFO A
Dual Watch OFF
Cross Band OFF
   |
   v
Gunakan APRFq khusus yang tersimpan (boot pertama mewarisi VFO-A)
Paksa APRS simplex saat runtime
   |
   v
Masuk APRS/iGate otomatis
   |
   v
IGATE ACTIVE
```

Standalone mempertahankan engine Bell-202, AX.25, UART framing, RF -> APRS-IS, serta queue IS -> RF yang sama dengan REV1A Normal.

**ESP32 tidak perlu firmware berbeda.**

## Fitur utama

- Bell 202 / AX.25 APRS 1200 baud.
- RF -> APRS-IS gating.
- APRS-IS -> RF dibatasi APRS message.
- Target IS -> RF harus terdengar DIRECT melalui RF dalam 30 menit terakhir.
- Callsign + SSID target harus cocok tepat.
- Anti-loop untuk NOGATE, RFONLY, TCPXX, Internet-heard station, duplicate dan third-party.
- Dashboard: `http://192.168.4.1/`.
- AP default: `NUSA-IGATE` / `12345678`.
- UART: **38400 baud, 8N1**.
- TOCALL beacon posisi: `APZUAG`.
- Path: `WIDE2-1`.
- Comment selalu diawali `NUSA IGATE`.
- REV1B Standalone dapat memilih beacon Station atau APRS Object.
- REV1B Standalone menyediakan Object Name 9 karakter, suffix comment 16 karakter, symbol table dan symbol code yang dapat diedit.

## Status pengujian

- REV1A Normal UV-K5: compile dan packed CRC sudah diverifikasi.
- Interface UART yang sudah dikoreksi: **sudah berhasil diuji pada hardware nyata**.
- REV1A Normal RF -> ESP32 -> APRS-IS: **sudah berhasil diuji pada hardware nyata**.
- REV1B Standalone UV-K5: **compile dan packed CRC sudah diverifikasi**.
- REV1B Standalone: **belum diuji pada hardware nyata**.
- APRS-IS -> RF message: sudah diimplementasikan, masih menunggu pengujian on-air.
- Paket ESP32-C3: **compile verified** menggunakan Arduino-ESP32 core 3.3.11 (`esp32:esp32:esp32c3`); pengujian hardware masih pending.

## File rilis

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
firmware/NUSA_UVK5_IGATE_REV1B_STANDALONE.packed.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip
esp32/NUSA_UVK5_ESP32C3_IGATE_REV1A_BIN.zip
FLASHING.md
SHA256SUMS.txt
```

ZIP ESP32 klasik berisi tepat empat file binary:

```text
NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

ZIP ESP32-C3 juga berisi tepat empat file binary:

```text
NUSA_UVK5_ESP32C3_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32C3_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32C3_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32C3_IGATE_REV1A_firmware.bin
```

Baca **[FLASHING.md](FLASHING.md)** untuk address. Pada ESP32-C3 bootloader menggunakan offset **0x0**.

## Koneksi UART

```text
UV-K5 2.5 mm RING   (UART TX) -- 1 kΩ --> ESP32 GPIO16 / RX2
UV-K5 2.5 mm SLEEVE (GND) -------------- ESP32 GND
ESP32 GPIO17 / TX2 -- 1 kΩ --> UV-K5 3.5 mm SLEEVE (MIC-/PTT/UART RX)
```

ESP32-C3:

```text
UV-K5 2.5 mm RING   (UART TX) -- 1 kΩ --> ESP32-C3 GPIO4 / RX
UV-K5 2.5 mm SLEEVE (GND) -------------- ESP32-C3 GND
ESP32-C3 GPIO5 / TX -- 1 kΩ --> UV-K5 3.5 mm SLEEVE (MIC-/PTT/UART RX)
```

Jangan sambungkan ke ESP32:

```text
UV-K5 2.5 mm TIP  = SPK+
UV-K5 3.5 mm RING = MIC+
UV-K5 3.5 mm TIP  = radio V+
```

**TIP 3.5 mm membawa tegangan radio. Jangan dihubungkan ke GPIO atau GND ESP32.**

## Credit

```text
NUSA UV-K5 iGATE
YF9UAG
Indonesia
```

## Keselamatan dan regulasi

Verifikasi wiring menggunakan multimeter sebelum memberikan daya. Jangan mengandalkan warna kabel untuk menentukan TIP/RING/SLEEVE. Operator bertanggung jawab terhadap frekuensi, power, callsign dan operasi unattended sesuai regulasi.
