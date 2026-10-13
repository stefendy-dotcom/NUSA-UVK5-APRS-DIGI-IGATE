# NUSA UV-K5 GPS APRS TRACKER REV1C — CALLSIGN EDIT FIX (PRETEST)

**Khusus UV-K5 V1 DP32G030/BK4819. BELUM diuji pada radio fisik. Tidak untuk UV-K5 V3/BK4829.**

## Bug yang diperbaiki dari REV1B
- Sebelumnya, menekan MENU setelah memilih huruf pertama pada menu `Call` langsung mengakhiri pengeditan, sebab handler string APRS melakukan quick-save tanpa memperhatikan posisi kursor.
- Sekarang, **tekan MENU pendek untuk berpindah ke huruf berikutnya** (maksimum 9 posisi).
- Tanda kursor `^` ditampilkan di bawah karakter yang sedang diedit.
- **Tahan MENU untuk menyimpan callsign kapan saja**, tidak perlu mengisi 9 posisi.
- Jika semua 9 posisi diisi, tekan MENU untuk menyimpan pada posisi terakhir.
- Tombol `EXIT` saat posisi >0 menghapus karakter sebelumnya (backspace). `EXIT` ketika di karakter pertama membatalkan.
- Tombol `▲` / `▼` mengganti huruf; input angka dua digit untuk karakter tetap tersedia.
- Penyimpanan menggunakan prosedur SETTINGS_SaveAPRS EEPROM yang sudah ada.
- Perilaku cepat simpan pada menu text APRS lain tidak diubah.

## Contoh memasukkan YF9UAG
1. `MENU` cari `Call`; tekan `MENU` untuk masuk; tekan `MENU` lagi jika baru menampilkan teks dan belum berkursor.
2. Pilih huruf `Y` dengan ▲/▼, tekan MENU singkat → pindah ke karakter kedua.
3. Pilih `F`, MENU; `9`, MENU; `U`, MENU; `A`, MENU; `G`.
4. **Tahan MENU hingga editor tertutup untuk menyimpan "YF9UAG"**. Jangan tekan EXIT untuk menyimpan.
5. Set SSID dari menu `SSID` terpisah (misalnya 7).
6. Periksa `Call` kembali dan pastikan "YF9UAG" masih terlihat.
7. Untuk APRS tracker, aktifkan APRS hanya setelah GPS fix dan frekuensi/izin benar.

## Hal yang tetap ada dari REV1B
- GPS UART **auto-baud 9600 / 38400 / 115200, 8N1**, scanning ~5 detik per baud, lock setelah 2 checksum NMEA valid.
- Pembacaan NMEA RMC/GGA, SmartBeaconing, corner pegging.
- Menu `Intv=SMART` atau angka 1–60 menit.
- APRS path WIDE2-2 dan TOCALL APZUAG.
- Flashing bootloader UV-K5 tetap menggunakan jack programming standar setelah GPS dicabut.

## Pengujian software
- ARM GCC 14.2.1, clean build: PASS.
- Ukuran mentah 61136 byte, di bawah batas firmware V1 61440 byte.
- Native GPS parser dan auto-baud unit tests ASan/UBSan: PASS (rerun).
- Pemeriksaan source memastikan MENU pendek maju, MENU panjang menyimpan, karakter aktif diberi caret: PASS.
- Packed firmware menggunakan firmware TA1JS fw-pack.py dan CRC XMODEM.
- Belum diuji penekanan tombol pada radio, koneksi GPS fisik, maupun output RF APRS. Status **PRETEST**.

## Source
- Berdasar https://github.com/bcanata/uv-k5-firmware-ta1js commit 6320cdac7a2dce46f88027d220ff67e47eb0e3fb.
- ZIP berisi full patch source REV1C, file inti NUSA, C test, BIN, README dan LICENSE. Patch source diterapkan ke commit dasar tersebut.
