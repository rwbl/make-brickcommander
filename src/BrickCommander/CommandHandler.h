/**
 * @file CommandHandler.h
 *
 * @brief Parses a JSON command and executes it on the corresponding controller.
 *
 * Supports lazy registration: if a controller instance for a given controller type
 * and MAC does not yet exist it will be created and registered on demand.
 *
 * Example JSON command:
 * {
 *   "controller":"legohubno4",
 *   "mac":"90:84:2B:C1:94:79",
 *   "port":0,
 *   "power":50,
 *   "direction":"forward",
 *   "disconnect":false
 * }
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once

#include <ArduinoJson.h>
#include "Log.h"
#include "Constants.h"
#include "StringUtils.h"
// Controllers
#include "ControllerRegistry.h"
#include "LEGOHubNo4Controller.h"
#include "BuWizz2Controller.h"
// add more like a custom controller

/**
 * @brief Handles a JSON command string.
 *
 * Creates and registers controllers on-demand and executes commands.
 *
 * @param jsonCommand A String containing the JSON command.
 * @return String with status:msg, where status is ok or error.
 */
inline String handleCommand(const String& jsonCommand) {
    LOGI("[CommandHandler] Handling JSON: %s", jsonCommand.c_str());

    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, jsonCommand);

    if (err) {
        LOGE("[CommandHandler] JSON parse error: %s", err.c_str());
        return StringUtils::formatStatus(COMMAND_STATUS::ERROR,
                                         "JSON parse error: %s", 
                                         err.c_str());
    }

    String ctrlName   = doc[COMMAND::CONTROLLER]   | "";
    String mac        = doc[COMMAND::MAC]          | "";
    int port          = doc[COMMAND::PORT]         | -1;
    int power         = doc[COMMAND::POWER]        | -1;
    int speed         = doc[COMMAND::SPEED]        | -1;
    String direction  = doc[COMMAND::DIRECTION]    | "";
    bool disconnect   = doc[COMMAND::DISCONNECT]   | false;
    String msg        = "";

    ctrlName.toLowerCase();
    direction.toLowerCase();

    LOGI("[CommandHandler] Controller=%s, MAC=%s, Port=%d, Power=%d, Speed=%d, Direction=%s, Disconnect=%d",
         ctrlName.c_str(), mac.c_str(), port, power, speed, direction.c_str(), disconnect);

    if (ctrlName.isEmpty() || mac.isEmpty()) {
        LOGE("[CommandHandler] Missing controller or MAC field.");
        return StringUtils::formatStatus(COMMAND_STATUS::ERROR, "Missing controller or MAC field");
    }

    BLEController* controller = ControllerRegistry::getInstance().getController(ctrlName, mac);

    if (!controller) {
        LOGI("[CommandHandler] No controller found for %s @ %s — creating.", ctrlName.c_str(), mac.c_str());

        if (ctrlName == StringUtils::toLower(LEGOHUBNO4::NAME)) {
            controller = new LEGOHubNo4Controller(mac);
        } else if (ctrlName == StringUtils::toLower(BUWIZZ2::NAME)) {
            controller = new BuWizz2Controller(mac);

        // Add additional Controllers here

        } else {
            LOGE("[CommandHandler] Unknown controller type: %s", ctrlName.c_str());
            return StringUtils::formatStatus(COMMAND_STATUS::ERROR,
                                "Unknown controller type: %s",
                                ctrlName.c_str());
        }

        ControllerRegistry::getInstance().registerController(ctrlName, mac, controller);
    }

    /*
     * Connect to the Controller
     */
    if (!controller->isConnected()) {
        LOGI("[CommandHandler] Connecting to controller %s at %s", ctrlName.c_str(), mac.c_str());
        // Connect
        if (!controller->connect()) {
            LOGE("[CommandHandler] Failed to connect to controller %s at %s", ctrlName.c_str(), mac.c_str());
            return StringUtils::formatStatus(COMMAND_STATUS::ERROR,
                                "Failed to connect to: %s",
                                mac.c_str());
        }
    }

    /*
     * Disconnect from the Controller
     */
    if (disconnect) {
        // Disconnect
        controller->disconnect();
        LOGI("[CommandHandler] Controller disconnected.");
        return StringUtils::formatStatus(COMMAND_STATUS::OK,
                        "Disconnected from %s",
                        mac.c_str());
    }

    /*
     * Set direction, power on port
     */
    if (!direction.isEmpty() && port >= 0) {
        bool forward = (direction == COMMAND::FORWARD);

        uint8_t percent = 50;
        if (speed >= 0) {
            percent = static_cast<uint8_t>(speed);
        } else if (power >= 0) {
            percent = static_cast<uint8_t>(power);
        }

        controller->setDirection(static_cast<uint8_t>(port), forward, percent);
        LOGI("[CommandHandler] Set direction on port %d to %s with %d%%.", port, forward ? "forward" : "backward", percent);
        msg = StringUtils::formatStatus(COMMAND_STATUS::OK,
                           "Set direction on port %d to %s with %d%%.", 
                           port, forward ? "forward" : "backward", percent);


    } else if (!direction.isEmpty() && port < 0) {
        LOGW("[CommandHandler] Direction specified but invalid port: %d", port);
        msg = StringUtils::formatStatus(COMMAND_STATUS::ERROR,
                           "Direction specified but invalid port: %d",
                           port);
    }

    /*
     * Set power on port
     */
    if (direction.isEmpty() && power >= 0 && port >= 0) {
        controller->setPortPercent(static_cast<uint8_t>(port), static_cast<uint8_t>(power));
        LOGI("[CommandHandler] Set power on port %d to %d%%.", port, power);
        msg = StringUtils::formatStatus(COMMAND_STATUS::OK,
                           "Set power on port %d to %d%%.", 
                           port, power);
    }

    return msg;
}

