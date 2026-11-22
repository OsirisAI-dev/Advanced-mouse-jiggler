#ifndef BLE_KEYS_H
#define BLE_KEYS_H

#include <Arduino.h>
#include <stdint.h>

// Initialization & Status
void initBLEKeyboard();
bool isBLEConnected();

// Keyboard Actions
void printBLEString(const String &payload);
void pressBLEKey(uint8_t key);
void releaseAllBLEKeys();

#endif