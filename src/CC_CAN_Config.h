#pragma once

#include <Arduino.h>
#include "Config.h"

/*
 * CanConfig.h — Gestion Canton 2026 (version CanUniversal)
 * ---------------------------------------------------------------------------
 * Initialise le bus CAN interne du CantonController via CanUniversal.
 *
 * Rôle :
 *   - fournir un point d’entrée unique pour l’initialisation CAN
 *   - déléguer la configuration matérielle à un CanConfigProvider interne
 *   - appeler CanInit pour initialiser le bus CAN de manière robuste
 *
 * Ce module ne contient aucune logique métier :
 *   → il prépare simplement le bus CAN utilisé par le CantonController.
 */

class CcCanConfig
{
public:
    // Classe non instanciable
    CcCanConfig() = delete;

    // Initialise le(s) bus CAN via CanUniversal
    static void setup();
};
