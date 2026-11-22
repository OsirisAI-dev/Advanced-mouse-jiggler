#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <Wire.h>
#include <OneButton.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include "index_html.h"
#include "net_scan.h"

// Include our custom wrappers
#include "usb_keys.h"
#include "ble_keys.h"

// --- Standard HID Key Definitions ---
#define KEY_LEFT_CTRL   0x80
#define KEY_LEFT_SHIFT  0x81
#define KEY_LEFT_ALT    0x82
#define KEY_LEFT_GUI    0x83
#define KEY_RETURN      0xB0
#define KEY_ESC         0xB1
#define KEY_BACKSPACE   0xB2
#define KEY_TAB         0xB3
#define KEY_DELETE      0xD4
#define KEY_HOME        0xD2
#define KEY_END         0xD5
#define KEY_UP_ARROW    0xDA
#define KEY_DOWN_ARROW  0xD9
#define KEY_LEFT_ARROW  0xD8
#define KEY_RIGHT_ARROW 0xD7

// --- Hardware & Display Setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_SDA 8
#define I2C_SCL 9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int brightness = 128; // Default OLED brightness

#define BTN_TOP 4
#define BTN_BOTTOM 5
OneButton topBtn(BTN_TOP, true);    
OneButton bottomBtn(BTN_BOTTOM, true);

// --- Network & Web Setup ---
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
const char* ap_ssid = "Osiris_AP";
const char* ap_pass = "Password1234";

// --- Persistent state (NVS) ---
Preferences prefs;

// --- State Variables ---
bool wifiActive = false;
bool apActive = false;
bool bleActive = false;
String staSsid = "";
String staPassword = "";
int jigglerTimeoutMin = 0; 
unsigned long lastJiggleTime = 0;
bool jigglerActive = false;
unsigned long jigglerStartTime = 0;
unsigned long screenTimeoutSec = 0;
unsigned long lastActivityMillis = 0;
bool screenAsleep = false;
bool apRadioOn = false;
bool webServerStarted = false;
bool usbHidConnected = false;
unsigned long apIdleStartMs = 0;
const unsigned long AP_IDLE_TIMEOUT_MS = 5UL * 60UL * 1000UL;

// Non-blocking action feedback splash
enum AppState { MAIN_SCREEN, MAIN_MENU, SUB_MENU, SPLASH_MESSAGE };
AppState currentState = MAIN_SCREEN;
AppState nextStateAfterSplash = MAIN_SCREEN;
unsigned long splashEndMillis = 0;
String splashLine1 = "";
String splashLine2 = "";
String splashLine3 = "";

int menuIndex = 0;
int subMenuIndex = 0;
int activeParentMenu = -1;
const int NUM_MAIN_ITEMS = 5;

// --- Display & Lang Variables ---
int currentLang = 0; // 0 = EN, 1 = FR, 2 = DE
bool menuGrid = false; 

const char* mainMenuItems[3][5] = {
  {"Mouse Jiggler", "AP mode", "Wifi mode", "Ble Mode", "Parameters"},
  {"Bougeur Souris", "Mode AP", "Mode Wifi", "Mode Ble", "Parametres"},
  {"Maus-Jiggler", "AP-Modus", "WLAN-Modus", "BLE-Modus", "Parameter"}
};

const char* paramSubItems[3][5] = {
  {"Lang: FR-EN-DE", "Brightness", "Timeout", "Menu: List", "Menu: Grid"},
  {"Lang: FR-EN-DE", "Luminosite", "Delai", "Menu: Liste", "Menu: Grille"},
  {"Sprache: FR-EN-DE", "Helligkeit", "Zeitlimit", "Menue: Liste", "Menue: Raster"}
};

const char* jigglerSubItems[3][5] = {
  {"Toggle: ON/OFF", "Timeout: 30m", "Timeout: 60m", "Timeout: 90m", "Timeout: Unlim"},
  {"Basculer: ON/OFF", "Delai: 30m", "Delai: 60m", "Delai: 90m", "Delai: Illimite"},
  {"Umschalten: ON/OFF", "Zeitlimit: 30m", "Zeitlimit: 60m", "Zeitlimit: 90m", "Unbegrenzt"}
};

// Dashboard Metrics
String pcTime = "--:--";
String pcDate = "--/--";

// --- Function Declarations ---
void handleCommand(String cmd);
void handleDashboardLine(const String &incoming);
void executeDirectAction(int item);
void executeSubAction(int parent, int sub);
void updateDisplay();
void noteActivity();
void sendState();
void loadSettings();
void saveSettings();
int getSubMenuCount(int parent);
void executeMacro(const String &macro);
void showSplash(const String &line1, const String &line2, const String &line3, unsigned long durationMs, AppState nextState);
void toggleAP(bool on);
void toggleWifi(bool on);
void toggleBLE(bool on);
void ensureWebServerStarted();
float readDeviceTempC();

