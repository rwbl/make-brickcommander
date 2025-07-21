/**
 * @file ControllerRegistry.h
 *
 * @brief Singleton registry managing all BLEController instances.
 *
 * Controllers are indexed by a key combining their type and MAC address,
 * allowing lookup and reuse of existing controller instances without duplication.
 *
 * Example key: "legohubno4|90:84:2B:C1:94:79"
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file for details.
 */

#pragma once

#include <map>
#include <Arduino.h>
#include "BLEController.h"
#include "Log.h"

/**
 * @class ControllerRegistry
 * @brief Singleton class managing registered controllers.
 */
class ControllerRegistry {
public:
    /**
     * @brief Access the singleton instance.
     */
    static ControllerRegistry& getInstance() {
        static ControllerRegistry instance;
        return instance;
    }

    /**
     * @brief Registers a controller under the specified type and MAC address.
     * If a controller with the same key already exists, it is overwritten.
     *
     * @param type Controller type string (e.g., "legohubno4").
     * @param mac  MAC address string (e.g., "90:84:2B:C1:94:79").
     * @param controller Pointer to BLEController instance (ownership remains external).
     */
    void registerController(const String& type, const String& mac, BLEController* controller) {
        String key = type + "|" + mac;
        controllers_[key] = controller;
        LOGI("[ControllerRegistry] Registered controller: %s", key.c_str());
    }

    /**
     * @brief Retrieves a registered controller by type and MAC.
     * @param type Controller type string.
     * @param mac  MAC address string.
     * @return Pointer to BLEController instance if found, nullptr otherwise.
     */
    BLEController* getController(const String& type, const String& mac) {
        String key = type + "|" + mac;
        auto it = controllers_.find(key);
        if (it != controllers_.end()) {
            return it->second;
        }
        return nullptr;
    }

    /**
     * @brief Disconnects and deletes all registered controllers.
     * Should be called during shutdown to free memory and clean up BLE.
     */
    void clear() {
        for (auto& [key, ctrl] : controllers_) {
            if (ctrl) {
                LOGI("[ControllerRegistry] Disconnecting & deleting: %s", key.c_str());
                ctrl->disconnect();
                delete ctrl;
            }
        }
        controllers_.clear();
        LOGI("[ControllerRegistry] Cleared all controllers.");
    }

private:
    std::map<String, BLEController*> controllers_; ///< map of (type|mac) → controller

    // Singleton: private constructor and deleted copy operations
    ControllerRegistry() {}
    ControllerRegistry(const ControllerRegistry&) = delete;
    ControllerRegistry& operator=(const ControllerRegistry&) = delete;
};
