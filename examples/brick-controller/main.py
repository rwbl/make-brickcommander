"""
Brick-Controller - Main Entry Point
----------------------------------------
BrickCommander Client to control BLE bricks.
Starts the Qt application and event loop.

Requires:
- PySide6
- qasync
"""

import sys
from PySide6.QtWidgets import QApplication
from qasync import QEventLoop
from controller import Controller

def main():
    app = QApplication(sys.argv)
    loop = QEventLoop(app)
    import asyncio
    asyncio.set_event_loop(loop)

    window = Controller()
    window.show()

    try:
        loop.run_forever()
    except KeyboardInterrupt:
        pass
    finally:
        loop.close()

if __name__ == "__main__":
    main()
