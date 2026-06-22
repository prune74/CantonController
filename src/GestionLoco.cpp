/*
 * GestionLoco.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Mise à jour de l’état de la locomotive en fonction de l’occupation du canton.
 *
 * Règles :
 *   - si le canton est libre :
 *        • reset complet de la locomotive (vitesse, sens, adresse)
 *
 *   - si le canton est occupé :
 *        • clamp de la vitesse si > maxSpeed()
 *
 * IMPORTANT :
 *   - Les capteurs ponctuels ne sont plus remis à zéro ici.
 *   - Leur état est géré par EXCC + CapteurPonctuel.
 */

#include "GestionLoco.h"
#include "debug_cc.h"

void gestionLoco_update(Canton *canton)
{
    if (!canton)
    {
        CC_LOG_ERROR("[GestionLoco][CC] canton nul\n");
        return;
    }

    Loco *loco = canton->getLoco();
    if (!loco)
        return;

    // -----------------------------------------------------------------------
    // Cas 1 : canton libre → reset complet de la loco
    // -----------------------------------------------------------------------
    if (!canton->busy())
    {
        loco->speed(0);
        loco->sens(SensHoraire); // valeur neutre par défaut
        loco->address(0);

        CC_LOG_TRACE("[GestionLoco][CC] Reset loco (canton libre)\n");
        return;
    }

    // -----------------------------------------------------------------------
    // Cas 2 : canton occupé → clamp vitesse
    // -----------------------------------------------------------------------
    if (loco->speed() > canton->maxSpeed())
    {
        loco->speed(canton->maxSpeed());
        CC_LOG_TRACE("[GestionLoco][CC] Clamp vitesse à %u\n", canton->maxSpeed());
    }
}
