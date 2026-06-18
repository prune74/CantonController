// ---------------------------------------------------------------------------
// main.cpp — Gestion Canton 2026
// ---------------------------------------------------------------------------
// Point d’entrée du Canton Controller (CC).
//
// Rôle :
//   - Initialisation générale (UART, SPIFFS, settings.json)
//   - Initialisation CAN (ACAN / MCP2515 selon config)
//   - Dialogue CAN avec la carte Main
//   - Initialisation EXCC (Extension Canton Controller)
//   - Gestion RailCom (mode normal) ou Exploration (mode interne)
//   - Interface Web + WiFi
//
// Ce fichier ne contient aucune logique ferroviaire :
//   → tout est délégué à Canton, GestionReseau, Exploration, EXCC_Link.
// ---------------------------------------------------------------------------

// --- Inclusions système -----------------------------------------------------
#include <Arduino.h>
#include "freertos/queue.h"

// --- CAN --------------------------------------------------------------------
#include "CanMsg.h"
#include "CanConfig.h"

// --- Configuration générale -------------------------------------------------
#include "Config.h"
#include "Settings.h"

#ifdef CHIP_INFO
#include "ChipInfo.h"
#endif

// --- Modules CC -------------------------------------------------------------
#include "Canton.h"
#include "Railcom.h"
#include "GestionReseau.h"
#include "Exploration.h"

// --- EXCC (Extension Canton Controller) -------------------------------------
#include "EXCC_Link.h"

// --- Interface Web + WiFi ---------------------------------------------------
#include "WebHandler.h"
#include "Wifi_fl.h"

// --- Logs CC ----------------------------------------------------------------
#include "debug_cc.h"

// --- Instances globales -----------------------------------------------------
Canton *canton = new Canton();
Fl_Wifi wifi;
WebHandler webHandler;
bool wifiOn = false;

/*============================================================================
                                setup()
============================================================================*/
void setup()
{
    Serial.begin(115200);
    while (!Serial) {}
    delay(100);

#ifdef CHIP_INFO
    ChipInfo::print();
#endif

    CC_LOG_INFO("\nProject   : %s", PROJECT);
    CC_LOG_INFO("\nVersion   : %s", VERSION);
    CC_LOG_INFO("\nAuteur    : %s", AUTHOR);
    CC_LOG_INFO("\nFichier   : %s", __FILE__);
    CC_LOG_INFO("\nCompiled  : %s - %s\n\n", __DATE__, __TIME__);
    CC_LOG_INFO("-----------------------------------\n\n");

    // ------------------------------------------------------------------------
    // Chargement settings.json (SPIFFS + JSON 2026)
    // ------------------------------------------------------------------------
    Settings::setup(canton);
    vTaskDelay(pdMS_TO_TICKS(100));

    // ------------------------------------------------------------------------
    // Initialisation CAN (ACAN / MCP2515 selon Config.h)
    // ------------------------------------------------------------------------
    CanConfig::setup();
    vTaskDelay(pdMS_TO_TICKS(100));
    CanMsg::setup(canton);
    vTaskDelay(pdMS_TO_TICKS(100));

    // ------------------------------------------------------------------------
    // Dialogue CAN avec la carte Main
    // ------------------------------------------------------------------------
    if (!Settings::begin())
    {
        CC_LOG_WARN("[Main][CC] Erreur : configuration CAN impossible\n");
        return;
    }

    CC_LOG_INFO("-----------------------------------\n");
    CC_LOG_INFO("[Main][CC] ID Canton : %d\n", canton->ID());
    CC_LOG_INFO("-----------------------------------\n\n");

    // ------------------------------------------------------------------------
    // Chargement complet settings.json
    // ------------------------------------------------------------------------
    Settings::loadFile(canton);

    // ------------------------------------------------------------------------
    // Initialisation GPIO étendus (MCP23017)
    // ------------------------------------------------------------------------
    canton->initMCP();

    // ------------------------------------------------------------------------
    // Déduction automatique du rôle ferroviaire
    // ------------------------------------------------------------------------
    canton->computeRole();

    // ------------------------------------------------------------------------
    // Mode Exploration interne ou mode normal (RailCom + GestionReseau)
    // ------------------------------------------------------------------------
    if (Settings::explorationOn())
    {
        Exploration::begin(canton);
    }
    else
    {
        Railcom::begin();
        GestionReseau::setup(canton);
    }

    // ------------------------------------------------------------------------
    // Initialisation EXCC (Extension Canton Controller)
    // ------------------------------------------------------------------------
    EXCC_Link::begin();

    // ------------------------------------------------------------------------
    // WiFi + Interface Web
    // ------------------------------------------------------------------------
    wifiOn = Settings::wifiOn();
    CC_LOG_INFO("[Main][CC] wifiOn = %d\n", wifiOn);

    if (wifiOn)
    {
        wifi.start();
        webHandler.init(canton, 80);
    }

    CC_LOG_INFO("[Main][CC] Exploration : %s\n",
                Settings::explorationOn() ? "on" : "off");
    CC_LOG_INFO("[Main][CC] WiFi        : %s\n",
                Settings::wifiOn() ? "on" : "off");

    CC_LOG_INFO("-----------------------------------\n");
    CC_LOG_INFO("[Main][CC] End setup\n\n");
    CC_LOG_INFO("-----------------------------------\n\n");
}

/*============================================================================
                                loop()
============================================================================*/
void loop()
{
    static uint16_t oldAddress = 0;

    // ------------------------------------------------------------------------
    // Interface Web
    // ------------------------------------------------------------------------
    if (wifiOn)
        webHandler.loop();

    // ------------------------------------------------------------------------
    // RailCom : mise à jour de l’adresse loco (mode normal)
    // ------------------------------------------------------------------------
    if (!Settings::explorationOn())
    {
        if (Railcom::address() && canton->busy())
        {
            Loco *loco = canton->getLoco();
            if (loco)
                loco->address(Railcom::address());
        }
    }

    // Log si l’adresse change
    {
        Loco *loco = canton->getLoco();
        uint16_t currentAddress = loco ? loco->address() : 0;

        if (currentAddress != oldAddress)
        {
            CC_LOG_INFO("[Main][CC] Railcom : adresse loco = %u\n",
                        currentAddress);
            oldAddress = currentAddress;
        }
    }

    // ------------------------------------------------------------------------
    // EXCC (Extension Canton Controller)
    // ------------------------------------------------------------------------
    EXCC_Link::loop();

    vTaskDelay(pdMS_TO_TICKS(50));
}
