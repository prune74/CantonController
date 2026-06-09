/*
   WebHandler_Aiguilles.cpp — Exploration 2026 (FINAL & CLEAN)
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include "SatTopologieUART.h"
#include "Canton.h"
#include "Aig.h"
#include "Settings.h"
#include "SA_RS485.h"
#include "Exploration_Protocol.h"

// ---------------------------------------------------------------------------
// handleServoSettings()
// ---------------------------------------------------------------------------
void WebHandler::handleServoSettings(JsonDocument &doc)
{
    const char *servoId = doc["servoSettings"][0];
    const uint16_t value = doc["servoSettings"][1];
    const uint8_t servoName = doc["servoSettings"][2];

    Aig *aig = canton->getAig(servoName);

    if (!aig)
    {
        SA_LOG_ERROR("[Aiguilles] servoSettings: aig[%u] inexistant\n", servoName);
        return;
    }

    char key[32];

    // -----------------------------------------------------------------------
    // 1) Mise à jour logique interne + servoCfg
    // -----------------------------------------------------------------------
    if (servoId[2] == '0') // posDroit
    {
        aig->posDroit(value);
        servoCfg[servoName].posDroit = value;

        snprintf(key, sizeof(key), "aig%uposDroit", servoName);
        Settings::set(key, value);

        SA_LOG_INFO("[Aiguilles] posDroit aiguille %u = %u\n", servoName, value);
    }
    else if (servoId[2] == '1') // posDevie
    {
        aig->posDevie(value);
        servoCfg[servoName].posDevie = value;

        snprintf(key, sizeof(key), "aig%uposDevie", servoName);
        Settings::set(key, value);

        SA_LOG_INFO("[Aiguilles] posDevie aiguille %u = %u\n", servoName, value);
    }
    else if (servoId[2] == '2') // speed (slider 0–10)
    {
        servoCfg[servoName].speed = value;

        snprintf(key, sizeof(key), "aig%uspeed", servoName);
        Settings::set(key, value);

        SA_LOG_INFO("[Aiguilles] speed slider aiguille %u = %u\n",
                    servoName, value);
    }

    Settings::save();
    Settings::load();

    // -----------------------------------------------------------------------
    // 2) Lecture JSON (source de vérité)
    // -----------------------------------------------------------------------
    snprintf(key, sizeof(key), "aig%uposDroit", servoName);
    uint16_t posDroit = Settings::get(key);

    snprintf(key, sizeof(key), "aig%uposDevie", servoName);
    uint16_t posDevie = Settings::get(key);

    snprintf(key, sizeof(key), "aig%uspeed", servoName);
    uint16_t speedSlider = Settings::get(key);

    uint16_t speed = 11000 - (speedSlider * 1000);

    // -----------------------------------------------------------------------
    // 3) Détermination EXSA H/AH
    // -----------------------------------------------------------------------
    uint8_t exsaAdresse =
        (aig->cantonPdroitIdx() == canton->SP1_idx()) ? 0 : 1;

    SA_LOG_TRACE("[Aiguilles] EXSA sélectionné = %u pour aiguille %u\n",
                 exsaAdresse, servoName);

    // -----------------------------------------------------------------------
    // 4) Envoi RS485 F1 : servoConfig
    // -----------------------------------------------------------------------
    envoyerServoConfig(
        exsaAdresse,
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
    const uint8_t servoName = doc["servoTest"][0];

    Aig *aig = canton->getAig(servoName);

    if (!aig)
    {
        SA_LOG_ERROR("[Aiguilles] servoTest: aig[%u] inexistant\n", servoName);
        return;
    }

    uint8_t exsaAdresse =
        (aig->cantonPdroitIdx() == canton->SP1_idx()) ? 0 : 1;

    SA_LOG_INFO("[Aiguilles] Test aiguille %u via EXSA %u\n",
                servoName, exsaAdresse);

    envoyerServoTest(exsaAdresse, servoName);
}
