#include <Arduino.h>
#include "CCWatchdog.h"
#include "Protocol.h"
#include "Settings.h"
#include "Canton.h"
#include "CanMsg.h"
#include "CanBus.h"
#include "debug_cc.h"

TaskHandle_t gHeartbeatTask = nullptr;

/* ============================================================================
 *  sendHeartbeat() — Envoi d’une trame 0x200 au Master
 * ---------------------------------------------------------------------------
 *  Format :
 *      ID   = 0x200 (CAN11_ID_HEARTBEAT)
 *      DLC  = 2
 *      DATA = [ID_High, ID_Low]
 *
 *  Conditions :
 *      - si l’ID du canton est invalide → aucun envoi
 * ==========================================================================*/
static void sendHeartbeat()
{
    Canton *canton = Settings::canton;
    if (!canton)
        return;

    uint16_t id = canton->ID();
    if (id == UNUSED_ID || id == 0xFFFF)
        return;

    CanMsg msg;
    msg.id = (uint16_t)Cmd_Global11::HEARTBEAT;  // 0x200
    msg.dlc = 2;

    msg.data[0] = id >> 8;
    msg.data[1] = id & 0xFF;

    CanBus::bus(0).send(msg);

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
        vTaskDelay(pdMS_TO_TICKS(100));
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
