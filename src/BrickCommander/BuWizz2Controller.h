/**
 * @file BuWizz2Controller.h
 *
 * @brief Controller for BuWizz 2.0 over BLE.
 *
 * Author: Robert W.B. Linn
 * License: MIT
 */

#pragma once

#include <BLEDevice.h>
#include <BLEClient.h>
#include <BLERemoteService.h>
#include <BLERemoteCharacteristic.h>
#include <functional>
#include "Log.h"
#include "Constants.h"
#include "BLEController.h"

/**
 * @class BuWizz2Controller
 * @brief Implements BLE control of a BuWizz 2.0 device.
 */
class BuWizz2Controller : public BLEController {
public:
    /**
     * @enum State
     * @brief Represents the connection state of the controller.
     */
    enum State {
        DISCONNECTED, ///< Not connected
        CONNECTED,    ///< Connected but not yet awake
        AWAKE         ///< Connected and ready for commands
    };

    /**
     * @brief Construct a new BuWizz2Controller.
     * @param mac MAC address of the BuWizz 2.0 device.
     */
    explicit BuWizz2Controller(const String& mac)
        : macAddress_(mac), client_(nullptr), characteristic_(nullptr),
          state_(DISCONNECTED), awake_(false), batteryVoltage_(0.0f) {}

    /**
     * @brief Destructor.
     * Disconnects and cleans up resources.
     */
    ~BuWizz2Controller() {
        disconnect();
    }

    /**
     * @brief Connect to the BuWizz 2.0 device.
     * @return true if connection was successful, false otherwise.
     */
    bool connect() override {
        if (client_ && client_->isConnected()) {
            LOGW("[BuWizz2Controller][connect] Already connected, disconnecting before reconnect");
            disconnect();
            delay(200);
        }
        return connectWithRetries(5);
    }

    /**
     * @brief Disconnect from the BuWizz 2.0 device.
     */
    void disconnect() override {
        if (client_) {
            if (client_->isConnected()) {
                LOGI("[BuWizz2Controller][disconnect] Disconnecting BLE client");
                client_->disconnect();
                delay(200);
            }
            delete client_;
            client_ = nullptr;
        }
        characteristic_ = nullptr;
        state_ = DISCONNECTED;
        awake_ = false;
        LOGI("[BLEController][disconnect] Disconnected from BuWizz2");
    }

    /**
     * @brief Wake up the BuWizz 2.0 and set output level.
     * @param level Output level (default 1).
     */
    void setOutputLevel(uint8_t level = 1) {
        if (!characteristic_) return;

        uint8_t cmd[] = { 0x11, static_cast<uint8_t>(level + 1) };

        LOGI("[BuWizz2Controller][setOutputLevel] level=%d", level);
        LOGIHEX("[BuWizz2Controller][setOutputLevel] cmd=", cmd, sizeof(cmd));

        characteristic_->writeValue(cmd, sizeof(cmd), true);
        delay(100);
        state_ = AWAKE;
        awake_ = true;
        LOGI("BuWizz2 is awake & ready.");
    }

    /**
     * @brief Set power level on a specific port.
     * @param port Port number (0–3).
     * @param power Power level (-100–100).
     */
    void setPortLevel(uint8_t port, int8_t power) override {
        if (!characteristic_ || port > 3) return;

        uint8_t cmd[] = { 0x10, 0, 0, 0, 0, 0 };
        cmd[1 + port] = static_cast<uint8_t>(power);

        LOGI("[BuWizz2Controller][setPortLevel] port=%u power=%d", port, power);
        LOGIHEX("[BuWizz2Controller][setPortLevel] cmd=", cmd, sizeof(cmd));

        characteristic_->writeValue(cmd, sizeof(cmd), true);
    }

    /**
     * @brief Get the current battery voltage.
     * @return Battery voltage in volts.
     */
    float getBatteryVoltage() const {
        return batteryVoltage_;
    }

    /**
     * @brief Get the device state as a JSON string.
     * @return JSON string of device state.
     */
    String getStateJson() override {
        String json = "{";
        json += "\"device\":\"BuWizz2\",";
        json += "\"connected\":" + String(isConnected() ? "true" : "false") + ",";
        json += "\"batteryVoltage\":" + String(batteryVoltage_, 2);
        json += "}";
        return json;
    }

