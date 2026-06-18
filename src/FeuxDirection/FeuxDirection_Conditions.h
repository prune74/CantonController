#pragma once
/**
 * ============================================================================
 *  FeuxDirection_Conditions.h — Gestion Canton 2026
 * ============================================================================
 * Vérification de l’alignement PHYSIQUE des aiguilles par rapport à la
 * géométrie décrite dans le code‑barres directionnel.
 *
 * Rôle de ce module :
 *   - comparer la géométrie logique (P0, P1, P2, T1, T2)
 *     avec les positions réelles des aiguilles (0 = normal, 1 = dévié)
 *   - déterminer si la voie demandée est physiquement ouverte
 *
 * IMPORTANT :
 *   - ne décode PAS le code‑barres
 *   - ne décide PAS quel feu allumer
 *   - ne connaît PAS les voisins
 *   - ne contient AUCUNE logique métier
 *
 * Ce module fait uniquement :
 *      géométrie (code‑barres)  ↔  aiguilles physiques (Canton/EXCC)
 *
 * La décision finale du feu est effectuée dans FeuxDirection.cpp.
 * ============================================================================
 */

#include "FeuxDirection_Types.h"

namespace FeuxDirection
{
    /* =========================================================================
     *  Interface IAiguillesPhysiques
     * -------------------------------------------------------------------------
     *  Abstraction minimale permettant d’interroger les positions réelles des
     *  aiguilles, indépendamment du matériel ou du canton.
     *
     *  Le SA (GestionRéseau) fournit une implémentation concrète.
     *
     *  Exemple :
     *      class AigFromCanton : public IAiguillesPhysiques {
     *          uint8_t getPositionAig(uint8_t idx) const override {
     *              return canton->getAigPosition(idx);
     *          }
     *      };
     *
     *  Cette abstraction permet de tester le module FeuxDirection sans dépendre
     *  du matériel ou du firmware du canton.
     * ========================================================================= */
    struct IAiguillesPhysiques
    {
        virtual ~IAiguillesPhysiques() = default;

        /**
         * Retourne la position physique d’une aiguille :
         *   - 0 : position normale
         *   - 1 : position déviée
         *
         * indexAig :
         *   - 0 → aig0
         *   - 1 → aig1
         *   - 2 → aig2
         */
        virtual uint8_t getPositionAig(uint8_t indexAig) const = 0;
    };

    /* =========================================================================
     *  Classe Conditions
     * -------------------------------------------------------------------------
     *  Vérifie si une voie est physiquement ouverte en comparant :
     *     - la géométrie du code‑barres (P0/P1/P2/T1/T2)
     *     - les positions physiques des aiguilles
     *
     *  Cette classe ne contient AUCUNE autre logique.
     * ========================================================================= */
    class Conditions
    {
    public:
        /**
         * --------------------------------------------------------------------
         * voieOuverte()
         * --------------------------------------------------------------------
         * Détermine si la voie demandée est réellement ouverte.
         *
         * Paramètres :
         *   - cb        : géométrie décodée (CodeBarreDecoded)
         *   - voie      : numéro de voie (1..nbVoies)
         *   - aiguilles : accès aux positions physiques
         *
         * Retour :
         *   - true  → toutes les aiguilles impliquées sont alignées
         *   - false → au moins une aiguille n’est pas dans la position attendue
         *
         * Cette fonction NE décide PAS quel feu allumer.
         * Elle répond uniquement :
         *      « Le chemin physique vers la voie N est‑il ouvert ? »
         * --------------------------------------------------------------------
         */
        static bool voieOuverte(const CodeBarreDecoded &cb,
                                uint8_t voie,
                                const IAiguillesPhysiques &aiguilles);
    };
}

/* ============================================================================
 *  Fin de FeuxDirection_Conditions.h
 * ============================================================================
 */
