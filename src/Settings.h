/*
 * Settings.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Orchestrateur principal du Canton Controller (CC).
 *
 * Rôle :
 *   - Initialisation générale du CC
 *   - Configuration UART RS485 (CC → EXCC)
 *   - Montage SPIFFS
 *   - Lecture / écriture du fichier settings.json (format JSON 2026)
 *   - Gestion des paramètres globaux (WiFi, Exploration, Booster)
 *   - Accès centralisé aux seuils Booster
 *
 * IMPORTANT 2026 :
 *   - Ce module ne contient AUCUNE logique ferroviaire.
 *   - Il orchestre uniquement l’initialisation et la persistance des données.
 *   - Toute la logique métier (BAL, signaux, manœuvre, topologie) est ailleurs.
 */

#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

class Canton;

class Settings
{
public:
    /* =======================================================================
     *  1) Initialisation générale du système
     *     - UART RS485
     *     - SPIFFS
     *     - Lecture settings.json
     * =====================================================================*/
    static void setup(Canton *nd);  // Initialisation complète (hors CAN)
    static bool begin();            // Dialogue CAN avec la carte Main
    static bool beginCAN();         // Initialisation du bus CAN

    /* =======================================================================
     *  2) UART RS485 (CC → EXCC)
     * =====================================================================*/
    static HardwareSerial &uart();

    /* =======================================================================
     *  3) Paramètres globaux (WiFi, Exploration interne)
     * =====================================================================*/
    static bool wifiOn();
    static void wifiOn(bool val);

    static bool explorationOn();
    static void explorationOn(bool val);

    static void sMainReady(bool val);

    /* =======================================================================
     *  4) Champs globaux accessibles
     * =====================================================================*/
    static bool WIFI_ON;
    static bool EXPLORATION_ON;

    static String ssid_str;
    static String password_str;
    static char ssid[64];
    static char password[64];

    static bool isMainReady;

    static Canton *canton;

    static HardwareSerial SerialUART;

    /* =======================================================================
     *  5) Fonctions internes (modules spécialisés)
     * =====================================================================*/
    static void setupUART();     // Configuration UART RS485
    static bool mountSPIFFS();   // Montage SPIFFS

    /* =======================================================================
     *  6) JSON (Gestion Canton 2026)
     *     - loadFile()  : lecture settings.json → objets Canton
     *     - writeFile() : objets Canton → settings.json
     * =====================================================================*/
    static void loadFile(Canton *canton);
    static void writeFile(Canton *canton);

    /* =======================================================================
     *  7) Booster — Seuils calibrés (API publique)
     * =====================================================================*/
    static void setBoosterSeuilLibre(uint16_t v)  { s_boosterSeuilLibre  = v; }
    static void setBoosterSeuilOccupe(uint16_t v) { s_boosterSeuilOccupe = v; }

    static uint16_t boosterSeuilLibre()  { return s_boosterSeuilLibre; }
    static uint16_t boosterSeuilOccupe() { return s_boosterSeuilOccupe; }

private:
    static uint16_t s_boosterSeuilLibre;
    static uint16_t s_boosterSeuilOccupe;
};
