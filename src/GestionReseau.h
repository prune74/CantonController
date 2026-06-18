/*
 * GestionReseau.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Supervision centrale du Canton Controller (CC).
 *
 * Rôle :
 *   - orchestrer la logique ferroviaire locale du canton
 *   - coordonner les modules spécialisés :
 *        • capteurs (occupation, IR, courant…)
 *        • déduction du sens de roulage
 *        • topologie SP1 / SM1
 *        • supervision CAN (diffusion réseau)
 *        • supervision cantonale (aspects SNCF)
 *        • pilotage distribué (ralentissement, arrêt…)
 *        • commande DCC++ (vitesse, sens)
 *        • pilotage des signaux via EXCC (AspectSignal)
 *
 * Ce module ne contient :
 *   - que la création de la tâche FreeRTOS principale
 *   - la boucle de supervision (loopTask)
 *   - le buffer des aspects envoyés aux signaux
 *
 * Toute la logique métier est déléguée aux modules spécialisés.
 */

#pragma once

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Modules spécialisés appelés dans loopTask()
#include "Canton.h"
#include "CapteursEtat.h"         // Lecture occupation / IR / courant
#include "SensRoulage.h"          // Déduction du sens de circulation
#include "TopologieSat.h"         // Mise à jour SP1 / SM1
#include "SupervisionCAN.h"       // Diffusion état canton sur CAN
#include "SupervisionCanton.h"    // Déduction des aspects cantonaux
#include "PilotageDistribue.h"    // Pilotage loco selon aspect
#include "CommandeDCC.h"          // Envoi commandes DCC++
#include "AspectSignal.h"         // Envoi aspects dynamiques aux signaux (UART → EXCC)
#include "SensEnum.h"             // Enumération des sens (horaire / anti-horaire)
#include "Exploration_Protocol.h" // ExccAspect (enum 1 octet)

class GestionReseau
{
public:
  GestionReseau() = delete; // Classe statique

  // -----------------------------------------------------------------------
  // setup() — création de la tâche FreeRTOS
  // -----------------------------------------------------------------------
  static void setup(Canton *canton);

  // -----------------------------------------------------------------------
  // loopTask() — boucle principale du CC (100 ms)
  // -----------------------------------------------------------------------
  static void IRAM_ATTR loopTask(void *pvParameters);

  // -----------------------------------------------------------------------
  // signalValue[2] — aspects envoyés aux signaux EXCC
  //   signalValue[0] : aspect horaire
  //   signalValue[1] : aspect anti‑horaire
  //
  // Typé en ExccAspect (1 octet) pour cohérence avec :
  //   - SupervisionCanton (calcul des aspects)
  //   - AspectSignal (envoi UART vers EXCC)
  //   - Exploration_Protocol.h (protocole commun)
  // -----------------------------------------------------------------------
  static ExccAspect signalValue[2];
};
