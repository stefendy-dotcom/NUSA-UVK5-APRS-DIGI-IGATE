# NUSA UV-K5 iGATE REV1A — Bahasa Indonesia

**NUSA UV-K5 iGATE REV1A** menjadikan Quansheng UV-K5/UV-5K sebagai sisi radio/modem RF untuk APRS iGate. ESP32 DevKit/WROOM-32 menangani Wi-Fi, APRS-IS, filter message, dan dashboard web.

REV1A adalah versi **normal / manual-start**. Setelah radio dinyalakan, UV-K5 masih dapat bekerja sebagai HT biasa. Masuk ke layar APRS/iGate menggunakan action APRS, biasanya **Long F2**.

## Arsitektur

```text
Stasiun APRS RF
      |
      v
Quansheng UV-K5
Decode Bell 202 / AX.25
      |
      | UART 38400 8N1
      v
ESP32 DevKit / WROOM-32
      |
      | Wi-Fi / TCP
      v
APRS-IS
```

Jalur balik:

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

## Fitur utama

- Firmware iGate APRS khusus UV-K5.
- Bell 202 / AX.25 1200 baud berjalan langsung di firmware UV-K5.
- ESP32 menghubungkan UV-K5 ke APRS-IS melalui Wi-Fi.
- Gating RF -> APRS-IS untuk packet direct/indirect.
- APRS-IS -> RF dibatasi **hanya APRS message**.
- Target IS -> RF harus terdengar **DIRECT melalui RF dalam 30 menit terakhir**.
- Callsign + SSID target harus cocok tepat.
- Proteksi loop untuk NOGATE, RFONLY, TCPXX, Internet-heard station, duplicate dan third-party.
- Dashboard web: `http://192.168.4.1/`.
- AP default: `NUSA-IGATE` / `12345678`.
- UART: **38400 baud, 8N1**.
- TOCALL beacon posisi: `APZUAG`.
- Path posisi: `WIDE2-1`.
- Comment posisi: `NUSA IGATE`.
- Dual Watch dimatikan saat APRS aktif agar packet tidak terpotong karena perpindahan VFO.

## Status pengujian

- Firmware UV-K5: compile sudah diverifikasi.
- UV-K5 `.packed.bin`: CRC packed sudah diverifikasi.
- Paket binary ESP32: compile berhasil menggunakan **Arduino-ESP32 core 3.3.11**, target `esp32:esp32:esp32`.
- Interface UART yang sudah dikoreksi: **sudah berhasil diuji pada hardware nyata**.
- RF -> ESP32 -> APRS-IS: **sudah berhasil diuji pada hardware nyata**.
- APRS-IS -> RF message: sudah diimplementasikan, masih menunggu pengujian on-air.

## File rilis

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip
FLASHING.md
SHA256SUMS.txt
```

ESP32 diberikan sebagai paket ZIP hasil compile yang berisi tepat empat file binary.

Setelah extract `NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip`:

```text
NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

Address flash:

```text
0x1000  bootloader
0x8000  partitions
0xE000  boot_app0
0x10000 firmware
```

Baca **[FLASHING.md](FLASHING.md)** untuk prosedur lengkap.

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

## Cara penggunaan dasar

1. Set frekuensi APRS pada VFO UV-K5 yang digunakan iGate.
2. Masuk layar APRS/iGate, biasanya Long F2.
3. Nyalakan ESP32 dan hubungkan ke AP `NUSA-IGATE`.
4. Buka `http://192.168.4.1/`, isi Wi-Fi, callsign/SSID iGate dan APRS-IS.
5. Pastikan Wi-Fi connected dan APRS-IS menunjukkan `VERIFIED`.
6. Kirim packet APRS dari stasiun RF lain.
7. Pastikan UV-K5 decode dan counter **RF -> APRS-IS** meningkat.
8. Pastikan packet masuk APRS-IS.

## Credit

```text
NUSA UV-K5 iGATE
YF9UAG
Indonesia
```

## Keselamatan dan regulasi

Verifikasi wiring menggunakan multimeter sebelum memberikan daya. Jangan mengandalkan warna kabel untuk menentukan TIP/RING/SLEEVE. Operator bertanggung jawab terhadap frekuensi, power, callsign dan operasi unattended sesuai regulasi yang berlaku.
