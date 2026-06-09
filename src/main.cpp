//--- Fichiers inclus ---------------------------------------------------------
// Bibliothèques système
#include <Arduino.h>
#include "freertos/queue.h"

// CAN (topologie Exploration)
#include "CanMsg.h"
#include "CanConfig.h"

// Configuration générale
#include "Config.h"
#include "Settings.h"

#ifdef CHIP_INFO
#include "ChipInfo.h"
#endif

// Modules SA
#include "Canton.h"
#include "Railcom.h"
#include "GestionReseau.h"
#include "Exploration.h"

// Interface Web + WiFi
#include "WebHandler.h"
#include "Wifi_fl.h"

// 🔥 Supervision EXSA (PING/PONG)
#include "SatEXSA_Link.h"

// Watchdog Master (heartbeat + arrêt en cas de blocage)
#include "SAWatchdog.h"

// Logs Exploration 2026
#include "debug_sa.h"

//--- Instances globales ------------------------------------------------------
// Le SA gère un seul canton principal
Canton *canton = new Canton();

// Gestion WiFi + interface Web
Fl_Wifi wifi;
WebHandler webHandler;

// Flag WiFi actif ou non
bool wifiOn = false;

/*============================================================================
                                setup()
============================================================================*/
void setup()
{
  //--- UART Debug ------------------------------------------------------------
  Serial.begin(115200);
  while (!Serial)
  {
  }
  delay(100);

#ifdef CHIP_INFO
  ChipInfo::print();
#endif

  Serial.printf("\nProject   : %s", PROJECT);
  Serial.printf("\nVersion   : %s", VERSION);
  Serial.printf("\nAuteur    : %s", AUTHOR);
  Serial.printf("\nFichier   : %s", __FILE__);
  Serial.printf("\nCompiled  : %s - %s\n\n", __DATE__, __TIME__);
  Serial.printf("-----------------------------------\n\n");

  //--- Chargement settings.json (UART + SPIFFS + JSON) -----------------------
  Settings::setup(canton);
  vTaskDelay(pdMS_TO_TICKS(100));

  //--- Initialisation CAN ----------------------------------------------------
  CanConfig::setup();
  vTaskDelay(pdMS_TO_TICKS(100));
  CanMsg::setup(canton);
  vTaskDelay(pdMS_TO_TICKS(100));

  //--- Dialogue CAN avec la carte Main --------------------------------------
  if (!Settings::begin())
  {
    Serial.printf("[Settings] : Echec de la configuration\n");
    return;
  }

  Serial.printf("-----------------------------------\n");
  Serial.printf("ID Canton : %d\n", canton->ID());
  Serial.printf("-----------------------------------\n\n");

  //--- ⚠️ CHARGEMENT settings.json COMPLET (SSID / PASSWORD / booster / aiguilles)
  Settings::loadFile(canton);

  //--- Mode Exploration --------------------------------------------------------
  if (Settings::explorationOn())
  {
    Exploration::begin(canton);
  }
  else
  {
    for (byte i = 0; i < signalSize; i++)
    {
      Signal *s = canton->getSignal(i);
      if (!s)
      {
        s = new Signal;
        canton->setSignal(i, s);
      }
      s->setup();
    }

    Railcom::begin();
    GestionReseau::setup(canton);
  }

  //--- 🔥 Supervision EXSA (PING/PONG) --------------------------------------
  // SatEXSA_Link::begin();

  //--- 🔥 Watchdog Exploration 2026 : Heartbeat + STOP -------------------------
  // SAWatchdog_begin();

  //--- WiFi + Interface Web --------------------------------------------------
  wifiOn = Settings::wifiOn(); // ⚠️ maintenant que loadFile() a rempli ssid_str
  Serial.printf(">>> DEBUG wifiOn = %d\n", wifiOn);

  if (wifiOn)
  {
    wifi.start(); // ⚠️ ssid_str et password_str sont maintenant valides
    webHandler.init(canton, 80);
  }

  Serial.printf(Settings::explorationOn() ? "[Exploration] : on\n" : "[Exploration] : off\n");
  Serial.printf(Settings::wifiOn() ? "[Wifi] : on\n" : "Wifi : off\n");
  Serial.printf("-----------------------------------\n");
  Serial.printf("[Main %d] : End setup\n\n", __LINE__);
  Serial.printf("-----------------------------------\n\n");

  // En mode release, on coupe le port série après 1 seconde
  // vTaskDelay(pdMS_TO_TICKS(1000));
  // Serial.end();
  // SA_LOG_INFO("Ne doit pas s'afficher !\n");
}

/*============================================================================
                                loop()
============================================================================*/
void loop()
{
  static uint16_t oldAddress = 0;

  //--- Interface Web ---------------------------------------------------------
  if (wifiOn)
    webHandler.loop();

  //--- Railcom : mise à jour adresse loco -----------------------------------
  if (!Settings::explorationOn())
  {
    if (Railcom::address() && canton->busy())
    {
      Loco *loco = canton->getLoco();
      if (loco)
      {
        loco->address(Railcom::address());
      }
    }
  }

  // Log si l’adresse loco change
  {
    Loco *loco = canton->getLoco();
    uint16_t currentAddress = (loco ? loco->address() : 0);

    if (currentAddress != oldAddress)
    {
      SA_LOG_INFO("[Main %d] Railcom - Numero de loco : %d\n",
                  __LINE__, currentAddress);
      oldAddress = currentAddress;
    }
  }

  //--- 🔥 Supervision EXSA : PING/PONG + ONLINE/OFFLINE ----------------------
  // SatEXSA_Link::loop();

  //--- Pause FreeRTOS --------------------------------------------------------
  vTaskDelay(pdMS_TO_TICKS(50));
}

/*============================================================================
                        Fin du fichier src/main.cpp
============================================================================*/
