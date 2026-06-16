#include "CapteursEtat.h"

/*************************************************************************************
 * Occupation du canton
 * et état des capteurs
 ************************************************************************************/
void mettreAJourCapteurs(Canton *canton)
{
    // Si le canton n'est pas occupé → reset complet
    if (!canton->busy())
    {
        // Capteurs ponctuels remis à zéro
        // Sensor[1] = H
        // Sensor[0] = AH
        canton->getSensor(1)->overrideState(false); // capteur H
        canton->getSensor(0)->overrideState(false); // capteur AH

        // Reset de la loco
        canton->getLoco()->speed(0);
        canton->getLoco()->sens(SensHoraire); // valeur neutre par défaut
        canton->getLoco()->address(0);
    }
    else
    {
        // Si la loco dépasse la vitesse max du canton → clamp
        if (canton->getLoco()->speed() > canton->maxSpeed())
        {
            canton->getLoco()->speed(canton->maxSpeed());
        }
    }
}
