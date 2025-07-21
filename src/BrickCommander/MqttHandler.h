/**
 * @file MqttHandler.h
 *
 * @brief Handles MQTT connection, subscription, and command message processing.
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once

#include <PubSubClient.h>
#include <WiFi.h>
#include "Log.h"
#include "Configuration.h"
#include "StringUtils.h"
#include "CommandHandler.h"

/**
 * @brief Handles MQTT connection, subscription, and command messages.
 */
class MqttHandler {
public:
    /**
     * @brief Constructor — no special arguments needed.
     * Initializes MQTT client topics based on CONFIG.
     */
    MqttHandler()
        : client(espClient)
    {
        String baseTopic    = String(CONFIG::MQTT_TOPIC_BASE);
        commandTopic        = baseTopic + "/" + CONFIG::MQTT_TOPIC_COMMAND_SUFFIX;
        configTopic         = baseTopic + "/" + CONFIG::MQTT_TOPIC_CONFIG_SUFFIX;
        stateTopic          = baseTopic + "/" + CONFIG::MQTT_TOPIC_STATUS_SUFFIX;
        availabilityTopic   = baseTopic + "/" + CONFIG::MQTT_TOPIC_AVAILABILITY_SUFFIX;
        brokerUsername      = "";
        brokerPassword      = "";
    }

    /**
     * @brief Initialize MQTT and set message callback.
     * @param broker MQTT broker hostname or IP address
     * @param port MQTT broker port (default 1883)
     * @param username MQTT broker username (default empty)
     * @param password MQTT broker password (default empty)
     */
    void begin(const char* broker, uint16_t port = 1883, const char* username = "", const char* password = "") {
        // Use config values if no arguments passed
        if (!broker) broker = config.mqtt_broker.c_str();
        if (!port)   port = config.mqtt_port;
        if (!username) username = config.mqtt_username.c_str();
        if (!password) password = config.mqtt_password.c_str();

        // Assign username & password to the class vars used to connect
        brokerUsername = String(username);
        brokerPassword = String(password);

        // Set the MQTT broker using ip & port
        client.setServer(broker, port);
        LOGI("[MqttHandler][begin] Broker set to %s:%d", broker, port);

        client.setCallback([this](char* topic, byte* payload, unsigned int length) {
            handleMessage(topic, payload, length);
        });
    }

    /**
     * @brief Keep MQTT connection alive and process incoming messages.
     */
    void loop() {
        if (!client.connected()) {
            reconnect();
        }
        client.loop();
    }

    /**
     * @brief Publish a status message as JSON to the state topic with retained false.
     * @param status Short status string (e.g., "ok", "error")
     * @param message More detailed description
     */
    void sendMqttStatus(const String& status, const String& message) {
        StaticJsonDocument<256> doc;
        doc["status"] = status;
        doc["message"] = message;

        char buf[256];
        size_t len = serializeJson(doc, buf, sizeof(buf));

        if (client.connected()) {
            if (client.publish(stateTopic.c_str(), buf, false)) {
                LOGI("[MqttHandler][sendMqttStatus] Published status to %s: %s", stateTopic.c_str(), buf);
            } else {
                LOGE("[MqttHandler][sendMqttStatus] Failed to publish status to %s", stateTopic.c_str());
            }
        } else {
            LOGW("[MqttHandler][sendMqttStatus] Client not connected — cannot send status");
        }
    }

private:
    WiFiClient espClient;       //< Underlying WiFi client used by MQTT
    PubSubClient client;        //< MQTT client instance

    String commandTopic;        //< Topic for incoming commands
    String configTopic;         //< Topic for incoming config change
    String stateTopic;          //< Topic for publishing status
    String availabilityTopic;   //< Topic for publishing availability (online/offline)
    String brokerUsername;      //< Username for client connection
    String brokerPassword;      //< Password for client connection

    /**
     * @brief Reconnect to MQTT broker and subscribe to command topic.
     * Retries indefinitely with a 5-second delay between attempts.
     */
    void reconnect() {
        while (!client.connected()) {
            LOGI("[MqttHandler][reconnect] Connecting to broker …");
            String clientId = String(CONFIG::PROJECT_NAME) + "-" + String(random(0xffff), HEX);

            if (client.connect(clientId.c_str(),                // ClientID unique
                               brokerUsername.c_str(),          // Username can be ""
                               brokerPassword.c_str(),          // Password can be ""
                               availabilityTopic.c_str(),       // Will topic
                               1,                               // Qos
                               true,                            // Retain
                               "offline")                       // Message
                               ) {
                LOGI("[MqttHandler][reconnect] Connected to MQTT broker.");

                // Subscribe to the command topics
                client.subscribe(commandTopic.c_str());
                LOGI("[MqttHandler][reconnect] Subscribed to: %s", commandTopic.c_str());

                // Subscribe to the config topics, like broker, port
                client.subscribe(configTopic.c_str());
                LOGI("[MqttHandler][reconnect] Subscribed to: %s", configTopic.c_str());

                client.publish(availabilityTopic.c_str(), "online", false);
            } else {
                LOGE("[MqttHandler][reconnect] Connection failed, rc=%d. Retrying in 5s.", client.state());
                delay(5000);
            }
        }
    }

