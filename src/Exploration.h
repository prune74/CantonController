/*
 * Exploration.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Module de découverte autonome du Canton Controller (CC).
 *
 * Rôle :
 *   - détecter les voisins via les boutons physiques (MCP23017)
 *   - construire la topologie SP1 / SP2 / SM1 / SM2
 *   - créer les aiguilles LOGIQUES (Aig) en fonction des voisins détectés
 *   - sauvegarder settings.json
 *   - envoyer la topologie vers l’Extension Canton Controller (EXCC)
 *   - sauvegarder settings.json
 *
 * Notes 2026 :
 *   - le CC ne pilote plus aucun servo
 *   - les aiguilles sont 100 % logiques
 *   - EXCC pilote physiquement les servos via PCA9685
 *   - Exploration ne manipule plus aucune GPIO ESP32
 *   - boutons + LED Exploration + LED Manoeuvre → MCP23017
 *
 * LEDS 2026 :
 *   - LED Exploration :
 *       → utilisée uniquement pendant la phase de découverte
 *       → éteinte en exploitation
 *
 *   - LED MANOEUVRE :
 *       → reflète l’état du mode manœuvre UNIQUEMENT en exploration
 *       → toujours éteinte en exploitation (même si le mode est ON)
 *
 * Ce module ne contient aucune logique ferroviaire :
 *   → il orchestre simplement la phase d’exploration.
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
    // -----------------------------------------------------------------------
    // LED Exploration (via MCP23017)
    //   - clignote / s’allume pendant la découverte
    //   - éteinte en exploitation
    // -----------------------------------------------------------------------
    static const uint8_t m_pinLed = MCP_PIN_LED_EXPLORATION;

    // -----------------------------------------------------------------------
    // Référence vers le Canton principal
    // -----------------------------------------------------------------------
    static Canton *canton;

    // -----------------------------------------------------------------------
    // Nombre d’aiguilles logiques détectées (0..6)
    // -----------------------------------------------------------------------
    static byte m_comptAig;

    // -----------------------------------------------------------------------
    // ID du satellite voisin détecté via CAN (opcode 0xC0)
    // -----------------------------------------------------------------------
    static byte m_ID_satPeriph;

    // -----------------------------------------------------------------------
    // État des boutons/switches (4 bits)
    //   bit0 = SAT_MOINS
    //   bit1 = SAT_PLUS
    //   bit2 = DEV2
    //   bit3 = DEV1
    // -----------------------------------------------------------------------
    static byte m_btnState;

    // -----------------------------------------------------------------------
    // Indique que l’exploration doit s’arrêter (topologie finalisée)
    //   → passage en mode exploitation
    //   → LED Exploration OFF
    //   → LED MANOEUVRE OFF
    // -----------------------------------------------------------------------
    static bool m_stopProcess;

public:
    Exploration() = delete; // Classe statique

    // -----------------------------------------------------------------------
    // begin() — initialisation générale (MCP, LED, tâches FreeRTOS)
    // -----------------------------------------------------------------------
    static void begin(Canton *);

    // -----------------------------------------------------------------------
    // process() — gestion des boutons + reset logique + notifications CAN
    //   - LED Exploration = activité découverte
    //   - LED MANOEUVRE = état du mode manœuvre (exploration uniquement)
    // -----------------------------------------------------------------------
    static void process(void *);

    // -----------------------------------------------------------------------
    // createAigEtCibles() — première passe au boot
    // -----------------------------------------------------------------------
    static void createAigEtCibles(void *);

    // -----------------------------------------------------------------------
    // Accesseurs internes
    // -----------------------------------------------------------------------
    static void comptAig(byte);
    static byte comptAig();

    static void ID_satPeriph(byte);
    static byte ID_satPeriph();

    static void btnState(byte);
    static byte btnState();

    // -----------------------------------------------------------------------
    // stopProcess() — fin de l’exploration (topologie envoyée)
    //   - LED Exploration OFF
    //   - LED MANOEUVRE OFF
    // -----------------------------------------------------------------------
    static void stopProcess(bool);
};

/* ---------------------------------------------------------------------------
 * Fin de Exploration.h
 * ---------------------------------------------------------------------------
 */
