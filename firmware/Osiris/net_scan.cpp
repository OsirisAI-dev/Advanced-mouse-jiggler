#include "net_scan.h"
#include <WiFi.h>

static String resultSSID[MAX_SCAN_RESULTS];
static int32_t resultRSSI[MAX_SCAN_RESULTS];
static bool resultEnc[MAX_SCAN_RESULTS];
static int resultCount = 0;

int scanNetworks() {
  resultCount = 0;
  // Station mode must be enabled (STA or AP+STA) for WiFi.scanNetworks()
  // when invoked over USB Web Serial while WiFi and SoftAP are both OFF.
  wifi_mode_t prevMode = WiFi.getMode();
  bool needSta = (prevMode == WIFI_OFF || prevMode == WIFI_AP);
  if (needSta) {
    WiFi.mode(prevMode == WIFI_AP ? WIFI_AP_STA : WIFI_STA);
  }
  int found = WiFi.scanNetworks();
  if (found <= 0) {
    WiFi.scanDelete();
    if (needSta) WiFi.mode(prevMode);
    return 0;
  }

  for (int i = 0; i < found && resultCount < MAX_SCAN_RESULTS; i++) {
    String ssid = WiFi.SSID(i);
    if (ssid.length() == 0) continue;

    // Dedup: keep only the strongest entry per SSID (networks with multiple
    // access points/repeaters show up once per radio otherwise).
    bool dup = false;
    for (int j = 0; j < resultCount; j++) {
      if (resultSSID[j] == ssid) {
        dup = true;
        if (WiFi.RSSI(i) > resultRSSI[j]) {
          resultRSSI[j] = WiFi.RSSI(i);
          resultEnc[j] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
        }
        break;
      }
    }
    if (dup) continue;

    resultSSID[resultCount] = ssid;
    resultRSSI[resultCount] = WiFi.RSSI(i);
    resultEnc[resultCount] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
    resultCount++;
  }

  // Simple strongest-first sort (small N, insertion sort is plenty).
  for (int i = 1; i < resultCount; i++) {
    String ssidTmp = resultSSID[i];
    int32_t rssiTmp = resultRSSI[i];
    bool encTmp = resultEnc[i];
    int j = i - 1;
    while (j >= 0 && resultRSSI[j] < rssiTmp) {
      resultSSID[j + 1] = resultSSID[j];
      resultRSSI[j + 1] = resultRSSI[j];
      resultEnc[j + 1] = resultEnc[j];
      j--;
    }
    resultSSID[j + 1] = ssidTmp;
    resultRSSI[j + 1] = rssiTmp;
    resultEnc[j + 1] = encTmp;
  }

  WiFi.scanDelete();
  if (needSta) WiFi.mode(prevMode);
  return resultCount;
}

String getScannedSSID(int i) {
  if (i < 0 || i >= resultCount) return "";
  return resultSSID[i];
}

int32_t getScannedRSSI(int i) {
  if (i < 0 || i >= resultCount) return 0;
  return resultRSSI[i];
}

bool getScannedEncrypted(int i) {
  if (i < 0 || i >= resultCount) return true;
  return resultEnc[i];
}