// Mirrors every device->browser reply to both transports at once.
void broadcastLine(const String &line) {
  Serial.println(line);
  if (webServerStarted) ws.textAll(line);
}

// --- WebSocket Event Handler ---
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      String message = (char*)data;
      if (message.startsWith("T:")) {
        handleDashboardLine(message);
      } else if (message.startsWith("CMD:")) {
        noteActivity();
        handleCommand(message.substring(4));
      } else {
        noteActivity();
        handleCommand(message);
      }
    }
  }
}

// --- Serial & Web Command Parsers ---
void handleDashboardLine(const String &incoming) {
  int dIdx = incoming.indexOf("|D:");
  int cIdx = incoming.indexOf("|C:");
  if (dIdx != -1 && cIdx != -1) {
    pcTime = incoming.substring(2, dIdx);
    pcDate = incoming.substring(dIdx + 3, cIdx);
    if (currentState == MAIN_SCREEN) {
      updateDisplay();
    }
  }
}

// ESP32-S3's internal die temperature sensor
float readDeviceTempC() {
  return temperatureRead();
}

void handleCommand(String cmd) {
  cmd.replace("\r", "");
  cmd.replace("\n", "");
  Serial.println("Command Received: " + cmd);

  bool settingsChanged = false;

  if (cmd == "STATE?") { sendState(); return; }
  else if (cmd == "JIG:ON") { jigglerActive = true; lastJiggleTime = millis(); jigglerStartTime = millis(); broadcastLine("OK: JIG ON"); }
  else if (cmd == "JIG:OFF") { jigglerActive = false; broadcastLine("OK: JIG OFF"); }
  else if (cmd.startsWith("JIG:TIME:")) { jigglerTimeoutMin = cmd.substring(9).toInt(); settingsChanged = true; }
  else if (cmd.startsWith("BRIGHT:")) {
    brightness = cmd.substring(7).toInt();
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command((uint8_t)brightness);
    settingsChanged = true;
  }
  else if (cmd.startsWith("TOUT:")) { screenTimeoutSec = cmd.substring(5).toInt(); settingsChanged = true; }
  else if (cmd.startsWith("MENU:")) { menuGrid = (cmd.substring(5) == "GRID"); settingsChanged = true; }
  else if (cmd == "LANG:EN") { currentLang = 0; settingsChanged = true; }
  else if (cmd == "LANG:FR") { currentLang = 1; settingsChanged = true; }
  else if (cmd == "LANG:DE") { currentLang = 2; settingsChanged = true; }
  else if (cmd == "AP:ON") { toggleAP(true); }
  else if (cmd == "AP:OFF") { toggleAP(false); }
  else if (cmd == "WIFI:ON") { toggleWifi(true); }
  else if (cmd == "WIFI:OFF") { toggleWifi(false); }
  else if (cmd.startsWith("WIFI:SET:")) {
    String rest = cmd.substring(9);
    int sep = rest.indexOf(':');
    if (sep != -1) {
      staSsid = rest.substring(0, sep);
      staPassword = rest.substring(sep + 1);
      prefs.putString("wifiSsid", staSsid);
      prefs.putString("wifiPass", staPassword);
      broadcastLine("OK:WIFI:network saved (" + staSsid + ")");
    }
  }
  else if (cmd == "BLE:ON") { toggleBLE(true); }
  else if (cmd == "BLE:OFF") { toggleBLE(false); }
  else if (cmd == "NET:SCAN") {
    broadcastLine("NET: scanning...");
    int found = scanNetworks();
    broadcastLine("NET:COUNT:" + String(found));
    for (int i = 0; i < found; i++) {
      broadcastLine("NET:ITEM:" + getScannedSSID(i) + ":" + String(getScannedRSSI(i)) + ":" + (getScannedEncrypted(i) ? "1" : "0"));
    }
    broadcastLine("NET:SCANDONE");
    return;
  }
  else if (cmd.startsWith("INJECT:STRING:")) {
    String payload = cmd.substring(14);
    if (bleActive && isBLEConnected()) printBLEString(payload);
    else printUSBString(payload);
    return;
  }
  else if (cmd.startsWith("INJECT:MACRO:")) {
    executeMacro(cmd.substring(13));
    return;
  }
  else if (cmd.startsWith("LIVE:KEY:")) {
    if (cmd.length() > 9) {
      pressKey(cmd.charAt(9));
      releaseKeys();
    }
    return;
  }
  else if (cmd.startsWith("MOUSE:MOVE:")) {
    String rest = cmd.substring(11);
    int sep = rest.indexOf(':');
    if (sep > 0 && sep + 1 < (int)rest.length()) {
      int dx = constrain(rest.substring(0, sep).toInt(), -127, 127);
      int dy = constrain(rest.substring(sep + 1).toInt(), -127, 127);
      if (isUSBConnected() && (dx != 0 || dy != 0)) {
        moveUSBMouse((int8_t)dx, (int8_t)dy);
      }
    }
    return;
  }
  else if (cmd == "MOUSE:CLICK:LEFT") {
    if (isUSBConnected()) clickUSBMouse(false);
    return;
  }
  else if (cmd == "MOUSE:CLICK:RIGHT") {
    if (isUSBConnected()) clickUSBMouse(true);
    return;
  }
  else if (cmd.startsWith("TIME:SET:")) {
    String rest = cmd.substring(9);
    int sep = rest.indexOf(':');
    if (sep > 0 && (int)rest.length() >= sep + 3) {
      pcTime = rest.substring(0, sep + 3); // "HH:MM"
      int dateStart = sep + 3;
      if ((int)rest.length() > dateStart && rest.charAt(dateStart) == ':') dateStart++;
      pcDate = rest.substring(dateStart);  // "DD/MM/YYYY"
      if (currentState == MAIN_SCREEN) updateDisplay();
    }
    return;
  }
  else if (cmd.startsWith("VAULT:SET:")) {
    String rest = cmd.substring(10);
    if (rest.length() < 3 || rest.charAt(0) < '0' || rest.charAt(0) > '7' || rest.charAt(1) != ':') {
      broadcastLine("ERR:VAULT:invalid slot");
      return;
    }
    int idx = rest.charAt(0) - '0';
    String payload = rest.substring(2); // after "N:"
    int p1 = payload.indexOf('|'), p2 = (p1 != -1) ? payload.indexOf('|', p1 + 1) : -1;
    if (p1 != -1 && p2 != -1 && p2 > p1) {
      Preferences vaultPrefs;
      vaultPrefs.begin("vault", false);
      char key[8];
      snprintf(key, sizeof(key), "n%d", idx);
      vaultPrefs.putString(key, payload.substring(0, p1));
      snprintf(key, sizeof(key), "u%d", idx);
      vaultPrefs.putString(key, payload.substring(p1 + 1, p2));
      snprintf(key, sizeof(key), "p%d", idx);
      vaultPrefs.putString(key, payload.substring(p2 + 1));
      vaultPrefs.end();
      broadcastLine("OK:VAULT:saved slot " + String(idx));
    } else {
      broadcastLine("ERR:VAULT:invalid payload");
    }
    return;
  }
  else if (cmd.startsWith("VAULT:RUN:")) {
    String idxStr = cmd.substring(10);
    if (idxStr.length() != 1 || idxStr.charAt(0) < '0' || idxStr.charAt(0) > '7') {
      broadcastLine("ERR:VAULT:invalid slot");
      return;
    }
    int idx = idxStr.charAt(0) - '0';
    Preferences vaultPrefs;
    vaultPrefs.begin("vault", true);
    char key[8];
    snprintf(key, sizeof(key), "u%d", idx);
    String user = vaultPrefs.getString(key, "");
    snprintf(key, sizeof(key), "p%d", idx);
    String pass = vaultPrefs.getString(key, "");
    vaultPrefs.end();
    if (user.length() == 0 && pass.length() == 0) {
      broadcastLine("ERR:VAULT:slot " + String(idx) + " is empty");
    } else {
      if (user.length()) {
        if (bleActive && isBLEConnected()) printBLEString(user);
        else printUSBString(user);
        delay(80);
      }
      executeMacro("TAB");
      delay(80);
      if (pass.length()) {
        if (bleActive && isBLEConnected()) printBLEString(pass);
        else printUSBString(pass);
      }
      broadcastLine("OK:VAULT:ran slot " + String(idx));
    }
    return;
  }
  else if (cmd == "VAULT:LIST?") {
    Preferences vaultPrefs;
    vaultPrefs.begin("vault", true);
    String out = "VAULT:LIST:";
    out.reserve(256);
    char key[8];
    for (int i = 0; i < 8; i++) {
      snprintf(key, sizeof(key), "n%d", i);
      String name = vaultPrefs.getString(key, "");
      snprintf(key, sizeof(key), "u%d", i);
      String user = vaultPrefs.getString(key, "");
      out += String(i) + "|" + name + "|" + user + ";";
    }
    vaultPrefs.end();
    broadcastLine(out);
    return;
  }
  else if (cmd.startsWith("VAULT:DEL:")) {
    String idxStr = cmd.substring(10);
    if (idxStr.length() != 1 || idxStr.charAt(0) < '0' || idxStr.charAt(0) > '7') {
      broadcastLine("ERR:VAULT:invalid slot");
      return;
    }
    int idx = idxStr.charAt(0) - '0';
    Preferences vaultPrefs;
    vaultPrefs.begin("vault", false);
    char key[8];
    snprintf(key, sizeof(key), "n%d", idx);
    vaultPrefs.putString(key, "");
    snprintf(key, sizeof(key), "u%d", idx);
    vaultPrefs.putString(key, "");
    snprintf(key, sizeof(key), "p%d", idx);
    vaultPrefs.putString(key, "");
    vaultPrefs.end();
    broadcastLine("OK:VAULT:cleared slot " + String(idx));
    return;
  }
  
  if (settingsChanged) {
    saveSettings();
  }
  sendState();
  if (currentState == MAIN_SCREEN) updateDisplay();
}

