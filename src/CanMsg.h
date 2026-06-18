#pragma once

/*
 * ============================================================================
 *  CanMsg.h — API CAN du CC (Canton Controller)
 *  --------------------------------------------------------------------------
 *  Ce module gère :
 *
 *    - la réception CAN (tâche FreeRTOS dédiée)
 *    - le décodage des trames Exploration 2026
 *    - le dispatch vers les handlers :
 *          * System (0xB3–CMD_SAVE_ALL)
 *          * Exploration (0xC0–0xC1)
 *          * Exploitation ferroviaire (0xE0–0xE9)
 *
 *    - l’envoi CAN via une API simplifiée :
 *          sendMsg(prio, cmde, resp, id, data…)
 *
 *  Format ID étendu (29 bits) Exploration 2026 :
 *
 *      [ 2 bits priorité ]   bits 26..25
 *      [ 8 bits commande ]   bits 24..17
 *      [ 1 bit response ]    bit  16
 *      [ 16 bits ID CC ]     bits 15..0
 *
 *  Exemple :
 *      prio = 1
 *      cmde = 0xE7
 *      resp = 0
 *      id   = 42
 *
 *      ID = (1 << 25) | (0xE7 << 17) | (0 << 16) | 42
 *
 *  Toutes les surcharges sendMsg() encapsulent ce format.
 * ============================================================================
 */

#include <Arduino.h>
#include <ACAN_ESP32.h>

#include "Config.h"
#include "Canton.h"
#include "Settings.h"
#include "Exploration.h"

/*
 * ============================================================================
 *  Classe CanMsg — Gestion Canton 2026
 *  --------------------------------------------------------------------------
 *  Classe purement statique :
 *    - aucune instance
 *    - toutes les méthodes sont statiques
 *
 *  Fournit :
 *    1) setup()          → création de la tâche FreeRTOS de réception CAN
 *    2) canReceiveMsg()  → boucle de réception / dispatch
 *    3) sendMsg()        → API d’envoi CAN (0 à 8 octets)
 *    4) testMemory()     → optionnel : surveillance stack FreeRTOS
 * ============================================================================
 */
class CanMsg
{
public:
    // -----------------------------------------------------------------------
    // setup(canton)
    // -----------------------------------------------------------------------
    // Initialise le module CAN :
    //   - crée la tâche FreeRTOS canReceiveMsg()
    //   - passe le Canton local en paramètre
    //
    // À appeler UNE SEULE FOIS dans setup().
    // -----------------------------------------------------------------------
    static void setup(Canton *canton);

    // -----------------------------------------------------------------------
    // Envoi brut d’une trame CAN déjà construite
    // -----------------------------------------------------------------------
    static void sendMsg(CANMessage &frame);

    // -----------------------------------------------------------------------
    // API simplifiée d’envoi CAN (0 à 8 octets)
    // -----------------------------------------------------------------------
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2, byte d3);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2, byte d3, byte d4);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2, byte d3, byte d4, byte d5);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2, byte d3, byte d4, byte d5, byte d6);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2, byte d3, byte d4, byte d5, byte d6, byte d7);

#ifdef TEST_MEMORY_TASK
    // -----------------------------------------------------------------------
    // testMemory() — surveillance stack FreeRTOS
    // -----------------------------------------------------------------------
    static void testMemory(void *pvParameters);
#endif

private:
    // -----------------------------------------------------------------------
    // canReceiveMsg()
    // -----------------------------------------------------------------------
    // Tâche FreeRTOS :
    //   - lit les trames CAN
    //   - décode l’ID étendu Exploration 2026
    //   - extrait :
    //        * commande
    //        * ID expéditeur
    //        * bit response
    //   - route vers :
    //        * handleSystemCommand()
    //        * handleExplorationCommand()
    //        * handleExploitCommand()
    //
    // Boucle toutes les 10 ms.
    // -----------------------------------------------------------------------
    static void canReceiveMsg(void *pvParameters);
};
