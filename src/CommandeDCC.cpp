/* 
CommandeDCC.cpp - Gestion des commandes DCC pour les locomotives 
*/

#include "CommandeDCC.h"
#include "debug_sa.h"   // système de log Discovery 2026
#include "CanMsg.h"     // pour CanMsg::sendMsg

static uint16_t oldLocAddress = 0;
static uint16_t oldLocSpeed   = 0;
static uint8_t  comptCmdLoco  = 0;

void envoyerCommandeDCC(Node* node)
{
    Loco* loco = node->getLoco();   // accès Discovery 2026

    if (loco->address() > 0)
    {
        // Si la vitesse change → reset du compteur d’envoi
        if (loco->speed() != oldLocSpeed)
            comptCmdLoco = 0;

        // On envoie la commande 5 fois pour fiabiliser le DCC
        if (comptCmdLoco < 5)
        {
            CanMsg::sendMsg(
                0,          // priorité
                0x04,       // commande DCC
                0,          // unused
                node->ID(), // ID du canton
                0x00,
                0x00,
                (loco->address() >> 8) & 0xFF,
                loco->address() & 0xFF,
                (loco->speed() >> 8) & 0xFF,
                loco->speed()
            );

            SA_LOG_INFO("[CommandeDCC] Loco %d vitesse %d\n",
                        loco->address(), loco->speed());

            oldLocAddress = loco->address();
            oldLocSpeed   = loco->speed();
            comptCmdLoco++;
        }
    }
}
