#include "CC_CAN.h"

#include "CanID.h"     // CanUniversal
#include "CanMsg.h"    // CanUniversal
#include "CanBus.h"    // CanUniversal

/*
 * ============================================================================
 *  CC_CAN_Send.cpp — Gestion Canton 2026 (version CanUniversal)
 * ============================================================================
 */

// ---------------------------------------------------------------------------
// Envoi brut d’une trame CANMessage (compatibilité legacy)
// ---------------------------------------------------------------------------
void CC_CAN::sendMsg(CANMessage &frame)
{
    // Conversion vers CanMsg universel
    CanMsg msg(frame);

    // Envoi via CanUniversal
    CanBus::bus(0).send(msg);
}

// ---------------------------------------------------------------------------
// Fonction interne : construction d’un CanMsg universel
// ---------------------------------------------------------------------------
static CanMsg makeUniversalMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId)
{
    uint32_t id29 = CanID::make29(prio, cmde, resp, thisCantonId);
    return CanMsg(id29, {});
}

// ---------------------------------------------------------------------------
// Surcharges sendMsg() — API simplifiée (0 à 8 octets)
// ---------------------------------------------------------------------------

// --- 0 octet ----------------------------------------------------------------
void CC_CAN::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId)
{
    CanMsg msg = makeUniversalMsg(prio, cmde, resp, thisCantonId);
    CanBus::bus(0).send(msg);
}

// --- 1 octet ----------------------------------------------------------------
void CC_CAN::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0)
{
    CanMsg msg = CanMsg(CanID::make29(prio, cmde, resp, thisCantonId), { d0 });
    CanBus::bus(0).send(msg);
}

// --- 2 octets ---------------------------------------------------------------
void CC_CAN::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1)
{
    CanMsg msg = CanMsg(CanID::make29(prio, cmde, resp, thisCantonId), { d0, d1 });
    CanBus::bus(0).send(msg);
}

// --- 3 octets ---------------------------------------------------------------
void CC_CAN::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2)
{
    CanMsg msg = CanMsg(CanID::make29(prio, cmde, resp, thisCantonId), { d0, d1, d2 });
    CanBus::bus(0).send(msg);
}

// --- 4 octets ---------------------------------------------------------------
void CC_CAN::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2, byte d3)
{
    CanMsg msg = CanMsg(CanID::make29(prio, cmde, resp, thisCantonId), { d0, d1, d2, d3 });
    CanBus::bus(0).send(msg);
}

// --- 5 octets ---------------------------------------------------------------
void CC_CAN::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2, byte d3, byte d4)
{
    CanMsg msg = CanMsg(CanID::make29(prio, cmde, resp, thisCantonId), { d0, d1, d2, d3, d4 });
    CanBus::bus(0).send(msg);
}

// --- 6 octets ---------------------------------------------------------------
void CC_CAN::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2, byte d3, byte d4, byte d5)
{
    CanMsg msg = CanMsg(CanID::make29(prio, cmde, resp, thisCantonId), { d0, d1, d2, d3, d4, d5 });
    CanBus::bus(0).send(msg);
}

// --- 7 octets ---------------------------------------------------------------
void CC_CAN::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2, byte d3, byte d4, byte d5, byte d6)
{
    CanMsg msg = CanMsg(CanID::make29(prio, cmde, resp, thisCantonId), { d0, d1, d2, d3, d4, d5, d6 });
    CanBus::bus(0).send(msg);
}

// --- 8 octets ---------------------------------------------------------------
void CC_CAN::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2, byte d3,
                     byte d4, byte d5, byte d6, byte d7)
{
    CanMsg msg = CanMsg(CanID::make29(prio, cmde, resp, thisCantonId),
                        { d0, d1, d2, d3, d4, d5, d6, d7 });
    CanBus::bus(0).send(msg);
}