    /*
     * @brief Update the configuration like mqtt broker ip, port, username, password.
     * @param jsonConfig String with the configuration items.
     * @return Nothing but sends status message and if configuration ok, the ESP will restart.
     */
    void handleConfig(const String& jsonConfig) {
        LOGI("[MqttHandler][handleConfig] Handling JSON: %s", jsonConfig.c_str());

        StaticJsonDocument<512> doc;
        DeserializationError err = deserializeJson(doc, jsonConfig);

        if (err) {
            LOGE("[MqttHandler][handleConfig] JSON parse error: %s", err.c_str());
            sendMqttStatus(COMMAND_STATUS::ERROR,
                           "Failed to parse the JSON configuration payload");
        }

        // Assign the configuration items.
        // Status
        uint16_t status = doc[CONFIG::MQTT_TOPIC_CONFIG_STATUS] | -1;
        LOGI("[MqttHandler][handleConfig] Request status=%d", status);

        // Check status request, send and leave
        if (!status != -1) {
            if (status == 1) {
                LOGI("[MqttHandler][handleConfig] Status request");
                char buf[128];
                snprintf(buf, 
                        sizeof(buf),
                        "{\"free\":%u,\"min_free\":%u,\"size\":%u,\"max_block\":%u}",
                        ESP.getFreeHeap(),
                        ESP.getMinFreeHeap(),
                        ESP.getHeapSize(),
                        ESP.getMaxAllocHeap());
                sendMqttStatus(COMMAND_STATUS::OK, String(buf));
                return;
            }
            // Add more status request options
        } 

        // MQTT
        String mqtt_broker      = doc[CONFIG::MQTT_TOPIC_CONFIG_BROKER]     | "";
        uint16_t mqtt_port      = doc[CONFIG::MQTT_TOPIC_CONFIG_PORT]       | CONFIG::MQTT_PORT;
        String mqtt_username    = doc[CONFIG::MQTT_TOPIC_CONFIG_USERNAME]   | "";
        String mqtt_password    = doc[CONFIG::MQTT_TOPIC_CONFIG_PASSWORD]   | "";
        // LOGI("[MqttHandler][handleConfig] Request broker=%s,port=%d,username=%s,password=%s", mqtt_broker, mqtt_port, mqtt_username, mqtt_password);

        // Check mqtt broker is mandatory, the others are set to default if not given
        if (mqtt_broker.isEmpty()) {
            LOGE("[MqttHandler][handleConfig] Missing mandatory MQTT Broker IP field NNN.NNN.NNN.NNN.");
            sendMqttStatus(COMMAND_STATUS::ERROR, 
                           "Missing mandatory MQTT Broker field.");
            return;
        }

        // Assign the parsed json fields to the config items
        config.mqtt_broker = mqtt_broker;
        config.mqtt_port = mqtt_port;
        config.mqtt_username = mqtt_username;
        config.mqtt_password = mqtt_password;

        // If all fields set then save
        LOGI("[MqttHandler][handleConfig] Saving broker=%s,port=%d,username=%s,password=%s", config.mqtt_broker, config.mqtt_port, config.mqtt_username, config.mqtt_password);
        config.save();
        sendMqttStatus(COMMAND_STATUS::OK, "Configuration updated, ESP restarting...");
        delay(1000);
        ESP.restart();
    }

    /**
     * @brief Handle incoming MQTT message.
     * Converts payload to String and processes commands on the command topic.
     * @param topic Topic string of the received message
     * @param payload Pointer to payload bytes
     * @param length Length of the payload
     */
    void handleMessage(char* topic, byte* payload, unsigned int length) {
        String payloadBuffer;
        for (unsigned int i = 0; i < length; i++) {
            payloadBuffer += (char)payload[i];
        }

        LOGI("[MqttHandler][handleMessage] Message on topic [%s]: %s", topic, payloadBuffer.c_str());

        // Handle config command which if OK restarts the ESP32
        if (String(topic) == configTopic) {
            LOGI("[MqttHandler][handleMessage] Processing config payload=%s", payloadBuffer.c_str());
            handleConfig(payloadBuffer);
            return;
        }

        // Handle brick command topics
        if (String(topic) == commandTopic) {
            LOGI("[MqttHandler][handleMessage] Processing command payload.");

            // Handle the command which returns a JSON string status ok or error and the message
            String msg = handleCommand(payloadBuffer);

            StaticJsonDocument<256> doc;
            DeserializationError err = deserializeJson(doc, msg);
            if (err) {
                sendMqttStatus(COMMAND_STATUS::ERROR, "Failed to parse response JSON");
                return;
            }

            String status  = doc["status"]  | COMMAND_STATUS::ERROR;
            String message = doc["message"] | "Unknown error";

            sendMqttStatus(status, message);

            return;
        }

        // Handle unknown topic
        LOGW("[MqttHandler][handleMessage] Received message on unknown topic: %s", topic);
        sendMqttStatus(COMMAND_STATUS::ERROR, "Received message on unknown topic: " + String(topic));
    }

};
