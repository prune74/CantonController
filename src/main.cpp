// ---------------------------------------------------------------------------
// main.cpp — Gestion Canton 2026
// ---------------------------------------------------------------------------
// Point d’entrée du Canton Controller (CC).
//
// Rôle :
//   - Initialisation générale (UART, SPIFFS, settings.json)
//   - Initialisation CAN (ACAN / MCP2515 selon config)
//   - Dialogue CAN avec la carte ERM (attribution ID, test bus…)
//   - Initialisation EXCC (Extension Canton Controller)
//   - Gestion RailCom (mode normal) ou Exploration (mode interne)
//   - Interface Web + WiFi
//   - Démarrage du Watchdog CC → ERM (heartbeat 0x200)
//
// Ce fichier NE CONTIENT AUCUNE LOGIQUE FERROVIAIRE :
//   → tout est délégué à Canton, GestionReseau, Exploration, EXCC_Link.
// ---------------------------------------------------------------------------

// --- Inclusions système -----------------------------------------------------
#include <Arduino.h>
#include "freertos/queue.h"

// --- CAN --------------------------------------------------------------------
#include "CC_CAN.h"
#include "CC_CAN_Config.h"

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
#include "SupervisionAiguilles.h"

// --- Interface Web + WiFi ---------------------------------------------------
#include "WebHandler.h"
#include "Wifi_fl.h"
#include <SPIFFS.h>


// --- Watchdog CC → ERM ---------------------------------------------------
#include "CCWatchdog.h"

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
    // ------------------------------------------------------------------------
    // UART debug
    // ------------------------------------------------------------------------
    Serial.begin(115200);
    while (!Serial)
    {
    }
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
        // Initialisation CAN (CanUniversal)
        // ------------------------------------------------------------------------
        CcCanConfig::setup(); // ✔ nouveau nom
        vTaskDelay(pdMS_TO_TICKS(100));
        CC_CAN::setup(canton); // ✔ nouvelle API CAN du CC
        vTaskDelay(pdMS_TO_TICKS(100));

        // ------------------------------------------------------------------------
        // Dialogue CAN avec la carte ERM (test bus, attribution ID…)
        // ------------------------------------------------------------------------
        if (!Settings::standalone())
        {
            if (!Settings::begin())
            {
                CC_LOG_WARN("[ERM][CC] Erreur : configuration CAN impossible\n");
                return;
            }

            CC_LOG_INFO("-----------------------------------\n");
            CC_LOG_INFO("[ERM][CC] ID Canton : %d\n", canton->ID());
            CC_LOG_INFO("-----------------------------------\n\n");
        }
    
    // ------------------------------------------------------------------------
    // Chargement complet settings.json
    // ------------------------------------------------------------------------
    Settings::loadFile(canton);

    // ------------------------------------------------------------------------
    // Initialisation GPIO étendus (MCP23017)
    // ------------------------------------------------------------------------
    /*canton->initMCP();
    
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
    */
    // ------------------------------------------------------------------------
    // Initialisation EXCC (Extension Canton Controller)
    // ------------------------------------------------------------------------
    SupervisionAiguilles::begin(canton);
    EXCC_Link::begin();

    // ------------------------------------------------------------------------
    // WiFi + Interface Web
    // ------------------------------------------------------------------------
    wifiOn = Settings::wifiOn();
    CC_LOG_INFO("[ERM][CC] wifiOn = %d\n", wifiOn);

    if (wifiOn)
    {
        wifi.start();
        webHandler.init(canton, 80);
    }

    CC_LOG_INFO("[ERM][CC] Exploration : %s\n",
                Settings::explorationOn() ? "on" : "off");
    CC_LOG_INFO("[ERM][CC] WiFi        : %s\n",
                Settings::wifiOn() ? "on" : "off");

    CC_LOG_INFO("-----------------------------------\n");
    CC_LOG_INFO("[ERM][CC] End setup\n\n");
    CC_LOG_INFO("-----------------------------------\n\n");

    // ------------------------------------------------------------------------
   // Watchdog CC → ERM (Heartbeat 0x200)
   // ------------------------------------------------------------------------
   // Rôle :
   //   - envoi périodique d’un heartbeat toutes les 100 ms
   //   - permet au ERM de superviser la présence du CC
   //
   // Important :
   //   - si le CC entre en STOP local → Canton_Stop.cpp suspend le heartbeat
   //   - le ERM détecte l’absence de heartbeat → STOP global réseau
   //
   // Le watchdog doit être démarré APRÈS :
   //   - l’attribution de l’ID canton
   //   - l’initialisation CAN
   //   - l’initialisation EXCC
   //
   // Et AVANT :
   //   - l’interface Web (pour éviter un délai de démarrage)
   // ------------------------------------------------------------------------
   if (!Settings::standalone())
   {
       CCWatchdog_begin();
   }
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
                CC_LOG_INFO("[ERM][CC] Railcom : adresse loco = %u\n",
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
