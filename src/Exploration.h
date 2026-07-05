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
 */

#pragma once

#include <Arduino.h>
#include "CC_CAN.h"
#include "Config.h"
#include "Canton.h"
#include "Settings.h"
#include "DeductionAspect.h"

class Exploration
{
private:
    // LED Exploration
    static const uint8_t m_pinLed = MCP_PIN_LED_EXPLORATION;

    // Référence vers le Canton principal
    static Canton *canton;

    // Compteur d’aiguilles logiques
    static uint8_t m_comptAig;

    // ID du CantonController voisin détecté via CAN
    static uint8_t m_ID_CCPeriph;

    // État des boutons (4 bits)
    static uint8_t m_btnState;

    // Fin de l’exploration
    static bool m_stopProcess;

public:
    Exploration() = delete;

    // Initialisation
    static void begin(Canton *);

    // Boucle exploration
    static void process(void *);

    // 1ère passe au boot
    static void CreationAiguilles(void *);

    // Accesseurs internes
    static void comptAig(uint8_t);
    static uint8_t comptAig();

    static void ID_CCPeriph(uint8_t);
    static uint8_t ID_CCPeriph();

    static void btnState(uint8_t);
    static uint8_t btnState();

    // -----------------------------------------------------------------------
    // stopProcess() — fin de l’exploration (topologie envoyée)
    //   - LED Exploration OFF
    //   - LED MANOEUVRE OFF
    // -----------------------------------------------------------------------
    static void stopProcess(bool);

    // 🔥 Getter pour EXCC_Link
    static Canton *getCanton() { return canton; }
};
