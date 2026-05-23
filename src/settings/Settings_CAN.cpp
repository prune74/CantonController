/*
 * Settings_CAN.cpp — Dialogue CAN avec la carte Main (Discovery 2026)
 * ---------------------------------------------------------------------------
 * Rôle :
 *   - Attendre que la carte Main signale qu’elle est prête (0xB3)
 *   - Demander un ID si le satellite n’en possède pas (0xB4)
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
#include "debug_sa.h"   // pour SA_LOG_INFO / WARN / ERROR

/* ============================================================================
 *  Callback : la carte Main a envoyé 0xB3 (Main ready)
 * ==========================================================================*/
void Settings::sMainReady(bool val)
{
    Settings::isMainReady = val;

    // Log Discovery 2026 : on remplace SA_LOG par SA_LOG_INFO
    SA_LOG_INFO("[Settings][CAN] Main ready = %d\n", val);
}

/* ============================================================================
 *  beginCAN() — Dialogue CAN initial (implémentation interne)
 * ==========================================================================*/
bool Settings::beginCAN()
{
    Serial.printf("[Settings][CAN] Attente de la carte Main (0xB3)...\n");

    uint8_t countReset = 0;

    /*
     * Étape 1 — Attente du message 0xB3
     */
    while (!isMainReady)
    {
        CanMsg::sendMsg(0, 0xB2, 0, node->ID());
        vTaskDelay(pdMS_TO_TICKS(1000));
        Serial.print(".");

        if (++countReset >= 10)
        {
            Serial.printf("\n[Settings][CAN] ❌ Main ne répond pas → reboot dans 5s\n");
            delay(5000);
            esp_restart();
        }
    }

    Serial.printf("\n[Settings][CAN] ✔ Carte Main prête\n");

    /*
     * Étape 2 — Demande d’ID (0xB4) si nécessaire
     */
    if (node->ID() == UNUSED_ID)
    {
        Serial.printf("[Settings][CAN] Le satellite n’a pas d’ID → demande 0xB4\n");

        while (node->ID() == UNUSED_ID)
        {
            CanMsg::sendMsg(0, 0xB4, 0, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
            Serial.print(".");
        }

        Serial.printf("\n[Settings][CAN] ✔ ID attribué : %d\n", node->ID());
        writeFile(Settings::node);  // sauvegarde settings.json
    }

    Serial.printf("[Settings][CAN] ✔ Initialisation CAN terminée\n");
    Serial.printf("---------------------------------------------\n\n");

    return true;
}
