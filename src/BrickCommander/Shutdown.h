/**
 * @file Shutdown.h
 *
 * @brief Defines a helper to cleanly shut down BrickCommander.
 *
 * Disconnects & deletes all controller instances and performs
 * any other cleanup steps (if needed in future).
 *
 * Author: Robert W.B. Linn
 * License: MIT
 */

#pragma once

#include "ControllerRegistry.h"
#include "Log.h"

/**
 * @brief Performs global shutdown and cleanup.
 *
 * Call this at program exit or when WiFi/BLE failure detected.
 */
inline void shutdownBrickCommander() {
    LOGI("[Shutdown][shutdownBrickCommander] Cleaning up all controllers …");
    ControllerRegistry::getInstance().clear();
    LOGI("[Shutdown][shutdownBrickCommander] Done.");
}
