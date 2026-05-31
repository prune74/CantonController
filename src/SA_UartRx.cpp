#include "SA_UartRx.h"
#include "SA_RS485.h"
#include "Discovery_Protocol.h"

#include "Sensor.h"
#include "ConsoCourant.h"
#include "CompteurEssieuxUart.h"
#include "SupervisionAiguilles.h"
#include "SatEXSA_Link.h"
#include "Railcom.h"
#include "Booster.h"

#include "debug_sa.h"

static TaskHandle_t s_uartTask = nullptr;

void SA_UartRx::begin()
{
    xTaskCreatePinnedToCore(
        SA_UartRx::uartTask,
        "SA_UART_RX",
        4096,
        nullptr,
        1,
        &s_uartTask,
        1
    );

    SA_LOG_INFO("[SA_UartRx] Tâche UART RS485 lancée\n");
}

void SA_UartRx::uartTask(void *param)
{
    uint8_t step     = 0;
    uint8_t opcode   = 0;
    uint8_t index    = 0;
    uint8_t data[8];
    uint8_t dataPos  = 0;
    uint8_t expected = 0;

    for (;;)
    {
        int b = SA_RS485::readByte();
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

        case 2: // INDEX EXSA
            index = byte;
            dataPos = 0;

            switch (opcode)
            {
            case PROTO_03_PONCTUEL:          expected = 1; break;
            case PROTO_04_OCCUPATION:        expected = 1; break;
            case PROTO_05_DELTA_AXE:         expected = 1; break;
            case PROTO_06_POSITION_AIGUILLE: expected = 3; break;
            case PROTO_07_BOOSTER:           expected = 4; break;
            case PROTO_08_RAILCOM_ADRESSE:   expected = 2; break;
            case PROTO_09_CALIB_BOOSTER:     expected = 4; break;
            case PROTO_PONG:                 expected = 0; break;
            default:                         expected = 0; break;
            }

            if (expected == 0)
            {
                SA_UartRx::dispatch(opcode, index, nullptr, 0);
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
                SA_UartRx::dispatch(opcode, index, data, expected);
                step = 0;
            }
            break;
        }
    }
}

void SA_UartRx::dispatch(uint8_t opcode, uint8_t index, uint8_t *data, uint8_t len)
{
    switch (opcode)
    {
    case PROTO_03_PONCTUEL:
        Sensor::onPonctuel(index, data[0]);
        break;

    case PROTO_04_OCCUPATION:
        ConsoCourant::onOccupation(index, data[0]);
        break;

    case PROTO_05_DELTA_AXE:
        CompteurEssieuxUart::onDelta(index, data[0]);
        break;

    case PROTO_06_POSITION_AIGUILLE:
        SupervisionAiguilles::onPosition(index, data[0], data[1], data[2]);
        break;

    case PROTO_07_BOOSTER:
        SatEXSA_Link::onBooster(index, data[0], data[1], data[2], data[3]);
        break;

    case PROTO_08_RAILCOM_ADRESSE:
        // data[0] = low, data[1] = high
        Railcom::onRailcom(index, data[0], data[1]);
        break;

    case PROTO_09_CALIB_BOOSTER:
        Booster::onCalib(index, data[0], data[1], data[2], data[3]);
        break;

    case PROTO_PONG:
        SatEXSA_Link::onPong(index);
        break;

    default:
        SA_LOG_WARN("[SA_UartRx] Opcode inconnu : %02X\n", opcode);
        break;
    }
}