void sendState() {
  String s;
  s.reserve(256);
  s = "STATE:JIG=";
  s += jigglerActive ? "ON" : "OFF";
  s += ";AP="; s += apActive ? "ON" : "OFF";
  s += ";APCLIENTS="; s += apActive ? String(WiFi.softAPgetStationNum()) : "0";
  s += ";WIFI="; s += wifiActive ? "ON" : "OFF";
  s += ";WIFISSID="; s += staSsid;
  s += ";WIFIIP="; s += (wifiActive && WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : "";
  s += ";BLE="; s += bleActive ? "ON" : "OFF";
  s += ";USB="; s += usbHidConnected ? "ON" : "OFF";
  s += ";BLECONN="; s += (bleActive && isBLEConnected()) ? "ON" : "OFF";
  s += ";JIGT="; s += String(jigglerTimeoutMin);
  s += ";BRIGHT="; s += String(brightness);
  s += ";TOUT="; s += String(screenTimeoutSec);
  s += ";MENU="; s += menuGrid ? "GRID" : "LIST";
  broadcastLine(s);
}

void loadSettings() {
  brightness = prefs.getInt("bright", 128);
  screenTimeoutSec = prefs.getULong("tout", 0);
  menuGrid = prefs.getBool("menuGrid", false);
  jigglerTimeoutMin = prefs.getInt("jigT", 0);
  currentLang = prefs.getInt("lang", 0);
  staSsid = prefs.getString("wifiSsid", "");
  staPassword = prefs.getString("wifiPass", "");
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command((uint8_t)brightness);
}

void saveSettings() {
  prefs.putInt("bright", brightness);
  prefs.putULong("tout", screenTimeoutSec);
  prefs.putBool("menuGrid", menuGrid);
  prefs.putInt("jigT", jigglerTimeoutMin);
  prefs.putInt("lang", currentLang);
}

// ------------------------------------------------------------------
// FreeRTOS Task for USB/BLE Initialization (Boot Hang Fix)
// ------------------------------------------------------------------
static SemaphoreHandle_t hidInitDone;
bool bleInitialized = false;

void hidInitTask(void *pv) {
  Serial.println("HID Task: starting USB core...");
  initUSBCore();
  Serial.println("HID Task: USB core OK, starting keyboard...");
  initUSBKeyboard();
  Serial.println("HID Task: keyboard OK, starting mouse...");
  initUSBMouse();
  Serial.println("HID Task: mouse OK, USB HID ready.");
  xSemaphoreGive(hidInitDone);
  vTaskDelete(NULL);
}

void toggleBLE(bool on) {
  if (on && usbHidConnected) {
    broadcastLine("ERR:BLE:USB is already connected as the keyboard — unplug it first");
    bleActive = false;
    return;
  }
  bleActive = on;
  if (bleActive && !bleInitialized) {
    Serial.println("Initializing BLE keyboard (lazy)...");
    initBLEKeyboard();
    bleInitialized = true;
    Serial.println("BLE keyboard initialized.");
  }
}

void onUsbHidConnectionChanged(bool connected) {
  if (usbHidConnected == connected) return;
  usbHidConnected = connected;
  if (connected && bleActive) {
    toggleBLE(false);
    broadcastLine("OK:BLE:auto-off (USB connected)");
  }
  sendState();
}

// --- Remote Keyboard ---
void pressKey(uint8_t key) {
  if (bleActive && isBLEConnected()) pressBLEKey(key);
  else pressUSBKey(key);
}

void releaseKeys() {
  if (bleActive && isBLEConnected()) releaseAllBLEKeys();
  else releaseAllUSBKeys();
}

void executeMacro(const String &macro) {
  int start = 0;
  int len = macro.length();

  while (start < len) {
    int plusIndex = macro.indexOf('+', start);
    String currentToken;

    if (plusIndex == -1) {
      currentToken = macro.substring(start);
      start = len;
    } else {
      currentToken = macro.substring(start, plusIndex);
      start = plusIndex + 1;
    }

    currentToken.trim();
    currentToken.toUpperCase();

    if (currentToken == "CTRL") pressKey(KEY_LEFT_CTRL);
    else if (currentToken == "ALT") pressKey(KEY_LEFT_ALT);
    else if (currentToken == "SHIFT") pressKey(KEY_LEFT_SHIFT);
    else if (currentToken == "WIN" || currentToken == "GUI") pressKey(KEY_LEFT_GUI);
    else if (currentToken == "ENTER") pressKey(KEY_RETURN);
    else if (currentToken == "TAB") pressKey(KEY_TAB);
    else if (currentToken == "ESC") pressKey(KEY_ESC);
    else if (currentToken == "BACKSPACE") pressKey(KEY_BACKSPACE);
    else if (currentToken == "DELETE" || currentToken == "DEL") pressKey(KEY_DELETE);
    else if (currentToken == "HOME") pressKey(KEY_HOME);
    else if (currentToken == "END") pressKey(KEY_END);
    else if (currentToken == "UP") pressKey(KEY_UP_ARROW);
    else if (currentToken == "DOWN") pressKey(KEY_DOWN_ARROW);
    else if (currentToken == "LEFT") pressKey(KEY_LEFT_ARROW);
    else if (currentToken == "RIGHT") pressKey(KEY_RIGHT_ARROW);
    else if (currentToken == "SPACE") pressKey(' ');
    else if (currentToken.length() >= 2 && currentToken.charAt(0) == 'F') {
      int fn = currentToken.substring(1).toInt();
      if (fn >= 1 && fn <= 12) pressKey((uint8_t)(0xC2 + (fn - 1)));
    }
    else if (currentToken.length() == 1) pressKey(currentToken.charAt(0));
  }

  delay(50);
  releaseKeys();
}

void ensureWebServerStarted() {
  if (webServerStarted) return;
  Serial.println("Starting web server (lazy)...");
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.begin();
  webServerStarted = true;
  Serial.println("Web server online.");

  if (MDNS.begin("osiris-device")) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS started: http://osiris-device.local");
  } else {
    Serial.println("mDNS start failed");
  }
}

