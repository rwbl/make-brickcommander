# TODO

**Project:** `make-brickcommander`

## NEW: CustomBrick ESP32 with BLE
Create a Custom Brick acting as a BLE server (Nordic UART Service) which Handles BLE commands and forwards them to the CommandHandler.
Hardware based on the ESP32 MCU, like the ESP-WROOM-32, M5Stack ATOM Lite ESP32 Development Kit + TailBAT Power Module.
### Status
* Started developing the CustomBrick with the ESP-WROOM-32 handling commands JSON format via BLE.
* Created simple Python test script using `bleak` for setting the onboard LED.
* Command example:
```
{"port":0,"power":100,"direction":"forward","disconnect":false}
```
---

## NEW: CustomBrick Raspberry Pi Pico with BLE
As CustomBrick ESP32 with BLE, but then using the Raspberry Pi Pico 2 WH.
### Status
Not started.
---

## NEW: BrickCommander Clients
- Node-RED
- Home Assistant
- RocRail
- B4J / B4A
- Any MQTT client
### Status
Not started.

---

## NEW: Configuration Update OTA
Option to set the WiFi secrets & MQTT broker IP & port Over-The-Air (OTA).
See IDEAS.md.
### Status
Not started.

---
