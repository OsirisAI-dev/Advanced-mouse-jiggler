#ifndef USB_KEYS_H
#define USB_KEYS_H

#include <Arduino.h>
#include <stdint.h>

// Initialization functions
void initUSBCore();
void initUSBKeyboard();
void initUSBMouse();

// Keyboard functions (Remote Keyboard: text sender + macro combos)
void pressUSBKey(uint8_t key);
void printUSBString(String str);
void releaseAllUSBKeys();

// Mouse functions (jiggler & touchpad)
void moveUSBMouse(int8_t x, int8_t y);
void clickUSBMouse(bool rightClick = false);

// Fires when the native USB port actually enumerates with a real host
// (a PC recognizing it as a device) or stops being seen by one — not
// just "has power". Registered once from main.ino's setup(), used to
// enforce "USB or BLE, never both": the moment a real USB HID host shows
// up, BLE gets switched off automatically.
typedef void (*UsbConnectionCallback)(bool connected);
void setUSBConnectionCallback(UsbConnectionCallback cb);

bool isUSBConnected();
#endif
