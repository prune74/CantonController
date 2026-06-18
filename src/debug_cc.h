#pragma once
#include <Arduino.h>
#include "Config.h"

/*
 * debug_cc.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Système de logs unifié pour le Canton Controller (CC).
 *
 * Caractéristiques :
 *   - timestamp intégré [HH:MM:SS]
 *   - couleurs ANSI (désactivées automatiquement sur moniteurs non compatibles)
 *   - niveaux de logs :
 *        CC_DEBUG = 0 → aucun log
 *        CC_DEBUG = 1 → INFO / WARN / ERROR
 *        CC_DEBUG = 2 → + TRACE
 *
 * Objectif :
 *   → fournir une trace claire, lisible et homogène dans tous les modules CC.
 */

// ---------------------------------------------------------------------------
// Couleurs ANSI
// ---------------------------------------------------------------------------
#define ANSI_RESET  "\033[0m"
#define ANSI_RED    "\033[31m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_GREEN  "\033[32m"
#define ANSI_CYAN   "\033[36m"
#define ANSI_GRAY   "\033[90m"

// ---------------------------------------------------------------------------
// Timestamp [HH:MM:SS]
// ---------------------------------------------------------------------------
inline const char *CC_timestamp()
{
    static char buffer[16];
    unsigned long sec = millis() / 1000;

    uint8_t h = (sec / 3600) % 24;
    uint8_t m = (sec / 60) % 60;
    uint8_t s = sec % 60;

    snprintf(buffer, sizeof(buffer), "[%02u:%02u:%02u]", h, m, s);
    return buffer;
}

// ---------------------------------------------------------------------------
// Niveaux de logs
// ---------------------------------------------------------------------------
// CC_DEBUG = 0 → tout désactivé
// CC_DEBUG = 1 → INFO / WARN / ERROR
// CC_DEBUG = 2 → + TRACE
// ---------------------------------------------------------------------------

#if CC_DEBUG >= 1
    #define CC_LOG_INFO(fmt, ...)  Serial.printf("%s %s[INFO]  "  fmt ANSI_RESET, CC_timestamp(), ANSI_GREEN,  ##__VA_ARGS__)
    #define CC_LOG_WARN(fmt, ...)  Serial.printf("%s %s[WARN]  "  fmt ANSI_RESET, CC_timestamp(), ANSI_YELLOW, ##__VA_ARGS__)
    #define CC_LOG_ERROR(fmt, ...) Serial.printf("%s %s[ERROR] " fmt ANSI_RESET, CC_timestamp(), ANSI_RED,    ##__VA_ARGS__)
#else
    #define CC_LOG_INFO(fmt, ...)
    #define CC_LOG_WARN(fmt, ...)
    #define CC_LOG_ERROR(fmt, ...)
#endif

#if CC_DEBUG >= 2
    #define CC_LOG_TRACE(fmt, ...) Serial.printf("%s %s[TRACE] " fmt ANSI_RESET, CC_timestamp(), ANSI_GRAY, ##__VA_ARGS__)
#else
    #define CC_LOG_TRACE(fmt, ...)
#endif
