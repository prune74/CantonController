/*
 * CCWatchdog.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Module responsable de l’envoi périodique du heartbeat CC → Master.
 *
 * Rôle :
 *   - transmettre l’ID du canton au Master toutes les 100 ms (CAN ID 0x200)
 *   - permettre au Master de superviser la présence du CC
 *   - fournir des fonctions système pour suspendre / reprendre le heartbeat
 *
 * Important :
 *   - aucune logique ferroviaire ici
 *   - aucune logique STOP ici
 *   - ce module ne fait qu’envoyer un heartbeat CAN
 *   - la décision d’arrêter / reprendre le heartbeat est prise ailleurs
 */

#include <Arduino.h>
#include "CCWatchdog.h"
#include "Exploration_Protocol.h"
#include "Settings.h"
#include "Canton.h"
#include "ACAN_ESP32.h"
#include "debug_cc.h"

extern Canton *canton;

/* ============================================================================
 *  Handle de la tâche FreeRTOS du heartbeat
 * ---------------------------------------------------------------------------
 *  - utilisé pour suspendre / reprendre l’envoi du heartbeat
 *  - exposé dans CCWatchdog.h
 * ==========================================================================*/
TaskHandle_t gHeartbeatTask = nullptr;

/* ============================================================================
 *  sendHeartbeat() — Envoi d’une trame 0x200 au Master
 * ---------------------------------------------------------------------------
 *  Format :
 *      ID   = 0x200 (EXPLORATION_CAN_ID_HEARTBEAT)
 *      DLC  = 2
 *      DATA = [ID_High, ID_Low]
 *
 *  Conditions :
 *      - si l’ID du canton est invalide → aucun envoi
 * ==========================================================================*/
static void sendHeartbeat()
{
    uint16_t id = canton->ID();
    if (id == UNUSED_ID || id == 0xFFFF)
        return;

    CANMessage msg;
    msg.id  = EXPLORATION_CAN_ID_HEARTBEAT; // 0x200
    msg.ext = false;                        // trame standard 11 bits
    msg.len = 2;

    msg.data[0] = id >> 8;
    msg.data[1] = id & 0xFF;

    ACAN_ESP32::can.tryToSend(msg);

    CC_LOG_TRACE("[CCWatchdog][CC] Heartbeat envoyé (ID=%u)\n", id);
}

/* ============================================================================
 *  taskHeartbeat() — Tâche FreeRTOS
 * ---------------------------------------------------------------------------
 *  - envoie un heartbeat toutes les 100 ms
 *  - peut être suspendue / reprise via CCWatchdog_suspend() / resume()
 * ==========================================================================*/
static void taskHeartbeat(void *pv)
{
    (void)pv;

    for (;;)
    {
        sendHeartbeat();
        vTaskDelay(pdMS_TO_TICKS(100)); // période fixe : 100 ms
    }
}

/* ============================================================================
 *  CCWatchdog_begin() — Démarrage du watchdog
 * ---------------------------------------------------------------------------
 *  - crée la tâche FreeRTOS
 *  - stocke son handle dans gHeartbeatTask
 * ==========================================================================*/
void CCWatchdog_begin()
{
    xTaskCreate(
        taskHeartbeat,
        "CC_HB",
        2048,
        nullptr,
        4,
        &gHeartbeatTask);

    CC_LOG_INFO("[CCWatchdog][CC] Watchdog démarré (heartbeat actif)\n");
}

/* ============================================================================
 *  CCWatchdog_suspend() — Arrêt du heartbeat
 * ---------------------------------------------------------------------------
 *  - utilisé lorsque le CC entre en STOP local
 *  - le Master ne reçoit plus 0x200 → STOP global
 * ==========================================================================*/
void CCWatchdog_suspend()
{
    if (gHeartbeatTask)
    {
        vTaskSuspend(gHeartbeatTask);
        CC_LOG_WARN("[CCWatchdog][CC] Heartbeat suspendu\n");
    }
}

/* ============================================================================
 *  CCWatchdog_resume() — Reprise du heartbeat
 * ---------------------------------------------------------------------------
 *  - utilisé lorsque le STOP est levé
 *  - le Master recommence à recevoir 0x200
 * ==========================================================================*/
void CCWatchdog_resume()
{
    if (gHeartbeatTask)
    {
        vTaskResume(gHeartbeatTask);
        CC_LOG_INFO("[CCWatchdog][CC] Heartbeat repris\n");
    }
}
