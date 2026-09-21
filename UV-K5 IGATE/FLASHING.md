# Flashing Guide — NUSA UV-K5 iGATE REV1A

This guide covers the UV-K5 firmware and the ESP32 **4-BIN ZIP package**.

# 1. UV-K5 firmware

Use:

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
```

Flash it with a Quansheng UV-K5 compatible updater that supports `.packed.bin`.

Official Quansheng support/download page:

https://en.qsfj.com/support/downloads/3268

Before flashing:

- Charge the UV-K5 battery sufficiently.
- Use a known-good programming cable.
- Keep a recovery firmware available.
- Do not disconnect the cable or remove power while writing.

After flashing, power the radio normally, select the intended APRS VFO/frequency and enter APRS/iGate mode, normally Long F2.

# 2. ESP32 package

Download:

```text
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip
```

Extract it. The ZIP contains exactly four files:

```text
NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

Use these offsets:

```text
0x1000  NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
0x8000  NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
0xE000  NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
0x10000 NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

The package was built with Arduino-ESP32 core 3.3.11 for classic ESP32 Dev Module / ESP32-WROOM-32 class hardware.

## Flash with esptool

Install esptool if required:

```text
python -m pip install esptool
```

Erase first:

```text
python -m esptool --chip esp32 --port COM5 --baud 460800 erase-flash
```

Then flash all four files in one command:

```text
python -m esptool --chip esp32 --port COM5 --baud 460800 write-flash \
  0x1000  NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin \
  0x8000  NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin \
  0xE000  NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin \
  0x10000 NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

Replace `COM5` with the actual ESP32 serial port.

Linux example: use `/dev/ttyUSB0` or the correct device instead of COM5.

If automatic download mode does not work, hold **BOOT**, tap **EN/RESET**, start flashing, then release BOOT when writing begins.

After flashing and resetting:

```text
SSID     : NUSA-IGATE
Password : 12345678
Web      : http://192.168.4.1/
```

# 3. ESP32 Flash Download Tool

If using Espressif's graphical Flash Download Tool, add four rows:

```text
0x1000  bootloader.bin
0x8000  partitions.bin
0xE000  boot_app0.bin
0x10000 firmware.bin
```

Select the corresponding NUSA filenames from the extracted ZIP.

# 4. Bahasa Indonesia — UV-K5

Gunakan:

```text
firmware/NUSA_UVK5_IGATE_REV1A.packed.bin
```

Flash menggunakan updater Quansheng UV-K5 yang mendukung `.packed.bin`.

Pastikan baterai cukup, kabel programming baik, dan jangan memutus daya selama proses flash.

# 5. Bahasa Indonesia — ESP32

Download dan extract:

```text
esp32/NUSA_UVK5_ESP32_IGATE_REV1A_BIN.zip
```

Di dalam ZIP ada tepat empat file `.bin`:

```text
NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin
NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin
NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin
NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

Masukkan address berikut dengan benar:

```text
0x1000  bootloader
0x8000  partitions
0xE000  boot_app0
0x10000 firmware
```

Contoh esptool:

```text
python -m esptool --chip esp32 --port COM5 --baud 460800 erase-flash

python -m esptool --chip esp32 --port COM5 --baud 460800 write-flash \
  0x1000  NUSA_UVK5_ESP32_IGATE_REV1A_bootloader.bin \
  0x8000  NUSA_UVK5_ESP32_IGATE_REV1A_partitions.bin \
  0xE000  NUSA_UVK5_ESP32_IGATE_REV1A_boot_app0.bin \
  0x10000 NUSA_UVK5_ESP32_IGATE_REV1A_firmware.bin
```

Setelah selesai, reset ESP32 kemudian hubungkan ke:

```text
SSID     : NUSA-IGATE
Password : 12345678
Web      : http://192.168.4.1/
```

# 6. Verify downloads

Compare the downloaded package hash with `SHA256SUMS.txt` before flashing.
