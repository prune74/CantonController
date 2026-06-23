/*
 * CanMsg.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Réception et dispatch des trames CAN.
 *
 * Rôle :
 *   - créer la tâche FreeRTOS de réception CAN
 *   - lire les trames CAN entrantes
 *   - décoder l’en-tête 29 bits :
 *        • commande (bits 17..24)
 *        • ID expéditeur (bits 0..15)
 *        • flag response (bit 16)
 *   - router la trame vers le bon handler :
 *        • System
 *        • Exploration
 *        • Exploitation
 *        • Supervision (ex : CC offline)
 *
 * IMPORTANT :
 *   - aucune logique métier
 *   - aucune logique topologique
 *   - aucune logique d’exploitation
 *
 * Ce module ne fait que distribuer les messages CAN.
 */

#include "CanMsg.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Handlers externes
// ---------------------------------------------------------------------------
void handleSystemCommand(uint8_t commande, const CANMessage &frame, Canton *canton, uint16_t idSatExpediteur);
void handleExplorationCommand(uint8_t commande, const CANMessage &frame, Canton *canton, uint16_t idSatExpediteur);
void handleExploitCommand(uint8_t commande, const CANMessage &frame, Canton *canton, uint16_t idSatExpediteur);
void handleSupervisionCommand(uint8_t commande, const CANMessage &frame, Canton *canton);

// ---------------------------------------------------------------------------
// setup() — création de la tâche de réception CAN
// ---------------------------------------------------------------------------
void CanMsg::setup(Canton *canton)
{
    CC_LOG_INFO("[CanMsg][CC] setup\n");

    TaskHandle_t canReceiveHandle = nullptr;

    xTaskCreatePinnedToCore(
        canReceiveMsg,
        "CanReceiveMsg",
        4 * 1024,
        (void *)canton,
        6,
        &canReceiveHandle,
        0);

#ifdef TEST_MEMORY_TASK
    xTaskCreate(
        testMemory,
        "TestMemory",
        2 * 1024,
        (void *)canReceiveHandle,
        2,
        nullptr);
#endif
}

#ifdef TEST_MEMORY_TASK
// ---------------------------------------------------------------------------
// testMemory() — surveille la mémoire de la tâche canReceiveMsg
// ---------------------------------------------------------------------------
void CanMsg::testMemory(void *pvParameters)
{
    TaskHandle_t canReceiveHandle = (TaskHandle_t)pvParameters;
    for (;;)
    {
        UBaseType_t freeStack = uxTaskGetStackHighWaterMark(canReceiveHandle);
        Serial.printf("[CanMsg][CC] free stack = %d bytes\n", freeStack);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
#endif

// ---------------------------------------------------------------------------
// canReceiveMsg() — tâche FreeRTOS de réception / dispatch CAN
// ---------------------------------------------------------------------------
void CanMsg::canReceiveMsg(void *pvParameters)
{
    Canton *canton = (Canton *)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        CANMessage frameIn;

        if (ACAN_ESP32::can.receive(frameIn))
        {
            // -------------------------------------------------------------------
            // 🔥 STOP global (ID = 0x201, 11 bits, DLC = 0)
            // -------------------------------------------------------------------
            if (frameIn.id == EXPLORATION_CAN_ID_EMERGENCY_STOP)
            {
                canton->setStopActive(true);
                CC_LOG_ERROR("[CanMsg][CC] STOP global reçu (canton %u)\n", canton->ID());
                continue;
            }

            // -------------------------------------------------------------------
            // ⚠️ Décodage standard 29 bits
            // -------------------------------------------------------------------
            const uint8_t  commande      = (uint8_t)((frameIn.id & 0x1FE0000) >> 17);
            const uint16_t idExpediteur = (uint16_t)(frameIn.id & 0xFFFF);
            const bool     response     = (frameIn.id & 0x10000) >> 16;
            (void)response;

            // -------------------------------------------------------------------
            // Gestion RTR
            // -------------------------------------------------------------------
            if (frameIn.rtr)
            {
                if (commande == 0x0F)
                    ACAN_ESP32::can.tryToSend(frameIn);

                continue;
            }

            // -------------------------------------------------------------------
            // Dispatch selon la plage de commandes
            // -------------------------------------------------------------------
            if (commande >= CMD_SAT_TEST_BUS_REPLY && commande <= CMD_SAVE_ALL)
            {
                handleSystemCommand(commande, frameIn, canton, idExpediteur);
            }
            else if (commande >= 0xC0 && commande <= 0xC1)
            {
                handleExplorationCommand(commande, frameIn, canton, idExpediteur);
            }
            else if (commande >= 0xE0 && commande <= 0xE9)
            {
                handleExploitCommand(commande, frameIn, canton, idExpediteur);
            }
            else if (commande == CMD_CC_OFFLINE)
            {
                handleSupervisionCommand(commande, frameIn, canton);
            }
            else
            {
                CC_LOG_WARN("[CanMsg][CC] Commande 0x%X non gérée\n", commande);
            }
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
}
