"""
Motor-Controller - constants
--------------------------
Defines MQTT and GUI defaults for the application.
"""

# MQTT Broker
MQTT_BROKER = "NNN.NNN.NNN.NNN"
MQTT_PORT = 1883
MQTT_TOPIC_COMMAND = "brickcommander/command"
MQTT_TOPIC_RESPONSE = "brickcommander/status"

# Window settings
WINDOW_TITLE = "BrickCommander Motor Controller v20250718"
WINDOW_ICON_PATH = "brick_icon_24x24.png"
WINDOW_WIDTH = 550
WINDOW_HEIGHT = 350

# Colors
COLOR_RUNNING = "green"
COLOR_STOPPED = "red"
COLOR_DIRECTION = "blue"
COLOR_SPEED = "black"
