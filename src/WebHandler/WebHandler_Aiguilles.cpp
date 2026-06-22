/*
 * WebHandler_Aiguilles.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des réglages d’aiguilles via l’interface Web :
 *   - mise à jour des positions logiques droite / déviée
 *   - mise à jour de la vitesse logique (slider 0–10)
 *   - envoi RS485 F1 (servoConfig) et F2 (servoTest)
 *
 * Le CC ne pilote aucun servo :
 *   → il transmet uniquement les paramètres logiques à l’EXCC.
 */

#include "WebHandler.h"
#include "debug_cc.h"
#include "SatTopologieUART.h"
#include "Canton.h"
#include "Aig.h"
#include "Settings.h"
#include "CC_RS485.h"
#include "Exploration_Protocol.h"

// ---------------------------------------------------------------------------
// handleServoSettings()
// ---------------------------------------------------------------------------
void WebHandler::handleServoSettings(JsonDocument &doc) // 🟢
{
    const char *servoId     = doc["servoSettings"][0];
    const uint16_t value    = doc["servoSettings"][1];
    const uint8_t servoName = doc["servoSettings"][2];

    Aig *aig = canton->getAig(servoName);

    if (!aig)
    {
        CC_LOG_ERROR("[Aiguilles][CC] servoSettings: aig[%u] inexistant\n", servoName);
        return;
    }

    // -----------------------------------------------------------------------
    // 1) Mise à jour logique interne + servoCfg
    // -----------------------------------------------------------------------
    if (servoId[2] == '0') // posDroit
    {
        aig->posDroit(value);
        servoCfg[servoName].posDroit = value;

        CC_LOG_INFO("[Aiguilles][CC] posDroit aiguille %u = %u\n", servoName, value);
    }
    else if (servoId[2] == '1') // posDevie
    {
        aig->posDevie(value);
        servoCfg[servoName].posDevie = value;

        CC_LOG_INFO("[Aiguilles][CC] posDevie aiguille %u = %u\n", servoName, value);
    }
    else if (servoId[2] == '2') // speed (slider 0–10)
    {
        servoCfg[servoName].speed = value;

        CC_LOG_INFO("[Aiguilles][CC] speed slider aiguille %u = %u\n",
                    servoName, value);
    }

    // -----------------------------------------------------------------------
    // 2) Lecture interne (source de vérité JSON 2026)
    // -----------------------------------------------------------------------
    uint16_t posDroit    = aig->posDroit();
    uint16_t posDevie    = aig->posDevie();
    uint16_t speedSlider = servoCfg[servoName].speed;

    // Conversion slider → vitesse EXCC
    uint16_t speed = 11000 - (speedSlider * 1000);

    // -----------------------------------------------------------------------
    // 3) EXCC unique : plus d’adresse 0/1, plus de logique H/AH
    // -----------------------------------------------------------------------
    CC_LOG_TRACE("[Aiguilles][CC] EXCC unique pour aiguille %u\n", servoName);

    // -----------------------------------------------------------------------
    // 4) Envoi RS485 F1 : servoConfig
    // -----------------------------------------------------------------------
    envoyerServoConfig(
        servoName,
        posDroit,
        posDevie,
        speed);
}

// ---------------------------------------------------------------------------
// handleServoTest()
// ---------------------------------------------------------------------------
void WebHandler::handleServoTest(JsonDocument &doc) // 🟢
{
    const uint8_t servoName = doc["servoTest"][0];

    Aig *aig = canton->getAig(servoName);

    if (!aig)
    {
        CC_LOG_ERROR("[Aiguilles][CC] servoTest: aig[%u] inexistant\n", servoName);
        return;
    }

    CC_LOG_INFO("[Aiguilles][CC] Test aiguille %u via EXCC unique\n",
                servoName);

    envoyerServoTest(servoName);
}
