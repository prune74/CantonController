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
#include "CC_CAN_CLF.h"

#include "debug_cc.h"

#include "CanMsg.h"
#include "CanBus.h"
#include <Protocol.h>

// Handlers CAN modernes
void handleSystemCommand(uint8_t commande, const CanMsg &msg, Canton *canton, uint16_t idCCExpediteur);
void handleExplorationCommand(uint8_t commande, const CanMsg &msg, Canton *canton, uint16_t idCCExpediteur);
void handleExploitCommand(uint8_t commande, const CanMsg &msg, Canton *canton, uint16_t idCCExpediteur);
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
}

// ---------------------------------------------------------------------------
// canReceiveMsg() — tâche FreeRTOS de réception / dispatch CAN
// ---------------------------------------------------------------------------
void CC_CAN::canReceiveMsg(void *pvParameters)
{
    Canton *canton = (Canton *)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        // Bus 0 : CAN Service
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
    if (bus == 0 && msg.is11())
    {
        Cmd_Global11 g = (Cmd_Global11)msg.id;

        if (g == Cmd_Global11::EMERGENCY_STOP)
        {
            canton->setStopActive(true);
            CC_LOG_ERROR("[CC_CAN][CC] STOP global reçu (canton %u)\n", canton->ID());
            return;
        }
    }

    if (!msg.is29())
        return;

    uint8_t commande = msg.cmde();
    uint16_t idExpediteur = msg.id & 0x7FF;

    // -----------------------------------------------------------------------
    // DISPATCH MODERNE — basé sur les enums par liaison
    // -----------------------------------------------------------------------

    // ============================
    // 1) COMMANDES ERM → CC
    // ============================
    switch ((Cmd_ERM_to_CC)commande)
    {
    case Cmd_ERM_to_CC::TEST_BUS_REPLY:
    case Cmd_ERM_to_CC::REQUEST_ID_REPLY:
    case Cmd_ERM_to_CC::RESTART_ALL:
    case Cmd_ERM_to_CC::WIFI_ON_OFF:
    case Cmd_ERM_to_CC::EXPLORATION_ON_OFF:
    case Cmd_ERM_to_CC::SAVE_ALL:
    case Cmd_ERM_to_CC::SET_PROFILE:
        handleSystemCommand(commande, msg, canton, idExpediteur);
        return;

    case Cmd_ERM_to_CC::OFFLINE:
        handleSupervisionCommand(commande, msg, canton);
        return;

    default:
        break;
    }

    // ============================
    // 2) COMMANDES EXPLORATION CC ↔ CC
    // ============================
    switch ((Cmd_Exploration_CC)commande)
    {
    case Cmd_Exploration_CC::DEMANDE_ID:
    case Cmd_Exploration_CC::ID_VOISIN:
    case Cmd_Exploration_CC::UPDATE_MASQUE_AIG:
        handleExplorationCommand(commande, msg, canton, idExpediteur);
        return;

    default:
        break;
    }

    // ============================
    // 3) COMMANDES EXPLOITATION CC ↔ CC
    // ============================
    switch ((Cmd_CC_to_CC)commande)
    {
    case Cmd_CC_to_CC::UPDATE_VOISINS:
    case Cmd_CC_to_CC::RESERVATION_LOCO:
    case Cmd_CC_to_CC::RAILCOM_VOISIN:
    case Cmd_CC_to_CC::ASPECT_VOISIN:
    case Cmd_CC_to_CC::AIGUILLAGE:
    case Cmd_CC_to_CC::MESURE_PREPARE:
        handleExploitCommand(commande, msg, canton, idExpediteur);
        return;

    default:
        break;
    }

    // ============================
    // 4) COMMANDES EXCC → CC
    // ============================
    switch ((Cmd_EXCC_to_CC)commande)
    {
    case Cmd_EXCC_to_CC::PONG:
    case Cmd_EXCC_to_CC::BOOSTER_INFO:
    case Cmd_EXCC_to_CC::POSITION_AIGUILLE:
    case Cmd_EXCC_to_CC::OCCUPATION:
    case Cmd_EXCC_to_CC::PONCTUEL_H:
    case Cmd_EXCC_to_CC::PONCTUEL_AH:
    case Cmd_EXCC_to_CC::RAILCOM_ADRESSE:
    case Cmd_EXCC_to_CC::CALIB_BOOSTER_INFO:
    case Cmd_EXCC_to_CC::ESSIEUX:
        CC_CAN_EXCC::handleEXCCCommand(commande, msg, canton, idExpediteur);
        return;

    default:
        break;
    }

    // ============================
    // 5) COMMANDES CLF → CC
    // ============================
    switch ((Cmd_CLF_to_CC)commande)
    {
    case Cmd_CLF_to_CC::TRAIN_VALIDE:
    case Cmd_CLF_to_CC::TRAIN_REARMER:
    case Cmd_CLF_to_CC::DEMANDE_MESURE:
        CC_CAN_CLF::handleCLFCommand(commande, msg, canton, idExpediteur);
        return;

    default:
        break;
    }

    // ============================
    // 6) Commande inconnue
    // ============================
    CC_LOG_WARN("[CC_CAN][CC] Cmd 0x%02X non gérée (bus %u)\n", commande, bus);
}