void toggleAP(bool on) {
  apActive = on;
  if (apActive && !apRadioOn) {
    Serial.println("Starting AP radio (lazy)...");
    WiFi.mode(wifiActive ? WIFI_AP_STA : WIFI_AP);
    WiFi.softAP(ap_ssid, ap_pass, 1, 0, 1);
    apRadioOn = true;
    apIdleStartMs = millis();
    ensureWebServerStarted();
    Serial.println("AP online.");
  } else if (!apActive && apRadioOn) {
    WiFi.softAPdisconnect(true);
    apRadioOn = false;
    WiFi.mode(wifiActive ? WIFI_STA : WIFI_OFF);
    Serial.println("AP stopped.");
  }
}

void manageApTimeout() {
  if (!apActive) return;
  if (WiFi.softAPgetStationNum() > 0) {
    apIdleStartMs = millis();
    return;
  }
  if (millis() - apIdleStartMs >= AP_IDLE_TIMEOUT_MS) {
    broadcastLine("OK:AP:auto-off (5 min idle)");
    toggleAP(false);
    sendState();
  }
}

void toggleWifi(bool on) {
  if (on && staSsid.length() == 0) {
    broadcastLine("ERR:WIFI:no network saved yet — set one below first");
    wifiActive = false;
    return;
  }
  wifiActive = on;
  if (wifiActive) {
    WiFi.mode(apRadioOn ? WIFI_AP_STA : WIFI_STA);
    WiFi.begin(staSsid.c_str(), staPassword.c_str());
    ensureWebServerStarted();
    broadcastLine("OK:WIFI:connecting to " + staSsid + "...");
  } else {
    WiFi.disconnect(false, true);
    WiFi.mode(apRadioOn ? WIFI_AP : WIFI_OFF);
    broadcastLine("OK:WIFI:disconnected");
  }
}

