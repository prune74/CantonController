#pragma once

#include <Arduino.h>
#include <ACAN_ESP32.h>
#include "Config.h"

/*
 * CanConfig.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Configuration du contrôleur CAN interne de l’ESP32 (TWAI).
 *
 * Rôle :
 *   - configurer les pins RX/TX définies dans Config.h
 *   - initialiser le contrôleur CAN interne via ACAN_ESP32
 *   - vérifier le code retour et logguer l’état
 *
 * Ce module ne contient aucune logique métier :
 *   → il prépare simplement le bus CAN Exploration 2026.
 */

class CanConfig
{
public:
    // Classe non instanciable
    CanConfig() = delete;

    // Initialise le contrôleur CAN interne
    static void setup();
};
