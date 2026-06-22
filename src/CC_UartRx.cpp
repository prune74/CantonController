/*
 * CC_UartRx.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Ce module lit les trames envoyées par l’unique EXCC via RS485.
 *
 * Format des trames (protocole sans index) :
 *   [SYNC=0xAA][OPCODE][DATA...]
 *
 * Rôle :
 *   - lire les octets reçus
 *   - détecter les trames valides
 *   - parser les données selon l’OPCODE
 *   - dispatcher vers les modules concernés :
 *        • Sensor (ponctuels)
 *        • Occupation (occupation)
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

#include "CapteurPonctuel.h"
#include "Occupation.h"
#include "SupervisionAiguilles.h"
#include "EXCC_Link.h"
#include "Railcom.h"
#include "Booster.h"
#include "Canton.h"

#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Tâche FreeRTOS
// ---------------------------------------------------------------------------
static TaskHandle_t s_uartTask = nullptr;

// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------
void CC_UartRx::begin() // 🟢
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
void CC_UartRx::uartTask(void *param) // 🟢
{
    uint8_t step = 0;
    uint8_t opcode = 0;
    uint8_t data[8];
    uint8_t dataPos = 0;
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
            dataPos = 0;

            // Détermination du nombre d’octets attendus
            switch (opcode)
            {
            case PROTO_03_H_PONCTUEL:
                expected = 1;
                break;
            case PROTO_03_AH_PONCTUEL:
                expected = 1;
                break;
            case PROTO_04_OCCUPATION:
                expected = 1;
                break;
            case PROTO_06_POSITION_AIGUILLE:
                expected = 3;
                break;
            case PROTO_07_BOOSTER:
                expected = 3;
                break; // etat, courant, tension
            case PROTO_08_RAILCOM_ADRESSE:
                expected = 2;
                break;
            case PROTO_09_CALIB_BOOSTER:
                expected = 4;
                break;
            case PROTO_PONG:
                expected = 0;
                break;
            default:
                expected = 0;
                break;
            }

            if (expected == 0)
            {
                CC_UartRx::dispatch(opcode, nullptr, 0);
                step = 0;
            }
            else
            {
                step = 2;
            }
            break;

        case 2: // DATA[n]
            data[dataPos++] = byte;

            if (dataPos >= expected)
            {
                CC_UartRx::dispatch(opcode, data, expected);
                step = 0;
            }
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Dispatch des trames vers les modules concernés
// ---------------------------------------------------------------------------
void CC_UartRx::dispatch(uint8_t opcode, uint8_t *data, uint8_t len) // 🟢
{
    switch (opcode)
    {
    case PROTO_03_H_PONCTUEL:
        CapteurPonctuel::onPonctuelH(data[0]);
        break;

    case PROTO_03_AH_PONCTUEL:
        CapteurPonctuel::onPonctuelAH(data[0]);
        break;

    case PROTO_04_OCCUPATION:
        Occupation::onOccupation(data[0]);
        break;

    case PROTO_06_POSITION_AIGUILLE:
        SupervisionAiguilles::onPosition(data[0], data[1], data[2]);
        break;

    case PROTO_07_BOOSTER:
        EXCC_Link::onBooster(data[0], data[1], data[2]);
        break;

    case PROTO_08_RAILCOM_ADRESSE:
        Railcom::onRailcom(data[0], data[1]);
        break;

    case PROTO_09_CALIB_BOOSTER:
        Booster::onCalib(data[0], data[1], data[2], data[3]);
        break;

    case PROTO_PONG:
        EXCC_Link::onPong();
        break;

    default:
        CC_LOG_WARN("[CC_UartRx][CC] Opcode inconnu : %02X\n", opcode);
        break;
    }
}
