# NUSA UV-K5 GPS APRS TRACKER REV1D — PTT GUARD (PRETEST)

**Hardware target:** UV-K5/K6 Board V1, DP32G030 + BK4819; NOT UV-K5 V3/BK4829.

## Masalah dari REV1C
Ketika kabel TX GPS dihubungkan ke jack samping radio, LED TX berkedip dan radio benar-benar melakukan transmisi tanpa beacon. Setelah kabel TX GPS dilepas (GND tetap terpasang), TX tak terduga berhenti.

Pada rangkaian Kenwood-style UV-K5, input UART serial dan PTT eksternal dapat berbagi koneksi. Firmware membaca GPIO PC5 sebagai tombol PTT dengan debounce 30 ms. Lalu lintas NMEA dapat menimbulkan pemicu PTT palsu.

## Perubahan REV1D (khusus build GPS)
1. Menonaktifkan pemantauan input fisik PTT (GPIO PC5) yang biasanya menimbulkan pemrosesan KEY_PTT di APP_TimeSlice10ms. Tidak sekadar mematikan LED TX.
2. Memblokir event KEY_PTT tambahan di ProcessKey.
3. Mengabaikan sinyal PTT eksternal saat layar startup menunggu semua tombol dilepas, agar trafik NMEA tidak menggantung proses boot aplikasi.
4. **APRS BEACON MANUAL di MENU > BEACON > MENU tetap aktif** melalui RADIO_PrepareTX, tanpa memerlukan GPIO PTT.
5. APRS otomatis SmartBeaconing, interval, AX.25, pembacaan koordinat NMEA, dan auto-baud 9600/38400/115200 tetap seperti REV1C.
6. Bug edit callsign yang diperbaiki di REV1C tetap diperbaiki: MENU singkat geser kursor, MENU panjang menyimpan.

## Batasan disengaja
- **Tombol PTT fisik maupun remote PTT NONAKTIF selama firmware REV1D dijalankan**, termasuk jika GPS dilepas. Ini firmware khusus tracker; tidak dapat digunakan sebagai HT voice TX dengan PTT tanpa memasang firmware normal.
- Modul GPS masih menggunakan kontak jack bersama. Revisi software ini belum membuktikan kompatibilitas elektris/kualitas data UART.
- **Bootloader PTT+Power terpisah dari aplikasi dan tidak diubah.** Modul GPS yang aktif saat radio dinyalakan dapat memengaruhi bootloader sebelum firmware REV1D sempat bekerja. Untuk pengujian, daya GPS harus MATI saat radio boot, lalu GPS dinyalakan setelah radio masuk layar utama.
- **Belum diuji pada radio asli** sehingga tidak ada klaim bahwa TX tak sengaja sudah hilang sepenuhnya.

## Rangkaian pengujian aman
1. Lepaskan GPS TX dari radio, flash REV1D.
2. Pasang dummy load antena 50 ohm dengan rating daya sesuai dan awasi RF menggunakan SDR/receiver terpisah. Set APRS OFF.
3. Dengan GPS tetap tidak terhubung, hidupkan radio hingga layar utama.
4. Matikan daya eksternal modul GPS sebelum memasang/menghubungkan jack pada kondisi aman, lalu nyalakan GPS setelah radio masuk layar utama (hindari hot-plug konektor saat hidup).
5. Amati 1–2 menit: **radio tidak boleh berkedip TX, tidak boleh memancarkan carrier, dan PTT fisik seharusnya tidak memancarkan RF**.
6. Buka menu Loc tanpa edit untuk melihat GPS fix. Bila NMEA belum terbaca, jangan TX.
7. Baru uji MENU > BEACON > MENU pada dummy load dengan decoder RF lokal. Perhatikan bahwa kompatibilitas packet antar-TNC juga masih perlu diuji.
8. Jika TX spontan masih muncul, segera lepas catu GPS dan hentikan uji. Jangan menganggap REV1D sudah aman untuk digunakan on-air.

## Pengujian software
- arm-none-eabi-gcc 14.2.1 Cortex-M0 clean build: PASS, raw 60488 bytes (limit 61440).
- Unit test NMEA GP/GN, SmartBeaconing, checksum: PASS.
- Unit test scanning 9600/38400/115200, lock, timeout recovery: PASS.
- Kode memastikan PC5 polling PTT dikompilasi keluar dalam ENABLE_NUSA_GPS serta startup mengabaikan PC5: PASS melalui build dan pemeriksaan source.
- Format packed menggunakan fw-pack.py resmi, CRC16-XMODEM dan obfuscation: PASS.
- Hardware GPS, PTT, bootloader, pemancar RF: **BELUM DIUJI**.

## Source code
- Baseline TA1JS: https://github.com/bcanata/uv-k5-firmware-ta1js (commit sesuai bundle REV1A)
- ZIP menyediakan patch diff lengkap, app/nusa_gps_* core dan autobaud, test program, packed BIN, dan atribusi lisensi.
- Jangan unggah sebagai firmware stabil sebelum pengujian perangkat.
