#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

class Node;

class Settings
{
public:
    /* =======================================================================
     *  API d’initialisation
     * =====================================================================*/
    static void setup(Node* nd);

    static bool begin();
    static bool beginCAN();

    /* =======================================================================
     *  UART RS485 (SA → EXSA)
     * =====================================================================*/
    static HardwareSerial& uart();

    /* =======================================================================
     *  Paramètres globaux (WiFi, Discovery, Node)
     * =====================================================================*/
    static bool wifiOn();
    static void wifiOn(bool val);

    static bool discoveryOn();
    static void discoveryOn(bool val);

    static void sMainReady(bool val);

    /* =======================================================================
     *  Champs globaux accessibles
     * =====================================================================*/
    static bool WIFI_ON;
    static bool DISCOVERY_ON;

    static String ssid_str;
    static String password_str;
    static char ssid[30];
    static char password[30];

    static bool isMainReady;

    static Node* node;

    static HardwareSerial SerialUART;

    /* =======================================================================
     *  Fonctions internes (modules spécialisés)
     * =====================================================================*/
    static void setupUART();
    static bool mountSPIFFS();

    /* =======================================================================
     *  JSON (Discovery 2026)
     * =====================================================================*/
    static void loadFile(Node* node);
    static void writeFile(Node* node);

    // API JSON key/value
    static void set(const char* key, uint16_t value);
    static uint16_t get(const char* key);

    static void save();   // écrit settings.json
    static void load();   // recharge settings.json

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
