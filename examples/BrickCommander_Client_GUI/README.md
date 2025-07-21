# README - BrickCommander Python GUI
Project: make-brickcommander

## Brief
BrickCommander is a **Python PySide6-based graphical controller** for LEGO®, BuWizz, and custom BLE-powered bricks, communicating via MQTT.  
It complements the ESP32-based BrickCommander firmware by providing a user-friendly desktop interface.

## Features

- Control LEGO® and BuWizz bricks over MQTT.
- Adjustable power slider with real-time updates.
- Forward/Backward direction control.
- Emergency stop button.
- Connect/disconnect from selected brick.
- Add and save custom bricks.
- Status indicators for connection, power, direction, and running state.
- Compatible with the ESP32 BrickCommander MQTT protocol.

---

## Screenshots

*(Insert a screenshot of your running app here!)*

---

## Supported Bricks

- LEGO® PoweredUp Hub No.4
- BuWizz2 Ludicrous

Planned:
Any BLE-enabled brick that implements the ESP32 BrickCommander MQTT protocol.

---

## MQTT Topics

| Purpose         | Topic                                  |
|-----------------|----------------------------------------|
| Command         | `brickcommander/command`              |
| Status          | `brickcommander/status`               |
| Availability    | `brickcommander/availability`         |

Command example (sent to `brickcommander/command`):

```json
{
  "controller": "LEGO®hubno4",
  "mac": "90:84:2B:C1:94:79",
  "port": 0,
  "power": 50,
  "direction": "forward",
  "disconnect": false
}
```
## Installation

### Prerequisites
- Python ≥ 3.8
- Mosquitto broker running locally or on your network
- ESP32 BrickCommander firmware flashed & connected to your MQTT broker

### Install dependencies
```
- pip install -r requirements.txt
```

**requirements.txt**
```
PySide6>=6.6.0
paho-mqtt>=1.6.1
```

## Run the GUI
```
python main.py
```

## Configuration
The bricks are defined in bricks.json, for example:
```json
[
    {
        "name": "BuWizz2 Port A",
        "controller": "buwizz2",
        "mac": "50:FA:AB:38:A6:1A",
        "port": 0
    },
    {
        "name": "LEGO®HubNo4 Port A",
        "controller": "LEGO®hubno4",
        "mac": "90:84:2B:C1:94:79",
        "port": 0
    }
]
```
You can also add bricks through the GUI using the "+" button, which will update bricks.json.

### Brick MAC Address
For each brick, its BLE MAC address is required in order to connect.
You can obtain this by using a BLE Scanner tool.
Ensure the brick is powered on prior scanning.

**Options**
Install a smartphone app such as:
- nRF Connect (Android & iOS)
- LightBlue (iOS/Android)
Or 
use the provided Python script example.
**BrickScanner.py**
The script will print a list of nearby BLE devices and their MAC addresses filtered by LEGO® / BuWizz Service UUID.

(requires bleak library)
```
pip install bleak
python BrickScanner.py
```
with output example:
```
BrickScanner v20250718
Scanning for BLE devices... (This may take ~8 seconds)
=DEVICE==========================================
Address:  90:84:2B:C1:94:79
Name:     <Unknown>
RSSI:     -70 dBm
Tx Power: N/A
Service UUIDs:
- 00001623-1212-efde-1623-785feabcd123 (LEGO® Wireless Protocol)
Manufacturer Data:
-   ID 919: 004107cf6300
===========================================

=DEVICE==========================================
Address:  50:FA:AB:38:A6:1A
Name:     <Unknown>
RSSI:     -42 dBm
Tx Power: N/A
Service UUIDs:
- 4e050000-74fb-4481-88b3-9919b1676e93 (BuWizz Service)
Manufacturer Data:
- None
===========================================
```

## Testing
- Run a Mosquitto broker on your PC or network:
	- mosquitto -v -c mosquitto.conf
- Run the ESP32 BrickCommander firmware.
- Run this GUI and connect to your desired brick.

### mosquitto.conf
Example:
```
listener 1883
allow_anonymous true
```

## License

This project is licensed under the MIT License — see the LICENSE file.

## Acknowledgements
- ESP32 BrickCommander firmware (by Robert W.B. Linn) — the BLE bridge.
- PySide6 — Qt for Python.
- Mosquitto — MQTT broker.

## Disclaimer
LEGO® is a trademark of the LEGO Group of companies which does not sponsor, authorize, or endorse this project.
This is an educational, non-commercial project for personal and learning purposes only.
The Bluetooth® word mark and logos are registered trademarks owned by Bluetooth SIG, Inc.

