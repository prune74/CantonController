/*
 * SatTopo_UART_Signaux.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Transmission des informations de signaux ferroviaires vers les EXCC via UART.
 *
 * OpCodes :
 *   - E5 : configuration des signaux (type + position)
 *   - E6 : aspect SNCF horaire
 *   - E7 : aspect SNCF anti‑horaire
 *
 * Rôle :
 *   - lire les types/positions depuis settings.json
 *   - transmettre la configuration des mâts (E5)
 *   - transmettre les aspects calculés par SupervisionCanton (E6/E7)
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune décision d’aspect
 *   - aucune interprétation SNCF
 *
 * Toute la logique métier est dans Settings_JSON_Signaux.cpp
 * et SupervisionCanton.cpp.
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_cc.h"

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>

extern HardwareSerial Serial1;

/* ============================================================================
 *  envoyerConfigurationSignauxDepuisSettings() — opcode E5
 * ---------------------------------------------------------------------------
 *  Envoie type + position des deux mâts (sign0 / sign1).
 * ==========================================================================*/
void envoyerConfigurationSignauxDepuisSettings() // 🟢
{
    CC_LOG_INFO("[TopoUART][CC] Envoi configuration signaux (E5)...\n");

    if (!SPIFFS.begin(true))
    {
        CC_LOG_ERROR("[TopoUART][CC] SPIFFS indisponible\n");
        return;
    }

    File file = SPIFFS.open("/settings.json", "r");
    if (!file)
    {
        CC_LOG_ERROR("[TopoUART][CC] settings.json introuvable\n");
        return;
    }

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        CC_LOG_ERROR("[TopoUART][CC] Erreur JSON : %s\n", error.c_str());
        return;
    }

    // -----------------------------------------------------------------------
    // Détection des mâts absents
    // -----------------------------------------------------------------------
    bool sign0Absent = doc["sign0"].isNull();
    bool sign1Absent = doc["sign1"].isNull();

    uint8_t type0 = sign0Absent ? SIG_ABSENT : (uint8_t)(doc["sign0type"]     | 0);
    uint8_t type1 = sign1Absent ? SIG_ABSENT : (uint8_t)(doc["sign1type"]     | 0);

    uint8_t pos0  = sign0Absent ? 0          : (uint8_t)(doc["sign0position"] | 0);
    uint8_t pos1  = sign1Absent ? 0          : (uint8_t)(doc["sign1position"] | 0);

    CC_LOG_TRACE("[TopoUART][CC] signaux : T0=%u T1=%u P0=%u P1=%u (abs0=%d abs1=%d)\n",
                 type0, type1, pos0, pos1, sign0Absent, sign1Absent);

    // -----------------------------------------------------------------------
    // Envoi E5
    // -----------------------------------------------------------------------
    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_E5_CONFIG_SIGNAUX);
    Serial1.write(type0);
    Serial1.write(type1);
    Serial1.write(pos0);
    Serial1.write(pos1);

    CC_LOG_INFO("[TopoUART][CC] Configuration signaux envoyée (E5)\n");
}

/* ============================================================================
 *  envoyerAspectSignalHoraire() — opcode E6
 * ==========================================================================*/
void envoyerAspectSignalHoraire(uint8_t aspect) // 🟢
{
    CC_LOG_TRACE("[TopoUART][CC] Aspect horaire (E6) = %u\n", aspect);

    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_E6_ASPECT_HORAIRE);
    Serial1.write(aspect);
}

/* ============================================================================
 *  envoyerAspectSignalAntiHoraire() — opcode E7
 * ==========================================================================*/
void envoyerAspectSignalAntiHoraire(uint8_t aspect) // 🟢
{
    CC_LOG_TRACE("[TopoUART][CC] Aspect anti-horaire (E7) = %u\n", aspect);

    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_E7_ASPECT_ANTIHORAIRE);
    Serial1.write(aspect);
}
