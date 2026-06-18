/*
 * CCWatchdog.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Envoi périodique du heartbeat CC → Master (CAN ID 0x200).
 *
 * Rôle :
 *   - transmettre l’ID du canton au Master toutes les 100 ms
 *   - permettre au Master de superviser la présence du CC
 *
 * Ce module ne contient aucune logique métier :
 *   → il transporte uniquement un heartbeat CAN.
 */

#include <Arduino.h>
#include "CCWatchdog.h"
#include "Exploration_Protocol.h"
#include "Settings.h"
#include "Canton.h"
#include "ACAN_ESP32.h"
#include "debug_cc.h"

extern Canton *canton;

// ---------------------------------------------------------------------------
// Envoi heartbeat CC → Master (0x200)
// ---------------------------------------------------------------------------
static void sendHeartbeat()
{
    uint16_t id = canton->ID();
    if (id == UNUSED_ID || id == 0xFFFF)
        return;

    CANMessage msg;
    msg.id  = EXPLORATION_CAN_ID_HEARTBEAT; // 0x200
    msg.ext = false;                        // 11 bits
    msg.len = 2;

    msg.data[0] = id >> 8;
    msg.data[1] = id & 0xFF;

    ACAN_ESP32::can.tryToSend(msg);

    CC_LOG_TRACE("[CCWatchdog][CC] Heartbeat ID=%u\n", id);
}

// ---------------------------------------------------------------------------
// Tâche FreeRTOS : heartbeat périodique
// ---------------------------------------------------------------------------
static void taskHeartbeat(void *pv)
{
    (void)pv;

    for (;;)
    {
        sendHeartbeat();
        vTaskDelay(pdMS_TO_TICKS(100)); // 100 ms
    }
}

// ---------------------------------------------------------------------------
// Initialisation du Watchdog CC
// ---------------------------------------------------------------------------
void CCWatchdog_begin()
{
    xTaskCreate(
        taskHeartbeat,
        "CC_HB",
        2048,
        nullptr,
        4,
        nullptr);

    CC_LOG_INFO("[CCWatchdog][CC] Watchdog démarré\n");
}
