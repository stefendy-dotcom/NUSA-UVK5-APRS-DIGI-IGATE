# NUSA AIS FEEDER REV1I — Windows x64 binary

## New functionality
- Saves serial COM port, AIS channel, feeding protocol (UDP/HTTP/TCP), destination host/port, and HTTP URL in %APPDATA%\\NUSA_AIS_FEEDER\\settings.ini.
- Reloads settings at launch and saves on START or close.
- AIS Monitor / Ship Map tabs; marine vessel location markers are based on decoded AIS messages 1/2/3/18/19, with actual AIS MMSI and reported position.
- Map displays OpenStreetMap tiles (requires internet). Supports drag to pan, mouse wheel or buttons to zoom, and Center Ships. Map tiles cached under %LOCALAPPDATA%\\NUSA_AIS_FEEDER.
- Displays up to 256 recently heard vessels in the current app session; not an AIS history database.
- Keeps existing monitor, copy NMEA, export CSV, Win32 System Log, UDP/TCP/HTTP forwarding, COM reconnect.
- Existing radio NUSAAISRX.app unchanged. Only one AIS RF channel active at a time.

## Installation
Close REV1H. Download NUSA_AIS_FEEDER_REV1I.exe and run. First start provides reasonable defaults (COM5, AIS2, UDP listener.marinetraffic.com port 14802), then settings persist for later sessions. Open the Ship Map tab and allow OpenStreetMap tile requests to load; AIS ship markers appear for valid position-bearing messages.

## Caveats
- Compiled successfully with MinGW Windows x64, but not GUI smoke-tested on user's Windows machine. Avoid calling it field-tested until user confirms.
- Map tiles require internet; unavailable tiles are shown as a simple colored background.
- TCP / UDP socket success is NOT MarineTraffic ingest confirmation. Station 57774 acceptance remains unverified.
- Tiles © OpenStreetMap contributors; respect OSM tile usage policy.
