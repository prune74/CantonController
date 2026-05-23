/*
   Wifi_fl.cpp — Gestion du WiFi (SA)
   Version Discovery 2026
*/

#include "Wifi_fl.h"
#include "Settings.h"
#include "debug_sa.h"

void Fl_Wifi::start()
{
#ifdef WIFI_AP_MODE

    WiFi.softAP(WIFI_SSID, WIFI_PSW);

    SA_LOG_INFO("------------ WIFI (AP MODE) ------------\n");
    SA_LOG_INFO("SSID        : %s\n", WIFI_SSID);
    SA_LOG_INFO("IP address  : %s\n", WiFi.softAPIP().toString().c_str());
    SA_LOG_INFO("----------------------------------------\n");

#else

    WiFi.begin(Settings::ssid, Settings::password);

    SA_LOG_INFO("Connexion au WiFi : %s\n", Settings::ssid);

    while (WiFi.status() != WL_CONNECTED)
    {
        SA_LOG_TRACE("Tentative de connexion WiFi...\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    SA_LOG_INFO("------------ WIFI (CLIENT MODE) ------------\n");
    SA_LOG_INFO("SSID        : %s\n", Settings::ssid);
    SA_LOG_INFO("IP address  : %s\n", WiFi.localIP().toString().c_str());
    SA_LOG_INFO("--------------------------------------------\n");

#endif
}
