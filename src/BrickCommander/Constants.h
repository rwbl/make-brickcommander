/**
 * @file Constants.h
 *
 * @brief Domain-specific constants: UUIDs, ports, commands.
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once

#include "Configuration.h"

// ============================================================================
// Project Information
// ============================================================================
namespace INFO {
    constexpr const char* NAME    = CONFIG::PROJECT_NAME;
    constexpr const char* VERSION = CONFIG::VERSION;
}

// ============================================================================
// BRICK BuWizz2 BLE UUIDs and name.
// ============================================================================
namespace BUWIZZ2 {
    constexpr const char* UUID_SERVICE         = "4e050000-74fb-4481-88b3-9919b1676e93";  // BuWizz2 service UUID
    constexpr const char* UUID_CHARACTERISTIC  = "000092d1-0000-1000-8000-00805f9b34fb";  // Control characteristic UUID
    constexpr const char* UUID_DEVICE_INFO     = "0000180a-0000-1000-8000-00805f9b34fb";  // Standard device info service UUID
    constexpr const char* UUID_MODEL_NUMBER    = "00002a24-0000-1000-8000-00805f9b34fb";  // Model number characteristic
    constexpr const char* UUID_FIRMWARE_REV    = "00002a26-0000-1000-8000-00805f9b34fb";  // Firmware revision characteristic
    constexpr const char* NAME                 = "BuWizz2";                               // Device advertised name
}

// ============================================================================
// BRICK LEGO Hub No 4 BLE UUIDs and name.
// ============================================================================
namespace LEGOHUBNO4 {
    constexpr const char* UUID_SERVICE         = "00001623-1212-efde-1623-785feabcd123";  // LEGO Hub No 4 service UUID
    constexpr const char* UUID_CHARACTERISTIC  = "00001624-1212-efde-1623-785feabcd123";  // Control characteristic UUID
    constexpr const char* NAME                 = "LEGOHubNo4";                            // Device advertised name
}

// ============================================================================
// Command keys & values
// ============================================================================
namespace COMMAND {
    constexpr const char* CONTROLLER = "controller";
    constexpr const char* MAC        = "mac";
    constexpr const char* PORT       = "port";
    constexpr const char* POWER      = "power";
    constexpr const char* SPEED      = "speed";         // Optional command
    constexpr const char* DIRECTION  = "direction";
    constexpr const char* DISCONNECT = "disconnect";
    constexpr const char* FORWARD    = "forward";
    constexpr const char* BACKWARD   = "backward";
}

// ============================================================================
// Terminal Command keys & values
// ============================================================================
namespace TERMINAL_COMMAND {
    constexpr const char* RESTART   = "restart";
    constexpr const char* RESET     = "reset";
    constexpr const char* STATUS    = "status";
}

// ============================================================================
// Status prefix used for MQTT response
// ============================================================================
namespace COMMAND_STATUS {
    constexpr const char* OK    = "OK";
    constexpr const char* ERROR = "ERROR";
}

// ============================================================================
// Motor port enums
// ============================================================================
namespace PORT {
    enum Port : uint8_t {
        A = 0,
        B = 1,
        C = 2,
        D = 3
    };
}

// Helper function to convert a port number (0–3) to string name.
constexpr const char* portName(uint8_t port) {
    switch (port) {
        case 0: return "PORT_A";
        case 1: return "PORT_B";
        case 2: return "PORT_C";
        case 3: return "PORT_D";
        default: return "UNKNOWN";
    }
}

// ============================================================================
// GPIO pins
// ============================================================================
namespace PINS {
    constexpr uint8_t STATUS_LED    = 2;   // onboard
    constexpr uint8_t CONNECTED_LED = 14;
}

