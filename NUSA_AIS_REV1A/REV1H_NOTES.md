# NUSA AIS FEEDER REV1H

Fix: the System Log was clipped below the Windows taskbar at common screen sizes / DPI scales.

- Main window starts fitted to the Windows work area and is DPI aware.
- AIS monitor table and embedded log reflow on window resize.
- VIEW SYSTEM LOG button is visible above the table.
- New independent, resizable system log window with live entries, Copy All Log, Save Log TXT and Clear Log.
- Every new log line has a local time prefix.
- Retains COM5 and AIS1/2, UDP/TCP/HTTP, Copy NMEA, Copy Selected Row, Export CSV, reset statistics.
- UV-K5 V3 radio .app unchanged.

This is a Windows x64 native cross-build; executable compile and Github upload verified. Native GUI and radio-to-server tests on the user's PC have not been run. TCP/UDP send completion does NOT confirm MarineTraffic's application-layer ingest.
