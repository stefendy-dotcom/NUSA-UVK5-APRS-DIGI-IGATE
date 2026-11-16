# NUSA DIGI REV1M — Automatic WIDE1-1 / WIDE2-1 / WIDE2-2, No Modes

## Bahasa Indonesia

**Perangkat:** Quansheng UV-K5 V3, firmware utama **F4HWN Labs v6.3.0 NUSA REV1E (API Level 4)**.

**APP:** `apps/NUSA_DIGI_REV1M_AUTO_WIDE1_WIDE2_NO_MODE_REV1E.app`, versi internal **1.2**.

### Perubahan REV1M
- **Menghapus mode W1 dan WN sepenuhnya**, termasuk pilihan mode melalui tombol 1.
- Digipeater otomatis memproses paket AX.25 valid dengan PATH **WIDE1-1**, **WIDE2-1**, dan **WIDE2-2** (juga WIDEn-N/TRACEn-N), dengan pemeriksaan duplikat, anti-loop, dan kondisi kanal.
- WIDE1-1 / WIDE2-1 → `CALLSIGN-DIGI*`; WIDE2-2 → `CALLSIGN-DIGI*,WIDE2-1`.
- Callsign + SSID diambil dari konfigurasi **APRS Edit**; DIGI dan beacon memakai identitas yang sama. Jika identitas tidak valid, tidak ada retransmisi/beacon.
- LED RX hijau, TX merah, beacon posisi otomatis 30 menit, beacon manual tombol 3, serta tiga halaman diagnostik tombol 2 tetap dipertahankan.
- Proteksi paket duplikat **30 detik** tetap aktif. Jangan menggunakan isi paket yang sama dalam 30 detik untuk membandingkan PATH.

### Uji dan pemasangan
- **Operator telah mengonfirmasi REV1M bekerja pada pengujian lapangan**, dengan bukti penerimaan paket APRS yang dibagikan pada 16 November 2026.
- Pengujian C/AX.25: WIDE1-1, WIDE2-1, WIDE2-2, penyesuaian PATH, CRC/FCS, LED, antrean, dan identitas APRS Edit lulus. Hasil uji kode tidak membuktikan semua skenario RF.
- **Tidak perlu flash ulang firmware utama** jika sudah menggunakan NUSA REV1E. Cukup cadangkan APP sebelumnya dan pasang APP REV1M melalui F4HWN App Loader.
- APP bukan firmware utama `.bin`; jangan gunakan pada UV-K5 Board V1 atau firmware F4HWN standar API 3.

### Unduhan
- [APP REV1M langsung dari repository](apps/NUSA_DIGI_REV1M_AUTO_WIDE1_WIDE2_NO_MODE_REV1E.app)
- [GitHub Release REV1M](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/uvk5-v3-f4hwn-labs-nusa-digi-rev1m) — berkas ZIP berisi satu file APP yang tidak dimodifikasi
- [Firmware utama NUSA REV1E](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/uvk5-v3-f4hwn-labs-rev1e) — rilis terpisah
- [Kode sumber REV1M](source/REV1M/aprsdigi_app.c)

SHA-256 APP: `9c4915c9e0a9cfa3225a2da9ce4d1d746c53f200d54643280d8b511287d8f490` (4.289 byte; kode ARM 3.960/4.096 byte).

## English

**Target:** Quansheng UV-K5 V3 with **F4HWN Labs v6.3.0 + NUSA REV1E resident firmware (API 4)**.

**NUSA DIGI REV1M** removes the W1/WN mode switch entirely and automatically supports **WIDE1-1, WIDE2-1, and WIDE2-2** with normal AX.25/WIDEn-N behavior. The mode toggle on key 1 is disabled. APRS Edit sets the digipeater/beacon callsign and SSID. RX green/TX red LEDs, 30-minute automatic position beacon, manual beacon (key 3), diagnostics (key 2), and duplicate/loop protection remain.

The operator reports successful on-air use of REV1M; source-level AX.25 path and CRC tests also pass. Retain the **NUSA REV1E** main firmware, back up the previous APP, and install only the replacement `.app` through the F4HWN App Loader. This is **not** for UV-K5 V1 or unmodified F4HWN Labs API 3 firmware.

**APP SHA-256:** `9c4915c9e0a9cfa3225a2da9ce4d1d746c53f200d54643280d8b511287d8f490`.
