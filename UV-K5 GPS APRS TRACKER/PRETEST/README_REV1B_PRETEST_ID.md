# NUSA UV-K5 GPS APRS TRACKER REV1B — PRETEST / BELUM DIUJI PADA RADIO

**Target:** UV-K5 V1, DP32G030 + BK4819. **Jangan gunakan pada UV-K5 V3 / BK4829.**

## Perubahan dibanding REV1A
- **Auto-baud GPS UART 9600 / 38400 / 115200 (8N1)**. Tidak perlu menyetel baud GPS di menu.
- Urutan scan 9600 → 38400 → 115200, masing-masing selama ±5 detik bila belum ditemukan data.
- Baud dianggap cocok setelah menerima **2 kalimat NMEA dengan checksum valid**. Auto-baud bisa terkunci meskipun GPS belum mendapat fix satelit (ini memang benar karena baud terpisah dari fix).
- Setelah terkunci, baud dipertahankan; bila **tidak ada NMEA checksum-valid selama 30 detik**, scanning dilanjutkan otomatis.
- Pada pergantian baud, UART FIFO, DMA RX dan parser kalimat NMEA diinisialisasi ulang. Riwayat waktu beacon tidak dihapus.
- **SmartBeaconing, menu Intv, APRS callsign, path WIDE2-2 dan format packet dari REV1A dipertahankan.**

## Pengaturan
1. Gunakan kabel GPS TX (UART TTL 3,3 V) → RX UART UV-K5 dan GND bersama. GPS memakai catu daya eksternal. **Verifikasi kontak jack dan tidak ada PTT tidak sengaja sebelum menyambungkan.**
2. NEO-6M umumnya 9600; modul M10 bisa 9600/38400/115200 tergantung konfigurasi vendor. Data wajib NMEA 0183 RMC/GGA. Bila modul memakai protokol UBX-only atau baud lain, firmware tidak akan membaca data posisi.
3. Buka radio, tunggu maksimal ±15 detik untuk menyapu tiga baud (dan waktu tambahan yang diperlukan GPS agar fix), lalu periksa menu `Loc`. Bila belum fix, tampil `NO GPS FIX`.
4. Set `Call` dan `SSID` ke identitas amatir resmi, `APRS` ON untuk TX beacon otomatis.
5. Set `Intv` = `SMART` untuk SmartBeaconing, atau angka 1–60 untuk interval beacon **menit**. Tidak ada menu pemilihan baud manual karena **AUTO**.
6. Hindari transmisi hingga jalur GPS, frekuensi yang sah, dan kualitas packet pada decoder diuji. TX APRS berlangsung lewat VFO/frekuensi radio.

## Parameter SmartBeaconing REV1B
- Kecepatan <5 km/jam → interval 30 menit.
- Di atas 70 km/jam → interval 60 detik.
- Kecepatan menengah → interval otomatis 42000 / (km/jam×10) detik, dibatasi 60–1800 detik.
- Corner pegging aktif bila perubahan arah melebihi ambang dinamis; minimum 15 detik antarbeacon.
- Waktu awal TX setelah GPS fix valid ±15 detik.

## Pengujian yang sudah dilakukan
- Native C (ASan/UBSan): parser GP/GN RMC/GGA, checksum, posisi, speed, corner, interval, stale/invalid: **PASS**.
- Native C (ASan/UBSan): urutan scan ketiga baud, 2 checksum-valid sebagai konfirmasi, timeout 30s, tick dan counter rollover: **PASS**.
- Clean ARM GCC 14.2.1 / Cortex-M0 cross-build: **PASS**.
- Image raw 60968 B, kapasitas area flash firmware UV-K5 V1 61440 B.
- Image packed dibuat menggunakan `fw-pack.py` bawaan firmware TA1JS, diverifikasi identik terhadap implementasi CRC16/XMODEM/obfuscation terpisah.
- **BELUM** diuji pada UV-K5 nyata: UART fisik terutama 115200, kontak jack, pemrograman ulang, dan TX AX.25 RF belum divalidasi.

## Catatan keselamatan/perangkat
- Wajib modul UART TTL 3,3 V dan common ground; bukan RS-232 ±12V.
- Konektor side jack UV-K5 digunakan bersama dengan jalur audio/PTT/programming; tes terlebih dahulu tanpa mengizinkan pancaran.
- Jalur UART radio pada firmware aplikasi berganti baud otomatis. Bootloader flashing terpisah; cabut GPS dan pasang kabel programming biasa saat flash.
- REV1B **PRETEST**, jangan publikasikan sebagai stable sebelum diuji di radio.

## Source code
- Upstream: https://github.com/bcanata/uv-k5-firmware-ta1js (berdasarkan commit 6320cdac7a2dce46f88027d220ff67e47eb0e3fb)
- Patch file: `nusa_uvk5_gps_rev1b.patch` diterapkan pada upstream tersebut.
- Berkas tambahan: `app/nusa_gps_core.{c,h}`, `app/nusa_gps_autobaud.{c,h}`.
- File test: `test_gps.c`, `test_gps_autobaud.c`.
- Atribusi dan lisensi asli Apache-2.0 tetap berlaku.