    /**
     * @brief Get the current connection state.
     * @return Current state.
     */
    State getState() const {
        return state_;
    }

    /**
     * @brief Check if the device is connected.
     * @return true if connected, false otherwise.
     */
    bool isConnected() const override {
        return state_ >= CONNECTED;
    }

    /**
     * @brief Check if the device is awake.
     * @return true if awake, false otherwise.
     */
    bool isAwake() const override {
        return awake_;
    }

private:
    /**
     * @brief Attempt to connect to the BuWizz with retries.
     * @param maxAttempts Maximum number of connection attempts.
     * @return true if connected, false otherwise.
     */
    bool connectWithRetries(int maxAttempts) {
        BLEDevice::init("");

        if (client_) {
            delete client_;
            client_ = nullptr;
        }
        client_ = BLEDevice::createClient();
        client_->setClientCallbacks(new ClientCallbacks(this));

        for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
            LOGI("Connecting attempt %d …", attempt);

            if (!client_->connect(BLEAddress(macAddress_.c_str()))) {
                LOGE("Failed to connect BLE, retrying …");
                delay(1000);
                continue;
            }

            BLERemoteService* service = client_->getService(BUWIZZ2::UUID_SERVICE);
            if (!service) {
                LOGE("BuWizz2 service not found, retrying …");
                client_->disconnect();
                delay(500);
                continue;
            }

            characteristic_ = service->getCharacteristic(BUWIZZ2::UUID_CHARACTERISTIC);
            if (!characteristic_) {
                LOGE("Control characteristic not found, retrying …");
                client_->disconnect();
                delay(500);
                continue;
            }

            if (characteristic_->canNotify()) {
                characteristic_->registerForNotify(
                    std::bind(&BuWizz2Controller::notificationCallback, this,
                              std::placeholders::_1, std::placeholders::_2,
                              std::placeholders::_3, std::placeholders::_4));
            }

            LOGI("Connected to BuWizz2");
            state_ = CONNECTED;

            setOutputLevel(1);
            return true;
        }

        LOGE("Unable to connect after %d attempts.", maxAttempts);

        if (client_) {
            client_->disconnect();
            delete client_;
            client_ = nullptr;
        }
        characteristic_ = nullptr;
        state_ = DISCONNECTED;
        awake_ = false;

        return false;
    }

    /**
     * @brief Handle BLE notifications from the device.
     * @param chr Characteristic that triggered the callback.
     * @param data Pointer to the data received.
     * @param length Length of the data.
     * @param isNotify true if notification, false otherwise.
     */
    void notificationCallback(BLERemoteCharacteristic* chr, uint8_t* data, size_t length, bool isNotify) {
        if (length >= 3 && data[0] == 0x00) {
            uint8_t rawVbat = data[2];
            batteryVoltage_ = 3.0f + rawVbat * 0.01f;
        }
    }

    /**
     * @class ClientCallbacks
     * @brief Internal BLE client callbacks for connect/disconnect events.
     */
    class ClientCallbacks : public BLEClientCallbacks {
    public:
        /**
         * @brief Construct callbacks tied to controller instance.
         * @param ctrl Pointer to BuWizz2Controller instance.
         */
        explicit ClientCallbacks(BuWizz2Controller* ctrl) : controller_(ctrl) {}

        /**
         * @brief Called when BLE client connects.
         * @param client BLE client.
         */
        void onConnect(BLEClient*) override {
            LOGI("BLE client connected");
            controller_->state_ = CONNECTED;
        }

        /**
         * @brief Called when BLE client disconnects.
         * @param client BLE client.
         */
        void onDisconnect(BLEClient*) override {
            LOGI("BLE client disconnected");
            controller_->state_ = DISCONNECTED;
            controller_->awake_ = false;
        }

    private:
        BuWizz2Controller* controller_;
    };

    String macAddress_; ///< MAC address of the BuWizz 2.0
    BLEClient* client_; ///< BLE client instance
    BLERemoteCharacteristic* characteristic_; ///< BLE characteristic for control
    volatile State state_; ///< Current connection state
    bool awake_; ///< Whether the device is awake
    float batteryVoltage_; ///< Last known battery voltage
};
