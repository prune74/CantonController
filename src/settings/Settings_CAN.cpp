/*
 * Settings_CAN.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Dialogue CAN initial avec la carte ERM :
 *
 *   Étape 1 : attendre TEST_BUS_REPLY (MainBoard prête)
 *   Étape 2 : demander un ID si le CC n’en possède pas (REQUEST_ID)
 *   Étape 3 : gérer les timeouts (reboot si MainBoard absente)
 */

#include "Settings.h"
#include "Canton.h"
#include "CC_CAN.h"
#include "debug_cc.h"
#include <Protocol.h>

/* ============================================================================
 *  Callback : la carte ERM a envoyé TEST_BUS_REPLY
 * ==========================================================================*/
void Settings::sMainReady(bool val)
{
    Settings::isMainReady = val;
    CC_LOG_INFO("[Settings][CAN][CC] MainBoard ready = %d\n", val);
}

/* ============================================================================
 *  beginCAN() — Dialogue CAN initial
 * ==========================================================================*/
bool Settings::beginCAN()
{
    Serial.printf("[Settings][CAN][CC] Attente de la carte ERM (TEST_BUS_REPLY)...\n");

    uint8_t countReset = 0;

#if (CC_STANDALONE_MODE)
    {
        CC_LOG_WARN("[Settings][CAN][CC] Mode standalone actif — handshake CAN ignoré");
        Settings::isMainReady = true;
        return true;
    }
#endif

    // -----------------------------------------------------------------------
    // Étape 1 — Attente du message TEST_BUS_REPLY
    // -----------------------------------------------------------------------
    while (!isMainReady)
    {
        CC_CAN::sendMsg(
            0,
            (uint16_t)Cmd_CC_to_ERM::TEST_BUS,
            0,
            canton->ID());

        vTaskDelay(pdMS_TO_TICKS(1000));
        Serial.print(".");

        if (++countReset >= 10)
        {
            Serial.printf("\n[Settings][CAN][CC] ❌ ERM ne répond pas → reboot dans 5s\n");
            delay(5000);
            esp_restart();
        }
    }

    Serial.printf("\n[Settings][CAN][CC] ✔ Carte ERM prête\n");

    // -----------------------------------------------------------------------
    // Étape 2 — Demande d’ID si nécessaire
    // -----------------------------------------------------------------------
    if (canton->ID() == UNUSED_ID)
    {
        Serial.printf("[Settings][CAN][CC] Le CC n’a pas d’ID → demande REQUEST_ID\n");

        while (canton->ID() == UNUSED_ID)
        {
            CC_CAN::sendMsg(
                0,
                (uint16_t)Cmd_CC_to_ERM::REQUEST_ID,
                0,
                0);

            vTaskDelay(pdMS_TO_TICKS(1000));
            Serial.print(".");
        }

        Serial.printf("\n[Settings][CAN][CC] ✔ ID attribué : %d\n", canton->ID());

        writeFile(Settings::canton);
    }

    // -----------------------------------------------------------------------
    // Fin du handshake CAN
    // -----------------------------------------------------------------------
    Serial.printf("[Settings][CAN][CC] ✔ Initialisation CAN terminée\n");
    Serial.printf("---------------------------------------------\n\n");

    return true;
}
