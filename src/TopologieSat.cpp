/*
 * TopologieSat.cpp — Gestion Canton 2026
 * ------------------------------------------------------------
 * Mise à jour de la topologie des satellites voisins pour le
 * Canton Controller (CC).
 *
 * Rôle :
 *   - déterminer SP1 et SM1 en fonction des aiguilles locales
 *   - utiliser uniquement les objets Aig (0–5)
 *   - fournir une topologie cohérente à l’EXCC
 *
 * IMPORTANT 2026 :
 *   - aucun masque d’aiguilles n’est utilisé ici
 *   - la topologie dépend EXCLUSIVEMENT des aiguilles locales
 *   - SP2 / SM2 sont gérés ailleurs (accès secondaires)
 */

#include "TopologieSat.h"
#include "debug_cc.h"
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
     *
     * Le calcul repose sur l’état des aiguilles locales :
     * - Aiguille principale (idx 0 ou 3)
     * - Aiguilles secondaires (idx 1–2 ou 4–5)
     *
     * Le résultat est un index virtuel 0–3 représentant le
     * satellite voisin dans la direction donnée.
     */
    auto rechercheSat = [canton](bool satPos) -> uint8_t
    {
        // Index des aiguilles selon le côté
        uint8_t idxA = satPos ? 3 : 0; // aiguille principale
        uint8_t idxS = satPos ? 4 : 0; // base topologique
        uint8_t idx = idxS;

        Aig *a0 = canton->getAig(idxA);

        // --------------------------------------------------------------------
        // Cas 0 : aucune aiguille → topologie triviale
        // --------------------------------------------------------------------
        if (!a0)
        {
            CC_LOG_WARN(
                "[Topo][CC] Aucun aiguillage côté %s → idx=%u (topologie simple)\n",
                satPos ? "Horaire" : "AntiHoraire",
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

        CC_LOG_TRACE(
            "[Topo][CC] rechercheSat(%s) → idxA=%u idxS=%u → idx=%u\n",
            satPos ? "Horaire" : "AntiHoraire",
            idxA,
            idxS,
            idx);

        return idx;
    };

    // -----------------------------------------------------------------------
    // Calcul SP1 / SM1 pour le Canton Controller
    // -----------------------------------------------------------------------
    uint8_t sp1 = rechercheSat(true);   // côté horaire
    uint8_t sm1 = rechercheSat(false);  // côté anti‑horaire

    canton->SP1_idx(sp1);
    canton->SM1_idx(sm1);

    CC_LOG_INFO(
        "[Topo][CC][Canton %d] Topologie mise à jour : SP1_idx=%d  SM1_idx=%d\n",
        canton->ID(),
        sp1,
        sm1);
}
