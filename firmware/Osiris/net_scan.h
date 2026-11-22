#ifndef NET_SCAN_H
#define NET_SCAN_H

#include <Arduino.h>

#define MAX_SCAN_RESULTS 15

// Scans for nearby WiFi networks (blocking, ~2-4s). Returns the number
// found (capped at MAX_SCAN_RESULTS). Results are cached internally and
// read back with the getters below, sorted strongest-first, deduplicated
// by SSID.
int scanNetworks();

String getScannedSSID(int i);
int32_t getScannedRSSI(int i);
bool getScannedEncrypted(int i);

#endif
