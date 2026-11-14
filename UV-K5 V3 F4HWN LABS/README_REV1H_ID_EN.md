# NUSA DIGI REV1H — RX Green LED / TX Red LED

## Bahasa Indonesia

**Perangkat:** Quansheng UV-K5 V3 dengan firmware utama **F4HWN Labs v6.3.0 NUSA REV1E (API 4)**.

**Aplikasi:** `apps/NUSA_DIGI_REV1H_RX_GREEN_TX_RED.app` (versi internal 0.7).

**Pembaruan dan hasil uji lapangan:**
- LED **hijau menyala saat menerima sinyal RF (RX)** dan padam ketika sinyal selesai.
- LED **merah menyala saat TX**, baik ketika memancarkan ulang paket APRS sebagai digipeater maupun saat mengirim beacon posisi.
- **Pengguna telah menguji langsung dan mengonfirmasi LED RX hijau serta TX merah berjalan normal.**
- Status LED mengikuti aktivitas RF, tidak hanya penghitung paket yang muncul di layar.

**Fitur yang dipertahankan:**
- Digipeater APRS WIDEn-N/TRACEn-N dengan pemeriksaan duplikat serta pencegahan loop.
- Penerimaan dan retransmisi paket AX.25, diagnostik tiga halaman (tombol 2).
- Beacon posisi otomatis setiap 30 menit dan beacon manual melalui tombol 3.
- Callsign yang sama untuk DIGI dan beacon posisi (contoh stasiun pengujian `YF9UAG-3`).
- Koordinat posisi dari profil APRS TX melalui APRS Edit.
- Auto Resume aplikasi setelah radio dimatikan dan dinyalakan kembali.

**Cara pemasangan:**
1. Simpan cadangan APP REV1G yang sebelumnya digunakan.
2. **Tidak perlu flash firmware utama lagi** apabila sudah menggunakan F4HWN Labs NUSA REV1E.
3. Pasang file `.app` REV1H melalui App Loader F4HWN Labs.
4. Uji RX, TX digipeater dengan PATH `WIDE2-2`, TX beacon manual (tombol 3), dan Auto Resume.

**Catatan:** LED RX/TX sudah dikonfirmasi pada perangkat fisik. Pengujian interval otomatis 30 menit khusus REV1H belum dilaporkan sebagai hasil akhir; jangan menganggapnya sudah tervalidasi hanya dari uji LED. Jangan memasang file ini sebagai firmware utama `.bin` atau pada UV-K5 V1.

## English

**Target hardware:** Quansheng UV-K5 V3 running **F4HWN Labs v6.3.0 with NUSA REV1E resident firmware (API level 4)**.

**Application:** `apps/NUSA_DIGI_REV1H_RX_GREEN_TX_RED.app` (internal version 0.7).

**Changes and field-test confirmation:**
- **Green LED illuminates during RF reception (RX)** and goes off when reception ends.
- **Red LED illuminates during TX**, for APRS digipeater retransmissions and for the station's own position beacons.
- **The operator has confirmed that both RX green and TX red LEDs work normally on the physical radio.**
- LED behavior indicates actual RF activity rather than merely a counter increment.

**Retained functionality:**
- APRS WIDEn-N/TRACEn-N digipeater with duplicate suppression and loop protection.
- AX.25 packet receive/repeat handling and three diagnostic screens (key 2).
- Automatic position beacon every 30 minutes and manual beacon with key 3.
- Same callsign/SSID for the digipeater and its position beacon (test station: `YF9UAG-3`).
- Position data taken from the APRS TX profile configured in APRS Edit.
- Automatic NUSA DIGI app resume after the radio is powered off and back on.

**Installation:**
1. Back up the previously working REV1G app.
2. **Do not reflash the main firmware** when NUSA REV1E Labs firmware is already installed.
3. Install REV1H `.app` using the compatible F4HWN Labs App Loader.
4. Test reception, `WIDE2-2` digipeating, manual position beacon (key 3), and power-cycle Auto Resume.

**Note:** Physical-radio RX/TX LED behavior is confirmed by the operator. The 30-minute automatic beacon interval has not been separately confirmed on REV1H. This app is not a main firmware `.bin` and is not intended for UV-K5 V1.

## Download / Unduhan

- Raw APP file / File APP langsung: [`NUSA_DIGI_REV1H_RX_GREEN_TX_RED.app`](apps/NUSA_DIGI_REV1H_RX_GREEN_TX_RED.app)
- GitHub Release asset / Berkas Release GitHub: `NUSA_DIGI_REV1H_RX_GREEN_TX_RED_APP.zip` (contains the unchanged `.app`).
- Main firmware / Firmware utama (separate earlier release / rilis terpisah): [NUSA Labs REV1E](https://github.com/stefendy-dotcom/NUSA-UVK5-APRS-DIGI-IGATE/releases/tag/uvk5-v3-f4hwn-labs-rev1e).

## Integrity / Integritas

`SHA-256: 78368cbf240f75141ca5642feedc81449e75bfc3ff72964f6060650c9f744e50`

File size / Ukuran berkas: **4,421 bytes**.
          DOC
