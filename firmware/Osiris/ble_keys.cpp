#include "ble_keys.h"

// Include the conflicting library ONLY here
#include <BleKeyboard.h>

// Instantiate with generic name, manufacturer, and battery level
BleKeyboard bleKeyboard("Osiris Keyboard", "OpenSource", 100);

void initBLEKeyboard() {
    bleKeyboard.begin();
}

bool isBLEConnected() {
    return bleKeyboard.isConnected();
}

void printBLEString(const String &payload) {
    bleKeyboard.print(payload);
}

void pressBLEKey(uint8_t key) {
    bleKeyboard.press(key);
}

void releaseAllBLEKeys() {
    bleKeyboard.releaseAll();
}
