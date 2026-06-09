/*
  SA_CanMsg_Exploration.cpp — Commandes Exploration (0xC0–0xC1)
  --------------------------------------------------------------------------
  Ces commandes sont utilisées UNIQUEMENT pendant le mode Exploration.
*/

#include "CanMsg.h"
#include "debug_sa.h" // cohérence Exploration 2026

void handleExplorationCommand(uint8_t commande, const CANMessage &frameIn,
                              Canton *canton, uint16_t idSatExpediteur)
{
    switch (commande)
    {
    case 0xC0:
        /**************************************************************************
         * 0xC0 — Réception de l’ID d’un satellite voisin
         **************************************************************************/
        Exploration::ID_satPeriph(idSatExpediteur);
        break;

    case 0xC1:
        /**************************************************************************
         * 0xC1 — Masque aiguilles pendant Exploration
         **************************************************************************/
        for (uint8_t i = 0; i < cantonPsize; i++)
        {
            CantonPeriph *el = canton->getCantonP(i);
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
