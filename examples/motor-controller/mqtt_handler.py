"""
BrickCommander - mqtt_handler
-----------------------------
Handles connection to MQTT and sending structured JSON commands.

Example:
cmd = {
    "controller": CONTROLLER_TYPE,
    "mac": MAC_ADDRESS,
    "port": 0,
    "power": self.power,
    "direction": self.direction,
    "disconnect": disconnect
}
"""

import json
import paho.mqtt.client as mqtt
from PySide6.QtCore import Signal, QObject
from constants import MQTT_BROKER, MQTT_PORT, MQTT_TOPIC_COMMAND, MQTT_TOPIC_RESPONSE

class MqttHandler(QObject):
    message_received = Signal(str)  # Define a signal to carry the message
    
    def __init__(self, parent=None):
        super().__init__(parent)  # THIS LINE is required!
        self.client = mqtt.Client()
        self.connected = False
        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_message = self.on_message
        self.client.on_log = self.on_log  # optional for debug

    def on_connect(self, client, userdata, flags, rc):
        #pylint: disable=unused-argument
        if rc == 0:
            self.connected = True
            print(f"[MQTT] Connected to {MQTT_BROKER}:{MQTT_PORT}")
            # subscribe to response topic
            client.subscribe(MQTT_TOPIC_RESPONSE)
            print(f"[MQTT] Subscribed to {MQTT_TOPIC_RESPONSE}")
        else:
            print(f"[MQTT] Connection failed with code {rc}")

    def on_disconnect(self, client, userdata, rc):
        #pylint: disable=unused-argument
        self.connected = False
        print("[MQTT] Disconnected")

    def on_message(self, client, userdata, msg):
        #pylint: disable=unused-argument
        # handle incoming messages here
        try:
            payload = msg.payload.decode()
            print(f"[MQTT] Received on {msg.topic}: {payload}")
            # emit signal so GUI can update
            self.message_received.emit(payload)
        except Exception as e:
            print(f"[MQTT] Failed to process message: {e}")

    def on_log(self, client, userdata, level, buf):
        # Uncomment for verbose logging
        # print(f"[MQTT LOG] {buf}")
        pass

    def connect(self):
        try:
            self.client.connect(MQTT_BROKER, MQTT_PORT, 60)
            self.client.loop_start()
        except Exception as e:
            print(f"[MQTT] Connection failed: {e}")

    def disconnect(self):
        if self.connected:
            self.client.loop_stop()
            self.client.disconnect()
            self.connected = False
            print("[MQTT] Disconnected")

    def send_command(self, cmd: dict):
        if not self.connected:
            print("[MQTT] Not connected, cannot send command")
            return
        try:
            payload = json.dumps(cmd)
            self.client.publish(MQTT_TOPIC_COMMAND, payload)
            print(f"[MQTT] Sent: {payload}")
        except Exception as e:
            print(f"[MQTT] Send failed: {e}")
