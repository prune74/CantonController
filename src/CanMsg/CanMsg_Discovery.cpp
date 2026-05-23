/*
  CanMsg_Discovery.cpp — Commandes Discovery (0xC0–0xC1)
  --------------------------------------------------------------------------
  Ces commandes sont utilisées UNIQUEMENT pendant le mode Discovery.
*/

#include "CanMsg.h"
#include "debug_sa.h"   // cohérence Discovery 2026

void handleDiscoveryCommand(uint8_t commande, const CANMessage &frameIn,
                            Node *node, uint16_t idSatExpediteur)
{
    switch (commande)
    {
        case 0xC0:
            /**************************************************************************
             * 0xC0 — Réception de l’ID d’un satellite voisin
             **************************************************************************/
            Discovery::ID_satPeriph(idSatExpediteur);
            break;

        case 0xC1:
            /**************************************************************************
             * 0xC1 — Masque aiguilles pendant Discovery
             **************************************************************************/
            for (uint8_t i = 0; i < nodePsize; i++)
            {
                NodePeriph* el = node->getNodeP(i);
                if (el != nullptr && el->ID() == idSatExpediteur)
                {
                    el->masqueAig(frameIn.data[0]);
                }
            }
            break;

        default:
            break;
    }
}
