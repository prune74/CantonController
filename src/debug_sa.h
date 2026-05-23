#pragma once
#include <Arduino.h>
#include "Config.h"

// ============================================================
//  Couleurs ANSI
// ============================================================
#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_GRAY    "\033[90m"

// ============================================================
//  Timestamp [HH:MM:SS]
// ============================================================
inline const char* sa_timestamp()
{
    static char buffer[16];
    unsigned long ms = millis() / 1000;
    uint8_t h = (ms / 3600) % 24;
    uint8_t m = (ms / 60) % 60;
    uint8_t s = ms % 60;
    snprintf(buffer, sizeof(buffer), "[%02u:%02u:%02u]", h, m, s);
    return buffer;
}

// ============================================================
//  Niveaux de logs
// ============================================================
// SA_DEBUG = 0 → tout désactivé
// SA_DEBUG = 1 → INFO/WARN/ERROR
// SA_DEBUG = 2 → + TRACE
// ============================================================

#if SA_DEBUG >= 1
    #define SA_LOG_INFO(fmt, ...)  Serial.printf("%s %s[INFO]  " fmt ANSI_RESET,  sa_timestamp(), ANSI_GREEN,  ##__VA_ARGS__)
    #define SA_LOG_WARN(fmt, ...)  Serial.printf("%s %s[WARN]  " fmt ANSI_RESET,  sa_timestamp(), ANSI_YELLOW, ##__VA_ARGS__)
    #define SA_LOG_ERROR(fmt, ...) Serial.printf("%s %s[ERROR] " fmt ANSI_RESET,  sa_timestamp(), ANSI_RED,    ##__VA_ARGS__)
#else
    #define SA_LOG_INFO(fmt, ...)
    #define SA_LOG_WARN(fmt, ...)
    #define SA_LOG_ERROR(fmt, ...)
#endif

#if SA_DEBUG >= 2
    #define SA_LOG_TRACE(fmt, ...) Serial.printf("%s %s[TRACE] " fmt ANSI_RESET, sa_timestamp(), ANSI_GRAY, ##__VA_ARGS__)
#else
    #define SA_LOG_TRACE(fmt, ...)
#endif
