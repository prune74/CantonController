#pragma once

#include <stdint.h>
#include "Canton.h"
#include "CanMsg.h"

class CC_CAN
{
public:
    // Initialisation : création de la tâche de réception
    static void setup(Canton *canton);

    // -----------------------------------------------------------------------
    // API d’envoi (bus Discovery = bus 0)
    // -----------------------------------------------------------------------

    // Envoi legacy (ACAN) — conservé uniquement si encore utilisé
    static void sendMsg(CANMessage &frame);

    // API moderne CanUniversal
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisCantonId);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisCantonId, uint8_t d0);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisCantonId, uint8_t d0, uint8_t d1);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisCantonId, uint8_t d0, uint8_t d1, uint8_t d2);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisCantonId, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisCantonId, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisCantonId, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisCantonId, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisCantonId,
                        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

private:
    // Tâche FreeRTOS de réception
    static void canReceiveMsg(void *pvParameters);

    // Traitement d’un message reçu (bus 0 ou bus 1)
    static void traiterMessageCAN(const CanMsg &msg, Canton *canton, uint8_t bus);

#ifdef TEST_MEMORY_TASK
    static void testMemory(void *pvParameters);
#endif
};
