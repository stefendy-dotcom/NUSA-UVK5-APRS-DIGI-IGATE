/*
  NUSA UV-K5 APRS IGATE REV1A
  ESP32 companion for dedicated UV-K5 IGATE firmware

  MODE
  ----
  RF -> APRS-IS : all valid APRS UI/F0 packets that are safe to gate.
                  qAR for stations heard DIRECT and eligible for messaging.
                  qAO for indirect/non-eligible stations.

  APRS-IS -> RF : MESSAGE ONLY, and ONLY when the exact addressee callsign+SSID
                  was heard DIRECT by this UV-K5 within the last 30 minutes.

  Additional anti-loop rules:
  - no IS->RF if sender was also heard DIRECT by RF within 30 minutes
  - no IS->RF if target was recently seen as an Internet-connected station
  - reject NOGATE/RFONLY/TCPXX
  - third-party RF packet format follows APRS IGate rules
  - q constructs are never transmitted on RF

  UV-K5 UART protocol
  -------------------
  UV-K5 -> ESP32:
    A5 5A 4E 55 01 LEN [raw AX.25 including FCS]

  ESP32 -> UV-K5:
    A5 5A 4E 55 02 LEN [raw AX.25 including FCS]

  UV-K5 -> ESP32 status:
    A5 5A 4E 55 03 01 CODE
      1 = queued
      2 = RF TX completed
      3 = rejected
      4 = radio queue busy
      5 = IS2RF disabled in UV-K5 menu

  Wiring (ESP32 DevKit / WROOM-32)
  --------------------------------
  UV-K5 2.5 mm RING   = UART TX ---- 1k ----> ESP32 GPIO16 RX2
  UV-K5 2.5 mm SLEEVE = GND ---------------- ESP32 GND
  UV-K5 3.5 mm SLEEVE = UART RX <--- 1k ----- ESP32 GPIO17 TX2

  NOT USED / DO NOT CONNECT TO ESP32:
  UV-K5 2.5 mm TIP  = SPK+
  UV-K5 3.5 mm RING = MIC+
  UV-K5 3.5 mm TIP  = radio V+

  Setup AP
  --------
  SSID     : NUSA-IGATE
  Password : 12345678
  Web      : http://192.168.4.1/

  NUSA / YF9UAG
*/

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// Forward declarations required by the Arduino sketch preprocessor.
struct RawAx25Info;
struct Tnc2Packet;

// -----------------------------------------------------------------------------
// Identity / defaults
// -----------------------------------------------------------------------------

static const char *FW_NAME = "NUSA UV-K5 IGATE";
static const char *FW_VER  = "REV1A";

static const char *AP_SSID = "NUSA-IGATE";
static const char *AP_PASS = "12345678";

static const int UVK5_RX_PIN = 16;
static const int UVK5_TX_PIN = 17;
static const uint32_t UVK5_BAUD = 38400;

static const uint32_t DIRECT_HEARD_MS = 30UL * 60UL * 1000UL;
static const uint32_t INTERNET_HEARD_MS = 30UL * 60UL * 1000UL;
static const uint32_t IS_MSG_DUPE_MS = 10000UL;

HardwareSerial RadioSerial(2);
WebServer web(80);
Preferences prefs;
WiFiClient aprsClient;

struct Config {
  String wifiSsid;
  String wifiPass;
  String igateCall;
  String aprsHost;
  uint16_t aprsPort;
  bool is2rf;
};

static Config cfg;

// -----------------------------------------------------------------------------
// Counters / status
// -----------------------------------------------------------------------------

static uint32_t rfFrames = 0;
static uint32_t rfValid = 0;
static uint32_t rfFcsBad = 0;
static uint32_t rfFiltered = 0;
static uint32_t rfDuplicates = 0;
static uint32_t rfQar = 0;
static uint32_t rfQao = 0;
static uint32_t isUploaded = 0;

static uint32_t isPackets = 0;
static uint32_t isMessages = 0;
static uint32_t isRfEligible = 0;
static uint32_t isRfBlocked = 0;
static uint32_t isRfQueued = 0;
static uint32_t isRfTxOk = 0;
static uint32_t isRfTxFail = 0;

static uint32_t aprsConnects = 0;

static String lastRf = "-";
static String lastUpload = "-";
static String lastIs = "-";
static String lastIsRf = "-";
static String lastDrop = "-";
static String lastServer = "-";
static String radioStatus = "WAIT";

static bool aprsVerified = false;
static uint32_t lastWifiAttempt = 0;
static uint32_t lastAprsAttempt = 0;
static bool restartRequested = false;
static uint32_t restartAt = 0;

// -----------------------------------------------------------------------------
// Utility
// -----------------------------------------------------------------------------

static String upperCopy(String s) {
  s.trim();
  s.toUpperCase();
  return s;
}

static String cleanCall(String s) {
  s = upperCopy(s);
  if (s.endsWith("-0")) s.remove(s.length() - 2);
  return s;
}

static bool equalsCall(const String &a, const String &b) {
  return cleanCall(a) == cleanCall(b);
}

static String htmlEsc(const String &s) {
  String o;
  o.reserve(s.length() + 16);
  for (size_t i = 0; i < s.length(); i++) {
    switch (s[i]) {
      case '&': o += F("&amp;"); break;
      case '<': o += F("&lt;"); break;
      case '>': o += F("&gt;"); break;
      case '"': o += F("&quot;"); break;
      default:  o += s[i]; break;
    }
  }
  return o;
}

static uint32_t fnv1a(const String &s) {
  uint32_t h = 2166136261UL;
  for (size_t i = 0; i < s.length(); i++) {
    h ^= (uint8_t)s[i];
    h *= 16777619UL;
  }
  return h;
}

static uint16_t aprsPasscode(String call) {
  call = cleanCall(call);
  int dash = call.indexOf('-');
  if (dash >= 0) call = call.substring(0, dash);

  uint16_t hash = 0x73e2;
  for (size_t i = 0; i < call.length(); i += 2) {
    hash ^= (uint16_t)((uint8_t)call[i]) << 8;
    if (i + 1 < call.length())
      hash ^= (uint8_t)call[i + 1];
  }
  return hash & 0x7fff;
}

