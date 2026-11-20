# NUSA AIS FEEDER REV1D

Portable Windows 64-bit AIS feeder. Radio uses NUSAAISRX.app (REV1A, unchanged).

REV1D additions:
- Reset session RX/Sent/Failed counters on START.
- Reset Stats button while connected.
- Clear Monitor button preserves counters.
- COM reconnect state and feed transport diagnostic messages.
- Monitor of time, MMSI, message type, destination, status and NMEA sentences.

UDP submitted/queued means Winsock sendto() accepted bytes, not that MarineTraffic received or processed them. For confirmation, inspect MarineTraffic Station Details, Current Status & Statistics and AIS Message Type Statistics. Confirm that the UDP port is the station-assigned port.

Status: Windows cross-compiled successfully; not hardware/live GUI tested. No changes to the radio .app.
