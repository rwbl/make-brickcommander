/**
 * @file StringUtils.h
 * @author Robert Linn (or your name)
 * @brief Utility functions for Arduino String manipulation.
 * 
 * This header provides a small set of helper functions for working
 * with C-style strings (`const char*`) and converting them into
 * Arduino `String` objects with common operations.
 * 
 * Features:
 * toLower — convert to lowercase
 * toUpper — convert to uppercase
 * trim    — remove leading/trailing whitespace
 * replace — replace all occurrences of a substring
 * 
 * Example usage:
 * ```
 * #include "StringUtils.h"
 * 
 * String topic = StringUtils::toLower("MyTopic");
 * String clean = StringUtils::trim("   padded   ");
 * String shout = StringUtils::toUpper("hello");
 * String changed = StringUtils::replace("red green red", "red", "blue");
 * ```
 * 
 * Notes:
 * - All functions take a `const char*` as input.
 * - They return a new `String` instance.
 * - These helpers use Arduino's `String` class internally.
 */

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

namespace StringUtils {

    /**
    * @brief Convert a C-string to a lowercase `String`.
    * 
    * @param str Input C-string.
    * @return String Lowercase copy of input.
    */
    inline String toLower(const char* str) {
        String result(str);
        result.toLowerCase();
        return result;
    }

    /**
    * @brief Convert a C-string to an uppercase `String`.
    * 
    * @param str Input C-string.
    * @return String Uppercase copy of input.
    */
    inline String toUpper(const char* str) {
        String result(str);
        result.toUpperCase();
        return result;
    }

    /**
    * @brief Trim leading and trailing whitespace from a C-string.
    * 
    * @param str Input C-string.
    * @return String Trimmed copy of input.
    */
    inline String trim(const char* str) {
        String result(str);
        result.trim();
        return result;
    }

    /**
    * @brief Replace all occurrences of a substring with another substring.
    * 
    * @param str Input C-string.
    * @param from Substring to replace.
    * @param to Replacement substring.
    * @return String Modified copy with replacements.
    */
    inline String replace(const char* str, const char* from, const char* to) {
        String result(str);
        result.replace(from, to);
        return result;
    }

    /**
    * @brief Formats a status + message JSON string.
    *
    * Example:
    * String json = formatStatus(STATUS::ERROR,
    *                             "Failed to connect to controller %s at %s",
    *                             ctrlName.c_str(), mac.c_str());
    *
    * Produces:
    * {
    *   "status": "error",
    *   "message": "Failed to connect to controller foo at bar"
    * }
    *
    * @param status  Status string, e.g., STATUS::OK or STATUS::ERROR
    * @param fmt     printf-style format string
    * @param ...     arguments for the format string
    * @return JSON string with keys `status` and `message`
    */
    inline String formatStatus(const char* status, const char* fmt, ...) {
        char msgBuf[128];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
        va_end(args);

        StaticJsonDocument<256> doc;
        doc["status"]  = status;
        doc["message"] = msgBuf;

        String json;
        serializeJson(doc, json);
        return json;
    }


} // namespace StringUtils
