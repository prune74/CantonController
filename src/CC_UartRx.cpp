/*
 * CC_UartRx.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Réception UART RS485 depuis l’EXCC.
 *
 * Format des trames :
 *   [SYNC=0xAA][OPCODE][INDEX_EXCC][DATA...]
 *
 * Rôle :
 *   - parser les trames EXCC → CC
 *   - dispatcher vers les modules concernés :
 *        • Sensor (ponctuels)
 *        • ConsoCourant (occupation)
 *        • Canton (compteur essieux)
 *        • SupervisionAiguilles (positions)
 *        • EXCC_Link (booster + pong)
 *        • Railcom (adresse loco)
 *        • Booster (calibration)
 *
 * Ce module ne contient aucune logique métier :
 *   → il distribue simplement les messages reçus.
 */

#include "CC_UartRx.h"
#include "CC_RS485.h"
#include "Exploration_Protocol.h"
#include "Canton.h"

#include "Sensor.h"
#include "ConsoCourant.h"
#include "SupervisionAiguilles.h"
#include "EXCC_Link.h"
#include "Railcom.h"
#include "Booster.h"

#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Tâche FreeRTOS
// ---------------------------------------------------------------------------
static TaskHandle_t s_uartTask = nullptr;

// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------
void CC_UartRx::begin()
{
    xTaskCreatePinnedToCore(
        CC_UartRx::uartTask,
        "CC_UART_RX",
        4096,
        nullptr,
        1,
        &s_uartTask,
        1);

    CC_LOG_INFO("[CC_UartRx][CC] Tâche UART RS485 lancée\n");
}

// ---------------------------------------------------------------------------
// Tâche UART : parsing byte par byte
// ---------------------------------------------------------------------------
void CC_UartRx::uartTask(void *param)
{
    uint8_t step     = 0;
    uint8_t opcode   = 0;
    uint8_t index    = 0;
    uint8_t data[8];
    uint8_t dataPos  = 0;
    uint8_t expected = 0;

    for (;;)
    {
        int b = CC_RS485::readByte();
        if (b < 0)
        {
            vTaskDelay(pdMS_TO_TICKS(2));
            continue;
        }

        uint8_t byte = uint8_t(b);

        switch (step)
        {
        case 0: // SYNC
            if (byte == PROTO_SYNC_BYTE)
                step = 1;
            break;

        case 1: // OPCODE
            opcode = byte;
            step = 2;
            break;

        case 2: // INDEX EXCC
            index = byte;
            dataPos = 0;

            switch (opcode)
            {
            case PROTO_03_H_PONCTUEL:        expected = 1; break;
            case PROTO_03_AH_PONCTUEL:       expected = 1; break;
            case PROTO_04_OCCUPATION:        expected = 1; break;
            case PROTO_05_COMPTEUR_ESSIEUX:  expected = 1; break;
            case PROTO_06_POSITION_AIGUILLE: expected = 3; break;
            case PROTO_07_BOOSTER:           expected = 4; break;
            case PROTO_08_RAILCOM_ADRESSE:   expected = 2; break;
            case PROTO_09_CALIB_BOOSTER:     expected = 4; break;
            case PROTO_PONG:                 expected = 0; break;
            default:                         expected = 0; break;
            }

            if (expected == 0)
            {
                CC_UartRx::dispatch(opcode, index, nullptr, 0);
                step = 0;
            }
            else
            {
                step = 3;
            }
            break;

        case 3: // DATA[n]
            data[dataPos++] = byte;

            if (dataPos >= expected)
            {
                CC_UartRx::dispatch(opcode, index, data, expected);
                step = 0;
            }
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Dispatch des trames vers les modules concernés
// ---------------------------------------------------------------------------
void CC_UartRx::dispatch(uint8_t opcode, uint8_t index, uint8_t *data, uint8_t len)
{
    switch (opcode)
    {
    case PROTO_03_H_PONCTUEL:
        Sensor::onPonctuelH(data[0]);
        break;

    case PROTO_03_AH_PONCTUEL:
        Sensor::onPonctuelAH(data[0]);
        break;

    case PROTO_04_OCCUPATION:
        ConsoCourant::onOccupation(index, data[0]);
        break;

    case PROTO_05_COMPTEUR_ESSIEUX:
        Canton::s_instance->setCompteurEssieux(data[0]);
        CC_LOG_TRACE("[CC_UartRx][CC] Compteur essieux = %d\n", data[0]);
        break;

    case PROTO_06_POSITION_AIGUILLE:
        SupervisionAiguilles::onPosition(index, data[0], data[1], data[2]);
        break;

    case PROTO_07_BOOSTER:
        EXCC_Link::onBooster(index, data[0], data[1], data[2], data[3]);
        break;

    case PROTO_08_RAILCOM_ADRESSE:
        Railcom::onRailcom(index, data[0], data[1]);
        break;

    case PROTO_09_CALIB_BOOSTER:
        Booster::onCalib(index, data[0], data[1], data[2], data[3]);
        break;

    case PROTO_PONG:
        EXCC_Link::onPong(index);
        break;

    default:
        CC_LOG_WARN("[CC_UartRx][CC] Opcode inconnu : %02X\n", opcode);
        break;
    }
}
