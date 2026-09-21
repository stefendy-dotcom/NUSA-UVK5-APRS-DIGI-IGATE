# NUSA UV-K5 iGATE REV1A — Bahasa Indonesia

**NUSA UV-K5 iGATE REV1A** menjadikan Quansheng UV-K5/UV-5K sebagai sisi radio/modem RF untuk APRS iGate. ESP32 DevKit/WROOM-32 menangani koneksi Wi-Fi, APRS-IS, filter message, dan dashboard web.

REV1A ini adalah versi **normal / manual-start**. Setelah radio dinyalakan, UV-K5 masih bekerja sebagai HT biasa. Masuk ke layar APRS/iGate menggunakan action APRS yang dikonfigurasi, biasanya **Long F2**. Versi standalone/autostart dapat dibuat kemudian tanpa mengubah engine RF yang sudah terbukti.

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

Jalur balik yang sudah diimplementasikan:

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
- Modem RF Bell 202 / AX.25 1200 baud dijalankan langsung di firmware UV-K5.
- ESP32 menghubungkan UV-K5 ke APRS-IS melalui Wi-Fi.
- Gating RF -> APRS-IS dengan penanganan stasiun direct/indirect.
- APRS-IS -> RF dibatasi **hanya untuk APRS message**.
- Target IS -> RF harus benar-benar terdengar **DIRECT melalui RF dalam 30 menit terakhir**.
- Callsign + SSID target harus cocok tepat.
- Proteksi loop untuk NOGATE, RFONLY, TCPXX, Internet-heard station, duplicate, dan third-party handling.
- Dashboard web ESP32 di `http://192.168.4.1/`.
- AP default ESP32: `NUSA-IGATE` / `12345678`.
- UART UV-K5 <-> ESP32: **38400 baud, 8N1**.
- TOCALL beacon posisi: `APZUAG`.
- Symbol posisi: alternate table `\` dan symbol `&` (`\&`).
- Path posisi: `WIDE2-1`.
- Comment posisi: `NUSA IGATE`.
- Saat APRS/iGate aktif receiver dikunci ke satu VFO terpilih. Dual Watch dimatikan agar penerimaan packet tidak terpotong karena radio berpindah VFO.

## Status pengujian saat ini

- Firmware UV-K5: compile sudah diverifikasi.
- UV-K5 `.packed.bin`: CRC packed sudah diverifikasi.
- Sketch ESP32: compile sudah berhasil menggunakan **Arduino-ESP32 core 3.3.11**, target `esp32:esp32:esp32` (ESP32 Dev Module / kelas WROOM-32).
- Interface UART yang sudah dikoreksi: **sudah berhasil diuji pada hardware nyata**.
- Jalur RF -> ESP32 -> APRS-IS: **sudah berhasil diuji pada hardware nyata**.
- Jalur APRS-IS -> RF message: sudah diimplementasikan, tetapi jangan disebut field-tested sampai benar-benar diuji on-air pada instalasi ini.

## File

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_FULL.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_APP.bin
esp32/NUSA_UVK5_ESP32_IGATE_REV1A.ino
docs/NUSA_UVK5_IGATE_REV1A_CONNECTION.png
FLASHING.md
SHA256SUMS.txt
```

Untuk pengguna ESP32 biasa, gunakan **`NUSA_UVK5_ESP32_IGATE_REV1A_FULL.bin`**. File ini merupakan merged image 4 MB dan di-flash mulai address `0x0`.

`APP.bin` disediakan untuk pengguna lanjutan/recovery dan hanya di-flash pada `0x10000` apabila bootloader dan partition table yang kompatibel sudah terpasang.

## Koneksi UART yang benar

Gunakan gambar koneksi di repository ini. Jalur utamanya:

```text
UV-K5 2.5 mm RING   (UART TX) -- 1 kΩ --> ESP32 GPIO16 / RX2
UV-K5 2.5 mm SLEEVE (GND) -------------- ESP32 GND
ESP32 GPIO17 / TX2 -- 1 kΩ --> UV-K5 3.5 mm SLEEVE (MIC-/PTT/UART RX)
```

Jangan sambungkan bagian berikut ke ESP32:

```text
UV-K5 2.5 mm TIP  = SPK+
UV-K5 3.5 mm RING = MIC+
UV-K5 3.5 mm TIP  = radio V+
```

**TIP 3.5 mm membawa tegangan dari radio. Jangan dihubungkan ke GPIO maupun GND ESP32.**

![Diagram koneksi NUSA UV-K5 iGATE](docs/NUSA_UVK5_IGATE_REV1A_CONNECTION.png)

## Cara penggunaan dasar

1. Set frekuensi APRS pada VFO UV-K5 yang akan digunakan iGate.
2. Masuk ke layar APRS/iGate UV-K5, biasanya dengan Long F2.
3. Nyalakan ESP32 dan hubungkan ke AP `NUSA-IGATE`.
4. Buka `http://192.168.4.1/`, lalu isi Wi-Fi, callsign/SSID iGate, dan server APRS-IS.
5. Pastikan Wi-Fi connected dan APRS-IS menunjukkan `VERIFIED`.
6. Kirim packet APRS dari radio/stasiun lain.
7. Pastikan UV-K5 decode packet dan counter **RF -> APRS-IS** di dashboard ESP32 meningkat.
8. Pastikan packet masuk APRS-IS / map APRS.

Baca **[FLASHING.md](FLASHING.md)** sebelum memasang binary UV-K5 maupun ESP32.

## Credit

```text
NUSA UV-K5 iGATE
YF9UAG
Indonesia
```

Dikembangkan dari ekosistem firmware open-source Quansheng UV-K5 serta referensi komunitas APRS.

## Catatan keselamatan dan regulasi

Sebelum menyalakan perangkat, verifikasi TIP/RING/SLEEVE dengan multimeter. Jangan mengandalkan warna kabel sebagai identifikasi pin. Frekuensi, power, identifikasi callsign, serta aturan operasi unattended berbeda di setiap negara; operator bertanggung jawab mengikuti regulasi radio amatir yang berlaku.
