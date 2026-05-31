//--- Fichiers inclus ---------------------------------------------------------
// Bibliothèques système
#include <Arduino.h>
#include "freertos/queue.h"

// CAN (topologie Discovery)
#include "CanMsg.h"
#include "CanConfig.h"

// Configuration générale
#include "Config.h"
#include "Settings.h"

#ifdef CHIP_INFO
#include "ChipInfo.h"
#endif

// Modules SA
#include "Node.h"
#include "Railcom.h"
#include "GestionReseau.h"
#include "Discovery.h"

// Interface Web + WiFi
#include "WebHandler.h"
#include "Wifi_fl.h"

// 🔥 Supervision EXSA (PING/PONG)
#include "SatEXSA_Link.h"

// Watchdog Master (heartbeat + arrêt en cas de blocage)
#include "SAWatchdog.h"

// Logs Discovery 2026
#include "debug_sa.h"

//--- Instances globales ------------------------------------------------------
// Le SA gère un seul canton principal
Node* node = new Node();

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
  while (!Serial) {}
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

  //--- Chargement settings.json ---------------------------------------------
  Settings::setup(node);
  vTaskDelay(pdMS_TO_TICKS(100));

  //--- Initialisation CAN ----------------------------------------------------
  CanConfig::setup();
  vTaskDelay(pdMS_TO_TICKS(100));
  CanMsg::setup(node);
  vTaskDelay(pdMS_TO_TICKS(100));

  //--- Lecture ID Node -------------------------------------------------------
  if (Settings::begin())
  {
    Serial.printf("-----------------------------------\n");
    Serial.printf("ID Node : %d\n", node->ID());
    Serial.printf("-----------------------------------\n\n");
  }
  else
  {
    Serial.printf("[Settings] : Echec de la configuration\n");
    return;
  }

  //--- Mode Discovery --------------------------------------------------------
  if (Settings::discoveryOn())
  {
    // Mode apprentissage topologie SP/SM
    Discovery::begin(node);
  }
  else
  {
    // Mode normal : initialisation des signaux
    for (byte i = 0; i < signalSize; i++)
    {
      Signal* s = node->getSignal(i);
      if (!s)
      {
        s = new Signal;
        node->setSignal(i, s);
      }

      s->setup();
    }

    // Railcom + logique ferroviaire
    Railcom::begin();
    GestionReseau::setup(node);
  }

  //--- WiFi + Interface Web --------------------------------------------------
  wifiOn = Settings::wifiOn();
  if (wifiOn)
  {
    wifi.start();
    webHandler.init(node, 80);
  }

  Serial.printf(Settings::discoveryOn() ? "[Discovery] : on\n" : "[Discovery] : off\n");
  Serial.printf(Settings::wifiOn() ? "[Wifi] : on\n" : "Wifi : off\n");
  Serial.printf("-----------------------------------\n");
  Serial.printf("[Main %d] : End setup\n\n", __LINE__);
  Serial.printf("-----------------------------------\n\n");

  //--- 🔥 Supervision EXSA (PING/PONG) --------------------------------------
  SatEXSA_Link::begin();

  //--- 🔥 Watchdog Discovery 2026 : Heartbeat + STOP -------------------------
  SAWatchdog_begin();

  // En mode release, on coupe le port série après 1 seconde
  vTaskDelay(pdMS_TO_TICKS(1000));
  Serial.end();
  SA_LOG_INFO("Ne doit pas s'afficher !\n");
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
  if (!Settings::discoveryOn())
  {
    if (Railcom::address() && node->busy())
    {
      Loco* loco = node->getLoco();
      if (loco)
      {
        loco->address(Railcom::address());
      }
    }
  }

  // Log si l’adresse loco change
  {
    Loco* loco = node->getLoco();
    uint16_t currentAddress = (loco ? loco->address() : 0);

    if (currentAddress != oldAddress)
    {
      SA_LOG_INFO("[Main %d] Railcom - Numero de loco : %d\n",
                  __LINE__, currentAddress);
      oldAddress = currentAddress;
    }
  }

  //--- 🔥 Supervision EXSA : PING/PONG + ONLINE/OFFLINE ----------------------
  SatEXSA_Link::loop();

  //--- Pause FreeRTOS --------------------------------------------------------
  vTaskDelay(pdMS_TO_TICKS(50));
}

/*============================================================================
                        Fin du fichier src/main.cpp
============================================================================*/
