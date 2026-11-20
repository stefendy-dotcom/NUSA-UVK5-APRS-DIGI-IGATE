# NUSA AIS RECEIVER REV1A — binary testing build

- NUSAAISRX.app: compiled overlay app for UV-K5 V3 F4HWN Labs V6.3.0, ABI 1 API min 3, serial capability required, overlay code 4040/4096 bytes.
- NUSA_AIS_FEEDER_REV1A.exe: Windows x64 GUI application, native standalone executable, no Python installation required.
- The .app derives from F4HWN AIS RX source (Apache-2.0); AIS frame payloads are sent over USB CDC as A5 5A LEN PAYLOAD XOR.
- Windows receives serial data at 115200 (USB CDC ignores baud), forwards valid AIS payload as NMEA !AIVDM via UDP or HTTP POST. Longer sentences are fragmented.
- Tune radio before starting app to AIS1 161.975 MHz or AIS2 162.025 MHz; one RX channel at a time.
- Start the radio app, connect the Windows COM port, configure destination, then press START.
- USB port is reserved by the overlay; live K5Viewer cannot use the same port concurrently.
- This is an untested hardware build: compile succeeded but RF receive/USB end-to-end tests not performed.
- The app serial_write function can block up to 50ms; packets arriving during transmission can be missed.
- RX only. Do not transmit on AIS maritime channels.
