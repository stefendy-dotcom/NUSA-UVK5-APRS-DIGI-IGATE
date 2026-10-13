# Building NUSA UV-K5 GPS Tracker REV1D from source

**Hardware target:** UV-K5/K6 V1 DP32G030/BK4819 only.

The checked-in `source/` tree contains the **complete REV1D unified patch relative to upstream**, four new GPS driver/parser source and header files, and two standalone C test programs. The source ZIP also includes the exact tested packed BIN and Apache-2.0 license.

## Apply source

```bash
git clone https://github.com/bcanata/uv-k5-firmware-ta1js.git
cd uv-k5-firmware-ta1js
git checkout 6320cdac7a2dce46f88027d220ff67e47eb0e3fb

# Copy or download source/nusa_uvk5_gps_rev1d.patch from this repository:
git apply nusa_uvk5_gps_rev1d.patch

# Copy all four files under source/app/ to the upstream app/ directory:
#   nusa_gps_core.c
#   nusa_gps_core.h
#   nusa_gps_autobaud.c
#   nusa_gps_autobaud.h

make -j2
```

You need a suitable ARM Cortex-M0 **arm-none-eabi-gcc** toolchain and matching embedded/newlib dependencies (see the upstream build instructions). To pack a raw firmware binary using the original upstream utility, install its `crcmod` dependency and run:

```bash
python3 fw-pack.py f4hwn.bin NUSA GPS1D f4hwn.packed.bin
```

The checked-in `firmware/*.packed.bin` is the **original tested artifact**, not a rebuilt or renamed replacement.

## Native tests

From the folder containing `test_gps.c`, `test_gps_autobaud.c` and `app/` copied from this repository:

```bash
gcc -std=c11 -O1 -g -fsanitize=address,undefined -Wall -Wextra -Werror \
  test_gps.c app/nusa_gps_core.c -o test_gps
./test_gps

gcc -std=c11 -O1 -g -fsanitize=address,undefined -Wall -Wextra -Werror \
  test_gps_autobaud.c app/nusa_gps_autobaud.c app/nusa_gps_core.c -o test_gps_autobaud
./test_gps_autobaud
```

## Firmware compatibility and restrictions

This tracker intentionally disables normal PTT voice transmit at the **application layer**. The independent bootloader is unchanged. It needs an external NMEA UART GPS powered separately, and must not be flashed to V3 radios. See [README](README.md) and [field test](FIELD_TEST.md).
