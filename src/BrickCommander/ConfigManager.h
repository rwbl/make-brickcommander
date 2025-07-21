/**
 * @file ConfigManager.h
 *
 * @brief Handles MQTT broker configuration load & save.
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once
#include <Preferences.h>
#include "Configuration.h"  
#include "Log.h"

class ConfigManager {
    Preferences prefs;

public:
    // Define the configuration items and set defaults from Configuration.h
    String mqtt_broker = CONFIG::MQTT_BROKER;
    uint16_t mqtt_port = CONFIG::MQTT_PORT;
    String mqtt_username = CONFIG::MQTT_USERNAME;
    String mqtt_password = CONFIG::MQTT_PASSWORD;

    /**
     * @brief Load the configuration items.
     */
    void load() {
        prefs.begin("brickcmd", true); // read-only
        // Assign the items with defaults if empty
        mqtt_broker     = prefs.getString("mqtt_broker", CONFIG::MQTT_BROKER);
        mqtt_port       = prefs.getUShort("mqtt_port", CONFIG::MQTT_PORT);
        mqtt_username   = prefs.getString("mqtt_username", CONFIG::MQTT_USERNAME);
        mqtt_password   = prefs.getString("mqtt_password", CONFIG::MQTT_PASSWORD);
        prefs.end();
        LOGI("[ConfigManager][load] Load broker=%s,port=%d,username=%s,password=%s", mqtt_broker.c_str(), mqtt_port, mqtt_username.c_str(), mqtt_password.c_str());
    }

    /**
     * @brief Store the configuration items.
     */
    void save() {
        prefs.begin("brickcmd", false); // read-write
        prefs.putString("mqtt_broker", mqtt_broker);
        prefs.putUShort("mqtt_port", mqtt_port);
        prefs.putString("mqtt_username", mqtt_username);
        prefs.putString("mqtt_password", mqtt_password);
        prefs.end();
        LOGI("[ConfigManager][save] Save broker=%s,port=%d,username=%s,password=%s", mqtt_broker.c_str(), mqtt_port, mqtt_username.c_str(), mqtt_password.c_str());
    }

    /**
     * @brief Reset the configuration items with defaults defined in Configuration.h
     */
    void reset() {
        prefs.begin("brickcmd", false); // read-only
        // Assig the items with defaults if empty
        prefs.putString("mqtt_broker", CONFIG::MQTT_BROKER);
        prefs.putUShort("mqtt_port", CONFIG::MQTT_PORT);
        prefs.putString("mqtt_username", CONFIG::MQTT_USERNAME);
        prefs.putString("mqtt_password", CONFIG::MQTT_PASSWORD);
        prefs.end();
        LOGI("[ConfigManager][reset] Reset broker=%s,port=%d,username=%s,password=%s", mqtt_broker.c_str(), mqtt_port, mqtt_username.c_str(), mqtt_password.c_str());
    }

};

inline ConfigManager config; // global instance
