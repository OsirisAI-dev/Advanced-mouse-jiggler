#include "usb_keys.h"
#include "tusb.h"

// Include the conflicting libraries ONLY here
#include "USB.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard usbKeyboard;
USBHIDMouse usbMouse;

static UsbConnectionCallback connectionCallback = nullptr;

// ARDUINO_USB_STARTED_EVENT/RESUME fire when a real host enumerates the
// port; STOPPED/SUSPEND fire when it goes away (unplugged, or the host
// sleeps it). A dumb USB power brick with no data lines never triggers
// STARTED at all, which is exactly the distinction we want — "plugged
// into a PC" vs "just plugged into power".
static void onUsbEvent(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (event_base != ARDUINO_USB_EVENTS) return;
  switch (event_id) {
    case ARDUINO_USB_STARTED_EVENT:
    case ARDUINO_USB_RESUME_EVENT:
      if (connectionCallback) connectionCallback(true);
      break;
    case ARDUINO_USB_STOPPED_EVENT:
    case ARDUINO_USB_SUSPEND_EVENT:
      if (connectionCallback) connectionCallback(false);
      break;
    default:
      break;
  }
}

void setUSBConnectionCallback(UsbConnectionCallback cb) {
  connectionCallback = cb;
}

void initUSBCore() {
    USB.onEvent(onUsbEvent);
    USB.begin();
}

void initUSBKeyboard() {
    usbKeyboard.begin();
}

void initUSBMouse() {
    usbMouse.begin();
}

void pressUSBKey(uint8_t key) {
    usbKeyboard.press(key);
}

void printUSBString(String str) {
    usbKeyboard.print(str);
}

void releaseAllUSBKeys() {
    usbKeyboard.releaseAll();
}

void moveUSBMouse(int8_t x, int8_t y) {
    usbMouse.move(x, y);
}

void clickUSBMouse(bool rightClick) {
    usbMouse.click(rightClick ? MOUSE_RIGHT : MOUSE_LEFT);
}

bool isUSBConnected() {
    return tud_mounted();
}
