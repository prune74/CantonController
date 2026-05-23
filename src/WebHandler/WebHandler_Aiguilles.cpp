/*
   WebHandler_Aiguilles.cpp — Discovery 2026 (CLEAN & FIXED)
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include "SatTopologieUART.h"
#include "Node.h"
#include "Aig.h"
#include "Settings.h"   // pour mise à jour JSON

// ---------------------------------------------------------------------------
// handleServoSettings()
// ---------------------------------------------------------------------------
void WebHandler::handleServoSettings(JsonDocument &doc)
{
    const char *servoId     = doc["servoSettings"][0];
    const uint16_t value    = doc["servoSettings"][1];   // slider ou position
    const uint8_t servoName = doc["servoSettings"][2];

    Aig* aig = node->getAig(servoName);

    if (!aig)
    {
        SA_LOG_ERROR("[Aiguilles] servoSettings: aig[%u] inexistant\n", servoName);
        return;
    }

    // -----------------------------------------------------------------------
    // 1) Mise à jour logique interne + servoCfg (UI)
    // -----------------------------------------------------------------------
    char key[32];

    if (servoId[2] == '0')   // posDroit
    {
        aig->posDroit(value);
        servoCfg[servoName].posDroit = value;

        snprintf(key, sizeof(key), "aig%uposDroit", servoName);
        Settings::set(key, value);

        SA_LOG_INFO("[Aiguilles] posDroit aiguille %u = %u\n", servoName, value);
    }
    else if (servoId[2] == '1')  // posDevie
    {
        aig->posDevie(value);
        servoCfg[servoName].posDevie = value;

        snprintf(key, sizeof(key), "aig%uposDevie", servoName);
        Settings::set(key, value);

        SA_LOG_INFO("[Aiguilles] posDevie aiguille %u = %u\n", servoName, value);
    }
    else if (servoId[2] == '2')  // speed (slider 0–10)
    {
        servoCfg[servoName].speed = value;   // slider 0–10

        snprintf(key, sizeof(key), "aig%uspeed", servoName);
        Settings::set(key, value);           // on stocke le slider dans JSON

        doc["servoSettingsSpeed"] = value;

        SA_LOG_INFO("[Aiguilles] speed slider aiguille %u = %u\n",
                    servoName, value);
    }

    // Sauvegarde settings.json
    Settings::save();

    // Recharger pour cohérence
    Settings::load();

    // -----------------------------------------------------------------------
    // 2) Lire les valeurs JSON (source de vérité)
    // -----------------------------------------------------------------------
    snprintf(key, sizeof(key), "aig%uposDroit", servoName);
    uint16_t posDroit = Settings::get(key);

    snprintf(key, sizeof(key), "aig%uposDevie", servoName);
    uint16_t posDevie = Settings::get(key);

    snprintf(key, sizeof(key), "aig%uspeed", servoName);
    uint16_t speedSlider = Settings::get(key);   // 0–10

    uint16_t speed = 11000 - (speedSlider * 1000);

    // -----------------------------------------------------------------------
    // 3) Détermination EXSA H/AH
    // -----------------------------------------------------------------------
    uint8_t exsaAdresse =
        (aig->nodePdroitIdx() == node->SP1_idx()) ? 0 : 1;

    SA_LOG_TRACE("[Aiguilles] EXSA sélectionné = %u pour aiguille %u\n",
                 exsaAdresse, servoName);

    // -----------------------------------------------------------------------
    // 4) Envoi RS485 F1 : servoConfig (Discovery 2026)
    // -----------------------------------------------------------------------
    envoyerServoConfig(
        exsaAdresse,
        servoName,
        posDroit,
        posDevie,
        speed
    );
}

// ---------------------------------------------------------------------------
// handleServoTest()
// ---------------------------------------------------------------------------
void WebHandler::handleServoTest(JsonDocument &doc)
{
    const uint8_t servoName = doc["servoTest"][0];

    Aig* aig = node->getAig(servoName);

    if (!aig)
    {
        SA_LOG_ERROR("[Aiguilles] servoTest: aig[%u] inexistant\n", servoName);
        return;
    }

    uint8_t exsaAdresse =
        (aig->nodePdroitIdx() == node->SP1_idx()) ? 0 : 1;

    SA_LOG_INFO("[Aiguilles] Test aiguille %u via EXSA %u\n",
                servoName, exsaAdresse);

    envoyerServoTest(exsaAdresse, servoName);
}
