# NUSA AIS FEEDER REV1J — Tab layout fix

The Ship Map tab was obscured by the monitor heading, buttons and statistics in REV1I.

- Moves AIS Monitor and Ship Map tabs to a dedicated row immediately below START/STOP.
- Positions the statistics and monitor controls below the tabs.
- Moves table/map contents below their respective tab controls.
- Maintains responsive sizing and the independent System Log.
- Retains saved settings in %APPDATA%\\NUSA_AIS_FEEDER\\settings.ini, all feeding protocols, ship map decoding and OpenStreetMap tiles.
- No UV-K5 V3 radio app replacement needed.

Windows x64 PE binary cross-compiled successfully. GUI layout must still be tested with the user's screen scaling and radio.