// ------------------------------------------------------------------
// Non-blocking Feedback Splash Screen
// ------------------------------------------------------------------
void showSplash(const String &line1, const String &line2, const String &line3, unsigned long durationMs, AppState nextState) {
  splashLine1 = line1;
  splashLine2 = line2;
  splashLine3 = line3;
  splashEndMillis = millis() + durationMs;
  nextStateAfterSplash = nextState;
  currentState = SPLASH_MESSAGE;
  updateDisplay();
}

// ------------------------------------------------------------------
// Button Callbacks
// ------------------------------------------------------------------
void onTopSingleTap() {
  noteActivity();
  if (currentState == SPLASH_MESSAGE) {
    currentState = nextStateAfterSplash;
    updateDisplay();
    return;
  }
  if (currentState == MAIN_MENU) {
    menuIndex--;
    if (menuIndex < 0) menuIndex = NUM_MAIN_ITEMS - 1;
    updateDisplay();
  } else if (currentState == SUB_MENU) {
    subMenuIndex--;
    int maxSub = getSubMenuCount(activeParentMenu) - 1;
    if (subMenuIndex < 0) subMenuIndex = maxSub;
    updateDisplay();
  }
}

void onTopDoubleTap() {
  noteActivity();
  if (currentState == SPLASH_MESSAGE) {
    currentState = nextStateAfterSplash;
    updateDisplay();
    return;
  }
  if (currentState == MAIN_MENU) {
    activeParentMenu = menuIndex;
    subMenuIndex = 0;
    if (activeParentMenu == 0 || activeParentMenu == 4) {
      currentState = SUB_MENU;
      updateDisplay();
    } else {
      executeDirectAction(activeParentMenu);
    }
  } else if (currentState == SUB_MENU) {
    executeSubAction(activeParentMenu, subMenuIndex);
  }
}

