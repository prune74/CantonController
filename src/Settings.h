#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

class Canton;

class Settings
{
public:
    /* =======================================================================
     *  API d’initialisation
     * =====================================================================*/
    static void setup(Canton *nd);

    static bool begin();
    static bool beginCAN();

    /* =======================================================================
     *  UART RS485 (SA → EXSA)
     * =====================================================================*/
    static HardwareSerial &uart();

    /* =======================================================================
     *  Paramètres globaux (WiFi, Exploration, Canton)
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
    static void setupUART();
    static bool mountSPIFFS();

    /* =======================================================================
     *  JSON (Exploration 2026)
     * =====================================================================*/
    static void loadFile(Canton *canton);
    static void writeFile(Canton *canton);

    // API JSON key/value
    static void set(const char *key, uint16_t value);
    static uint16_t get(const char *key);

    static void save(); // écrit settings.json
    static void load(); // recharge settings.json

    /* =======================================================================
     *  Booster — Seuils calibrés (PROTO_09)
     * =====================================================================*/
    static void setBoosterSeuilLibre(uint16_t v);
    static void setBoosterSeuilOccupe(uint16_t v);

    static uint16_t boosterSeuilLibre();
    static uint16_t boosterSeuilOccupe();

private:
    static uint16_t s_boosterSeuilLibre;
    static uint16_t s_boosterSeuilOccupe;
};
