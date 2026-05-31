#pragma once

/*
 * ============================================================================
 *  CanMsg.h — API CAN du SA (Satellite d’Aiguillage)
 *  --------------------------------------------------------------------------
 *  Ce module gère :
 *
 *    - la réception CAN (tâche FreeRTOS dédiée)
 *    - le décodage des trames Discovery 2026
 *    - le dispatch vers les handlers :
 *          * System (0xB3–CMD_SAVE_ALL)
 *          * Discovery (0xC0–0xC1)
 *          * Exploitation ferroviaire (0xE0–0xE9)
 *
 *    - l’envoi CAN via une API simplifiée :
 *          sendMsg(prio, cmde, resp, id, data…)
 *
 *  Le protocole CAN Discovery utilise un ID étendu (29 bits) structuré ainsi :
 *
 *      [ 2 bits priorité ]   bits 26..25
 *      [ 8 bits commande ]   bits 24..17
 *      [ 1 bit response ]    bit  16
 *      [ 16 bits ID SA ]     bits 15..0
 *
 *  Exemple :
 *      prio = 1
 *      cmde = 0xE7
 *      resp = 0
 *      id   = 42
 *
 *      ID = (1 << 25) | (0xE7 << 17) | (0 << 16) | 42
 *
 *  Toutes les fonctions sendMsg() encapsulent ce format.
 *
 * ============================================================================
 */

#include <Arduino.h>
#include <ACAN_ESP32.h>

#include "Config.h"
#include "Node.h"
#include "Settings.h"
#include "Discovery.h"

/*
 * ============================================================================
 *  Classe CanMsg
 *  --------------------------------------------------------------------------
 *  Cette classe est purement statique :
 *    - aucune instance n’est créée
 *    - toutes les méthodes sont statiques
 *
 *  Elle fournit :
 *
 *    1) setup()
 *       → crée la tâche FreeRTOS de réception CAN
 *
 *    2) canReceiveMsg()
 *       → boucle de réception CAN (décodage + dispatch)
 *
 *    3) sendMsg()
 *       → API d’envoi CAN (0 à 8 octets)
 *
 *    4) testMemory()
 *       → optionnel : surveille la stack de la tâche CAN
 *
 * ============================================================================
 */
class CanMsg
{
public:
  // -------------------------------------------------------------------------
  // setup(node)
  // -------------------------------------------------------------------------
  // Initialise le module CAN :
  //   - crée la tâche FreeRTOS canReceiveMsg()
  //   - passe le Node local en paramètre
  //
  // Cette fonction doit être appelée UNE SEULE FOIS dans setup().
  // -------------------------------------------------------------------------
  static void setup(Node *node);

  // -------------------------------------------------------------------------
  // sendMsg(CANMessage&)
  // -------------------------------------------------------------------------
  // Envoi brut d’une trame CAN déjà formatée.
  // Utilisé par toutes les surcharges ci‑dessous.
  // -------------------------------------------------------------------------
  static void sendMsg(CANMessage &frame);

  // -------------------------------------------------------------------------
  // API simplifiée d’envoi CAN
  // -------------------------------------------------------------------------
  // Ces surcharges permettent d’envoyer une trame CAN sans construire
  // manuellement un CANMessage.
  //
  // Exemple :
  //     CanMsg::sendMsg(1, 0xE7, 0, node->ID(), aspect);
  //
  // Toutes utilisent le format Discovery 2026 (ID étendu).
  // -------------------------------------------------------------------------
  static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisNodeId);
  static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisNodeId, byte data0);
  static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisNodeId, byte data0, byte data1);
  static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisNodeId, byte data0, byte data1, byte data2);
  static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisNodeId, byte data0, byte data1, byte data2, byte data3);
  static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisNodeId, byte data0, byte data1, byte data2, byte data3, byte data4);
  static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisNodeId, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5);
  static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisNodeId, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5, byte data6);
  static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisNodeId, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5, byte data6, byte data7);

#ifdef TEST_MEMORY_TASK
  // -------------------------------------------------------------------------
  // testMemory()
  // -------------------------------------------------------------------------
  // Tâche optionnelle permettant de surveiller la mémoire restante
  // de la tâche canReceiveMsg().
  //
  // Utile pour débuguer les stacks FreeRTOS.
  // -------------------------------------------------------------------------
  static void testMemory(void *pvParameters);
#endif

private:
  // -------------------------------------------------------------------------
  // canReceiveMsg()
  // -------------------------------------------------------------------------
  // Tâche FreeRTOS :
  //   - lit les trames CAN
  //   - décode l’ID étendu
  //   - extrait :
  //        * commande (0xB3–0xE9)
  //        * ID expéditeur
  //        * bit response
  //   - route vers :
  //        * handleSystemCommand()
  //        * handleDiscoveryCommand()
  //        * handleExploitCommand()
  //
  // Cette fonction tourne en permanence (10 ms).
  // -------------------------------------------------------------------------
  static void canReceiveMsg(void *pvParameters);
};
