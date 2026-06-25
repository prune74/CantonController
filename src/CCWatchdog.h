#pragma once

/*
 * CCWatchdog.h — Gestion Canton 2026 (CAN Universal)
 * ---------------------------------------------------------------------------
 * Interface du module Watchdog CC.
 *
 * Rôle :
 *   - démarrer la tâche FreeRTOS qui envoie le heartbeat CC → Master
 *   - suspendre / reprendre le heartbeat (STOP local)
 *
 * Important :
 *   - aucune logique métier ici
 *   - aucune logique STOP ici
 *   - ce module ne fait qu’exposer l’API du watchdog
 */

#include <Arduino.h>

/* ============================================================================
 *  Handle de la tâche heartbeat
 * ---------------------------------------------------------------------------
 *  - utilisé pour suspendre / reprendre l’envoi du heartbeat
 *  - défini dans CCWatchdog.cpp
 * ==========================================================================*/
extern TaskHandle_t gHeartbeatTask;

/* ============================================================================
 *  API Watchdog CC
 * ==========================================================================*/

/// Démarre la tâche FreeRTOS qui envoie un heartbeat toutes les 100 ms.
void CCWatchdog_begin();

/// Suspend l’envoi du heartbeat (STOP local).
void CCWatchdog_suspend();

/// Reprend l’envoi du heartbeat (STOP levé).
void CCWatchdog_resume();
