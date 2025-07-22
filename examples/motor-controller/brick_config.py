"""
BrickCommander - brick_config
-----------------------------
Loads & saves the list of bricks from/to a JSON config file.
"""

import json
from brick_state import BrickState

# Path to config file
CONFIG_FILE = "bricks.json"


def load_bricks():
    """
    Load bricks from JSON file.

    If file does not exist, return an empty list.
    If some bricks in the file don’t have `.connected`, defaults to False.
    """
    try:
        with open(CONFIG_FILE, "r", encoding="utf-8") as f:
            data = json.load(f)
            bricks = []
            for d in data:
                # Ensure backward compatibility: add missing fields
                if "connected" not in d:
                    d["connected"] = False
                bricks.append(BrickState.from_dict(d))
            return bricks
    except FileNotFoundError:
        print(f"[INFO] Config file '{CONFIG_FILE}' not found, starting with empty list.")
        return []
    except Exception as e:
        print(f"[ERROR] Failed to load config: {e}")
        return []


def save_bricks(bricks):
    """
    Save bricks to JSON file.

    :param bricks: list of BrickState
    """
    try:
        with open(CONFIG_FILE, "w", encoding="utf-8") as f:
            json.dump([b.to_dict() for b in bricks], f, indent=4, ensure_ascii=False)
        print(f"[INFO] Saved {len(bricks)} bricks to '{CONFIG_FILE}'.")
    except Exception as e:
        print(f"[ERROR] Failed to save config: {e}")
