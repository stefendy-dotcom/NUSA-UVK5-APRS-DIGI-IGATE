# Flashing Guide — NUSA UV-K5 iGATE

This guide covers the REV1A Normal UV-K5 firmware, the REV1B Standalone UV-K5 firmware, and the ESP32 **4-BIN ZIP package**.

# 1. Choose UV-K5 firmware

## Normal / manual-start

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
```

Use this when you want normal HT operation after power-on and enter APRS/iGate manually, normally with Long F2.

## Standalone / autostart

```text
firmware/NUSA_UVK5_IGATE_REV1B_STANDALONE.packed.bin
```

Use this for a dedicated iGate. It starts APRS/iGate automatically and uses the dedicated stored `APRFq` setting.

Before flashing either UV-K5 firmware:

- Charge the battery sufficiently.
- Use a known-good programming cable.
- Keep recovery firmware available.
- Do not disconnect the cable or remove power while writing.

Use a Quansheng UV-K5 compatible updater that supports `.packed.bin`.

Official Quansheng support/download page:

https://en.qsfj.com/support/downloads/3268

For REV1B Standalone, set the APRS frequency from:

```text
MENU → APRFq
```

Enter the frequency in 10 Hz units, for example `14439000` for 144.39000 MHz, then press MENU to save. On the first upgrade boot only, if no dedicated APRFq has been saved yet, the firmware inherits the existing VFO-A frequency. Standalone then uses its stored APRFq and forces simplex APRS runtime.

# 2. ESP32 package — same for Normal and Standalone

Download:

```text
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip
```

Extract it. The ZIP contains:

```text
NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

Flash offsets:

```text
0x1000  NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
0x8000  NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
0xE000  NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
0x10000 NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

The ESP32 package was built with Arduino-ESP32 core 3.3.11 for classic ESP32 Dev Module / ESP32-WROOM-32 class hardware.

## esptool example

```text
python -m esptool --chip esp32 --port COM5 --baud 460800 erase-flash

python -m esptool --chip esp32 --port COM5 --baud 460800 write-flash \
  0x1000  NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin \
  0x8000  NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin \
  0xE000  NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin \
  0x10000 NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

Replace `COM5` with the actual serial port.

After reset:

```text
SSID     : NUSA-IGATE
Password : 12345678
Web      : http://192.168.4.1/
```

# 3. Bahasa Indonesia — pilih firmware UV-K5

## Normal

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
```

Setelah power-on radio tetap normal. Masuk APRS/iGate secara manual, biasanya Long F2.

## Standalone

```text
firmware/NUSA_UVK5_IGATE_REV1B_STANDALONE.packed.bin
```

Digunakan untuk iGate dedicated. Setelah boot, firmware otomatis masuk APRS/iGate dan menggunakan frekuensi khusus yang tersimpan pada `APRFq`.

Untuk REV1B Standalone buka:

```text
MENU → APRFq
```

Masukkan frekuensi dalam satuan 10 Hz, contoh `14439000` untuk 144.39000 MHz, lalu tekan MENU untuk menyimpan. Hanya pada boot upgrade pertama, jika APRFq belum pernah disimpan, firmware mengambil nilai awal dari VFO-A. Setelah itu Standalone memakai APRFq tersimpan dan memaksa APRS simplex saat runtime.

# 4. Bahasa Indonesia — ESP32

Firmware ESP32 **tetap sama** untuk REV1A Normal dan REV1B Standalone; protokol UART tidak berubah.

Extract:

```text
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip
```

Gunakan address:

```text
0x1000  bootloader
0x8000  partitions
0xE000  boot_app0
0x10000 firmware
```

Setelah flash dan reset:

```text
SSID     : NUSA-IGATE
Password : 12345678
Web      : http://192.168.4.1/
```

# 5. Verify downloads

Compare SHA256 values with `SHA256SUMS.txt` before flashing.


# 6. ESP32-C3 package

Use:

```text
esp32/NUSA_UVK5_ESP32C3_IGATE_REV1A_BIN.zip
```

This package is for ESP32-C3 boards that expose GPIO4 and GPIO5. UART to the UV-K5 is **UART1, 38400 8N1**:

```text
GPIO4 = RX from UV-K5 UART TX
GPIO5 = TX to UV-K5 UART RX
```

The ZIP contains exactly four files:

```text
NUSA_UVK5_ESP32C3_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32C3_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32C3_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32C3_IGATE_REV1A_firmware.bin
```

ESP32-C3 flash offsets verified from the Arduino build:

```text
0x0     bootloader
0x8000  partitions
0xE000  boot_app0
0x10000 firmware
```

Example:

```text
python -m esptool --chip esp32c3 --port COM5 --baud 460800 erase-flash

python -m esptool --chip esp32c3 --port COM5 --baud 460800 write-flash \
  0x0     NUSA_UVK5_ESP32C3_IGATE_REV1A_bootloader.bin \
  0x8000  NUSA_UVK5_ESP32C3_IGATE_REV1A_partitions.bin \
  0xE000  NUSA_UVK5_ESP32C3_IGATE_REV1A_boot_app0.bin \
  0x10000 NUSA_UVK5_ESP32C3_IGATE_REV1A_firmware.bin
```

## Bahasa Indonesia — ESP32-C3

Paket ini untuk board ESP32-C3 yang menyediakan GPIO4 dan GPIO5. Gunakan GPIO4 sebagai RX dari UV-K5 dan GPIO5 sebagai TX ke UV-K5. Protokol UART tetap **38400 8N1** dan kompatibel dengan firmware UV-K5 iGate Normal maupun Standalone.

Perhatikan bahwa address bootloader C3 adalah **0x0**, berbeda dengan ESP32 klasik yang menggunakan **0x1000**.
