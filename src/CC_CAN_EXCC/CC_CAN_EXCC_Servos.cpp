/*
 * CC_CAN_EXCC_Servos.cpp — Gestion Canton 2026 (CAN → EXCC)
 * ---------------------------------------------------------------------------
 * Envoi de la configuration des servos depuis settings.json vers l’EXCC.
 * 
 * Données envoyées :
 *   - position droite
 *   - position déviée
 *   - vitesse (convertie depuis slider)
 */

#include "CC_CAN_EXCC.h"
#include "Settings.h"
#include "Aig.h"
#include "debug_cc.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

namespace CC_CAN_EXCC
{

    /* ============================================================================
     *  Lecture settings.json et transmission SERVO_CONFIG
     * ==========================================================================*/
    void sendConfigurationServosDepuisSettings()
    {
        CC_LOG_INFO("[ServoCAN][CC] Envoi configuration servos (SERVO_CONFIG)...\n");

        // Accès SPIFFS
        if (!SPIFFS.begin(true))
        {
            CC_LOG_ERROR("[ServoCAN][CC] SPIFFS indisponible\n");
            return;
        }

        // Ouverture du fichier JSON
        File file = SPIFFS.open("/settings.json", "r");
        if (!file)
        {
            CC_LOG_ERROR("[ServoCAN][CC] settings.json introuvable\n");
            return;
        }

        // JSON V7
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, file);
        if (err)
        {
            CC_LOG_ERROR("[ServoCAN][CC] Erreur JSON servos\n");
            return;
        }

        // Parcours des servos logiques
        for (uint8_t servo = 0; servo < aigSize; ++servo)
        {
            Aig *aig = Settings::canton->getAig(servo);
            if (!aig)
                continue;

            // Construction des clés JSON
            char keyPosDroit[16];
            char keyPosDevie[16];
            char keySpeed[16];

            snprintf(keyPosDroit, sizeof(keyPosDroit), "aig%uposDroit", servo);
            snprintf(keyPosDevie, sizeof(keyPosDevie), "aig%uposDevie", servo);
            snprintf(keySpeed, sizeof(keySpeed), "aig%uspeed", servo);

            // Lecture des valeurs (fallback = valeurs internes)
            uint16_t posDroit = doc[keyPosDroit] | aig->posDroit();
            uint16_t posDevie = doc[keyPosDevie] | aig->posDevie();
            uint16_t speedSlider = doc[keySpeed] | 5;

            // Conversion slider → vitesse réelle
            uint16_t speed = 11000 - (speedSlider * 1000);

            // Envoi CAN → EXCC
            sendServoConfig(Settings::canton, servo, posDroit, posDevie, speed);

            CC_LOG_INFO("[ServoCAN][CC] SERVO_CONFIG → servo %u (D:%u V:%u S:%u)\n",
                        servo, posDroit, posDevie, speed);
        }

        CC_LOG_INFO("[ServoCAN][CC] Configuration servos envoyée\n");
    }

}
