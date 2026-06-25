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
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2, byte d3);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2, byte d3, byte d4);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2, byte d3, byte d4, byte d5);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte d0, byte d1, byte d2, byte d3, byte d4, byte d5, byte d6);
    static void sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                        byte d0, byte d1, byte d2, byte d3, byte d4, byte d5, byte d6, byte d7);

private:
    // Tâche FreeRTOS de réception
    static void canReceiveMsg(void *pvParameters);

    // Traitement d’un message reçu (bus 0 ou bus 1)
    static void traiterMessageCAN(const CanMsg &msg, Canton *canton, uint8_t bus);

#ifdef TEST_MEMORY_TASK
    static void testMemory(void *pvParameters);
#endif
};