static bool parseAx25Call(const String &in, String &base, uint8_t &ssid) {
  String s = cleanCall(in);
  int dash = s.indexOf('-');

  if (dash >= 0) {
    base = s.substring(0, dash);
    String n = s.substring(dash + 1);
    if (!n.length() || n.length() > 2) return false;
    for (size_t i = 0; i < n.length(); i++)
      if (n[i] < '0' || n[i] > '9') return false;
    int v = n.toInt();
    if (v < 0 || v > 15) return false;
    ssid = (uint8_t)v;
  } else {
    base = s;
    ssid = 0;
  }

  if (base.length() < 1 || base.length() > 6) return false;
  for (size_t i = 0; i < base.length(); i++) {
    char c = base[i];
    if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')))
      return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
// Persistent configuration
// -----------------------------------------------------------------------------

static void loadConfig() {
  prefs.begin("nusaigate", true);
  cfg.wifiSsid = prefs.getString("ssid", "");
  cfg.wifiPass = prefs.getString("wpass", "");
  cfg.igateCall = prefs.getString("call", "YF9UAG-3");
  cfg.aprsHost = prefs.getString("host", "asia.aprs2.net");
  cfg.aprsPort = prefs.getUShort("port", 14580);
  cfg.is2rf = prefs.getBool("is2rf", true);
  prefs.end();

  cfg.igateCall = cleanCall(cfg.igateCall);
  if (!cfg.aprsHost.length()) cfg.aprsHost = "asia.aprs2.net";
  if (!cfg.aprsPort) cfg.aprsPort = 14580;
}

static void saveConfig() {
  prefs.begin("nusaigate", false);
  prefs.putString("ssid", cfg.wifiSsid);
  prefs.putString("wpass", cfg.wifiPass);
  prefs.putString("call", cleanCall(cfg.igateCall));
  prefs.putString("host", cfg.aprsHost);
  prefs.putUShort("port", cfg.aprsPort);
  prefs.putBool("is2rf", cfg.is2rf);
  prefs.end();
}

// -----------------------------------------------------------------------------
// DIRECT-heard table
// Exact callsign+SSID only. No SSID collapsing.
// -----------------------------------------------------------------------------

struct HeardSlot {
  String call;
  uint32_t atMs;
};

static const uint8_t HEARD_MAX = 24;
static HeardSlot directHeard[HEARD_MAX];

static int findHeardSlot(const String &call) {
  String c = cleanCall(call);
  for (uint8_t i = 0; i < HEARD_MAX; i++)
    if (directHeard[i].call.length() && directHeard[i].call == c)
      return i;
  return -1;
}

static bool heardDirectRecently(const String &call) {
  int i = findHeardSlot(call);
  if (i < 0) return false;
  return (uint32_t)(millis() - directHeard[i].atMs) <= DIRECT_HEARD_MS;
}

static void purgeOldHeard() {
  uint32_t now = millis();
  for (uint8_t i = 0; i < HEARD_MAX; i++) {
    if (directHeard[i].call.length() &&
        (uint32_t)(now - directHeard[i].atMs) > DIRECT_HEARD_MS) {
      directHeard[i].call = "";
      directHeard[i].atMs = 0;
    }
  }
}

static bool filterDirty = true;
static uint32_t lastFilterPush = 0;

static void markDirectHeard(const String &call) {
  String c = cleanCall(call);
  if (!c.length() || equalsCall(c, cfg.igateCall)) return;

  uint32_t now = millis();
  int slot = findHeardSlot(c);
  if (slot >= 0) {
    directHeard[slot].atMs = now;
    return;
  }

  int empty = -1;
  uint8_t oldest = 0;
  uint32_t oldestAge = 0;

  for (uint8_t i = 0; i < HEARD_MAX; i++) {
    if (!directHeard[i].call.length()) {
      empty = i;
      break;
    }
    uint32_t age = (uint32_t)(now - directHeard[i].atMs);
    if (age >= oldestAge) {
      oldestAge = age;
      oldest = i;
    }
  }

  uint8_t use = (empty >= 0) ? (uint8_t)empty : oldest;
  directHeard[use].call = c;
  directHeard[use].atMs = now;
  filterDirty = true;
}

// -----------------------------------------------------------------------------
// Internet-heard table
// Used only as an extra anti-loop criterion for IS->RF.
// -----------------------------------------------------------------------------

static const uint8_t NET_MAX = 16;
static HeardSlot internetHeard[NET_MAX];

static void markInternetHeard(const String &call) {
  String c = cleanCall(call);
  if (!c.length() || equalsCall(c, cfg.igateCall)) return;

  uint32_t now = millis();
  int empty = -1;
  uint8_t oldest = 0;
  uint32_t oldestAge = 0;

  for (uint8_t i = 0; i < NET_MAX; i++) {
    if (internetHeard[i].call == c) {
      internetHeard[i].atMs = now;
      return;
    }
    if (!internetHeard[i].call.length() && empty < 0) empty = i;
    uint32_t age = (uint32_t)(now - internetHeard[i].atMs);
    if (age >= oldestAge) {
      oldestAge = age;
      oldest = i;
    }
  }

  uint8_t use = (empty >= 0) ? (uint8_t)empty : oldest;
  internetHeard[use].call = c;
  internetHeard[use].atMs = now;
}

static bool heardInternetRecently(const String &call) {
  String c = cleanCall(call);
  uint32_t now = millis();
  for (uint8_t i = 0; i < NET_MAX; i++) {
    if (internetHeard[i].call == c &&
        (uint32_t)(now - internetHeard[i].atMs) <= INTERNET_HEARD_MS)
      return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
// Duplicate filters
// -----------------------------------------------------------------------------

struct DupeSlot {
  uint32_t hash;
  uint32_t atMs;
};

static const uint8_t RF_DUPE_MAX = 24;
static DupeSlot rfDupes[RF_DUPE_MAX];
static uint8_t rfDupeNext = 0;

static bool isRfDuplicate(const String &s) {
  const uint32_t now = millis();
  const uint32_t h = fnv1a(s);
  for (uint8_t i = 0; i < RF_DUPE_MAX; i++) {
    if (rfDupes[i].hash == h &&
        (uint32_t)(now - rfDupes[i].atMs) < 30000UL)
      return true;
  }
  rfDupes[rfDupeNext].hash = h;
  rfDupes[rfDupeNext].atMs = now;
  rfDupeNext = (uint8_t)((rfDupeNext + 1) % RF_DUPE_MAX);
  return false;
}

static const uint8_t MSG_DUPE_MAX = 16;
static DupeSlot msgDupes[MSG_DUPE_MAX];
static uint8_t msgDupeNext = 0;

static bool isIsMessageDuplicate(const String &s) {
  const uint32_t now = millis();
  const uint32_t h = fnv1a(s);
  for (uint8_t i = 0; i < MSG_DUPE_MAX; i++) {
    if (msgDupes[i].hash == h &&
        (uint32_t)(now - msgDupes[i].atMs) < IS_MSG_DUPE_MS)
      return true;
  }
  msgDupes[msgDupeNext].hash = h;
  msgDupes[msgDupeNext].atMs = now;
  msgDupeNext = (uint8_t)((msgDupeNext + 1) % MSG_DUPE_MAX);
  return false;
}

// -----------------------------------------------------------------------------
// AX.25 helpers
// -----------------------------------------------------------------------------

static uint16_t ax25Fcs(const uint8_t *data, size_t len) {
  uint16_t crc = 0xffff;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; b++)
      crc = (crc & 1) ? (uint16_t)((crc >> 1) ^ 0x8408)
                      : (uint16_t)(crc >> 1);
  }
  return (uint16_t)~crc;
}

static String decodeAx25Addr(const uint8_t *a, bool digi) {
  String c;
  c.reserve(10);

  for (uint8_t i = 0; i < 6; i++) {
    char ch = (char)(a[i] >> 1);
    if (ch >= 32 && ch <= 126) c += ch;
  }
  while (c.endsWith(" ")) c.remove(c.length() - 1);

  uint8_t ssid = (a[6] >> 1) & 0x0f;
  if (ssid) {
    c += '-';
    c += String(ssid);
  }
  if (digi && (a[6] & 0x80)) c += '*';

  return cleanCall(c);
}

static void putAx25Addr(uint8_t *out, const String &callIn, bool end) {
  String base;
  uint8_t ssid = 0;
  if (!parseAx25Call(callIn, base, ssid)) {
    base = "NOCALL";
    ssid = 0;
  }

  for (uint8_t i = 0; i < 6; i++) {
    char c = (i < base.length()) ? base[i] : ' ';
    out[i] = (uint8_t)((uint8_t)c << 1);
  }

  out[6] = (uint8_t)(0x60 | ((ssid & 0x0f) << 1));
  if (end) out[6] |= 0x01;
}

static String pathRoot(String p) {
  p.trim();
  if (p.endsWith("*")) p.remove(p.length() - 1);
  p.toUpperCase();
  int dash = p.indexOf('-');
  if (dash >= 0) p = p.substring(0, dash);
  return p;
}

static bool forbiddenRfPathWord(const String &p) {
  String r = pathRoot(p);
  return r == "TCPIP" || r == "TCPXX" || r == "NOGATE" || r == "RFONLY";
}

static bool containsNoGateWord(const String &header) {
  String s = upperCopy(header);
  return s.indexOf(",NOGATE") >= 0 ||
         s.indexOf(",RFONLY") >= 0 ||
         s.indexOf(",TCPXX") >= 0;
}

static bool containsInternetOriginWord(const String &header) {
  String s = upperCopy(header);
  return s.indexOf(",TCPIP*") >= 0 ||
         s.indexOf(",TCPXX*") >= 0;
}

static bool containsQConstruct(const String &header) {
  // q constructs must never originate on RF.
  return header.indexOf(",qA") >= 0 ||
         header.indexOf(",QA") >= 0;
}

struct RawAx25Info {
  String src;
  String dst;
  String path;
  String info;
  bool direct;
  bool thirdParty;
};

static bool decodeRawAx25(const uint8_t *f, uint8_t len, RawAx25Info &r) {
  if (len < 18) return false;

  uint16_t stored = (uint16_t)f[len - 2] | ((uint16_t)f[len - 1] << 8);
  uint16_t calc = ax25Fcs(f, len - 2);
  if (stored != calc) {
    rfFcsBad++;
    lastDrop = "RF FCS bad";
    return false;
  }

  uint16_t pos = 0;
  uint8_t addrCount = 0;
  bool ended = false;
  bool anyUsedVia = false;

  r.path = "";
  r.direct = true;
  r.thirdParty = false;

  while (pos + 7 <= (uint16_t)(len - 4) && addrCount < 10) {
    bool last = (f[pos + 6] & 0x01) != 0;
    bool used = (f[pos + 6] & 0x80) != 0;
    String a = decodeAx25Addr(&f[pos], addrCount >= 2);

    if (addrCount == 0) r.dst = a;
    else if (addrCount == 1) r.src = a;
    else {
      if (used) anyUsedVia = true;
      if (forbiddenRfPathWord(a)) {
        rfFiltered++;
        lastDrop = "RF forbidden path " + a;
        return false;
      }
      if (r.path.length()) r.path += ',';
      r.path += a;
    }

    pos += 7;
    addrCount++;
    if (last) {
      ended = true;
      break;
    }
  }

  if (!ended || addrCount < 2 || pos + 2 > (uint16_t)(len - 2))
    return false;

  if (f[pos] != 0x03 || f[pos + 1] != 0xf0) {
    rfFiltered++;
    lastDrop = "RF not UI/F0";
    return false;
  }
  pos += 2;

  r.info = "";
  r.info.reserve(len - 2 - pos);
  for (uint16_t i = pos; i < (uint16_t)(len - 2); i++) {
    char c = (char)f[i];
    if (c == '\r' || c == '\n') c = ' ';
    r.info += c;
  }

  if (r.info.length() && r.info[0] == '?') {
    rfFiltered++;
    lastDrop = "RF generic query";
    return false;
  }

  r.thirdParty = r.info.length() && r.info[0] == '}';
  r.direct = !anyUsedVia && !r.thirdParty;

  return true;
}

static String rawToTnc2Base(const RawAx25Info &r) {
  String line = r.src + ">" + r.dst;
  if (r.path.length()) line += "," + r.path;
  line += ":" + r.info;
  return line;
}

// Strip outer RF header for legal non-Internet 3rd-party packets.
static bool normalizeThirdParty(const RawAx25Info &r, String &base) {
  if (!r.thirdParty) {
    base = rawToTnc2Base(r);
    return true;
  }

  String inner = r.info.substring(1);
  int gt = inner.indexOf('>');
  int col = inner.indexOf(':');
  if (gt < 1 || col <= gt) {
    rfFiltered++;
    lastDrop = "bad RF 3rd-party";
    return false;
  }

  String h = inner.substring(0, col);
  String hu = upperCopy(h);
  if (hu.indexOf(",TCPIP") >= 0 || hu.indexOf(",TCPXX") >= 0 ||
      hu.indexOf(",NOGATE") >= 0 || hu.indexOf(",RFONLY") >= 0 ||
      containsQConstruct(h)) {
    rfFiltered++;
    lastDrop = "Internet 3rd-party loop blocked";
    return false;
  }

  String info = inner.substring(col + 1);
  if (info.length() && info[0] == '?') {
    rfFiltered++;
    lastDrop = "3rd-party query";
    return false;
  }

  base = inner;
  return true;
}

// -----------------------------------------------------------------------------
// RF -> APRS-IS queue
// -----------------------------------------------------------------------------

static const uint8_t ISQ_MAX = 12;
static String isQueue[ISQ_MAX];
static uint8_t isQHead = 0, isQTail = 0, isQCount = 0;

static bool enqueueIs(const String &s) {
  if (isQCount >= ISQ_MAX) {
    lastDrop = "APRS-IS queue full";
    return false;
  }
  isQueue[isQTail] = s;
  isQTail = (uint8_t)((isQTail + 1) % ISQ_MAX);
  isQCount++;
  return true;
}

static bool dequeueIs(String &s) {
  if (!isQCount) return false;
  s = isQueue[isQHead];
  isQueue[isQHead] = "";
  isQHead = (uint8_t)((isQHead + 1) % ISQ_MAX);
  isQCount--;
  return true;
}

// -----------------------------------------------------------------------------
// APRS-IS -> RF queue
// -----------------------------------------------------------------------------

struct RadioTxItem {
  uint8_t frame[160];
  uint8_t len;
  uint8_t tries;
  String desc;
};

static const uint8_t RTX_MAX = 6;
static RadioTxItem rtx[RTX_MAX];
static uint8_t rtxHead = 0, rtxTail = 0, rtxCount = 0;

static bool radioAwaiting = false;
static bool radioAccepted = false;
static uint32_t radioSentAt = 0;
static uint32_t radioRetryAt = 0;

static bool enqueueRadio(const uint8_t *frame, uint8_t len, const String &desc) {
  if (!len || len > sizeof(rtx[0].frame) || rtxCount >= RTX_MAX) {
    lastDrop = "IS->RF queue full/oversize";
    isRfTxFail++;
    return false;
  }

  RadioTxItem &q = rtx[rtxTail];
  memcpy(q.frame, frame, len);
  q.len = len;
  q.tries = 0;
  q.desc = desc;

  rtxTail = (uint8_t)((rtxTail + 1) % RTX_MAX);
  rtxCount++;
  isRfQueued++;
  return true;
}

static void popRadio() {
  if (!rtxCount) return;
  rtx[rtxHead].len = 0;
  rtx[rtxHead].tries = 0;
  rtx[rtxHead].desc = "";
  rtxHead = (uint8_t)((rtxHead + 1) % RTX_MAX);
  rtxCount--;
}

static void sendRadioFront() {
  if (!rtxCount || radioAwaiting) return;
  if ((int32_t)(millis() - radioRetryAt) < 0) return;

  RadioTxItem &q = rtx[rtxHead];
  if (q.tries >= 3) {
    lastDrop = "UV-K5 no response after 3 tries";
    isRfTxFail++;
    popRadio();
    return;
  }

  const uint8_t h[6] = { 0xA5, 0x5A, 0x4E, 0x55, 0x02, q.len };
  RadioSerial.write(h, sizeof(h));
  RadioSerial.write(q.frame, q.len);
  RadioSerial.flush();

  q.tries++;
  radioAwaiting = true;
  radioAccepted = false;
  radioSentAt = millis();
  radioStatus = "SENT";
}

static void handleRadioStatus(uint8_t code) {
  switch (code) {
    case 1:
      radioAccepted = true;
      radioStatus = "QUEUED";
      break;

    case 2:
      radioStatus = "TX OK";
      radioAwaiting = false;
      radioAccepted = false;
      if (rtxCount) {
        lastIsRf = rtx[rtxHead].desc;
        popRadio();
      }
      isRfTxOk++;
      radioRetryAt = millis() + 1000UL;
      break;

    case 3:
      radioStatus = "REJECT";
      radioAwaiting = false;
      radioAccepted = false;
      lastDrop = "UV-K5 rejected IS->RF frame";
      isRfTxFail++;
      if (rtxCount) popRadio();
      radioRetryAt = millis() + 1000UL;
      break;

    case 4:
      radioStatus = "BUSY";
      radioAwaiting = false;
      radioAccepted = false;
      radioRetryAt = millis() + 2000UL;
      break;

    case 5:
      radioStatus = "IS2RF OFF";
      radioAwaiting = false;
      radioAccepted = false;
      lastDrop = "UV-K5 menu IS2RF=OFF";
      isRfTxFail++;
      if (rtxCount) popRadio();
      radioRetryAt = millis() + 2000UL;
      break;

    default:
      radioStatus = "STATUS ?";
      break;
  }
}

static void maintainRadioTx() {
  if (radioAwaiting) {
    uint32_t timeout = radioAccepted ? 20000UL : 8000UL;
    if ((uint32_t)(millis() - radioSentAt) > timeout) {
      radioAwaiting = false;
      radioAccepted = false;
      radioStatus = "TIMEOUT";
      radioRetryAt = millis() + 2000UL;
    }
    return;
  }
  sendRadioFront();
}

// -----------------------------------------------------------------------------
// APRS-IS connection
// -----------------------------------------------------------------------------

static void closeAprs() {
  aprsClient.stop();
  aprsVerified = false;
  filterDirty = true;
}

static void pushDirectMessageFilter() {
  if (!aprsClient.connected() || !aprsVerified) return;

  purgeOldHeard();

  String cmd = "#filter";
  bool any = false;

  for (uint8_t i = 0; i < HEARD_MAX; i++) {
    if (!directHeard[i].call.length()) continue;
    if ((uint32_t)(millis() - directHeard[i].atMs) > DIRECT_HEARD_MS) continue;

    if (!any) {
      cmd += " g/";
      any = true;
    } else {
      cmd += '/';
    }
    cmd += directHeard[i].call;

    // Keep command comfortably below APRS-IS line limits.
    if (cmd.length() > 430) break;
  }

  if (!any) cmd = "#filter default";
  cmd += "\r\n";
  aprsClient.print(cmd);

  filterDirty = false;
  lastFilterPush = millis();
}

static void startAprsConnect() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (cfg.igateCall.length() < 3) return;

  closeAprs();

  if (!aprsClient.connect(cfg.aprsHost.c_str(), cfg.aprsPort)) {
    lastServer = "connect failed";
    return;
  }

  aprsClient.setNoDelay(true);
  aprsConnects++;

  String login = "user " + cleanCall(cfg.igateCall)
               + " pass " + String(aprsPasscode(cfg.igateCall))
               + " vers NUSA-UVK5-IGATE 1.0\r\n";

  aprsClient.print(login);
  lastServer = "login sent";
}

static bool sendAprsLineNow(const String &line) {
  if (!aprsClient.connected() || !aprsVerified) return false;
  if (line.length() > 509) return false;

  size_t n1 = aprsClient.print(line);
  size_t n2 = aprsClient.print("\r\n");

  if (n1 != line.length() || n2 != 2) {
    closeAprs();
    return false;
  }

  isUploaded++;
  lastUpload = line;
  return true;
}

static void flushIsQueue() {
  if (!aprsClient.connected() || !aprsVerified) return;

  String s;
  uint8_t n = 0;
  while (isQCount && n < 3) {
    if (!dequeueIs(s)) break;
    if (!sendAprsLineNow(s)) {
      enqueueIs(s);
      break;
    }
    n++;
  }
}

// -----------------------------------------------------------------------------
// TNC2 parser for APRS-IS
// -----------------------------------------------------------------------------

struct Tnc2Packet {
  String src;
  String dst;
  String header;
  String info;
};

static bool parseTnc2(const String &line, Tnc2Packet &p) {
  int gt = line.indexOf('>');
  int col = line.indexOf(':');
  if (gt < 1 || col <= gt + 1) return false;

  p.src = cleanCall(line.substring(0, gt));
  p.header = line.substring(gt + 1, col);
  p.info = line.substring(col + 1);

  int comma = p.header.indexOf(',');
  p.dst = cleanCall((comma >= 0) ? p.header.substring(0, comma) : p.header);

  return p.src.length() && p.dst.length();
}

static bool getMessageAddressee(const String &info, String &target) {
  if (info.length() < 11 || info[0] != ':' || info[10] != ':')
    return false;

  target = info.substring(1, 10);
  target.trim();
  target = cleanCall(target);
  return target.length() > 0;
}

static bool senderBlockedByHeader(const String &header) {
  String h = upperCopy(header);
  return h.indexOf(",TCPXX") >= 0 ||
         h.indexOf(",NOGATE") >= 0 ||
         h.indexOf(",RFONLY") >= 0;
}

// -----------------------------------------------------------------------------
// Build mandatory APRS 3rd-party packet for Internet -> RF.
// Outer RF path is DIRECT because target was heard DIRECT.
// -----------------------------------------------------------------------------

static bool buildThirdPartyRf(const Tnc2Packet &p, uint8_t *out, uint8_t &outLen) {
  String igBase;
  uint8_t igSsid;
  if (!parseAx25Call(cfg.igateCall, igBase, igSsid)) {
    lastDrop = "Invalid iGate AX.25 callsign";
    return false;
  }

  String inner = p.src + ">" + p.dst
               + ",TCPIP," + cleanCall(cfg.igateCall) + "*:"
               + p.info;

  String third = "}" + inner;

  size_t needed = 14 + 2 + third.length() + 2;
  if (needed > 160) {
    lastDrop = "IS->RF message too long";
    return false;
  }

  uint8_t n = 0;
  putAx25Addr(&out[n], "APRS", false); n += 7;
  putAx25Addr(&out[n], cfg.igateCall, true); n += 7;

  out[n++] = 0x03;
  out[n++] = 0xF0;

  for (size_t i = 0; i < third.length(); i++)
    out[n++] = (uint8_t)third[i];

  uint16_t fcs = ax25Fcs(out, n);
  out[n++] = (uint8_t)fcs;
  out[n++] = (uint8_t)(fcs >> 8);

  outLen = n;
  return true;
}

// -----------------------------------------------------------------------------
// Internet -> RF decision.
// -----------------------------------------------------------------------------

static void handleAprsIsPacket(const String &line) {
  isPackets++;
  lastIs = line;

  Tnc2Packet p;
  if (!parseTnc2(line, p)) return;

  // Extra anti-loop memory: a station originating with TCPIP* is Internet-heard.
  if (containsInternetOriginWord(p.header))
    markInternetHeard(p.src);

  String target;
  if (!getMessageAddressee(p.info, target))
    return;

  isMessages++;

  if (!cfg.is2rf) {
    isRfBlocked++;
    lastDrop = "ESP32 IS->RF disabled";
    return;
  }

  if (equalsCall(target, cfg.igateCall)) {
    isRfBlocked++;
    lastDrop = "message addressed to iGate itself";
    return;
  }

  // User requirement: exact callsign+SSID must be heard DIRECT <= 30 minutes.
  if (!heardDirectRecently(target)) {
    isRfBlocked++;
    lastDrop = "target not DIRECT-heard <=30 min: " + target;
    return;
  }

  // APRS IGate anti-loop criteria.
  if (heardDirectRecently(p.src)) {
    isRfBlocked++;
    lastDrop = "sender also local RF: " + p.src;
    return;
  }

  if (heardInternetRecently(target)) {
    isRfBlocked++;
    lastDrop = "target recently Internet-heard: " + target;
    return;
  }

  if (senderBlockedByHeader(p.header)) {
    isRfBlocked++;
    lastDrop = "sender header blocks RF gate";
    return;
  }

  if (isIsMessageDuplicate(line)) {
    isRfBlocked++;
    lastDrop = "IS message duplicate <10s";
    return;
  }

  uint8_t frame[160];
  uint8_t len = 0;
  if (!buildThirdPartyRf(p, frame, len)) {
    isRfBlocked++;
    return;
  }

  String desc = p.src + " -> " + target + " : " + p.info;
  if (!enqueueRadio(frame, len, desc)) {
    isRfBlocked++;
    return;
  }

  isRfEligible++;
}

// -----------------------------------------------------------------------------
// RF -> APRS-IS processing
// -----------------------------------------------------------------------------

static void processRfFrame(const uint8_t *f, uint8_t len) {
  rfFrames++;

  RawAx25Info r;
  if (!decodeRawAx25(f, len, r)) return;

  String base;
  if (!normalizeThirdParty(r, base)) return;

  rfValid++;
  lastRf = base;

  // Only an actual direct RF source becomes eligible for IS->RF messaging.
  if (r.direct)
    markDirectHeard(r.src);

  // Duplicate check happens AFTER direct-heard timestamp refresh.
  if (isRfDuplicate(base)) {
    rfDuplicates++;
    return;
  }

  int col = base.indexOf(':');
  if (col < 1) return;

  String head = base.substring(0, col);
  String body = base.substring(col);

  if (containsQConstruct(head)) {
    rfFiltered++;
    lastDrop = "q construct found on RF";
    return;
  }

  const bool msgCapable = r.direct && !r.thirdParty;

  String gated = head
               + (msgCapable ? ",qAR," : ",qAO,")
               + cleanCall(cfg.igateCall)
               + body;

  if (msgCapable) rfQar++;
  else rfQao++;

  if (!sendAprsLineNow(gated))
    enqueueIs(gated);
}

// -----------------------------------------------------------------------------
// UV-K5 UART parser
// -----------------------------------------------------------------------------

static uint8_t uartState = 0;
static uint8_t uartType = 0;
static uint8_t uartLen = 0;
static uint8_t uartPos = 0;
static uint8_t uartPayload[160];

static void uartRestart(uint8_t b) {
  uartState = (b == 0xA5) ? 1 : 0;
  uartType = uartLen = uartPos = 0;
}

static void feedRadioByte(uint8_t b) {
  switch (uartState) {
    case 0:
      if (b == 0xA5) uartState = 1;
      break;

    case 1:
      if (b == 0x5A) uartState = 2;
      else uartRestart(b);
      break;

    case 2:
      if (b == 0x4E) uartState = 3;
      else uartRestart(b);
      break;

    case 3:
      if (b == 0x55) uartState = 4;
      else uartRestart(b);
      break;

    case 4:
      if (b == 0x01 || b == 0x03) {
        uartType = b;
        uartState = 5;
      } else {
        uartRestart(b);
      }
      break;

    case 5:
      uartLen = b;
      uartPos = 0;
      if (!uartLen || uartLen > sizeof(uartPayload)) {
        uartState = 0;
      } else {
        uartState = 6;
      }
      break;

    case 6:
      uartPayload[uartPos++] = b;
      if (uartPos >= uartLen) {
        if (uartType == 0x01)
          processRfFrame(uartPayload, uartLen);
        else if (uartType == 0x03 && uartLen == 1)
          handleRadioStatus(uartPayload[0]);

        uartState = 0;
        uartType = uartLen = uartPos = 0;
      }
      break;

    default:
      uartState = 0;
      break;
  }
}

static void pollRadio() {
  while (RadioSerial.available())
    feedRadioByte((uint8_t)RadioSerial.read());
}

// -----------------------------------------------------------------------------
// APRS-IS input / maintenance
// -----------------------------------------------------------------------------

static void drainAprsServer() {
  while (aprsClient.connected() && aprsClient.available()) {
    String s = aprsClient.readStringUntil('\n');
    s.trim();
    if (!s.length()) continue;

    if (s[0] == '#') {
      lastServer = s;

      if (s.startsWith("# logresp")) {
        String lo = s;
        lo.toLowerCase();
        if (lo.indexOf("unverified") >= 0) {
          aprsVerified = false;
        } else if (lo.indexOf("verified") >= 0) {
          aprsVerified = true;
          filterDirty = true;
        }
      }
      continue;
    }

    if (aprsVerified)
      handleAprsIsPacket(s);
  }
}

static void maintainAprs() {
  if (WiFi.status() != WL_CONNECTED) {
    if (aprsClient.connected()) closeAprs();
    return;
  }

  if (!aprsClient.connected()) {
    uint32_t now = millis();
    if ((uint32_t)(now - lastAprsAttempt) >= 10000UL) {
      lastAprsAttempt = now;
      startAprsConnect();
    }
    return;
  }

  drainAprsServer();

  if (aprsVerified) {
    flushIsQueue();

    if (filterDirty ||
        (uint32_t)(millis() - lastFilterPush) >= 60000UL) {
      pushDirectMessageFilter();
    }
  }
}

// -----------------------------------------------------------------------------
// Wi-Fi
// -----------------------------------------------------------------------------

static void startWifi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoReconnect(true);
  WiFi.softAP(AP_SSID, AP_PASS);

  if (cfg.wifiSsid.length()) {
    WiFi.begin(cfg.wifiSsid.c_str(), cfg.wifiPass.c_str());
    lastWifiAttempt = millis();
  }
}

