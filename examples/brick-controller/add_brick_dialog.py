"""
Brick-Controller - add_brick_dialog
---------------------------------
Add a new brick to the JSON config file.
"""

from PySide6.QtWidgets import QDialog, QFormLayout, QLineEdit, QDialogButtonBox
from brick_state import BrickState

class AddBrickDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Add Brick")
        layout = QFormLayout(self)

        self.name_edit = QLineEdit()
        self.controller_edit = QLineEdit()
        self.mac_edit = QLineEdit()
        self.port_edit = QLineEdit()

        layout.addRow("Name:", self.name_edit)
        layout.addRow("Controller:", self.controller_edit)
        layout.addRow("MAC:", self.mac_edit)
        layout.addRow("Port:", self.port_edit)

        self.button_box = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        self.button_box.accepted.connect(self.accept)
        self.button_box.rejected.connect(self.reject)
        layout.addWidget(self.button_box)

    def get_brick(self):
        return BrickState(
            self.name_edit.text(),
            self.controller_edit.text(),
            self.mac_edit.text(),
            int(self.port_edit.text())
        )
