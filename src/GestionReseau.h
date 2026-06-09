/*
  GestionReseau.h — Version 2026
  ------------------------------------------------------------
  Supervision centrale du satellite ferroviaire (SA)

  Rôle global :
  - Orchestrer la logique ferroviaire locale du canton
  - Coordonner les modules spécialisés :
        * Capteurs (occupation, IR, courant…)
        * Déduction du sens de roulage
        * Topologie SP1 / SM1
        * Supervision CAN (diffusion réseau)
        * Supervision cantonale (aspects)
        * Pilotage distribué (ralentissement, arrêt…)
        * Commande DCC++ (vitesse, sens)
        * AspectSignal (pilotage des signaux via EXSA)

  Cette classe ne contient que :
  - la création de la tâche FreeRTOS principale
  - la boucle de supervision (loopTask)
  - le buffer des aspects à transmettre aux signaux

  Toute la logique est déléguée aux modules spécialisés.
  ------------------------------------------------------------
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
#include "AspectSignal.h"         // Envoi aspects dynamiques aux signaux (UART → EXSA)
#include "SensEnum.h"             // Enumération des sens (horaire / anti-horaire)
#include "Exploration_Protocol.h" // ExsaAspect (enum 1 octet)

class GestionReseau
{
public:
  // Classe statique → pas de constructeur
  GestionReseau() = delete;

  /*************************************************************************************
   * setup()
   *************************************************************************************/
  static void setup(Canton *canton);

  /*************************************************************************************
   * loopTask()
   * ------------------------------------------------------------
   * Boucle principale du satellite (SA)
   * Exécutée toutes les 100 ms.
   *************************************************************************************/
  static void IRAM_ATTR loopTask(void *pvParameters);

  /*************************************************************************************
   * signalValue[2]
   * ------------------------------------------------------------
   * Buffer contenant les aspects à transmettre aux signaux :
   *   - signalValue[0] : aspect horaire
   *   - signalValue[1] : aspect anti-horaire
   *
   * Typé en ExsaAspect (1 octet) pour cohérence avec :
   *   - SupervisionCanton (calcul des aspects)
   *   - AspectSignal (envoi UART vers EXSA)
   *   - Exploration_Protocol.h (protocole commun)
   *************************************************************************************/
  static ExsaAspect signalValue[2];
};
