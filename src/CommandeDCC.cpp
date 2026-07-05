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
 *   → il transporte uniquement les commandes DCC++ vers la carte LaBox.
 */

#include "CommandeDCC.h"
#include "debug_cc.h"
#include "CC_CAN.h"

#include "CanBus.h"
#include "CanMsg.h"
#include "protocol.h"        // ⭐ Ajout indispensable

// ---------------------------------------------------------------------------
// Variables internes
// ---------------------------------------------------------------------------
static uint16_t oldLocAddress = 0;
static uint16_t oldLocSpeed   = 0;
static uint8_t  comptCmdLoco  = 0;

// ---------------------------------------------------------------------------
// envoyerCommandeDCC()
// ---------------------------------------------------------------------------
void envoyerCommandeDCC(Canton *canton)
{
    Loco *loco = canton->getLoco();
    if (!loco || loco->address() == 0)
        return;

    if (loco->speed() != oldLocSpeed)
        comptCmdLoco = 0;

    if (comptCmdLoco < 5)
    {
        CC_CAN::sendMsg(
            0,                                           // priorité
            static_cast<uint8_t>(Cmd_CAN_LaBox::LOCO_SPEED), // opcode Marklin
            0,                                           // pas une réponse
            canton->ID(),                                // expéditeur
            0x00, 0x00,                                   // Marklin header
            (loco->address() >> 8) & 0xFF,
            loco->address() & 0xFF,
            (loco->speed() >> 8) & 0xFF,
            loco->speed() & 0xFF
        );

        CC_LOG_INFO("[CommandeDCC][CC] Loco %u vitesse %u (envoi %u/5)\n",
                    loco->address(),
                    loco->speed(),
                    comptCmdLoco + 1);

        oldLocAddress = loco->address();
        oldLocSpeed   = loco->speed();
        comptCmdLoco++;
    }
}
