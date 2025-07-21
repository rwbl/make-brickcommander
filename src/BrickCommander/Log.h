/**
 * @file Log.h
 *
 * @brief Central logging macros with timestamp, optional ANSI colors, and severity levels.
 *        Used throughout all modules. Falls back to no-op if NO_LOGS is defined.
 *
 * Author: Robert W.B. Linn
 * Copyright © 2025 Robert W.B. Linn
 * License: MIT — see LICENSE file.
 */

#pragma once

#include <Arduino.h>

// Uncomment to disable color output
#define NO_COLOR

#ifndef NO_LOGS

#ifndef NO_COLOR
#define COLOR_RESET   "\033[0m"
#define COLOR_INFO    "\033[32m"
#define COLOR_WARN    "\033[33m"
#define COLOR_ERROR   "\033[31m"
#define COLOR_SETUP   "\033[36m"
#else
#define COLOR_RESET   ""
#define COLOR_INFO    ""
#define COLOR_WARN    ""
#define COLOR_ERROR   ""
#define COLOR_SETUP   ""
#endif

/**
 * @brief Prints current timestamp in milliseconds to Serial.
 */
#define LOG_TIMESTAMP() \
    Serial.print("["); Serial.print(millis()); Serial.print("] ")

/**
 * @brief Prints setup banner with project name and version.
 */
#define LOG_SETUP() \
    do { \
        LOG_TIMESTAMP(); \
        Serial.print(COLOR_SETUP "[SETUP] "); \
        Serial.print(F(INFO::NAME)); \
        Serial.print(F(" v")); \
        Serial.print(F(INFO::VERSION)); \
        Serial.println(COLOR_RESET); \
    } while(0)

/**
 * @brief Prints an info log message with timestamp and color.
 */
#define LOGI(...) \
    do { LOG_TIMESTAMP(); Serial.print(COLOR_INFO "[INFO] "); Serial.printf(__VA_ARGS__); Serial.print(COLOR_RESET "\n"); } while(0)

/**
 * @brief Prints an info log with a buffer as hex string.
 * @param prefix Message prefix (e.g. "CMD")
 * @param data Pointer to buffer
 * @param len Length of buffer
 * Example:
 * @code
 * uint8_t cmd[] = { 0x11, static_cast<uint8_t>(level + 1) };
 * LOGIHEX("Command", cmd, sizeof(cmd));
 * @endcode
 */
#define LOGIHEX(prefix, data, len)                                       \
    do {                                                                  \
        LOG_TIMESTAMP();                                                  \
        Serial.print(COLOR_INFO "[INFO] ");                               \
        Serial.print(prefix);                                             \
        Serial.print("");                                               \
        for (size_t __i = 0; __i < (len); ++__i) {                        \
            if ((data)[__i] < 0x10) Serial.print('0');                    \
            Serial.print((data)[__i], HEX);                               \
            Serial.print(__i < (len)-1 ? " " : "");                       \
        }                                                                 \
        Serial.print(COLOR_RESET "\n");                                   \
    } while(0)

/**
 * @brief Prints an info log with heap information.
 * @param prefix Message prefix (e.g. "CMD")
 * @code
 * LOGIHEAP("HeapCheck");
 * @endcode
 */
 #define LOGIHEAP(prefix)                                                         \
    do {                                                                         \
        LOG_TIMESTAMP();                                                        \
        Serial.print(COLOR_INFO "[INFO] ");                                     \
        Serial.print(prefix);                                                  \
        Serial.println(F(" — HEAP STATUS:"));                                   \
        Serial.printf("   Free heap:         %u bytes\n", ESP.getFreeHeap());   \
        Serial.printf("   Minimum free heap: %u bytes\n", ESP.getMinFreeHeap());\
        Serial.printf("   Heap size:         %u bytes\n", ESP.getHeapSize());   \
        Serial.printf("   Max alloc block:   %u bytes\n", ESP.getMaxAllocHeap());\
        Serial.print(COLOR_RESET "\n");                                         \
    } while(0)

/**
 * @brief Prints an info log with heap information in JSON format.
 *
 * Logs to Serial in this format:
 * [123456] [INFO] HeapCheck — {"free":192488,"min_free":187256,"size":327680,"max_block":142000}
 *
 * @param prefix Message prefix (e.g., "HeapCheck")
 *
 * @code
 * LOGIHEAP_JSON("HeapCheck");
 * @endcode
 */
#define LOGIHEAP_JSON(prefix)                                                   \
    do {                                                                        \
        LOG_TIMESTAMP();                                                       \
        Serial.print(COLOR_INFO "[INFO] ");                                    \
        Serial.print(prefix);                                                  \
        Serial.print(F(" — "));                                               \
        Serial.printf("{\"free\":%u,\"min_free\":%u,\"size\":%u,\"max_block\":%u}", \
                      ESP.getFreeHeap(),                                       \
                      ESP.getMinFreeHeap(),                                    \
                      ESP.getHeapSize(),                                       \
                      ESP.getMaxAllocHeap());                                  \
        Serial.print(COLOR_RESET "\n");                                        \
    } while(0)

/**
 * @brief Returns heap information as a JSON-formatted String.
 *
 * Example output:
 * {"free":192488,"min_free":187256,"size":327680,"max_block":142000}
 *
 * @return String containing JSON heap stats.
 */
inline String getHeapInfoJson() {
    char buf[128];
    snprintf(buf, sizeof(buf),
             "{\"free\":%u,\"min_free\":%u,\"size\":%u,\"max_block\":%u}",
             ESP.getFreeHeap(),
             ESP.getMinFreeHeap(),
             ESP.getHeapSize(),
             ESP.getMaxAllocHeap());
    return String(buf);
}

/**
 * @brief Prints a warning log message with timestamp and color.
 */
#define LOGW(...) \
    do { LOG_TIMESTAMP(); Serial.print(COLOR_WARN "[WARN] "); Serial.printf(__VA_ARGS__); Serial.print(COLOR_RESET "\n"); } while(0)

/**
 * @brief Prints an error log message with timestamp and color.
 */
#define LOGE(...) \
    do { LOG_TIMESTAMP(); Serial.print(COLOR_ERROR "[ERROR] "); Serial.printf(__VA_ARGS__); Serial.print(COLOR_RESET "\n"); } while(0)

#else

// Disable logging completely if NO_LOGS is defined
#define LOG_SETUP()
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)

#endif
