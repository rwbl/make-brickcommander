/**
 * @file 
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once

#include <Arduino.h>
#include "Log.h"

class BLEController {
public:
    virtual ~BLEController() = default;

    /**
     * Connects to the BLE device.
     * @return true if connection was successful, false otherwise.
     */
    virtual bool connect() = 0;

    /**
     * Disconnects from the BLE device.
     */
    virtual void disconnect() = 0;

    /**
     * Sets the motor power level (raw value) for a specific port.
     * @param port Port number (typically 0–3).
     * @param level Raw power level (-127…127).
     */
    virtual void setPortLevel(uint8_t port, int8_t level) = 0;

    /**
     * Sets the motor power as a percentage (0–100%) for a specific port.
     * Internally converted to a raw level and calls setPortLevel.
     * @param port Port number (typically 0–3).
     * @param percent Power percentage (0–100).
     */
    virtual void setPortPercent(uint8_t port, uint8_t percent) {
        if (percent > 100) percent = 100;
        int8_t level = static_cast<int8_t>(percent * 127 / 100);
        setPortLevel(port, level);
    }

    /**
     * Starts the motor at given power percentage.
     * Default implementation calls setPortPercent.
     * @param port Port number (0–3).
     * @param percent Power percentage (default = 100%).
     */
    virtual void startMotor(uint8_t port, uint8_t percent = 100) {
        setPortPercent(port, percent);
    }

    /**
     * Stops the motor on specified port (sets power to 0).
     * Default implementation calls setPortLevel with 0.
     * @param port Port number (0–3).
     */
    virtual void stopMotor(uint8_t port) {
        setPortLevel(port, 0);
    }

    /**
     * Sets motor direction and power.
     * Default implementation maps forward=true to positive power,
     * forward=false to negative power, then calls setPortLevel.
     * @param port Port number (0–3).
     * @param forward true = forward direction, false = backward.
     * @param percent Power percentage (default = 50%).
     */
    virtual void setDirection(uint8_t port, bool forward, uint8_t percent = 50) {
        if (percent > 100) percent = 100;
        int8_t level = static_cast<int8_t>(percent * 127 / 100);
        if (forward) {
            level = -level;
        }
        //if (!forward) {
        //    level = -level;
        //}

        setPortLevel(port, level);
    }

    /**
     * Checks if the controller is currently awke.
     * @return true if connected, false otherwise.
     */
    virtual bool isAwake() const { return false; }

    /**
     * Checks if the controller is currently connected.
     * @return true if connected, false otherwise.
     */
    virtual bool isConnected() const = 0;

    /**
     * Gets the current state of the controller as a JSON string.
     * @return JSON-formatted string representing the current state.
     */
    virtual String getStateJson() = 0;
};
