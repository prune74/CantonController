/*
 * CommandeDCC.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Envoi des commandes DCC++ pour la locomotive du canton.
 *
 * Rôle :
 *   - récupérer la loco associée au canton
 *   - envoyer la commande DCC (adresse + vitesse)
 *   - répéter l’envoi 5 fois pour fiabiliser la transmission
 *
 * Ce module ne contient aucune logique ferroviaire :
 *   → il transporte uniquement les commandes DCC++ vers la carte Main.
 */

#include "CommandeDCC.h"
#include "debug_cc.h"
#include "CC_CAN.h"

#include "CanBus.h"
#include "CanMsg.h"

// ---------------------------------------------------------------------------
// Variables internes
// ---------------------------------------------------------------------------
static uint16_t oldLocAddress = 0;
static uint16_t oldLocSpeed = 0;
static uint8_t comptCmdLoco = 0;

// ---------------------------------------------------------------------------
// envoyerCommandeDCC()
// ---------------------------------------------------------------------------
void envoyerCommandeDCC(Canton *canton)
{
    Loco *loco = canton->getLoco();
    if (!loco)
    {
        CC_LOG_WARN("[CommandeDCC][CC] Aucun objet Loco associé au canton\n");
        return;
    }

    if (loco->address() == 0)
        return;

    if (loco->speed() != oldLocSpeed)
        comptCmdLoco = 0;

    if (comptCmdLoco < 5)
    {
        CanMsg msg;

        uint32_t id =
            (0 << 26) |            // priorité
            (0x04 << 18) |         // opcode DCC
            (canton->ID());        // nodeId

        msg.id  = id;
        msg.dlc = 6;

        msg.data[0] = 0x00;
        msg.data[1] = 0x00;
        msg.data[2] = (loco->address() >> 8) & 0xFF;
        msg.data[3] = loco->address() & 0xFF;
        msg.data[4] = (loco->speed() >> 8) & 0xFF;
        msg.data[5] = loco->speed();

        CanBus::bus(0).send(msg);

        CC_LOG_INFO("[CommandeDCC][CC] Loco %u vitesse %u (envoi %u/5)\n",
                    loco->address(),
                    loco->speed(),
                    comptCmdLoco + 1);

        oldLocAddress = loco->address();
        oldLocSpeed   = loco->speed();
        comptCmdLoco++;
    }
}
