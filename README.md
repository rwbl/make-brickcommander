# BrickCommander

## Project: make-brickcommander

### Brief

**BrickCommander** is an ESP32-based **command dispatcher** for LEGO®, BuWizz, and custom BLE-controlled devices.  
It accepts commands over MQTT and forwards them to the appropriate BLE device.  
Designed for modularity, scalability, and fun!  
This is an educational, non-commercial project for personal and learning purposes only.

---

## Educational Goals

This project demonstrates and teaches:
- Arduino programming in C++ with ESP32.
- Python programming with PySide6 to create a simple GUI for controlling bricks.
- Basics of BLE and MQTT communication.
- Basics of controlling hardware with ESP32.

---

## Features

- Supports LEGO® PoweredUp Hub No.4.
- Supports BuWizz2 Ludicrous.
- Easily extendable to custom ESP32 or other BLE controllers.
- Accepts commands via MQTT.
- Reports presence (`online`/`offline`) via MQTT LWT.
- Modular and lightweight design.
- Multiple controllers can be connected and controlled simultaneously.

---

## Supported Bricks

- LEGO® PoweredUp [88009](https://www.lego.com/en-us/product/hub-88009) Hub to power and control PoweredUp sensors and motors.
- [BuWizz](https://buwizz.com/) 2.0 Ludicrous: a remote control & battery in one brick, compatible with all LEGO® Power Functions motors and lights.
- Custom BLE-based ESP32 bricks (planned).

---

## Prototype

<img width="553" height="419" alt="image" src="https://github.com/user-attachments/assets/72d7e7f6-4ef1-4071-b1dd-eb3b141584a9" />

## Communication

<img width="825" height="458" alt="image" src="https://github.com/user-attachments/assets/0e99ece6-6aa5-4044-986b-81d73813a3ac" />

---

## Hardware

- ESP-WROOM-32 for the BrickCommander.

---

## Software

- Arduino IDE 2.3.6 with libraries:
  - `ArduinoJson`
  - `PubSubClient`
  - `ESP32 BLE`
- MQTT Mosquitto broker with pub/sub clients.
- Python 3.11.4 with PySide6 for the BrickCommander GUI Client.

---

## MQTT Topics

| Purpose         | Topic                        |
|-----------------|------------------------------|
| Command         | `brickcommander/command`     |
| Config          | `brickcommander/config`      |
| Status          | `brickcommander/status`      |
| Availability    | `brickcommander/availability`|

The prefix `brickcommander` can be changed in `Configuration.h`.

---

## Command Message

**Topic:**  
`brickcommander/command`

### Payload Fields (JSON)

| Field        | Type      | Description                                     |
|--------------|-----------|-------------------------------------------------|
| controller   | `string`  | Controller name: `legohubno4`, `buwizz2`, etc. |
| mac          | `string`  | BLE MAC of the device                          |
| port         | `int`     | Port number                                    |
| power        | `int`     | Power percentage (0–100)                       |
| direction    | `string`  | `forward` or `backward`                        |
| disconnect   | `bool`    | Disconnect after command                       |

### Example
```json
{
  "controller": "legohubno4",
  "mac": "90:84:2B:C1:94:79",
  "port": 0,
  "power": 50,
  "direction": "forward",
  "disconnect": false
}
```

---

## Config Message

**Topic:**  
`brickcommander/config`

You can either request status or update the MQTT configuration.

### Request Status

#### Payload Fields (JSON)

| Field   | Type  | Description      |
|---------|-------|------------------|
| status  | `int` | Set to `1`       |

#### Example
```json
{
  "status": 1
}
```

#### Example Output
```json
{
  "status": "OK",
  "message": "{"free":146584,"min_free":144016,"size":243356,"max_block":110580}"
}
```

---

### Update MQTT Configuration

#### Payload Fields (JSON)

| Field         | Type      | Description                       |
|---------------|-----------|-----------------------------------|
| mqtt_broker   | `string`  | Broker IP address                 |
| mqtt_port     | `int`     | Broker port number (default 1883)|
| mqtt_username | `string`  | Broker username                   |
| mqtt_password | `string`  | Broker password                   |

#### Example
```json
{
  "mqtt_broker": "192.168.2.101",
  "mqtt_port": 1183,
  "mqtt_username": "",
  "mqtt_password": ""
}
```

---

## Status & Availability

BrickCommander publishes the following topics:

| Topic                          | Values                |
|--------------------------------|-----------------------|
| `brickcommander/availability`  | `online` / `offline` |
| `brickcommander/status`        | JSON-formatted state |

---

## Example Clients

- Python (PySide6 GUI) - implemented.
- Others planned — see [TODO.md](TODO.md).

---

## Building & Flashing

1. Install Arduino IDE or PlatformIO.
2. Install ESP32 board support & required libraries (`ArduinoJson`, `PubSubClient`, `ESP32 BLE`).
3. Clone this repository.
4. Update `Configuration.h` with your WiFi credentials and MQTT broker IP & port.
5. Select your ESP32 board (e.g., Wrover Kit).
6. Flash the firmware and monitor logs via Serial.

---

## Terminal Commands

You can also interact via the Serial terminal (all commands in lowercase):

| Command   | Description                               |
|-----------|-------------------------------------------|
| `restart` | Restart the ESP32 BrickCommander          |
| `reset`   | Reset configuration to defaults           |
| `status`  | Print current heap information            |

---

## Next Steps

Planned & documented in [TODO.md](TODO.md)

---

## License

MIT License — see [LICENSE](LICENSE) for details.

---

## Disclaimer

- LEGO® is a trademark of the LEGO Group of companies, which does not sponsor, authorize, or endorse this project.
- The Bluetooth® word mark and logos are registered trademarks owned by Bluetooth SIG, Inc.
- BuWizz is a trademark of Fortronik d.o.o.
- All trademarks are property of their respective owners.
- This is an educational, non-commercial project for personal and learning purposes only.
