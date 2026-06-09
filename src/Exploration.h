/*
  Exploration.h — Version 2026 (CLEAN)
  ------------------------------------------------------------
  Rôle du module Exploration :
  - Détecter les satellites voisins via les boutons physiques.
  - Construire la topologie SP1 / SP2 / SM1 / SM2.
  - Créer les aiguilles LOGIQUES (Aig) en fonction des voisins.
  - Associer chaque aiguille à un EXSA (H/AH).
  - Déduire les signaux (cibles) en fonction de la topologie.
  - Sauvegarder settings.json et envoyer la topologie à la carte Main.

  Notes 2026 :
  - Le SA ne pilote plus aucun servo.
  - Les aiguilles sont 100 % logiques.
  - EXSA pilote physiquement les servos via PCA9685.
  - Exploration ne gère plus aucune pin d’aiguille.
  ------------------------------------------------------------
*/

#pragma once

#include <Arduino.h>
#include "CanMsg.h"
#include "Config.h"
#include "Canton.h"
#include "Settings.h"
#include "SatTopologieUART.h"
#include "DeductionAspect.h"

class Exploration
{
private:
  // Entrées physiques :
  //  - BTN_SAT_MOINS
  //  - BTN_SAT_PLUS
  //  - INTER_DEV_2
  //  - INTER_DEV_1
  // Ces 4 entrées permettent de sélectionner les voisins SP/SM.
  static const gpio_num_t m_pinIn[];

  // LED d’état du Exploration (clignotement, validation…)
  static const gpio_num_t m_pinLed;

  // Référence vers le Canton principal (structure ferroviaire)
  static Canton *canton;

  // Nombre d’aiguilles logiques détectées (0 à 6)
  static byte m_comptAig;

  // ID du satellite voisin détecté via CAN (0xC0)
  static byte m_ID_satPeriph;

  // État des boutons/switches (4 bits)
  static byte m_btnState;

  // Indique que Exploration doit s’arrêter (fin de topologie)
  static bool m_stopProcess;

public:
  // Classe statique → pas de constructeur
  Exploration() = delete;

  // Initialisation générale (boutons, LED, tâches FreeRTOS)
  static void begin(Canton *);

  // Tâche FreeRTOS : gestion des boutons + envoi CAN
  static void process(void *);

  // Tâche FreeRTOS : création aiguilles + signaux + topologie
  static void createAigEtCibles(void *);

  // Gestion du compteur d’aiguilles
  static void comptAig(byte);
  static byte comptAig();

  // Gestion de l’ID du satellite voisin détecté
  static void ID_satPeriph(byte);
  static byte ID_satPeriph();

  // Gestion de l’état des boutons/switches
  static void btnState(byte);
  static byte btnState();

  // Arrêt de l'Exploration (sauvegarde + envoi topologie + reboot)
  static void stopProcess(bool);
};
/* ------------------------------------------------------------
  Fin de Exploration.h
  ------------------------------------------------------------
*/