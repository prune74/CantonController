/*
 * CC_CAN.cpp — Gestion Canton 2026 (version CanUniversal)
 * ---------------------------------------------------------------------------
 * Réception et dispatch des trames CAN.
 */

#include "CanMsg.h" // CanUniversal
#include "CanBus.h" // CanUniversal

#include "CC_CAN_Config.h"
#include "CC_CAN.h"
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
void CC_CAN::setup(Canton *canton)
{
    CC_LOG_INFO("[CC_CAN][CC] setup\n");

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
void CC_CAN::testMemory(void *pvParameters)
{
    TaskHandle_t canReceiveHandle = (TaskHandle_t)pvParameters;
    for (;;)
    {
        UBaseType_t freeStack = uxTaskGetStackHighWaterMark(canReceiveHandle);
        Serial.printf("[CC_CAN][CC] free stack = %d bytes\n", freeStack);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
#endif

// ---------------------------------------------------------------------------
// canReceiveMsg() — tâche FreeRTOS de réception / dispatch CAN
// ---------------------------------------------------------------------------
void CC_CAN::canReceiveMsg(void *pvParameters)
{
    Canton *canton = (Canton *)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        // Lecture via CanUniversal
        CanMsg msg;

        if (CanBus::bus(0).receive(msg))
        {
            // STOP global
            if (msg.is11() && msg.id == EXPLORATION_CAN_ID_EMERGENCY_STOP)
            {
                canton->setStopActive(true);
                CC_LOG_ERROR("[CC_CAN][CC] STOP global reçu (canton %u)\n", canton->ID());
                vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
                continue;
            }

            // ID 29 bits
            if (!msg.is29())
            {
                vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
                continue;
            }

            const uint8_t commande = msg.cmde();
            const uint16_t idExpediteur = msg.nodeId();
            const bool response = msg.resp();
            (void)response;

            // Conversion vers CANMessage
            CANMessage frameIn = msg.toFrame();

            // Gestion RTR
            if (frameIn.rtr)
            {
                if (commande == 0x0F)
                    CanBus::bus(0).send(msg);

                vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
                continue;
            }

            // Dispatch
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
                CC_LOG_WARN("[CC_CAN][CC] Commande 0x%X non gérée\n", commande);
            }
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
}
