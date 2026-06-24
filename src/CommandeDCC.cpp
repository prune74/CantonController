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

// ---------------------------------------------------------------------------
// Variables internes
// ---------------------------------------------------------------------------
static uint16_t oldLocAddress = 0;
static uint16_t oldLocSpeed = 0;
static uint8_t comptCmdLoco = 0;

// ---------------------------------------------------------------------------
// envoyerCommandeDCC()
// ---------------------------------------------------------------------------
void envoyerCommandeDCC(Canton *canton) // 🟢
{
    Loco *loco = canton->getLoco();
    if (!loco)
    {
        CC_LOG_WARN("[CommandeDCC][CC] Aucun objet Loco associé au canton\n");
        return;
    }

    if (loco->address() == 0)
        return; // aucune loco → aucune commande

    // Si la vitesse change → reset du compteur d’envoi
    if (loco->speed() != oldLocSpeed)
        comptCmdLoco = 0;

    // On envoie la commande 5 fois pour fiabiliser le DCC
    if (comptCmdLoco < 5)
    {
        CC_CAN::sendMsg(
            0,            // priorité
            0x04,         // opcode DCC
            0,            // unused
            canton->ID(), // ID du canton
            0x00,
            0x00,
            (loco->address() >> 8) & 0xFF,
            loco->address() & 0xFF,
            (loco->speed() >> 8) & 0xFF,
            loco->speed());

        CC_LOG_INFO("[CommandeDCC][CC] Loco %u vitesse %u (envoi %u/5)\n",
                    loco->address(),
                    loco->speed(),
                    comptCmdLoco + 1);

        oldLocAddress = loco->address();
        oldLocSpeed = loco->speed();
        comptCmdLoco++;
    }
}
