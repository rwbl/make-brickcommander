/**
 * @file Configuration.h
 *
 * @brief Project configuration constants.
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once

namespace CONFIG {
    constexpr const char* PROJECT_NAME      = "BrickCommander";
    constexpr const char* VERSION           = "20250721";

    constexpr const char* WIFI_SSID         = "***";
    constexpr const char* WIFI_PASSWORD     = "***";

    constexpr const char* MQTT_BROKER       = "NNN.NNN.NNN.NNN";
    constexpr uint16_t    MQTT_PORT         = 1883;
    constexpr const char* MQTT_USERNAME     = "";
    constexpr const char* MQTT_PASSWORD     = "";

    constexpr const char* MQTT_TOPIC_BASE                = "brickcommander";
    constexpr const char* MQTT_TOPIC_COMMAND_SUFFIX      = "command";
    constexpr const char* MQTT_TOPIC_STATUS_SUFFIX       = "status";
    constexpr const char* MQTT_TOPIC_AVAILABILITY_SUFFIX = "availability";

    constexpr const char* MQTT_TOPIC_CONFIG_SUFFIX       = "config";
    constexpr const char* MQTT_TOPIC_CONFIG_STATUS       = "status";
    constexpr const char* MQTT_TOPIC_CONFIG_BROKER       = "mqtt_broker";
    constexpr const char* MQTT_TOPIC_CONFIG_PORT         = "mqtt_port";
    constexpr const char* MQTT_TOPIC_CONFIG_USERNAME     = "mqtt_username";
    constexpr const char* MQTT_TOPIC_CONFIG_PASSWORD     = "mqtt_password";

    constexpr const char* MQTT_AVAILABILITY_ONLINE       = "online";
    constexpr const char* MQTT_AVAILABILITY_OFFLINE      = "offline";
}
