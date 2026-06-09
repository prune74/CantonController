/*
 * TopologieSat.cpp — Mise à jour de la topologie des satellites
 */

#include "TopologieSat.h"
#include "debug_sa.h"
#include "Canton.h"

/*************************************************************************************
 * Mise à jour de la topologie des satellites voisins
 *************************************************************************************/

void mettreAJourTopologie(Canton *canton)
{
    /*
     * rechercheSat(satPos)
     * satPos = true  → côté horaire  (SP1)
     * satPos = false → côté anti‑horaire (SM1)
     */
    auto rechercheSat = [canton](bool satPos) -> uint8_t
    {
        // Index des aiguilles selon le côté
        uint8_t idxA = satPos ? 3 : 0; // aiguille principale
        uint8_t idxS = satPos ? 4 : 0; // satellite de base (virtuel)
        uint8_t idx = idxS;

        Aig *a0 = canton->getAig(idxA);

        // --------------------------------------------------------------------
        // Cas 0 : aucune aiguille → topologie triviale
        // --------------------------------------------------------------------
        if (!a0)
        {
            SA_LOG_WARN(
                "[Topo] Aucun aiguillage côté %s → idx=%u (topologie simple)\n",
                satPos ? "H" : "AH",
                idxS);
            return idxS;
        }

        // --------------------------------------------------------------------
        // Cas 1 : aiguille principale → directe / déviée
        // --------------------------------------------------------------------
        if (a0->estDroit())
        {
            idx = idxS;

            Aig *a1 = canton->getAig(idxA + 1);
            if (a1)
                idx = a1->estDroit() ? idxS + 1 : idxS;
        }
        else
        {
            idx = idxS + 1;

            Aig *a2 = canton->getAig(idxA + 2);
            if (a2)
                idx = a2->estDroit() ? idxS + 2 : idxS + 3;
        }

        SA_LOG_TRACE(
            "[Topo] rechercheSat(%s) → idxA=%u idxS=%u → idx=%u (masqueAig=0x%02X)\n",
            satPos ? "H" : "AH",
            idxA,
            idxS,
            idx,
            canton->masqueAig());

        return idx;
    };

    // -----------------------------------------------------------------------
    // Calcul SP1 / SM1 avec les enums modernes
    // -----------------------------------------------------------------------
    uint8_t sp1 = rechercheSat(true);  // SensHoraire
    uint8_t sm1 = rechercheSat(false); // SensAntiHoraire

    canton->SP1_idx(sp1);
    canton->SM1_idx(sm1);

    SA_LOG_INFO(
        "[Canton %d] Topologie mise à jour : SP1_idx=%d  SM1_idx=%d  (masqueAig=0x%02X)\n",
        canton->ID(),
        sp1,
        sm1,
        canton->masqueAig());
}
