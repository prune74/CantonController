#include <Arduino.h>
#include "SAWatchdog.h"
#include "Exploration_Protocol.h"
#include "Settings.h"
#include "Canton.h"
#include "ACAN_ESP32.h"

extern Canton *canton;

// ---------------------------------------------------------------------
// 🔁 Envoi heartbeat SA → Master (0x200)
// ---------------------------------------------------------------------
static void sendHeartbeat()
{
    uint16_t id = canton->ID();
    if (id == 0xFFFF)
        return; // ou UNUSED_ID selon ton define

    CANMessage msg;
    msg.id = EXPLORATION_CAN_ID_HEARTBEAT; // 0x200
    msg.ext = false;                       // 11 bits
    msg.len = 2;

    msg.data[0] = id >> 8;
    msg.data[1] = id & 0xFF;

    ACAN_ESP32::can.tryToSend(msg);
}

// ---------------------------------------------------------------------
// 🧵 Tâche FreeRTOS : heartbeat périodique
// ---------------------------------------------------------------------
static void taskHeartbeat(void *pv)
{
    (void)pv;
    for (;;)
    {
        sendHeartbeat();
        vTaskDelay(pdMS_TO_TICKS(100)); // 100 ms
    }
}

// ---------------------------------------------------------------------
// 🚀 Initialisation du Watchdog SA
// ---------------------------------------------------------------------
void SAWatchdog_begin()
{
    xTaskCreate(
        taskHeartbeat,
        "SA_HB",
        2048,
        nullptr,
        4,
        nullptr);
}
