/**
 * @file Pins.h
 *
 * @brief Initializes and controls GPIO pins.
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once

#include <Arduino.h>
#include "Constants.h"

/**
 * @brief Initialize all required pins to their default states.
 */
inline void initPins() {
    pinMode(PINS::CONNECTED_LED, OUTPUT);

    digitalWrite(PINS::CONNECTED_LED, LOW);
}

/**
 * @brief Set the state of the connected LED.
 * @param on true to turn on, false to turn off
 */
inline void setConnectedLED(bool on) {
    digitalWrite(PINS::CONNECTED_LED, on ? HIGH : LOW);
}
