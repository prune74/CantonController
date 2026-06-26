/*
 * CC_CAN.cpp — Gestion Canton 2026 (version CanUniversal)
 * ---------------------------------------------------------------------------
 * Réception et dispatch des trames CAN.
 * Bus 0 = réseau Discovery
 * Bus 1 = EXCC (MCP2515)
 */

#include "CC_CAN.h"
#include "CC_CAN_Config.h"
#include "CC_CAN_EXCC.h"

#include "debug_cc.h"

#include "CanMsg.h"
#include "CanBus.h"

// Handlers CAN modernes
void handleSystemCommand(uint8_t commande, const CanMsg &msg, Canton *canton, uint16_t idSatExpediteur);
void handleExplorationCommand(uint8_t commande, const CanMsg &msg, Canton *canton, uint16_t idSatExpediteur);
void handleExploitCommand(uint8_t commande, const CanMsg &msg, Canton *canton, uint16_t idSatExpediteur);
void handleSupervisionCommand(uint8_t commande, const CanMsg &msg, Canton *canton);

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
// ---------------------------------------------------------------------------
// testMemory() — surveillance de la stack FreeRTOS
// ---------------------------------------------------------------------------
void CC_CAN::testMemory(void *pvParameters)
{
    TaskHandle_t canReceiveHandle = (TaskHandle_t)pvParameters;

    for (;;)
    {
        UBaseType_t freeStack = uxTaskGetStackHighWaterMark(canReceiveHandle);
        Serial.printf("[CC_CAN][CC] free stack = %u bytes\n", freeStack);
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
        // Bus 0 : Discovery
        CanMsg msg0;
        if (CanBus::bus(0).receive(msg0))
            traiterMessageCAN(msg0, canton, 0);

        // Bus 1 : EXCC
        CanMsg msg1;
        if (CanBus::bus(1).receive(msg1))
            traiterMessageCAN(msg1, canton, 1);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
}

// ---------------------------------------------------------------------------
// Traitement d’un message CAN (bus 0 ou bus 1)
// ---------------------------------------------------------------------------
void CC_CAN::traiterMessageCAN(const CanMsg &msg, Canton *canton, uint8_t bus)
{
    // STOP global uniquement sur bus 0
    if (bus == 0 && msg.is11() && msg.id == EXPLORATION_CAN_ID_EMERGENCY_STOP)
    {
        canton->setStopActive(true);
        CC_LOG_ERROR("[CC_CAN][CC] STOP global reçu (canton %u)\n", canton->ID());
        return;
    }

    if (!msg.is29())
        return;

    uint8_t commande = msg.cmde();
    uint16_t idExpediteur = msg.nodeId();

    // -----------------------------------------------------------------------
    // Dispatch moderne (100 % CanMsg)
    // -----------------------------------------------------------------------

    // Commandes système
    if (commande >= CMD_ERM_CC_TEST_BUS_REPLY && commande <= CMD_ERM_CC_SAVE_ALL)
    {
        handleSystemCommand(commande, msg, canton, idExpediteur);
        return;
    }

    // Exploration (0xC0–0xC1)
    if (commande >= 0xC0 && commande <= 0xC1)
    {
        handleExplorationCommand(commande, msg, canton, idExpediteur);
        return;
    }

    // Exploitation (0xE0–0xE9)
    if (commande >= 0xE0 && commande <= 0xE9)
    {
        handleExploitCommand(commande, msg, canton, idExpediteur);
        return;
    }

    // Supervision (CMD_ERM_CC_OFFLINE)
    if (commande == CMD_ERM_CC_OFFLINE)
    {
        handleSupervisionCommand(commande, msg, canton);
        return;
    }

    // Commandes EXCC (0xD0–0xDF)
    if (commande >= 0xD0 && commande <= 0xDF)
    {
        CC_CAN_EXCC::handleEXCCCommand(commande, msg, canton, idExpediteur);
        return;
    }

    CC_LOG_WARN("[CC_CAN][CC] Cmd 0x%02X non gérée (bus %u)\n", commande, bus);
}
