# NUSA UV-K5 iGATE REV1A — Bahasa Indonesia

**NUSA UV-K5 iGATE REV1A** menjadikan Quansheng UV-K5/UV-5K sebagai sisi radio/modem RF untuk APRS iGate. ESP32 DevKit/WROOM-32 menangani Wi-Fi, APRS-IS, filtering, dan dashboard web.

Tersedia dua varian firmware UV-K5:

1. **REV1A Normal** — APRS/iGate masuk secara manual, biasanya Long F2.
2. **REV1A Standalone** — mode iGate dedicated aktif otomatis setelah radio dinyalakan.

Kedua varian menggunakan **firmware ESP32 dan protokol UART yang sama**.

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

| Fungsi | REV1A Normal | REV1A Standalone |
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

### REV1A Normal

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
```

Pilih VFO/frekuensi APRS, kemudian masuk APRS/iGate secara manual.

### REV1A Standalone

Firmware:

```text
firmware/NUSA_UVK5_IGATE_REV1A_STANDALONE.packed.bin
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
Gunakan frekuensi RX VFO A yang tersimpan
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
- Comment: `NUSA IGATE`.

## Status pengujian

- REV1A Normal UV-K5: compile dan packed CRC sudah diverifikasi.
- Interface UART yang sudah dikoreksi: **sudah berhasil diuji pada hardware nyata**.
- REV1A Normal RF -> ESP32 -> APRS-IS: **sudah berhasil diuji pada hardware nyata**.
- REV1A Standalone UV-K5: **compile dan packed CRC sudah diverifikasi**.
- REV1A Standalone: **belum diuji pada hardware nyata**.
- APRS-IS -> RF message: sudah diimplementasikan, masih menunggu pengujian on-air.

## File rilis

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
firmware/NUSA_UVK5_IGATE_REV1A_STANDALONE.packed.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip
FLASHING.md
SHA256SUMS.txt
```

ZIP ESP32 berisi tepat empat file binary:

```text
NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

Baca **[FLASHING.md](FLASHING.md)** untuk address ESP32 dan prosedur flash UV-K5.

## Koneksi UART

```text
UV-K5 2.5 mm RING   (UART TX) -- 1 kΩ --> ESP32 GPIO16 / RX2
UV-K5 2.5 mm SLEEVE (GND) -------------- ESP32 GND
ESP32 GPIO17 / TX2 -- 1 kΩ --> UV-K5 3.5 mm SLEEVE (MIC-/PTT/UART RX)
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
