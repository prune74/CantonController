#include "FeuxDirection_Conditions.h"

namespace FeuxDirection
{
    /**
     * ------------------------------------------------------------
     * voieOuverte()
     * ------------------------------------------------------------
     * Détermine si la voie demandée est physiquement ouverte.
     *
     * Rappel :
     *   - Le code-barres décrit la géométrie logique :
     *       P0, P1, P2 = positions attendues des aiguilles
     *       T1, T2     = topologie (indique si aig1/aig2 sont impliquées)
     *
     *   - Les aiguilles physiques sont lues via IAiguillesPhysiques :
     *       getPositionAig(0) → aig0
     *       getPositionAig(1) → aig1
     *       getPositionAig(2) → aig2
     *
     * Cette fonction compare les deux :
     *   géométrie (code-barres)  ↔  aiguilles physiques
     *
     * Elle NE décide PAS quel feu allumer.
     * Elle répond uniquement :
     *
     *   "Le chemin vers la voie N est-il ouvert ?"
     *
     * ------------------------------------------------------------
     */
    bool Conditions::voieOuverte(const CodeBarreDecoded& cb,
                                 uint8_t voie,
                                 const IAiguillesPhysiques& aiguilles)
    {
        // --------------------------------------------------------
        // 1) Vérifications de base
        // --------------------------------------------------------
        if (!cb.valide)
            return false;

        if (voie == 0 || voie > cb.nbVoies)
            return false;

        // On récupère la voie (index 0-based)
        const VoieCodeBarre& v = cb.voies[voie - 1];

        // --------------------------------------------------------
        // 2) Vérification aiguille par aiguille
        //
        // Règle générale :
        //   - Si une aiguille est impliquée dans la voie (selon T1/T2),
        //     alors sa position physique doit correspondre à P0/P1/P2.
        //
        //   - Si une aiguille n’est pas impliquée (T1/T2 = 0),
        //     alors on ne vérifie pas sa position.
        //
        //   - Pour V2 : seule aig0 existe.
        //   - Pour V3 : aig0 et aig1 existent.
        //   - Pour V4 : aig0, aig1, aig2 existent.
        // --------------------------------------------------------

        // --- AIGUILLE 0 (aig0) ---
        // Toujours utilisée dans V2, V3, V4
        {
            uint8_t posPhys = aiguilles.getPositionAig(0);
            if (posPhys != v.P0)
                return false;
        }

        // --- AIGUILLE 1 (aig1) ---
        if (cb.type == FaisceauType::V3 || cb.type == FaisceauType::V4)
        {
            // T1 = 1 → aig1 est impliquée dans cette voie
            if (v.T1)
            {
                uint8_t posPhys = aiguilles.getPositionAig(1);
                if (posPhys != v.P1)
                    return false;
            }
        }

        // --- AIGUILLE 2 (aig2) ---
        if (cb.type == FaisceauType::V4)
        {
            // T2 = 1 → aig2 est impliquée dans cette voie
            if (v.T2)
            {
                uint8_t posPhys = aiguilles.getPositionAig(2);
                if (posPhys != v.P2)
                    return false;
            }
        }

        // --------------------------------------------------------
        // 3) Toutes les aiguilles impliquées sont alignées
        // --------------------------------------------------------
        return true;
    }
}
