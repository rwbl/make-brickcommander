/**
 * @file LEGOHubNo4Controller.h
 *
 * @brief Controller for LEGO PoweredUp Hub (Hub No.4) over BLE.
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once

#include "BLEController.h"
#include <BLEDevice.h>
#include <BLEClient.h>
#include <BLERemoteService.h>
#include <BLERemoteCharacteristic.h>
#include "Log.h"
#include "Constants.h"
#include "BLEController.h"

/**
 * LEGOHubNo4Controller
 *
 * Manages BLE communication with a LEGO PoweredUp Hub No.4.
 * Supports connection handling and motor control on ports A and B.
 */
class LEGOHubNo4Controller : public BLEController {
public:
    /**
     * Constructor
     * @param mac BLE MAC address of the LEGO Hub device.
     */
    explicit LEGOHubNo4Controller(const String& mac)
        : macAddress_(mac), client_(nullptr), characteristic_(nullptr), connected_(false) {}

    /**
     * Destructor to ensure clean disconnection.
     */
    ~LEGOHubNo4Controller() {
        disconnect();
    }

    /**
     * Connects to the LEGO Hub via BLE.
     * Initializes BLE client, connects, and retrieves control characteristic.
     *
     * @return true if connection succeeded, false otherwise.
     */
    bool connect() override {
        BLEDevice::init("");
        client_ = BLEDevice::createClient();

        if (!client_->connect(BLEAddress(macAddress_.c_str()))) {
            LOGE("[LEGOHubNo4Controller][connect] Failed to connect to LEGO Hub No.4");
            return false;
        }

        BLERemoteService* service = client_->getService(LEGOHUBNO4::UUID_SERVICE);
        if (!service) {
            LOGE("[LEGOHubNo4Controller][connect] LEGO Hub No.4 service not found");
            client_->disconnect();
            return false;
        }

        characteristic_ = service->getCharacteristic(LEGOHUBNO4::UUID_CHARACTERISTIC);
        if (!characteristic_) {
            LOGE("[LEGOHubNo4Controller][connect] Control characteristic not found");
            client_->disconnect();
            return false;
        }

        connected_ = true;
        LOGI("[LEGOHubNo4Controller][connect] Connected to LEGO Hub No.4");
        return true;
    }

    /**
     * Disconnects from the LEGO Hub if connected.
     */
    void disconnect() override {
        if (client_ && client_->isConnected()) {
            client_->disconnect();
            LOGI("[LEGOHubNo4Controller][Disconnect] Disconnected from LEGO Hub No.4");
        }
        connected_ = false;
    }

    /**
     * Checks if the controller is connected.
     * @return true if connected, false otherwise.
     */
    bool isConnected() const override {
        return connected_;
    }

    /**
     * Sends a power command to a given port A(0) or B(1).
     *
     * @param port Port (0 or 1).
     * @param power Signed power level (-127 to 127).
     */
    void setPortLevel(uint8_t port, int8_t power) override {
        if (!characteristic_ || port > 1) return;

        // Command format for LEGO Hub No.4 motor control
        uint8_t cmd[] = {
            0x08, 0x00, 0x81, port, 0x11, 0x51, 0x00,
            static_cast<uint8_t>(power)
        };

        LOGI("[LEGOHubNo4Controller][setPortLevel] port=%u power=%d", port, power);
        LOGIHEX("[LEGOHubNo4Controller][setPortLevel] cmd=", cmd, sizeof(cmd));

        characteristic_->writeValue(cmd, sizeof(cmd), true);
    }

    /**
     * Returns a JSON-formatted string representing the controller state.
     * Includes device name and connection status.
     */
    String getStateJson() override {
        String json = "{";
        json += "\"device\":\"" + String(LEGOHUBNO4::NAME) + "\",";
        json += "\"connected\":" + String(isConnected() ? "true" : "false");
        json += "}";
        return json;
    }

private:
    String macAddress_;                      ///< BLE MAC address of the device
    BLEClient* client_;                      ///< BLE client instance
    BLERemoteCharacteristic* characteristic_; ///< Control characteristic for commands
    bool connected_;                        ///< Connection status flag
};
