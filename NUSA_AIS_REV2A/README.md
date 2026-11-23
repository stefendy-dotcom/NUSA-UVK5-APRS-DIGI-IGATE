# NUSA AIS RX REV2A — UV-K5 V3 F4HWN v7

- Based on upstream F4HWN v7.0.0 release commit f6f34cbc3e10dd688f8348cc7f4300a859064621; decoder AIS RX v0.3 source is unchanged in v7 compared to the F4HWN base used by NUSA REV1A.
- NUSA-specific frame export retained: A5 5A LEN PAYLOAD XOR over USB CDC. On a CRC-valid frame, sends payload without FCS.
- Checks APP_SERIAL_OK and APP_SERIAL_HOST (DTR asserted) before attempting USB serial_write to avoid gratuitous blocking if no host opened the port.
- Requires resident serial capability and minimum API level 3; compatible with a v7 firmware that has USB CDC serial API. Requires correct app firmware ABI and overlay address.
- RX only, one AIS channel per radio VFO. Does not change RF DSP tuning or promise increased sensitivity.
- Frame decoding supports AIS types 1,2,3,5,8 inland,18,19,24 per upstream.
- Compiled code 4044/4096 bytes; .app container 4591 bytes. Build succeeded on ARM GCC. Upstream run_tests.sh executed successfully. Hardware USB feeder and radio reception must be retested.
- Install alongside production REV1A temporarily if possible; retain old NUSAAISRX.app as backup and test with Windows feeder before moving to ESP32-S3.
- NUSA RX frames are forwarded even when the on-screen vessel list is on MENU hold.
- F4HWN upstream Apache-2.0 source attribution preserved.
