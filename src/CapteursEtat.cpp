#include "CapteursEtat.h"

/*************************************************************************************
 * Occupation du canton
 * et état des capteurs
 ************************************************************************************/
void mettreAJourCapteurs(Node* node)
{
    // Si le canton n'est pas occupé → reset complet
    if (!node->busy())
    {
        // Capteurs ponctuels remis à zéro
        // index_exsa = 0 → capteur H
        // index_exsa = 1 → capteur AH
        node->getSensor(0)->overrideState(false);   // capteur H
        node->getSensor(1)->overrideState(false);   // capteur AH

        // Reset de la loco
        node->getLoco()->speed(0);
        node->getLoco()->sens(SensHoraire);         // valeur neutre par défaut
        node->getLoco()->address(0);
    }
    else
    {
        // Si la loco dépasse la vitesse max du canton → clamp
        if (node->getLoco()->speed() > node->maxSpeed())
        {
            node->getLoco()->speed(node->maxSpeed());
        }
    }
}
