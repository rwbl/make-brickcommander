/**
 * @file BrickCommander.ino
 *
 * @brief Main program for the BrickCommander program running on an ESP32.
 *        IMPORTANT: Set ESP32 board partition to Huge App.
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#include <WiFi.h>
#include "Log.h"
#include "Configuration.h"
#include "Pins.h"
#include "WiFiMod.h"
#include "ConfigManager.h"
#include "MqttHandler.h"
#include "Shutdown.h"
#include "TerminalCommandHandler.h"

// Instantiate ConfigManager, TerminalCommandHandler, WiFi & MQTT
// ConfigManager config is defined as global instance in ConfigManager.h
TerminalCommandHandler terminalHandler;
WiFiMod wifi;
MqttHandler mqtt; 
#warning "Building with WiFi & MQTT support"

/**
 * @brief Main setup function called once at start.
 * Initializes serial, logging, pins, WiFi, and MQTT connection.
 */
void setup() {
    Serial.begin(115200);
    LOGI("[SetUp] ", CONFIG::PROJECT_NAME, " ", CONFIG::VERSION);

    LOG_SETUP();
    initPins();

    // Load the stored configuration
    config.load();

    // Connect to WiFi followed by MQTT broker using the config credentials
    if (wifi.connect()) {
        mqtt.begin(config.mqtt_broker.c_str(), config.mqtt_port, config.mqtt_username.c_str(), config.mqtt_password.c_str());
    } 
    else {
        LOGE("[SetUp] WiFi failed. Cannot use MQTT.");
    }
}

/**
 * @brief Main loop called repeatedly.
 *        Processes terminal commands, MQTT events and monitors WiFi connection status.
 */
void loop() {

    terminalHandler.loop();

    mqtt.loop();
    delay(10);
    
    if (!wifi.isConnected()) {
        LOGW("WiFi lost — shutting down BrickCommander.");
        shutdownBrickCommander();
        // optionally reset/restart here
    }
}
