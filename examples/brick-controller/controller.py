"""
Brick-Controller - controller
---------------------------
PySide6-based GUI to control bricks over MQTT.

Workflow:
- Button [Connect]: Connect to the selected brick (speed = 0).
- Button [Start]: Start motor at current slider speed.
- Slider: Adjust power and send command on release.
- Button [Stop]: Stop motor (speed = 0).
- Button [Disconnect]: Disconnect brick.
- Button [Close]: Stop & disconnect all bricks, then quit.
"""

import time
from PySide6.QtWidgets import (
    QWidget, QListWidget, QListWidgetItem, QLabel, QPushButton,
    QSlider, QVBoxLayout, QHBoxLayout, QGridLayout, QFrame, QStyle,
    QApplication, QSizePolicy
)
from PySide6.QtCore import Qt
from PySide6.QtGui import QIcon

from mqtt_handler import MqttHandler
from brick_config import load_bricks, save_bricks
from brick_state import BrickState
from add_brick_dialog import AddBrickDialog
import json
from constants import *

class Controller(QWidget):
    """Class creating the GUI and handles MQTT handler status updates"""
    def __init__(self):
        super().__init__()
        self.setWindowTitle(WINDOW_TITLE)
        self.setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT)
        self.setWindowFlags(self.windowFlags() & ~Qt.WindowMaximizeButtonHint)

        if WINDOW_ICON_PATH:
            self.setWindowIcon(QIcon(WINDOW_ICON_PATH))

        # MQTT handler
        self.mqtt = MqttHandler(parent=self)
        # Connect MQTT signal to GUI method via Controller
        self.mqtt.message_received.connect(self.set_status_bar)
        self.mqtt.connect()

        # Load bricks & set active brick
        self.bricks = load_bricks()
        self.active_brick = None

        # Setup UI & populate list
        self.setup_ui()
        if self.bricks:
            self.select_brick(self.bricks[0])

    def setup_ui(self):
        main_layout = QHBoxLayout()
        style = self.style()

        # Left: brick list & add button
        left_layout = QVBoxLayout()
        self.brick_list = QListWidget()
        self.brick_list.itemClicked.connect(self.on_brick_selected)
        left_layout.addWidget(self.brick_list)

        self.add_btn = QPushButton("+")
        self.add_btn.clicked.connect(self.add_brick)
        left_layout.addWidget(self.add_btn)

        main_layout.addLayout(left_layout, 1)

        # Right: controls
        self.controls_layout = QVBoxLayout()

        # Info Status & reconnect
        hlayout = QHBoxLayout()
        self.status_label = QLabel("Connecting…")
        hlayout.addWidget(self.status_label)

        self.reconnect_btn = QPushButton("Reconnect")
        self.reconnect_btn.setIcon(style.standardIcon(QStyle.SP_BrowserReload))
        self.reconnect_btn.clicked.connect(self.reconnect)
        hlayout.addWidget(self.reconnect_btn)
        self.controls_layout.addLayout(hlayout)

        # Brick Status grid: running/direction/power
        grid = QGridLayout()
        for i, text in enumerate(["<b>Status</b>", "<b>Direction</b>", "<b>Power</b>"]):
            grid.addWidget(QLabel(text), 0, i, alignment=Qt.AlignCenter)

        self.lbl_brick_status = QLabel()
        self.lbl_brick_status.setAlignment(Qt.AlignCenter)
        grid.addWidget(self.lbl_brick_status, 1, 0)

        self.lbl_brick_direction = QLabel()
        self.lbl_brick_direction.setAlignment(Qt.AlignCenter)
        grid.addWidget(self.lbl_brick_direction, 1, 1)

        self.lbl_brick_power = QLabel()
        self.lbl_brick_power.setAlignment(Qt.AlignCenter)
        grid.addWidget(self.lbl_brick_power, 1, 2)

        self.controls_layout.addLayout(grid)
        self.controls_layout.addWidget(self._hline())

        # Brick Power slider
        self.slider = QSlider(Qt.Horizontal)
        self.slider.setRange(0, 100)
        self.slider.setValue(0)
        self.slider.setTickInterval(10)
        self.slider.setTickPosition(QSlider.TicksBelow)
        self.slider.sliderReleased.connect(self.on_slider_released)
        self.slider.valueChanged.connect(self.on_slider_moved)
        self.slider.setEnabled(False)
        self.controls_layout.addWidget(QLabel("Power (%)"))
        self.controls_layout.addWidget(self.slider)

        self.controls_layout.addWidget(self._hline())

        # Brick Direction buttons
        hdir = QHBoxLayout()
        self.forward_btn = QPushButton("Forward")
        self.forward_btn.setEnabled(False)
        self.forward_btn.clicked.connect(lambda: self.set_direction("forward"))
        hdir.addWidget(self.forward_btn)

        self.backward_btn = QPushButton("Backward")
        self.backward_btn.clicked.connect(lambda: self.set_direction("backward"))
        self.backward_btn.setEnabled(False)
        hdir.addWidget(self.backward_btn)
        self.controls_layout.addLayout(hdir)

        self.controls_layout.addWidget(self._hline())

        # Brick Stop Button
        hss = QHBoxLayout()
        self.stop_btn = QPushButton("Stop")
        self.stop_btn.setEnabled(False)
        self.stop_btn.clicked.connect(self.stop)
        hss.addWidget(self.stop_btn)

        self.controls_layout.addLayout(hss)

        self.controls_layout.addWidget(self._hline())

        # Brick Connect/Disconnect
        hcd = QHBoxLayout()
        self.connect_btn = QPushButton("Connect")
        self.connect_btn.clicked.connect(self.connect_brick)
        hcd.addWidget(self.connect_btn)

        self.disconnect_btn = QPushButton("Disconnect")
        self.disconnect_btn.clicked.connect(self.disconnect_brick)
        hcd.addWidget(self.disconnect_btn)

        self.controls_layout.addLayout(hcd)

        self.controls_layout.addWidget(self._hline())

        # Close Script
        hclose = QHBoxLayout()
        hclose.addStretch()
        self.close_btn = QPushButton("Close")
        self.close_btn.clicked.connect(self.close_app)
        hclose.addWidget(self.close_btn)
        self.controls_layout.addLayout(hclose)

        # Statusbar
        hstatusbar = QHBoxLayout()
        self.statusbar_label = QLabel("Welcome")
        self.statusbar_label.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        self.statusbar_label.setWordWrap(True)
        hstatusbar.addWidget(self.statusbar_label)        
        self.controls_layout.addLayout(hstatusbar)

        main_layout.addLayout(self.controls_layout, 2)
        self.setLayout(main_layout)

        self.refresh_brick_list()

    def _hline(self):
        """Horizontal separator line."""
        line = QFrame()
        line.setFrameShape(QFrame.HLine)
        line.setFrameShadow(QFrame.Sunken)
        return line

    def refresh_brick_list(self):
        """Reloads brick list widget from saved bricks."""
        self.brick_list.clear()
        for brick in self.bricks:
            brick.connected = False
            item = QListWidgetItem(brick.name)
            self.brick_list.addItem(item)
        self.status_label.setText(f"Ready. {len(self.bricks)} bricks loaded.")

    def select_brick(self, brick: BrickState):
        """Selects a brick and updates UI accordingly."""
        self.active_brick = brick
        self.slider.setValue(brick.power)
        self.update_status_labels()
        self.status_label.setText(f"Selected: {brick.name}")

    def on_brick_selected(self, item: QListWidgetItem):
        name = item.text()
        brick = next(b for b in self.bricks if b.name == name)
        self.select_brick(brick)

    def on_slider_moved(self, value: int):
        if not self.active_brick:
            return
        self.active_brick.power = value
        self.update_status_labels()

    def on_slider_released(self):
        if not self.active_brick:
            return
        self.active_brick.running = self.active_brick.power > 0
        self.update_status_labels()
        self.send_command()

    def set_direction(self, direction: str):
        if not self.active_brick:
            return
        self.active_brick.direction = direction
        self.update_status_labels()
        self.send_command()

    def connect_brick(self):
        if not self.active_brick:
            return
        self.active_brick.running = False
        self.active_brick.power = 0
        self.active_brick.connected = True
        self.slider.setValue(0)
        self.slider.setEnabled(self.active_brick.connected)
        self.forward_btn.setEnabled(self.active_brick.connected)
        self.backward_btn.setEnabled(self.active_brick.connected)
        self.stop_btn.setEnabled(self.active_brick.connected)
        self.update_status_labels()
        self.send_command(disconnect=False)
        self.status_label.setText(f"Connected to: {self.active_brick.name}")

    def disconnect_brick(self):
        if not self.active_brick:
            return
        self.active_brick.running = False
        self.active_brick.power = 0
        self.active_brick.connected = False
        self.slider.setEnabled(self.active_brick.connected)
        self.forward_btn.setEnabled(self.active_brick.connected)
        self.backward_btn.setEnabled(self.active_brick.connected)
        self.stop_btn.setEnabled(self.active_brick.connected)
        self.slider.setValue(0)
        self.update_status_labels()
        self.send_command(disconnect=True)
        self.status_label.setText(f"Disconnected from: {self.active_brick.name}")

    def start(self):
        """Start motor at slider power (no hardcoded 10)."""
        if not self.active_brick:
            return
        power = self.slider.value()
        self.active_brick.power = power
        self.active_brick.running = power > 0
        self.update_status_labels()
        self.send_command()

    def stop(self):
        """Stop motor (speed=0)."""
        if not self.active_brick:
            return
        self.active_brick.running = False
        self.active_brick.power = 0
        self.slider.setValue(0)
        self.update_status_labels()
        self.send_command()

    def update_status_labels(self):
        b = self.active_brick
        if b is None:
            return

        self.set_brick_status("Running" if b.running else "Stopped")
        self.lbl_brick_status.setStyleSheet(f"color: {COLOR_RUNNING if b.running else COLOR_STOPPED}; font-weight: bold;")

        self.lbl_brick_direction.setText(b.direction.capitalize())
        self.lbl_brick_direction.setStyleSheet(f"color: {COLOR_DIRECTION}; font-weight: bold;")

        self.lbl_brick_power.setText(f"{b.power}%")
        self.lbl_brick_power.setStyleSheet(f"color: {COLOR_SPEED}; font-weight: bold;")

        if getattr(b, 'connected', False):
            self.status_label.setStyleSheet("color: green;")
        else:
            self.status_label.setStyleSheet("color: red;")

    def send_command(self, disconnect=False):
        if not self.active_brick:
            return
        b = self.active_brick
        cmd = {
            "controller": b.controller,
            "mac": b.mac,
            "port": b.port,
            "power": b.power,
            "direction": b.direction,
            "disconnect": disconnect
        }
        self.mqtt.send_command(cmd)

    def reconnect(self):
        self.mqtt.disconnect()
        self.mqtt.connect()
        self.status_label.setText("Reconnecting...")

    def add_brick(self):
        dialog = AddBrickDialog(self)
        if dialog.exec():
            new_brick = dialog.get_brick()
            new_brick.connected = False
            self.bricks.append(new_brick)
            save_bricks(self.bricks)
            self.refresh_brick_list()

    def close_app(self):
        """Stop & disconnect all bricks before quitting."""
        for brick in self.bricks:
            self.mqtt.send_command({
                "controller": brick.controller,
                "mac": brick.mac,
                "port": brick.port,
                "power": 0,
                "direction": "forward",
                "disconnect": False
            })
            time.sleep(0.2)
            self.mqtt.send_command({
                "controller": brick.controller,
                "mac": brick.mac,
                "port": brick.port,
                "power": 0,
                "direction": "forward",
                "disconnect": True
            })
            time.sleep(0.2)
        self.mqtt.disconnect()
        QApplication.quit()

    def set_brick_status(self, text):
        """Update the brick label status running, stopped."""
        self.lbl_brick_status.setText(text)
        
    def set_status_bar(self, text):
        data = json.loads(text)
        status = data["status"]
        message = data["message"]
        self.statusbar_label.setText(f"{status},{message}")