void onTopLongPress() {
  noteActivity();
  if (currentState == SPLASH_MESSAGE) {
    currentState = nextStateAfterSplash;
    updateDisplay();
    return;
  }
  if (currentState == MAIN_SCREEN) {
    jigglerActive = !jigglerActive;
    if (jigglerActive) {
      jigglerStartTime = millis();
      lastJiggleTime = millis();
      if (!apActive) toggleAP(true);
    } else {
      if (apActive && WiFi.softAPgetStationNum() == 0) toggleAP(false);
    }
    sendState();
    updateDisplay();
  }
}

void onBottomSingleTap() {
  noteActivity();
  if (currentState == SPLASH_MESSAGE) {
    currentState = nextStateAfterSplash;
    updateDisplay();
    return;
  }
  if (currentState == MAIN_MENU) {
    menuIndex++;
    if (menuIndex >= NUM_MAIN_ITEMS) menuIndex = 0;
    updateDisplay();
  } else if (currentState == SUB_MENU) {
    subMenuIndex++;
    int maxSub = getSubMenuCount(activeParentMenu) - 1;
    if (subMenuIndex > maxSub) subMenuIndex = 0;
    updateDisplay();
  }
}

void onBottomDoubleTap() {
  noteActivity();
  if (currentState == SPLASH_MESSAGE) {
    currentState = nextStateAfterSplash;
    updateDisplay();
    return;
  }
  if (currentState == SUB_MENU) {
    currentState = MAIN_MENU;
    updateDisplay();
  } else if (currentState == MAIN_MENU) {
    currentState = MAIN_SCREEN;
    updateDisplay();
  }
}

void onBottomLongPress() {
  noteActivity();
  if (currentState == SPLASH_MESSAGE) {
    currentState = nextStateAfterSplash;
    updateDisplay();
    return;
  }
  if (currentState == MAIN_SCREEN) {
    currentState = MAIN_MENU;
    menuIndex = 0;
    updateDisplay();
  }
}

// ------------------------------------------------------------------
// SETUP
// ------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("\n--- BOOTING OSIRIS ---");

  prefs.begin("phx", false); 
  Serial.println("Boot Step 1: Preferences Loaded");

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000); // 400kHz fast mode for SSD1306 OLED (4x faster redraws)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("CRITICAL: SSD1306 allocation failed!"));
  } else {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("OSIRIS");
    display.display();
    Serial.println("Boot Step 2: OLED Initialized");
  }

  loadSettings();

  topBtn.attachClick(onTopSingleTap);
  topBtn.attachDoubleClick(onTopDoubleTap);
  topBtn.attachLongPressStart(onTopLongPress);
  bottomBtn.attachClick(onBottomSingleTap);
  bottomBtn.attachDoubleClick(onBottomDoubleTap);
  bottomBtn.attachLongPressStart(onBottomLongPress);

  Serial.println("Boot Step 3: Starting HID Tasks...");
  setUSBConnectionCallback(onUsbHidConnectionChanged);
  hidInitDone = xSemaphoreCreateBinary();
  xTaskCreate(hidInitTask, "hidInit", 4096, NULL, 1, NULL);
  if (xSemaphoreTake(hidInitDone, pdMS_TO_TICKS(8000)) != pdTRUE) {
    Serial.println("WARN: USB/BLE init timeout - Bypassing");
  } else {
    Serial.println("Boot Step 4: HID Ready");
  }
  vSemaphoreDelete(hidInitDone);

  lastActivityMillis = millis();
  delay(1000); 
  
  Serial.println("READY:OSIRIS:v2");
  updateDisplay();
}