static void maintainWifi() {
  if (!cfg.wifiSsid.length()) return;
  if (WiFi.status() == WL_CONNECTED) return;

  uint32_t now = millis();
  if ((uint32_t)(now - lastWifiAttempt) >= 15000UL) {
    lastWifiAttempt = now;
    WiFi.disconnect(false, false);
    WiFi.begin(cfg.wifiSsid.c_str(), cfg.wifiPass.c_str());
  }
}

// -----------------------------------------------------------------------------
// Web UI
// -----------------------------------------------------------------------------

static String directHeardHtml() {
  purgeOldHeard();

  String h;
  h.reserve(1800);
  h += F("<table><tr><th>Call</th><th>Age</th><th>IS-&gt;RF</th></tr>");

  bool any = false;
  uint32_t now = millis();

  for (uint8_t i = 0; i < HEARD_MAX; i++) {
    if (!directHeard[i].call.length()) continue;
    uint32_t age = (uint32_t)(now - directHeard[i].atMs);
    if (age > DIRECT_HEARD_MS) continue;

    any = true;
    h += F("<tr><td><b>");
    h += htmlEsc(directHeard[i].call);
    h += F("</b></td><td>");
    h += String(age / 60000UL);
    h += F(" min</td><td class='ok'>ELIGIBLE</td></tr>");
  }

  if (!any)
    h += F("<tr><td colspan='3'>Belum ada station DIRECT dalam 30 menit.</td></tr>");

  h += F("</table>");
  return h;
}

