#include "FeuxDirection_Conditions.h"

namespace FeuxDirection
{
    /* =========================================================================
     *  voieOuverte()
     * -------------------------------------------------------------------------
     *  Détermine si la voie demandée est physiquement ouverte.
     *
     *  Rappel :
     *    - Le code‑barres décrit la géométrie logique :
     *         P0, P1, P2 = positions attendues des aiguilles
     *         T1, T2     = topologie (indique si aig1/aig2 sont impliquées)
     *
     *    - Les aiguilles physiques sont lues via IAiguillesPhysiques :
     *         getPositionAig(0) → aig0
     *         getPositionAig(1) → aig1
     *         getPositionAig(2) → aig2
     *
     *  Cette fonction compare :
     *       géométrie (code‑barres)  ↔  positions physiques
     *
     *  IMPORTANT :
     *    - aucune logique métier
     *    - ne décide PAS quel feu allumer
     *    - répond uniquement : « le chemin vers la voie N est‑il ouvert ? »
     * ========================================================================= */
    bool Conditions::voieOuverte(const CodeBarreDecoded &cb,
                                 uint8_t voie,
                                 const IAiguillesPhysiques &aiguilles)
    {
        // ---------------------------------------------------------------------
        // 1) Vérifications de base
        // ---------------------------------------------------------------------
        if (!cb.valide)
            return false;

        if (voie == 0 || voie > cb.nbVoies)
            return false;

        // Voie demandée (index 0‑based)
        const VoieCodeBarre &v = cb.voies[voie - 1];

        // ---------------------------------------------------------------------
        // 2) Vérification aiguille par aiguille
        //
        // Règle générale :
        //   - Si une aiguille est impliquée (T1/T2 = 1),
        //     sa position physique doit correspondre à P0/P1/P2.
        //
        //   - Si elle n’est pas impliquée, on ne vérifie pas.
        //
        //   - V2 : aig0 uniquement
        //   - V3 : aig0 + aig1
        //   - V4 : aig0 + aig1 + aig2
        // ---------------------------------------------------------------------

        // --- AIGUILLE 0 (toujours utilisée) ---
        {
            uint8_t posPhys = aiguilles.getPositionAig(0);
            if (posPhys != v.P0)
                return false;
        }

        // --- AIGUILLE 1 ---
        if (cb.type == FaisceauType::V3 || cb.type == FaisceauType::V4)
        {
            if (v.T1) // aig1 impliquée
            {
                uint8_t posPhys = aiguilles.getPositionAig(1);
                if (posPhys != v.P1)
                    return false;
            }
        }

        // --- AIGUILLE 2 ---
        if (cb.type == FaisceauType::V4)
        {
            if (v.T2) // aig2 impliquée
            {
                uint8_t posPhys = aiguilles.getPositionAig(2);
                if (posPhys != v.P2)
                    return false;
            }
        }

        // ---------------------------------------------------------------------
        // 3) Toutes les aiguilles impliquées sont alignées
        // ---------------------------------------------------------------------
        return true;
    }
}
