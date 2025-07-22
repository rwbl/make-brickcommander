# brick_state.py

"""
Brick-Controller - BrickState
---------------------------
Represents a single brick's state:
- name, mac, port, controller
- power, direction, running, connected
"""

class BrickState:
    def __init__(self, name, mac, port, controller,
                 power=0, direction="forward", running=False, connected=False):
        """
        Create a BrickState.

        :param name: Brick name (string)
        :param mac: BLE MAC address (string)
        :param port: Port number (int)
        :param controller: Controller type (string)
        :param power: Power level (0–100, default 0)
        :param direction: "forward" or "backward" (default "forward")
        :param running: True if currently running (default False)
        :param connected: True if currently connected (default False)
        """
        self.name = name
        self.mac = mac
        self.port = port
        self.controller = controller

        self.power = power
        self.direction = direction
        self.running = running
        self.connected = connected

    def __repr__(self):
        return (f"BrickState(name={self.name!r}, mac={self.mac!r}, port={self.port}, "
                f"controller={self.controller!r}, power={self.power}, "
                f"direction={self.direction!r}, running={self.running}, "
                f"connected={self.connected})")

    def to_dict(self):
        """
        Convert to a serializable dict (useful for saving).
        """
        return {
            "name": self.name,
            "mac": self.mac,
            "port": self.port,
            "controller": self.controller,
            "power": self.power,
            "direction": self.direction,
            "running": self.running,
            "connected": self.connected
        }

    @staticmethod
    def from_dict(d):
        """
        Create a BrickState from a dict (e.g., loaded from JSON).
        """
        return BrickState(
            name=d["name"],
            mac=d["mac"],
            port=d["port"],
            controller=d["controller"],
            power=d.get("power", 0),
            direction=d.get("direction", "forward"),
            running=d.get("running", False),
            connected=d.get("connected", False)
        )