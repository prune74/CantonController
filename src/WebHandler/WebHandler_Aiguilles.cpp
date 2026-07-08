/*
 * WebHandler_Aiguilles.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des réglages d’aiguilles via l’interface Web :
 *   - mise à jour des positions logiques droite / déviée
 *   - mise à jour de la vitesse logique (slider 0–10)
 *   - envoi CAN vers l’EXCC :
 *       - SERVO_CONFIG : servoConfig (positions + vitesse)
 *       - SERVO_TEST : servoTest
 * Le CC ne pilote aucun servo :
 *   → il transmet uniquement les paramètres logiques à l’EXCC via CAN.
 */

#include "WebHandler.h"
#include "debug_cc.h"
#include "Canton.h"
#include "Aig.h"
#include "Settings.h"
#include "CC_CAN_EXCC.h"
#include <Protocol.h>

// ---------------------------------------------------------------------------
// handleServoSettings()
// ---------------------------------------------------------------------------
void WebHandler::handleServoSettings(JsonDocument &doc)
{

    JsonVariant v = doc["servoSettings"];
    if (v.isNull())
    {
        CC_LOG_WARN("[Aiguilles][CC] servoSettings absent\n");
        return;
    }

    JsonArray arr = v.as<JsonArray>();
    if (arr.size() < 3)
    {
        CC_LOG_WARN("[Aiguilles][CC] servoSettings incomplet\n");
        return;
    }

    const char *servoId = arr[0].as<const char *>();
    uint16_t value = arr[1] | 0;
    uint8_t servoName = arr[2] | 0;

    Aig *aig = canton->getAig(servoName);
    if (!aig)
    {
        CC_LOG_ERROR("[Aiguilles][CC] servoSettings: aig[%u] inexistant\n", servoName);
        return;
    }

    auto &cfg = canton->getServoCfg(servoName);

    switch (servoId[2])
    {
    case '0': // posDroit
        aig->posDroit(value);
        cfg.posDroit = value;
        CC_LOG_INFO("[Aiguilles][CC] posDroit aiguille %u = %u\n", servoName, value);
        break;

    case '1': // posDevie
        aig->posDevie(value);
        cfg.posDevie = value;
        CC_LOG_INFO("[Aiguilles][CC] posDevie aiguille %u = %u\n", servoName, value);
        break;

    case '2': // speed
        cfg.speed = value;
        CC_LOG_INFO("[Aiguilles][CC] speed slider aiguille %u = %u\n", servoName, value);
        break;

    default:
        CC_LOG_WARN("[Aiguilles][CC] servoId inconnu : %c\n", servoId[2]);
        break;
    }

    uint16_t posDroit = aig->posDroit();
    uint16_t posDevie = aig->posDevie();
    uint16_t speedSlider = cfg.speed;

    uint16_t speed = 11000 - (speedSlider * 1000);

    CC_LOG_TRACE("[Aiguilles][CC] EXCC unique pour aiguille %u\n", servoName);

    CC_CAN_EXCC::sendServoConfig(
        canton,
        servoName,
        posDroit,
        posDevie,
        speed);
}

// ---------------------------------------------------------------------------
// handleServoTest()
// ---------------------------------------------------------------------------
void WebHandler::handleServoTest(JsonDocument &doc)
{
    JsonVariant v = doc["servoTest"];
    if (v.isNull())
    {
        CC_LOG_WARN("[Aiguilles][CC] servoTest absent\n");
        return;
    }

    JsonArray arr = v.as<JsonArray>();
    if (arr.size() < 1)
    {
        CC_LOG_WARN("[Aiguilles][CC] servoTest incomplet\n");
        return;
    }

    uint8_t servoName = arr[0] | 0;

    Aig *aig = canton->getAig(servoName);
    if (!aig)
    {
        CC_LOG_ERROR("[Aiguilles][CC] servoTest: aig[%u] inexistant\n", servoName);
        return;
    }

    CC_LOG_INFO("[Aiguilles][CC] Test aiguille %u via EXCC unique\n", servoName);

    CC_CAN_EXCC::sendServoTest(canton, servoName);
}