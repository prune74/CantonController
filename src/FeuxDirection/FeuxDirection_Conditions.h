#pragma once
/**
 * ------------------------------------------------------------
 * FeuxDirection_Conditions.h
 * ------------------------------------------------------------
 * Ce fichier déclare les outils permettant de vérifier si les
 * aiguilles PHYSIQUES sont alignées conformément à la géométrie
 * décrite dans le code-barres.
 *
 * Rôle de ce module :
 *   - comparer la géométrie (P0, P1, P2, T1, T2)
 *     avec les positions réelles des aiguilles (0 ou 1)
 *   - déterminer si la voie demandée est réellement ouverte
 *
 * IMPORTANT :
 *   - Ce module NE décode PAS le code-barres.
 *   - Ce module NE décide PAS quel feu allumer.
 *   - Ce module NE connaît PAS les voisins.
 *
 * Il fait uniquement :
 *   géométrie (code-barres)  ↔  aiguilles physiques (Node/EXSA)
 *
 * La décision finale du feu est faite dans FeuxDirection.cpp.
 * ------------------------------------------------------------
 */

#include "FeuxDirection_Types.h"

namespace FeuxDirection
{
    // --------------------------------------------------------
    // Interface d’accès aux aiguilles physiques
    // --------------------------------------------------------
    /**
     * IAiguillesPhysiques
     * --------------------------------------------------------
     * Interface minimale permettant au module FeuxDirection
     * d’interroger les positions réelles des aiguilles.
     *
     * Le SA (GestionReseau) fournira une implémentation concrète
     * basée sur Node/EXSA.
     *
     * Exemple d’implémentation :
     *   class AigFromNode : public IAiguillesPhysiques {
     *       uint8_t getPositionAig(uint8_t idx) const override {
     *           return node->getAigPosition(idx);
     *       }
     *   };
     *
     * Cette abstraction permet de tester le module indépendamment
     * du matériel.
     */
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


    // --------------------------------------------------------
    // Classe Conditions
    // --------------------------------------------------------
    /**
     * Conditions
     * --------------------------------------------------------
     * Contient la logique permettant de déterminer si une voie
     * est réellement ouverte, en comparant :
     *
     *   - la géométrie du code-barres (P0/P1/P2/T1/T2)
     *   - les positions physiques des aiguilles
     *
     * Cette classe ne fait AUCUNE autre logique.
     */
    class Conditions
    {
    public:
        /**
         * --------------------------------------------------------
         * voieOuverte()
         * --------------------------------------------------------
         * Détermine si la voie demandée est réellement ouverte.
         *
         * Paramètres :
         *   - cb        : géométrie décodée (CodeBarreDecoded)
         *   - voie      : numéro de voie (1..nbVoies)
         *   - aiguilles : accès aux positions physiques
         *
         * Retour :
         *   - true  → toutes les aiguilles sont alignées
         *   - false → au moins une aiguille n’est pas dans la
         *             position attendue pour cette voie
         *
         * Cette fonction NE décide PAS si un feu doit s’allumer.
         * Elle répond uniquement à la question :
         *
         *   "Le chemin physique vers la voie N est-il ouvert ?"
         * --------------------------------------------------------
         */
        static bool voieOuverte(const CodeBarreDecoded& cb,
                                uint8_t voie,
                                const IAiguillesPhysiques& aiguilles);
    };
}
/* ------------------------------------------------------------
  Fin de FeuxDirection_Conditions.h
  ------------------------------------------------------------
*/