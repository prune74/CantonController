/*
  CanMsg_System.cpp — Commandes système (CMD_SAT_TEST_BUS_REPLY à CMD_SAVE_ALL)
  --------------------------------------------------------------------------
  Ces commandes ne concernent PAS l’exploitation ferroviaire.
  Elles servent à :
    - tester le bus CAN
    - attribuer un ID au SA
    - activer/désactiver WiFi
    - activer/désactiver Discovery
    - sauvegarder settings.json
*/

#include "CanMsg.h"

/**
 * handleSystemCommand()
 * --------------------------------------------------------------------------
 * @param commande         → code CAN (CMD_SAT_TEST_BUS_REPLY à CMD_SAVE_ALL)
 * @param frameIn          → trame CAN reçue
 * @param node             → canton local
 * @param idSatExpediteur  → ID du satellite expéditeur
 *
 * Ce handler ne modifie JAMAIS la topologie ni l’exploitation.
 * Il agit uniquement sur les paramètres système.
 */
void handleSystemCommand(uint8_t commande, const CANMessage &frameIn, Node *node, uint16_t idSatExpediteur)
{
  (void)idSatExpediteur; // non utilisé dans ces commandes

  switch (commande)
  {
  case CMD_SAT_TEST_BUS_REPLY:
    /**************************************************************************
     * CMD_SAT_TEST_BUS_REPLY — Réponse au test du bus CAN
     * ------------------------------------------------------------------------
     * frameIn.data[0] = 1 → le SA répond correctement
     * Le Main peut alors considérer le SA comme "READY".
     **************************************************************************/
    if (frameIn.data[0])
      Settings::sMainReady(true);
    break;

  case CMD_SAT_REQUEST_ID_REPLY:
    /**************************************************************************
     * CMD_SAT_REQUEST_ID_REPLY — Attribution d’ID
     * ------------------------------------------------------------------------
     * Lorsqu’un SA démarre sans ID (UNUSED_ID), il demande un ID au Main.
     * Le Main répond avec cette commande.
     **************************************************************************/
    if (node->ID() == UNUSED_ID)
      node->ID(frameIn.data[0]);
    break;

  case CMD_RESTART_ALL:
    /**************************************************************************
     * CMD_RESTART_ALL — Reset ESP32
     * ------------------------------------------------------------------------
     * Commande critique : permet au Main de redémarrer un SA à distance.
     **************************************************************************/
    ESP.restart();
    break;

  case CMD_WIFI_ON_OFF:
    /**************************************************************************
     * CMD_WIFI_ON_OFF — Activation / désactivation du WiFi
     * ------------------------------------------------------------------------
     * frameIn.data[0] = 0 → WiFi OFF
     * frameIn.data[0] = 1 → WiFi ON
     *
     * Après modification → sauvegarde → reboot.
     **************************************************************************/
    Settings::wifiOn(frameIn.data[0]);
    Settings::writeFile(Settings::node);
    delay(1000);
    ESP.restart();
    break;

  case CMD_DISCOVERY_ON_OFF:
    /**************************************************************************
     * CMD_DISCOVERY_ON_OFF — Activation / désactivation du mode Discovery
     * ------------------------------------------------------------------------
     * Discovery = apprentissage automatique de la topologie SP/SM.
     *
     * Si ON → reboot pour entrer en mode apprentissage.
     * Si OFF → arrêt immédiat du processus Discovery.
     **************************************************************************/
    if (frameIn.data[0])
    {
      Settings::discoveryOn(true);
      Settings::writeFile(Settings::node);
      delay(1000);
      ESP.restart();
    }
    else
    {
      Settings::discoveryOn(false);
      Discovery::stopProcess(true);
      Settings::writeFile(Settings::node);
    }
    break;

  case CMD_SAVE_ALL:
    /**************************************************************************
     * CMD_SAVE_ALL — Sauvegarde settings.json
     * ------------------------------------------------------------------------
     * Permet au Main d’imposer une sauvegarde immédiate.
     **************************************************************************/
#ifdef SAUV_BY_MAIN
    Settings::writeFile(Settings::node);
#else
    SA_LOG("Sauvegarde automatique désactivée.\n");
#endif
    break;

  default:
    break;
  }
}
