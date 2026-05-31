/*
 * Settings_CAN.cpp — Dialogue CAN avec la carte Main (Discovery 2026)
 * ---------------------------------------------------------------------------
 * Rôle :
 *   - Attendre que la carte Main signale qu’elle est prête (CMD_SAT_TEST_BUS_REPLY)
 *   - Demander un ID si le satellite n’en possède pas (CMD_SAT_REQUEST_ID)
 *   - Gérer les timeouts et redémarrer si nécessaire
 *   - Logguer proprement chaque étape
 *
 * Ce module est totalement indépendant :
 *   - pas d’UART
 *   - pas de SPIFFS
 *   - pas de JSON
 *
 * Il est appelé depuis Settings::begin() (wrapper public).
 */

#include "settings/Settings_Internal.h"
#include "debug_sa.h" // pour SA_LOG_INFO / WARN / ERROR

/* ============================================================================
 *  Callback : la carte Discovery_Master_Board a envoyé CMD_SAT_TEST_BUS_REPLY (Discovery_Master_Board ready)
 * ==========================================================================*/
void Settings::sMainReady(bool val)
{
    Settings::isMainReady = val;

    // Log Discovery 2026 : on remplace SA_LOG par SA_LOG_INFO
    SA_LOG_INFO("[Settings][CAN] Discovery_Master_Board ready = %d\n", val);
}

/* ============================================================================
 *  beginCAN() — Dialogue CAN initial (implémentation interne)
 * ==========================================================================*/
bool Settings::beginCAN()
{
    Serial.printf("[Settings][CAN] Attente de la carte Discovery_Master_Board (CMD_SAT_TEST_BUS_REPLY)...\n");

    uint8_t countReset = 0;

    /*
     * Étape 1 — Attente du message CMD_SAT_TEST_BUS_REPLY
     */
    while (!isMainReady)
    {
        CanMsg::sendMsg(0, CMD_SAT_TEST_BUS, 0, node->ID());
        vTaskDelay(pdMS_TO_TICKS(1000));
        Serial.print(".");

        if (++countReset >= 10)
        {
            Serial.printf("\n[Settings][CAN] ❌ Discovery_Master_Board ne répond pas → reboot dans 5s\n");
            delay(5000);
            esp_restart();
        }
    }

    Serial.printf("\n[Settings][CAN] ✔ Carte Discovery_Master_Board prête\n");

    /*
     * Étape 2 — Demande d’ID (CMD_SAT_REQUEST_ID) si nécessaire
     */
    if (node->ID() == UNUSED_ID)
    {
        Serial.printf("[Settings][CAN] Le satellite n’a pas d’ID → demande CMD_SAT_REQUEST_ID\n");

        while (node->ID() == UNUSED_ID)
        {
            CanMsg::sendMsg(0, CMD_SAT_REQUEST_ID, 0, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
            Serial.print(".");
        }

        Serial.printf("\n[Settings][CAN] ✔ ID attribué : %d\n", node->ID());
        writeFile(Settings::node); // sauvegarde settings.json
    }

    Serial.printf("[Settings][CAN] ✔ Initialisation CAN terminée\n");
    Serial.printf("---------------------------------------------\n\n");

    return true;
}
