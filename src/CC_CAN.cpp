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
        // Bus 0 : réseau GC2026
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
    if (bus == 0 && msg.is11() && msg.id == CAN11_ID_EMERGENCY_STOP)
    {
        canton->setStopActive(true);
        CC_LOG_ERROR("[CC_CAN][CC] STOP global reçu (canton %u)\n", canton->ID());
        return;
    }

    if (!msg.is29())
        return;

    uint8_t commande = msg.cmde();
    uint16_t idExpediteur = msg.nodeId();

    // Conversion vers enum class
    CanCmd cmd = static_cast<CanCmd>(commande);

    // -----------------------------------------------------------------------
    // DISPATCH MODERNE — 100 % conforme à l’enum CanCmd
    // -----------------------------------------------------------------------

    // -------------------------
    // Commandes SYSTÈME
    // -------------------------
    switch (cmd)
    {
    case CanCmd::CMD_ERM_CC_TEST_BUS_REPLY:
    case CanCmd::CMD_ERM_CC_REQUEST_ID:
    case CanCmd::CMD_ERM_CC_RESTART_ALL:
    case CanCmd::CMD_ERM_CC_WIFI_ON_OFF:
    case CanCmd::CMD_ERM_CC_EXPLORATION_ON_OFF:
    case CanCmd::CMD_ERM_CC_SAVE_ALL:
    case CanCmd::CMD_ERM_CC_SET_PROFILE:
        handleSystemCommand(commande, msg, canton, idExpediteur);
        return;

    case CanCmd::CMD_ERM_CC_OFFLINE:
        handleSupervisionCommand(commande, msg, canton);
        return;

    default:
        break;
    }

    // -------------------------
    // Commandes EXPLORATION
    // -------------------------
    if (cmd == CanCmd::CMD_EXPLORATION_CC_DEMANDE_ID ||
        cmd == CanCmd::CMD_EXPLORATION_ID_VOISIN ||
        cmd == CanCmd::CMD_EXPLORATION_UPDATE_MASQUE_AIG)
    {
        handleExplorationCommand(commande, msg, canton, idExpediteur);
        return;
    }

    // -------------------------
    // Commandes EXPLOITATION
    // -------------------------
    if (cmd == CanCmd::CMD_EXPLOITATION_UPDATE_VOISINS ||
        cmd == CanCmd::CMD_EXPLOITATION_RESERVATION_LOCO ||
        cmd == CanCmd::CMD_EXPLOITATION_RAILCOM_VOISIN ||
        cmd == CanCmd::CMD_EXPLOITATION_ASPECT_VOISIN ||
        cmd == CanCmd::CMD_EXPLOITATION_AIGUILLAGE)
    {
        handleExploitCommand(commande, msg, canton, idExpediteur);
        return;
    }

    // -------------------------
    // Commandes EXCC → CC
    // -------------------------
    if (cmd == CanCmd::EXCC_CC_PONG ||
        cmd == CanCmd::EXCC_CC_BOOSTER_INFO ||
        cmd == CanCmd::EXCC_CC_POSITION_AIGUILLE ||
        cmd == CanCmd::EXCC_CC_OCCUPATION ||
        cmd == CanCmd::EXCC_CC_PONCTUEL_H ||
        cmd == CanCmd::EXCC_CC_PONCTUEL_AH ||
        cmd == CanCmd::EXCC_CC_RAILCOM_ADRESSE ||
        cmd == CanCmd::EXCC_CC_CALIB_BOOSTER_INFO)
    {
        CC_CAN_EXCC::handleEXCCCommand(commande, msg, canton, idExpediteur);
        return;
    }

    CC_LOG_WARN("[CC_CAN][CC] Cmd 0x%02X non gérée (bus %u)\n", commande, bus);
}
