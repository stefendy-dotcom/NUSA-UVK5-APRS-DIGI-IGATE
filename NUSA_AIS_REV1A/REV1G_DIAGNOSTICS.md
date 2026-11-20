# NUSA AIS FEEDER REV1G (diagnostic build)

- Windows 64-bit executable. No replacement of the radio NUSAAISRX.app necessary.
- Adds TCP NMEA (diagnostic) choice alongside UDP NMEA and HTTP POST.
- TCP connects to the UDP host/port text fields, using a persistent stream, 3-second connection timeout, and reconnection after send error.
- Validates local NMEA checksum before sending.
- UDP sent and TCP sent report successful local socket operations, not MarineTraffic ingest confirmation.
- MarineTraffic official documentation lists listener.marinetraffic.com / 5.9.207.224 and the account-assigned port; some receivers can use UDP or TCP.
- For Station 57774, account screenshots showed port 14802 and 0 messages; test TCP only as an isolated diagnostic and compare MarineTraffic statistics after a few minutes. Stop the prior version first.
- Source CSV (6 rows from REV1F) passed checksum and decoded positions in Jayapura, but remote server acceptance could not be verified.
- Cross-compiled but not smoke-tested on a user's actual Windows machine; server-side host accepted a TCP connect to 14802 in a separate non-data test.
- No sample AIS data transmitted to MarineTraffic by this build process.