// ------------------------------------------------------------------
// LOOP
// ------------------------------------------------------------------
void loop() {
  ws.cleanupClients(); 
  topBtn.tick();
  bottomBtn.tick();
  
  // Non-blocking serial command receiver
  static String serialRxBuf = "";
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (serialRxBuf.length() > 0) {
        String incoming = serialRxBuf;
        serialRxBuf = "";
        incoming.trim();
        if (incoming.length() > 0) {
          if (incoming.startsWith("T:")) {
            handleDashboardLine(incoming);
          } else if (incoming.startsWith("CMD:")) {
            noteActivity();
            handleCommand(incoming.substring(4));
          } else {
            noteActivity();
            handleCommand(incoming);
          }
        }
      }
    } else {
      if (serialRxBuf.length() < 256) {
        serialRxBuf += c;
      }
    }
  }

  // Non-blocking mouse jiggler state machine (eliminates delay(50))
  static bool jigglePhase = false;
  static unsigned long jigglePhaseStart = 0;
  if (jigglerActive) {
    unsigned long now = millis();
    if (!jigglePhase && (now - lastJiggleTime >= 2000)) {
      if (!(bleActive && isBLEConnected())) {
        moveUSBMouse(5, 0);
      }
      jigglePhase = true;
      jigglePhaseStart = now;
    } else if (jigglePhase && (now - jigglePhaseStart >= 50)) {
      if (!(bleActive && isBLEConnected())) {
        moveUSBMouse(-5, 0);
      }
      jigglePhase = false;
      lastJiggleTime = now;
    }

    if (jigglerTimeoutMin > 0 && now - jigglerStartTime > (unsigned long)jigglerTimeoutMin * 60000UL) {
      jigglerActive = false;
      jigglePhase = false;
      sendState();
      if (currentState == MAIN_SCREEN) updateDisplay();
    }
  }

  // Non-blocking action splash expiration (wrap-safe)
  if (currentState == SPLASH_MESSAGE && (long)(millis() - splashEndMillis) >= 0) {
    currentState = nextStateAfterSplash;
    updateDisplay();
  }

  // True hardware OLED powerdown on timeout
  if (!screenAsleep && screenTimeoutSec > 0 && millis() - lastActivityMillis > screenTimeoutSec * 1000UL) {
    screenAsleep = true;
    display.clearDisplay();
    display.display();
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }

  // Broadcasts the ESP's own temp every 5s
  static unsigned long lastTempBroadcast = 0;
  if (millis() - lastTempBroadcast > 5000) {
    lastTempBroadcast = millis();
    broadcastLine("DEVTEMP:" + String(readDeviceTempC(), 1));
  }

  // AP idle-timeout check
  static unsigned long lastApCheck = 0;
  if (millis() - lastApCheck > 1000) {
    lastApCheck = millis();
    manageApTimeout();
  }

  // Backup USB host-connection check
  static unsigned long lastUsbCheck = 0;
  if (millis() - lastUsbCheck > 250) {
    lastUsbCheck = millis();
    bool nowConnected = isUSBConnected();
    if (nowConnected != usbHidConnected) {
      onUsbHidConnectionChanged(nowConnected);
    }
  }
}

// ------------------------------------------------------------------
// Physical Menu Execution Logic
// ------------------------------------------------------------------
void noteActivity() {
  lastActivityMillis = millis();
  if (screenAsleep) {
    screenAsleep = false;
    display.ssd1306_command(SSD1306_DISPLAYON);
    updateDisplay();
  }
}

int getSubMenuCount(int parent) {
  if (parent == 0) return 5; // Jiggler
  if (parent == 4) return 5; // Parameters (now correctly reports 5 items)
  return 0;
}

void executeDirectAction(int item) {
  if (item == 1) {
    toggleAP(!apActive);
    sendState();
    showSplash("Mode:", mainMenuItems[currentLang][item], apActive ? "[ON]" : "[OFF]", 1200, MAIN_SCREEN);
  } else if (item == 2) {
    if (!wifiActive && staSsid.length() == 0) {
      showSplash("No network set.", "Use the web page", "to configure WiFi.", 1500, MAIN_MENU);
    } else {
      toggleWifi(!wifiActive);
      sendState();
      showSplash("Mode:", mainMenuItems[currentLang][item], wifiActive ? "[Connecting]" : "[OFF]", 1200, MAIN_SCREEN);
    }
  } else if (item == 3) {
    toggleBLE(!bleActive);
    sendState();
    showSplash("Mode:", mainMenuItems[currentLang][item], bleActive ? "[ON]" : "[OFF]", 1200, MAIN_SCREEN);
  }
}

