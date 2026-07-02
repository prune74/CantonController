/*
 * CC_CAN_EXCC_Signaux.cpp — Gestion Canton 2026 (CAN → EXCC)
 * ---------------------------------------------------------------------------
 * Envoi de la configuration des signaux depuis settings.json vers l’EXCC.
 *
 * Données envoyées :
 *   - type signal 0
 *   - type signal 1
 *   - position signal 0
 *   - position signal 1
 */

#include "CC_CAN_EXCC.h"
#include "Settings.h"
#include "debug_cc.h"

#include <SPIFFS.h>
#include <ArduinoJson.h>

namespace CC_CAN_EXCC
{

    /* ============================================================================
     *  Lecture settings.json et transmission CONFIG_SIGNAUX
     * ==========================================================================*/
    void sendConfigurationSignauxDepuisSettings()
    {
        CC_LOG_INFO("[SignauxCAN][CC] Envoi configuration signaux (CONFIG_SIGNAUX)...\n");

        // Accès SPIFFS
        if (!SPIFFS.begin(true))
        {
            CC_LOG_ERROR("[SignauxCAN][CC] SPIFFS indisponible\n");
            return;
        }

        // Ouverture du fichier JSON
        File file = SPIFFS.open("/settings.json", "r");
        if (!file)
        {
            CC_LOG_ERROR("[SignauxCAN][CC] settings.json introuvable\n");
            return;
        }

        // JSON V7
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, file);
        if (err)
        {
            CC_LOG_ERROR("[SignauxCAN][CC] Erreur JSON\n");
            return;
        }

        // Lecture V7 : JsonVariant + isNull() + v | fallback
        JsonVariant v0 = doc["sign0"];
        JsonVariant v1 = doc["sign1"];

        bool sign0Absent = v0.isNull();
        bool sign1Absent = v1.isNull();

        uint8_t type0 = sign0Absent ? SIG_ABSENT : (uint8_t)(doc["sign0type"] | 0);
        uint8_t type1 = sign1Absent ? SIG_ABSENT : (uint8_t)(doc["sign1type"] | 0);

        uint8_t pos0 = sign0Absent ? 0 : (uint8_t)(doc["sign0position"] | 0);
        uint8_t pos1 = sign1Absent ? 0 : (uint8_t)(doc["sign1position"] | 0);

        CC_LOG_TRACE("[SignauxCAN][CC] signaux : T0=%u T1=%u P0=%u P1=%u\n",
                     type0, type1, pos0, pos1);

        // Envoi CAN → EXCC
        sendConfigSignaux(Settings::canton, type0, type1, pos0, pos1);

        CC_LOG_INFO("[SignauxCAN][CC] Configuration signaux envoyée (CONFIG_SIGNAUX)\n");
    }

}