static void handleRoot() {
  String h;
  h.reserve(10000);

  h += F("<!doctype html><html><head><meta charset='utf-8'>");
  h += F("<meta name='viewport' content='width=device-width,initial-scale=1'>");
  h += F("<title>NUSA UV-K5 IGATE</title><style>");
  h += F("body{font-family:Arial,sans-serif;background:#10151c;color:#eaf2fa;margin:0;padding:18px}");
  h += F(".wrap{max-width:900px;margin:auto}.card{background:#19232e;border:1px solid #314254;border-radius:14px;padding:16px;margin:12px 0}");
  h += F("h1{font-size:24px;margin:0 0 4px}h2{font-size:17px;color:#9fd2ff;margin:0 0 12px}");
  h += F(".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(170px,1fr));gap:9px}");
  h += F(".stat{background:#111922;border-radius:10px;padding:10px}.k{font-size:12px;color:#8da4b8}.v{font-size:16px;font-weight:bold;margin-top:3px;word-break:break-word}");
  h += F(".ok{color:#7ee787}.bad{color:#ff9b9b}.warn{color:#ffd479}.small{font-size:12px;color:#91a6b8}");
  h += F("label{display:block;color:#9fb2c4;font-size:13px;margin-top:10px}input{width:100%;box-sizing:border-box;padding:10px;border-radius:8px;border:1px solid #42566b;background:#0f171f;color:white}");
  h += F("input[type=checkbox]{width:auto}button{margin-top:14px;padding:11px 17px;border:0;border-radius:9px;font-weight:bold;background:#dbefff;color:#0d2235}");
  h += F("code{white-space:pre-wrap;word-break:break-word;color:#c8f0c8}table{width:100%;border-collapse:collapse}th,td{text-align:left;padding:8px;border-bottom:1px solid #314254}th{color:#9fd2ff}");
  h += F("</style></head><body><div class='wrap'>");

  h += F("<h1>NUSA UV-K5 IGATE</h1><div class='small'>REV1A &bull; RF &harr; APRS-IS &bull; IS-&gt;RF message-only, DIRECT heard 30 min</div>");

  h += F("<div class='card'><h2>Status</h2><div class='grid'>");

  h += F("<div class='stat'><div class='k'>Wi-Fi</div><div class='v ");
  h += (WiFi.status() == WL_CONNECTED ? "ok'>" : "bad'>");
  h += (WiFi.status() == WL_CONNECTED ? htmlEsc(WiFi.localIP().toString()) : "DISCONNECTED");
  h += F("</div></div>");

  h += F("<div class='stat'><div class='k'>APRS-IS</div><div class='v ");
  h += (aprsVerified ? "ok'>VERIFIED" : "bad'>NOT VERIFIED");
  h += F("</div></div>");

  h += F("<div class='stat'><div class='k'>iGate</div><div class='v'>");
  h += htmlEsc(cfg.igateCall);
  h += F("</div></div>");

  h += F("<div class='stat'><div class='k'>UV-K5 UART</div><div class='v'>");
  h += htmlEsc(radioStatus);
  h += F("</div></div>");

  h += F("<div class='stat'><div class='k'>IS-&gt;RF</div><div class='v ");
  h += (cfg.is2rf ? "ok'>ENABLED" : "warn'>DISABLED");
  h += F("</div></div>");

  h += F("<div class='stat'><div class='k'>Radio TX queue</div><div class='v'>");
  h += String(rtxCount);
  h += F("</div></div>");

  h += F("</div></div>");

  h += F("<div class='card'><h2>RF &rarr; APRS-IS</h2><div class='grid'>");
  h += "<div class='stat'><div class='k'>RF frames</div><div class='v'>" + String(rfFrames) + "</div></div>";
  h += "<div class='stat'><div class='k'>RF valid</div><div class='v'>" + String(rfValid) + "</div></div>";
  h += "<div class='stat'><div class='k'>qAR direct</div><div class='v'>" + String(rfQar) + "</div></div>";
  h += "<div class='stat'><div class='k'>qAO indirect</div><div class='v'>" + String(rfQao) + "</div></div>";
  h += "<div class='stat'><div class='k'>Uploaded</div><div class='v'>" + String(isUploaded) + "</div></div>";
  h += "<div class='stat'><div class='k'>RF duplicates</div><div class='v'>" + String(rfDuplicates) + "</div></div>";
  h += F("</div></div>");

  h += F("<div class='card'><h2>APRS-IS &rarr; RF Message</h2><div class='grid'>");
  h += "<div class='stat'><div class='k'>IS packets</div><div class='v'>" + String(isPackets) + "</div></div>";
  h += "<div class='stat'><div class='k'>Messages</div><div class='v'>" + String(isMessages) + "</div></div>";
  h += "<div class='stat'><div class='k'>Eligible</div><div class='v'>" + String(isRfEligible) + "</div></div>";
  h += "<div class='stat'><div class='k'>Blocked</div><div class='v'>" + String(isRfBlocked) + "</div></div>";
  h += "<div class='stat'><div class='k'>RF TX OK</div><div class='v'>" + String(isRfTxOk) + "</div></div>";
  h += "<div class='stat'><div class='k'>RF TX fail</div><div class='v'>" + String(isRfTxFail) + "</div></div>";
  h += F("</div></div>");

  h += F("<div class='card'><h2>DIRECT heard &le; 30 minutes</h2>");
  h += directHeardHtml();
  h += F("</div>");

  h += F("<div class='card'><h2>Last activity</h2>");
  h += F("<div class='k'>Last RF</div><code>"); h += htmlEsc(lastRf); h += F("</code><br><br>");
  h += F("<div class='k'>Last RF -&gt; IS</div><code>"); h += htmlEsc(lastUpload); h += F("</code><br><br>");
  h += F("<div class='k'>Last APRS-IS</div><code>"); h += htmlEsc(lastIs); h += F("</code><br><br>");
  h += F("<div class='k'>Last IS -&gt; RF</div><code>"); h += htmlEsc(lastIsRf); h += F("</code><br><br>");
  h += F("<div class='k'>Last drop/block</div><code>"); h += htmlEsc(lastDrop); h += F("</code><br><br>");
  h += F("<div class='k'>Server</div><code>"); h += htmlEsc(lastServer); h += F("</code></div>");

  h += F("<div class='card'><h2>Configuration</h2><form method='POST' action='/save'>");
  h += F("<label>Wi-Fi SSID</label><input name='ssid' value='"); h += htmlEsc(cfg.wifiSsid); h += F("'>");
  h += F("<label>Wi-Fi Password</label><input type='password' name='wpass' value='"); h += htmlEsc(cfg.wifiPass); h += F("'>");
  h += F("<label>iGate Callsign-SSID (harus sama dengan IgCall + IgSSID di UV-K5)</label><input name='call' value='"); h += htmlEsc(cfg.igateCall); h += F("'>");
  h += F("<label>APRS-IS Server</label><input name='host' value='"); h += htmlEsc(cfg.aprsHost); h += F("'>");
  h += F("<label>APRS-IS Port</label><input type='number' name='port' min='1' max='65535' value='"); h += String(cfg.aprsPort); h += F("'>");
  h += F("<label><input type='checkbox' name='is2rf' value='1' ");
  if (cfg.is2rf) h += F("checked");
  h += F("> Enable IS-&gt;RF message gating</label>");
  h += F("<button type='submit'>SAVE & RESTART</button></form>");
  h += F("<p class='small'>AP: NUSA-IGATE / 12345678 &bull; http://192.168.4.1/</p></div>");

  h += F("<div class='card'><h2>Rule IS-&gt;RF</h2>");
  h += F("<code>MESSAGE only\nExact target callsign+SSID\nDIRECT RF heard <= 30 minutes\nRF output = DIRECT (no WIDE path)\n3rd-party = TCPIP,IGATECALL*\nq construct never sent to RF</code>");
  h += F("</div>");

  h += F("</div></body></html>");

  web.send(200, "text/html; charset=utf-8", h);
}