void executeSubAction(int parent, int sub) {
  if (parent == 0) { 
    if (sub == 0) { 
      jigglerActive = !jigglerActive; 
      if (jigglerActive) {
        jigglerStartTime = millis();
        lastJiggleTime = millis();
      }
    }
    else if (sub == 1) jigglerTimeoutMin = 30;
    else if (sub == 2) jigglerTimeoutMin = 60;
    else if (sub == 3) jigglerTimeoutMin = 90;
    else if (sub == 4) jigglerTimeoutMin = 0;
    saveSettings();
    sendState();
    showSplash("Executing:", jigglerSubItems[currentLang][sub], "", 1200, MAIN_MENU);
  } else if (parent == 4) { 
    if (sub == 0) {
      currentLang++;
      if (currentLang > 2) currentLang = 0;
    } else if (sub == 1) {
      brightness = (brightness + 127) % 256;
      display.ssd1306_command(SSD1306_SETCONTRAST);
      display.ssd1306_command((uint8_t)brightness);
    } else if (sub == 2) {
      screenTimeoutSec = (screenTimeoutSec == 0) ? 30 : 0;
    } else if (sub == 3) {
      menuGrid = false;
    } else if (sub == 4) {
      menuGrid = true; 
    }
    saveSettings();
    sendState();
    showSplash("Executing:", paramSubItems[currentLang][sub], "", 1200, MAIN_MENU);
  }
}

// ------------------------------------------------------------------
// UI Renderer
// ------------------------------------------------------------------
void updateDisplay() {
  if (screenAsleep) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (currentState == MAIN_SCREEN) {
    display.setCursor(0, 0);
    display.println("OSIRIS");
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

    display.setCursor(0, 16);
    display.print("Time:    "); display.println(pcTime);
    display.setCursor(0, 30);
    display.print("Date:    "); display.println(pcDate);
    display.setCursor(0, 44);
    display.print("ESP Temp:"); display.print(readDeviceTempC(), 1); display.println("C");

    if (jigglerActive) {
      display.setCursor(98, 0);
      display.println("[JIG]");
    }
  } 
  else if (currentState == MAIN_MENU) {
    display.setCursor(0, 0);
    display.println("--- MENU ---");
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

    if (menuGrid) {
      // 2 columns, max 3 rows
      for (int i = 0; i < NUM_MAIN_ITEMS; i++) {
        int col = i % 2;        // 0 for left, 1 for right
        int row = i / 2;        // 0, 1, or 2
        int x = col * 64;       // 0 or 64
        int y = 15 + (row * 16); // 15, 31, or 47

        if (i == menuIndex) {
          display.fillRect(x, y - 2, 62, 12, SSD1306_WHITE);
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        } else {
          display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
        }

        String shortName = String(mainMenuItems[currentLang][i]).substring(0, 10);
        display.setCursor(x + 2, y);
        display.print(shortName);
        display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
      }
    } else {
      // List view with scrolling
      int displayOffset = menuIndex - 3;
      if (displayOffset < 0) displayOffset = 0;
      int maxOffset = (NUM_MAIN_ITEMS > 4) ? (NUM_MAIN_ITEMS - 4) : 0;
      if (displayOffset > maxOffset) displayOffset = maxOffset;

      for (int i = 0; i < NUM_MAIN_ITEMS; i++) {
        if (i >= displayOffset && i < displayOffset + 4) {
          int screenY = 14 + ((i - displayOffset) * 12);
          display.setCursor(2, screenY);
          if (i == menuIndex) {
            display.print("> ");
          } else {
            display.print("  ");
          }
          display.println(mainMenuItems[currentLang][i]);
        }
      }
    }
  }
  else if (currentState == SUB_MENU) {
    display.setCursor(0, 0);
    display.print("-> ");
    display.println(mainMenuItems[currentLang][activeParentMenu]);
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

    int subCount = getSubMenuCount(activeParentMenu);
    int displayOffset = subMenuIndex - 3;
    if (displayOffset < 0) displayOffset = 0;
    int maxOffset = (subCount > 4) ? (subCount - 4) : 0;
    if (displayOffset > maxOffset) displayOffset = maxOffset;

    for (int i = 0; i < subCount; i++) {
      if (i >= displayOffset && i < displayOffset + 4) {
        int screenY = 14 + ((i - displayOffset) * 12);
        display.setCursor(2, screenY);
        if (i == subMenuIndex) {
          display.print("> ");
        } else {
          display.print("  ");
        }
        if (activeParentMenu == 0) display.println(jigglerSubItems[currentLang][i]);
        else if (activeParentMenu == 4) display.println(paramSubItems[currentLang][i]);
      }
    }
  }
  else if (currentState == SPLASH_MESSAGE) {
    display.setCursor(0, 0);
    display.println("--- OSIRIS ---");
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

    display.setCursor(0, 16);
    if (splashLine1.length()) display.println(splashLine1);
    display.setCursor(0, 30);
    if (splashLine2.length()) display.println(splashLine2);
    display.setCursor(0, 44);
    if (splashLine3.length()) display.println(splashLine3);
  }

  display.display();
}
