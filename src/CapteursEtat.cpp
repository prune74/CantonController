/*
 * CapteursEtat.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Mise à jour des capteurs ponctuels et de l’état de la locomotive
 * en fonction de l’occupation du canton.
 *
 * Règles :
 *   - si le canton est libre :
 *        • reset des capteurs ponctuels (H / AH)
 *        • reset complet de la locomotive (vitesse, sens, adresse)
 *
 *   - si le canton est occupé :
 *        • clamp de la vitesse si > maxSpeed()
 *
 * Ce module ne contient aucune logique ferroviaire globale :
 *   → il applique uniquement les règles locales du canton.
 */

#include "CapteursEtat.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Mise à jour des capteurs et de la loco
// ---------------------------------------------------------------------------
void mettreAJourCapteurs(Canton *canton)
{
    if (!canton)
    {
        CC_LOG_ERROR("[CapteursEtat][CC] canton nul\n");
        return;
    }

    // -----------------------------------------------------------------------
    // Cas 1 : canton libre → reset complet
    // -----------------------------------------------------------------------
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

        CC_LOG_TRACE("[CapteursEtat][CC] Reset capteurs + loco (canton libre)\n");
        return;
    }

    // -----------------------------------------------------------------------
    // Cas 2 : canton occupé → clamp vitesse
    // -----------------------------------------------------------------------
    if (canton->getLoco()->speed() > canton->maxSpeed())
    {
        canton->getLoco()->speed(canton->maxSpeed());
        CC_LOG_TRACE("[CapteursEtat][CC] Clamp vitesse à %u\n", canton->maxSpeed());
    }
}
