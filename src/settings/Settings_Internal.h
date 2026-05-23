#pragma once

#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>

#include <map>
#include <string>

#include "Settings.h"
#include "Node.h"
#include "Aig.h"
#include "Signal.h"
#include "CanMsg.h"
#include "Discovery.h"
/* ============================================================================
 *  Settings_Internal.h — Implémentation interne Settings (Discovery 2026)
 * ---------------------------------------------------------------------------
 * Ce fichier contient l’implémentation interne des fonctions
 * Settings qui ne sont pas destinées à être exposées publiquement.
 *
 * Il est inclus par les fichiers sources spécialisés :
 *   - Settings_UART.cpp
 *   - Settings_SPIFFS.cpp
 *   - Settings_JSON.cpp
 *   - Settings_CAN.cpp
 *
 * Il ne doit JAMAIS être inclus directement par du code extérieur.
 * ---------------------------------------------------------------------------
 */
