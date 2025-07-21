/**
 * @file WiFiMod.h
 *
 * @brief // WiFi helper class for connecting to a WiFi network and checking connection status. Turns on a status LED if defined.
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once

#include <WiFi.h>
#include "Log.h"
#include "Configuration.h"
#include "Pins.h"

/**
 * @brief WiFiMod
 *
 * Handles connecting to WiFi and reporting connection status.
 */
class WiFiMod {
public:

    /**
     * @brief Connects to the WiFi network specified in CONFIG.
     *
     * @return true if connected, false otherwise
     */
    bool connect() {
        WiFi.begin(CONFIG::WIFI_SSID, CONFIG::WIFI_PASSWORD);

        LOGI("[WiFi][connect] Connecting");
        for (int i = 0; i < 30 && WiFi.status() != WL_CONNECTED; i++) {
            delay(500);
        }

        if (WiFi.status() == WL_CONNECTED) {
            LOGI("[WiFi][connect] Connected with IP %s", WiFi.localIP().toString().c_str());
            setConnectedLED(true);
            return true;
        }

        LOGE("[WiFi][connect] Connection failed.");
        setConnectedLED(false);
        return false;
    }

    /**
     * @brief Checks if WiFi is currently connected.
     *
     * @return true if connected
     */
    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

private:
    /**
     * @brief Turns the connected LED on or off if defined.
     *
     * @param on true = LED on, false = LED off
     */
    void setConnectedLED(bool on) {
#ifdef PINS::CONNECTED_LED
        pinMode(PINS::CONNECTED_LED, OUTPUT);
        digitalWrite(PINS::CONNECTED_LED, on ? HIGH : LOW);
#endif
    }
};
