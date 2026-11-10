# NUSA DIGI — UV-K5 V3 F4HWN Labs v6.3.0

This firmware family is **for UV-K5 V3 with F4HWN Labs v6.3.0**, NOT for the UV-K5 V1 DIGI/iGATE builds in other folders.

## Release pairing

- `firmware/F4HWN_LABS_V630_NUSA_DIGI_SAME_CALL_REV1E.bin` — resident F4HWN Labs firmware REV1E
- `apps/NUSA_DIGI_REV1D_30MIN_BEACON.app` — NUSA DIGI overlay app REV1D

Both files are required. Their revision numbers differ because the resident same-callsign fix is in the REV1E main firmware; the REV1D app binary is unchanged. GitHub Release offers the APP in a ZIP because GitHub blocks uploading .app extensions as release assets. The raw .app remains in this repository.

## Features

- APRS WIDEn-N/TRACEn-N digipeater with duplicate suppression
- Same digipeater and position-beacon identity (`YF9UAG-3` on the operator's test station)
- APRS position, symbol, PATH and comment from the APRS TX profile edited in APRS Edit
- 30-minute automatic position-beacon interval; press **3** for manual position beacon
- Auto Resume after power-on, plus hold EXIT on startup to bypass

## Installation

1. Back up the currently working Labs firmware and app.
2. Flash the REV1E `.bin` with the correct UV-K5 V3 flashing method.
3. Install the REV1D `.app` with the compatible Labs App Loader.
4. Install/retain F4HWN APRS TX + APRS Edit, set the real location, symbol, PATH and comment, and use a permitted frequency.
5. Start NUSA DIGI, press **3** and verify the source callsign and coordinates with a second APRS receiver.

## Checksums (SHA-256)

```text
041266862f650b50ef92a04df850a2500f77b0d1d7246d3c6c69c9cf34b96dc8  F4HWN_LABS_V630_NUSA_DIGI_SAME_CALL_REV1E.bin
10592e1469b54a38a68bfd94522fcdc22b14e6e349dec2ec1fec704103b87033  NUSA_DIGI_REV1D_30MIN_BEACON.app
```

## Testing status

The operator confirmed DIGI retransmission and a manual beacon with `YF9UAG-3` and APRS Edit position on the radio. Auto Resume was confirmed on earlier REV1C; the automatic 30-minute beacon and post-upgrade power-cycle on REV1E remain to be field-verified.

Never flash these binaries to UV-K5 V1 or other hardware variants. Keep a working rollback copy.