static void handleSave() {
  cfg.wifiSsid = web.arg("ssid");
  cfg.wifiPass = web.arg("wpass");
  cfg.igateCall = cleanCall(web.arg("call"));
  cfg.aprsHost = web.arg("host");
  cfg.aprsHost.trim();
  cfg.is2rf = web.hasArg("is2rf");

  long p = web.arg("port").toInt();
  if (p < 1 || p > 65535) p = 14580;
  cfg.aprsPort = (uint16_t)p;

  String base;
  uint8_t ssid;
  if (!parseAx25Call(cfg.igateCall, base, ssid))
    cfg.igateCall = "YF9UAG-3";
  if (!cfg.aprsHost.length())
    cfg.aprsHost = "asia.aprs2.net";

  saveConfig();

  web.send(200, "text/html",
    "<html><body style='font-family:Arial'><h2>Saved.</h2>"
    "<p>ESP32 restarting...</p></body></html>");

  restartRequested = true;
  restartAt = millis() + 1200UL;
}

static void startWeb() {
  web.on("/", HTTP_GET, handleRoot);
  web.on("/save", HTTP_POST, handleSave);
  web.onNotFound([]() {
    web.sendHeader("Location", "/", true);
    web.send(302, "text/plain", "");
  });
  web.begin();
}

// -----------------------------------------------------------------------------
// Setup / loop
// -----------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(100);

  loadConfig();

  RadioSerial.begin(UVK5_BAUD, SERIAL_8N1, UVK5_RX_PIN, UVK5_TX_PIN);

  startWifi();
  startWeb();

  Serial.println();
  Serial.println("NUSA UV-K5 IGATE REV1A");
  Serial.print("Setup AP: ");
  Serial.println(AP_SSID);
  Serial.print("iGate: ");
  Serial.println(cfg.igateCall);
}

void loop() {
  pollRadio();
  maintainRadioTx();

  web.handleClient();
  maintainWifi();
  maintainAprs();

  purgeOldHeard();

  if (restartRequested && (int32_t)(millis() - restartAt) >= 0) {
    delay(50);
    ESP.restart();
  }

  delay(1);
}
