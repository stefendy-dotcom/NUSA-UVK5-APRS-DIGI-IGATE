# NUSA UV-K5 GPS APRS TRACKER REV1A — PRETEST / BELUM DIUJI PADA RADIO

**JANGAN MENYEBARKAN SEBAGAI FIRMWARE STABIL.** Build firmware berhasil, pengujian GPS/parser pada komputer berhasil. Belum diuji konektor UV-K5, GPS fisik, TX RF, atau kompatibilitas penerima APRS.

## Perangkat target
- HANYA UV-K5/K6 Board V1 dengan MCU DP32G030 + BK4819; tidak untuk UV-K5 Board V3/BK4829.
- GPS UART NMEA 0183: NEO-6 / NEO-6M / NEO-M8N / ATGM336H, set 9600 baud 8N1.
- GPS menggunakan baterai eksternal terpisah. TX GPS (level logika 3,3 V) ke RX UART radio via konektor samping, GND GPS bersama dengan GND radio. Jangan sambung suplai GPS ke jack. **Pastikan pinout jack pada perangkat dengan pengukuran terlebih dahulu.**
- GPS module TX ONLY; GPS RX tidak diperlukan.

## Pengaturan radio
1. Sebelum flash, pastikan benar UV-K5 Board V1, dan sediakan firmware stock yang cocok untuk pemulihan.
2. Setelah flash, atur frekuensi kanal APRS sesuai alokasi/izin setempat (contoh 144.390 MHz jika legal dan dipakai lokal).
3. Set menu APRS->Call dan SSID memakai callsign resmi. Firmware tidak memancar jika panggilan masih N0CALL.
4. APRS->APRS ON agar scheduler beacon aktif.
5. APRS->Intv = SMART (0) untuk SmartBeaconing. Nilai 1–60 untuk interval menit.
6. APRS->Loc menampilkan lokasi GPS saat fix valid, atau NO GPS FIX saat GPS belum valid.
7. APRS->Cmnt: komentar posisi. Simbol posisi car '>' dan path WIDE2-2, TOCALL APZUAG.
8. Beacon pertama ~15 detik setelah GPS memperoleh fix valid berkelanjutan.
9. GPS fix hilang atau NMEA invalid: otomatis tidak beacon posisi.
10. APZUAG dan jalur APRS lain digunakan atas kewenangan operator.

## SmartBeaconing prototipe
- Bergerak < 5 km/jam: 30 menit.
- Bergerak 70 km/jam atau lebih: 60 detik.
- Kecepatan menengah: periode otomatis berdasarkan speed.
- Corner Pegging min 15 detik, ambang perubahan heading bergantung speed.
- 'Intv' 1–60 menit mengaktifkan interval tetap dan menonaktifkan SmartBeaconing.
- Parameter SmartBeaconing REV1A masih ditetapkan di kode sumber, **belum** ada submenu konfigurasi threshold.

## Keterbatasan dan kewaspadaan
- Firmware belum diuji pada perangkat fisik / radio lain. Tes awal sebaiknya via dummy load dan decoder APRS lokal.
- Konektor samping programming bisa berbagi kontak dengan PTT/audio. Cek adanya potensi PTT tidak sengaja pada radio dan pengaruh level sinyal GPS.
- TX APRS mengikuti VFO/frekuensi radio. Jangan pancarkan sebelum parameter cocok dengan ketentuan lokal.
- Selama mode GPS, UART aplikasi menggunakan 9600 baud dan bukan protokol pemrograman kontrol PC biasa; bootloader DFU radio tetap untuk flashing.
- Menu LCD GPS hanya menunjukkan koordinat pada halaman Loc; status satelit/speed di LCD utama belum diimplementasikan.
- Penyesuaian menu, panjang komentar, koordinat, dan frekuensi bergantung firmware dasar.
- GPS dan antena masih memerlukan uji kondisi nyata untuk akurasi NMEA, integritas RF, beban CPU, dan RF decoding.

## Dasar firmware
Sumber: https://github.com/bcanata/uv-k5-firmware-ta1js
Mewarisi kode Apache-2.0 milik pengembang asal dan atribusi dari repository. Firmware GPS ini belum dipublikasikan sebagai versi stabil.

## Tes yang dilakukan
- Toolchain cross compile arm-none-eabi-gcc 14.2.1: berhasil.
- Batas flash V1: firmware mentah 60420 bytes, flash limit linker 61440 bytes.
- Parser native dengan ASan/UBSan: NMEA GP/GN RMC/GGA, checksum, koordinat negatif, speed, heading, fix invalid/stale, SmartBeaconing corner, interval tetap: PASS.
- Format BIN packed menggunakan obfuscation/CRC identik format fw-pack.py upstream; pemeriksaan CRC XMODEM PASS.
- Pengujian hardware/SDR/radio on-air: BELUM.
