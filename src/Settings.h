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
 *   - Gestion des paramètres globaux (WiFi, exploration, booster)
 *   - Accès centralisé aux seuils Booster
 *
 * Ce module ne contient aucune logique ferroviaire : il orchestre
 * simplement l’initialisation et la persistance des paramètres.
 */

#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

class Canton;

class Settings
{
public:
    /* =======================================================================
     *  Initialisation générale du système
     * =====================================================================*/
    static void setup(Canton *nd);  // Initialisation complète (hors CAN)
    static bool begin();            // Dialogue CAN avec la carte Main
    static bool beginCAN();         // Initialisation du bus CAN

    /* =======================================================================
     *  UART RS485 (CC → EXCC)
     * =====================================================================*/
    static HardwareSerial &uart();

    /* =======================================================================
     *  Paramètres globaux (WiFi, Exploration interne)
     * =====================================================================*/
    static bool wifiOn();
    static void wifiOn(bool val);

    static bool explorationOn();
    static void explorationOn(bool val);

    static void sMainReady(bool val);

    /* =======================================================================
     *  Champs globaux accessibles
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
     *  Fonctions internes (modules spécialisés)
     * =====================================================================*/
    static void setupUART();     // Configuration UART RS485
    static bool mountSPIFFS();   // Montage SPIFFS

    /* =======================================================================
     *  JSON (Gestion Canton 2026)
     * =====================================================================*/
    static void loadFile(Canton *canton);   // Lecture settings.json
    static void writeFile(Canton *canton);  // Écriture settings.json

    /* =======================================================================
     *  Booster — Seuils calibrés (API publique)
     * =====================================================================*/
    static void setBoosterSeuilLibre(uint16_t v)  { s_boosterSeuilLibre  = v; }
    static void setBoosterSeuilOccupe(uint16_t v) { s_boosterSeuilOccupe = v; }

    static uint16_t boosterSeuilLibre()  { return s_boosterSeuilLibre; }
    static uint16_t boosterSeuilOccupe() { return s_boosterSeuilOccupe; }

private:
    static uint16_t s_boosterSeuilLibre;
    static uint16_t s_boosterSeuilOccupe;
};
