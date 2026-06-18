#pragma once

#include <Arduino.h>
#include <core_version.h>   // ARDUINO_ESP32_RELEASE
#include "Config.h"
#include "debug_cc.h"

/*
 * ChipInfo.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Affiche les informations matérielles de l’ESP32 :
 *   - version Arduino-ESP32
 *   - révision du chip
 *   - SDK
 *   - fréquence CPU
 *   - nombre de cœurs
 *   - taille de la flash (interne/externe)
 *   - horloge APB
 *   - mémoire libre
 *
 * Ce module est purement informatif :
 *   → aucune logique métier
 *   → aucune dépendance au Canton
 */

class ChipInfo
{
public:
    // Affiche les informations du chip ESP32
    static void print();
};
