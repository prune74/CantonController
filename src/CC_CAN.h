#pragma once

/*
 * ============================================================================
 *  CC_CAN.h — API CAN du CC (Canton Controller)
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
 *  IMPORTANT :
 *    Cette classe est l’API HAUT NIVEAU du CantonController.
 *    Le bas niveau (drivers, abstraction, ID 29 bits, conversions)
 *    est désormais géré par CanUniversal.
 * ============================================================================
 */

#include <Arduino.h>
#include <ACAN_ESP32.h>   // uniquement pour le type CANMessage (handlers existants)

#include "Config.h"
#include "Canton.h"
#include "Settings.h"
#include "Exploration.h"
#include "Aig.h"

class CC_CAN
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
    // (compatibilité avec code existant)
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
    //   - lit les trames CAN via CanUniversal
    //   - décode l’ID étendu Exploration 2026 via CanID
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
