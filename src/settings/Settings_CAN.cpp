/*
 * Settings_CAN.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Dialogue CAN initial avec la carte Main :
 *
 *   Étape 1 : attendre CMD_SAT_TEST_BUS_REPLY (MainBoard prête)
 *   Étape 2 : demander un ID si le CC n’en possède pas (CMD_SAT_REQUEST_ID)
 *   Étape 3 : gérer les timeouts (reboot si MainBoard absente)
 *
 * Ce module est totalement indépendant :
 *   - pas d’UART
 *   - pas de SPIFFS
 *   - pas de JSON
 *
 * Appelé depuis Settings::begin() (wrapper public).
 */

#include "Settings.h"
#include "Canton.h"
#include "CanMsg.h"
#include "debug_cc.h"

/* ============================================================================
 *  Callback : la carte Main a envoyé CMD_SAT_TEST_BUS_REPLY
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
    Serial.printf("[Settings][CAN][CC] Attente de la carte Main (CMD_SAT_TEST_BUS_REPLY)...\n");

    uint8_t countReset = 0;

    // -----------------------------------------------------------------------
    // Mode autonome (tests Web sans carte Main)
    // -----------------------------------------------------------------------
#if (CC_STANDALONE_MODE)
    {
        CC_LOG_WARN("[Settings][CAN][CC] Mode standalone actif — handshake CAN ignoré");

        // Autoriser la WebUI même sans MainBoard
        Settings::isMainReady = true;

        return true;
    }
#endif

    // -----------------------------------------------------------------------
    // Étape 1 — Attente du message CMD_SAT_TEST_BUS_REPLY
    // -----------------------------------------------------------------------
    while (!isMainReady)
    {
        CanMsg::sendMsg(0, CMD_SAT_TEST_BUS, 0, canton->ID());
        vTaskDelay(pdMS_TO_TICKS(1000));
        Serial.print(".");

        if (++countReset >= 10)
        {
            Serial.printf("\n[Settings][CAN][CC] ❌ MainBoard ne répond pas → reboot dans 5s\n");
            delay(5000);
            esp_restart();
        }
    }

    Serial.printf("\n[Settings][CAN][CC] ✔ Carte Main prête\n");

    // -----------------------------------------------------------------------
    // Étape 2 — Demande d’ID si nécessaire
    // -----------------------------------------------------------------------
    if (canton->ID() == UNUSED_ID)
    {
        Serial.printf("[Settings][CAN][CC] Le CC n’a pas d’ID → demande CMD_SAT_REQUEST_ID\n");

        while (canton->ID() == UNUSED_ID)
        {
            CanMsg::sendMsg(0, CMD_SAT_REQUEST_ID, 0, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
            Serial.print(".");
        }

        Serial.printf("\n[Settings][CAN][CC] ✔ ID attribué : %d\n", canton->ID());

        // Sauvegarde settings.json
        writeFile(Settings::canton);
    }

    // -----------------------------------------------------------------------
    // Fin du handshake CAN
    // -----------------------------------------------------------------------
    Serial.printf("[Settings][CAN][CC] ✔ Initialisation CAN terminée\n");
    Serial.printf("---------------------------------------------\n\n");

    return true;
}
