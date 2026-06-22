#pragma once

/*
 * CCWatchdog.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Interface du module Watchdog CC.
 *
 * Rôle :
 *   - démarrer la tâche FreeRTOS qui envoie le heartbeat CC → Master
 *   - fournir des fonctions système pour suspendre / reprendre le heartbeat
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
 *  CCWatchdog_begin()
 * ---------------------------------------------------------------------------
 *  Démarre la tâche FreeRTOS qui envoie un heartbeat toutes les 100 ms.
 * ==========================================================================*/
void CCWatchdog_begin();

/* ============================================================================
 *  CCWatchdog_suspend()
 * ---------------------------------------------------------------------------
 *  Suspend l’envoi du heartbeat.
 *  Utilisé lorsque le CC entre en STOP local.
 * ==========================================================================*/
void CCWatchdog_suspend();

/* ============================================================================
 *  CCWatchdog_resume()
 * ---------------------------------------------------------------------------
 *  Reprend l’envoi du heartbeat.
 *  Utilisé lorsque le STOP est levé.
 * ==========================================================================*/
void CCWatchdog_resume();
