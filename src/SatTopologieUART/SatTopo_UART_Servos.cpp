/*
 * SatTopo_UART_Servos.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Transmission des commandes servo vers les EXCC via UART.
 *
 * OpCodes :
 *   - F0 : mouvement réel (servoMove)
 *   - F1 : configuration servo (posDroit / posDevie / speed)
 *   - F2 : test servo
 *
 * Rôle :
 *   - envoyer les mouvements réels (F0)
 *   - envoyer la configuration physique issue de settings.json (F1)
 *   - envoyer une commande de test (F2)
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune décision droit/dévié
 *   - aucune interpolation servo
 *
 * Toute la logique métier est dans Aig.cpp et Canton_Aiguilles.cpp.
 * Ce module ne fait que traduire les paramètres Exploration 2026
 * (slider 0–10) en valeurs EXCC (11000 → 1000).
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_cc.h"

#include "Settings.h"
#include "Aig.h"
#include "Canton.h"

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>

extern HardwareSerial Serial1;

/* ============================================================================
 *  F0 — Mouvement réel du servo
 * ==========================================================================*/
void envoyerServoMove(uint8_t exccAdresse, uint8_t servoIndex)
{
    CC_LOG_INFO("[ServoUART][CC] Move : excc=%u servo=%u\n",
                exccAdresse, servoIndex);

    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_F0_SERVO_MOVE);
    Serial1.write(exccAdresse);
    Serial1.write(servoIndex);
}

/* ============================================================================
 *  Envoi de la configuration des servos depuis settings.json (F1)
 * ==========================================================================*/
void envoyerConfigurationServosDepuisSettings()
{
    CC_LOG_INFO("[ServoUART][CC] Envoi configuration servos (F1)...\n");

    if (!SPIFFS.begin(true))
    {
        CC_LOG_ERROR("[ServoUART][CC] SPIFFS indisponible\n");
        return;
    }

    File file = SPIFFS.open("/settings.json", "r");
    if (!file)
    {
        CC_LOG_ERROR("[ServoUART][CC] settings.json introuvable\n");
        return;
    }

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        CC_LOG_ERROR("[ServoUART][CC] Erreur JSON servos : %s\n", error.c_str());
        return;
    }

    for (uint8_t servo = 0; servo < 6; ++servo)
    {
        Aig *aig = Settings::canton->getAig(servo);
        if (!aig)
            continue;

        // -------------------------------------------------------------------
        // Déterminer quel EXCC pilote ce servo
        // -------------------------------------------------------------------
        uint8_t exccAdresse =
            (aig->cantonPdroitIdx() == Settings::canton->SP1_idx()) ? 0 : 1;

        // -------------------------------------------------------------------
        // Lecture JSON (source de vérité)
        // -------------------------------------------------------------------
        char keyPosDroit[16];
        char keyPosDevie[16];
        char keySpeed[16];

        snprintf(keyPosDroit, sizeof(keyPosDroit), "aig%uposDroit", servo);
        snprintf(keyPosDevie, sizeof(keyPosDevie), "aig%uposDevie", servo);
        snprintf(keySpeed, sizeof(keySpeed), "aig%uspeed", servo);

        uint16_t posDroit = doc[keyPosDroit] | aig->posDroit();
        uint16_t posDevie = doc[keyPosDevie] | aig->posDevie();

        // speed = slider 0–10
        uint16_t speedSlider = doc[keySpeed] | 5;

        // Conversion Exploration 2026 → EXCC
        uint16_t speed = 11000 - (speedSlider * 1000);

        // -------------------------------------------------------------------
        // Envoi F1
        // -------------------------------------------------------------------
        envoyerServoConfig(exccAdresse, servo, posDroit, posDevie, speed);

        CC_LOG_INFO("[ServoUART][CC] F1 → EXCC %u, servo %u (D:%u V:%u S:%u)\n",
                    exccAdresse, servo, posDroit, posDevie, speed);
    }

    CC_LOG_INFO("[ServoUART][CC] Configuration servos envoyée\n");
}

/* ============================================================================
 *  F1 — Configuration servo
 * ==========================================================================*/
void envoyerServoConfig(uint8_t exccAdresse,
                        uint8_t servoIndex,
                        uint16_t posDroit,
                        uint16_t posDevie,
                        uint16_t speed)
{
    CC_LOG_INFO("[ServoUART][CC] Config : excc=%u servo=%u droit=%u devie=%u speed=%u\n",
                exccAdresse, servoIndex, posDroit, posDevie, speed);

    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_F1_SERVO_CONFIG);
    Serial1.write(exccAdresse);
    Serial1.write(servoIndex);

    Serial1.write(posDroit >> 8);
    Serial1.write(posDroit & 0xFF);

    Serial1.write(posDevie >> 8);
    Serial1.write(posDevie & 0xFF);

    Serial1.write(speed >> 8);
    Serial1.write(speed & 0xFF);
}

/* ============================================================================
 *  F2 — Test servo
 * ==========================================================================*/
void envoyerServoTest(uint8_t exccAdresse, uint8_t servoIndex)
{
    CC_LOG_INFO("[ServoUART][CC] Test : excc=%u servo=%u\n",
                exccAdresse, servoIndex);

    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_F2_SERVO_TEST);
    Serial1.write(exccAdresse);
    Serial1.write(servoIndex);
}
