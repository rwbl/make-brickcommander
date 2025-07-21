/**
 * @file TerminalCommandHandler.h
 *
 * @brief Parses terminal commands (in lowercase):
 *        restart - Restart the ESP32 BrickCommander
 *        reset - Reset the configuration to defaults set in Configuration.h
 *        status - Obtain Heap information
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once

#include <Arduino.h>
#include "Constants.h"
#include "Log.h"
#include "ConfigManager.h"

/**
 * @class TerminalCommandHandler
 * @brief Handles commands entered via the serial terminal.
 */
class TerminalCommandHandler {
public:
    TerminalCommandHandler() = default;

    /**
     * @brief Initialize the handler.
     * @param baud Baud rate for Serial. If 0, assumes Serial is already initialized.
     */
    void begin(unsigned long baud = 0) {
        if (baud > 0) {
            Serial.begin(baud);
        }
        _inputBuffer.reserve(64);
        LOGI("[TerminalCommandHandler][begin] Ready to accept commands.");
    }

    /**
     * @brief Call in `loop()` to process incoming commands.
     */
    void loop() {
        while (Serial.available()) {
            char c = (char)Serial.read();
            if (c == '\n' || c == '\r') {
                if (_inputBuffer.length() > 0) {
                    _inputBuffer.trim();
                    processCommand(_inputBuffer);
                    _inputBuffer = "";
                }
            } else {
                _inputBuffer += c;
            }
        }
    }

private:
    String _inputBuffer;

    void processCommand(const String& cmd) {
        if (cmd == TERMINAL_COMMAND::RESTART) {
            LOGI("[TerminalCommandHandler][processCommand] Restarting...");
            ESP.restart();
        } else if (cmd == TERMINAL_COMMAND::RESET) {
            LOGI("[TerminalCommandHandler][processCommand] Resetting configuration...");
            config.reset();
        } else if (cmd == TERMINAL_COMMAND::STATUS) {
            LOGI("[TerminalCommandHandler][processCommand] Status: OK.");
            LOGIHEAP("HeapCheck");
        } else {
            LOGW("[TerminalCommandHandler][processCommand] Unknown command: ", cmd);
        }
    }
};
