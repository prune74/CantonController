
/**
 * ------------------------------------------------------------
 * FeuxDirection.h
 * ------------------------------------------------------------
 * API PUBLIQUE du module FeuxDirection.
 *
 * Ce fichier est le SEUL que GestionReseau doit inclure pour
 * utiliser les feux directionnels.
 *
 * Rôle :
 *   - recevoir :
 *       • le code-barres (string binaire)
 *       • la voie demandée (1..4)
 *       • l’état d’occupation du canton
 *       • l’accès aux aiguilles physiques
 *
 *   - renvoyer :
 *       • DirectionState (voieActive, ok, erreur, geometry)
 *
 * Ce module :
 *   - NE décode PAS le code-barres (délégué à FeuxDirection_CodeBarre)
 *   - NE lit PAS les aiguilles physiques (délégué à Conditions)
 *   - NE gère PAS les voisins
 *
 * Il orchestre simplement :
 *   1. Décodage du code-barres
 *   2. Vérification des aiguilles physiques
 *   3. Décision du feu (voieActive)
 *
 * ------------------------------------------------------------
 */

#pragma once

#include <string>
#include "FeuxDirection/FeuxDirection_Types.h"
#include "FeuxDirection/FeuxDirection_CodeBarre.h"
#include "FeuxDirection/FeuxDirection_Conditions.h"

namespace FeuxDirection
{
    class FeuxDirection
    {
    public:
        /**
         * --------------------------------------------------------
         * compute()
         * --------------------------------------------------------
         * Fonction principale appelée par GestionReseau.
         *
         * Paramètres :
         *   - codeBarre    : string binaire (ex : "01101001010000010")
         *   - voieDemandee : 1..4 (choix utilisateur / voisin)
         *   - cantonOccupe : true si le canton SA est occupé
         *   - aiguilles    : accès aux positions physiques
         *
         * Retour :
         *   DirectionState :
         *     - voieActive = 0 → aucun feu
         *     - voieActive = N → feu de la voie N
         *     - ok = false → erreur (code-barres invalide, etc.)
         *
         * Cette fonction est volontairement simple :
         *   - elle ne fait aucune logique cachée
         *   - elle ne modifie rien
         *   - elle ne dépend que de ses paramètres
         *
         * Elle est 100% déterministe.
         * --------------------------------------------------------
         */
        static DirectionState compute(const std::string& codeBarre,
                                      uint8_t voieDemandee,
                                      bool cantonOccupe,
                                      const IAiguillesPhysiques& aiguilles);
    };
}
