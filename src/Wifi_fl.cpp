/*
 * Wifi_fl.cpp — Version robuste (alignée sur ERM)
 */

#include "Wifi_fl.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include "Config.h"
#include "Settings.h"
#include <Arduino.h>

void Fl_Wifi::start()
{
#ifdef WIFI_AP_MODE

    // -----------------------------------------------------------------------
    // MODE AP (Point d’accès)
    // -----------------------------------------------------------------------
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PSW);

    Serial.println();
    Serial.println("------------ WIFI (AP MODE) ------------");
    Serial.printf("SSID       : %s\n", WIFI_SSID);
    Serial.printf("Password   : %s\n", WIFI_PSW);
    Serial.printf("IP address : %s\n", WiFi.softAPIP().toString().c_str());
    Serial.println("----------------------------------------\n");

#else

    // -----------------------------------------------------------------------
    // MODE STA (Client)
    // -----------------------------------------------------------------------
    const char *ssid = Settings::ssid;
    const char *psw  = Settings::password;

    Serial.println();
    Serial.println("------------ WIFI (STA MODE) ------------");
    Serial.printf("Connexion à SSID=\"%s\"\n", ssid);

    // Obligatoire : sinon l’ESP32 peut relancer un ancien WiFi
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid, psw);

    uint32_t timeout = millis() + 30000;   // 30 secondes
    uint32_t lastLog = 0;

    while (WiFi.status() != WL_CONNECTED && millis() < timeout)
    {
        if (millis() - lastLog > 500)
        {
            Serial.print(".");
            lastLog = millis();
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Non bloquant
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("------------ WIFI CONNECTÉ ------------");
        Serial.printf("SSID       : %s\n", ssid);
        Serial.printf("IP address : %s\n", WiFi.localIP().toString().c_str());
        Serial.println("----------------------------------------\n");
    }
    else
    {
        Serial.println("------------ WIFI ÉCHEC ---------------");
        Serial.printf("Impossible de se connecter à \"%s\"\n", ssid);
        Serial.println("Vérifier SSID / mot de passe");
        Serial.println("----------------------------------------\n");
    }

#endif
}
