/*
 * FeuxDirection.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * API PUBLIQUE du module FeuxDirection.
 *
 * Ce fichier est le SEUL que GestionReseau doit inclure pour utiliser
 * les feux directionnels.
 *
 * Rôle :
 *   - recevoir :
 *       • le code‑barres (string binaire décrivant la géométrie)
 *       • la voie demandée (1..4)
 *       • l’état d’occupation du canton
 *       • l’accès aux aiguilles physiques (interface IAiguillesPhysiques)
 *
 *   - renvoyer :
 *       • DirectionState :
 *            - voieActive = 0 → aucun feu
 *            - voieActive = N → feu de la voie N
 *            - ok = false → erreur (code‑barres invalide, voie hors limites…)
 *            - geometry → structure décodée du code‑barres
 *
 * Ce module NE fait PAS :
 *   - de logique de voisinage
 *   - de SP/SM
 *   - de sécurité globale
 *   - d’interprétation d’image
 *
 * Il orchestre simplement :
 *   1. Décodage du code‑barres
 *   2. Vérification des aiguilles physiques
 *   3. Décision du feu (voieActive)
 *
 * Il est 100 % déterministe.
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
        /*
         * compute()
         * --------------------------------------------------------------------
         * Fonction principale appelée par GestionReseau.
         *
         * Paramètres :
         *   - codeBarre    : string binaire (ex : "01101001010000010")
         *   - voieDemandee : 1..4 (choix utilisateur ou logique cantonale)
         *   - cantonOccupe : true si un train est présent dans le canton
         *   - aiguilles    : accès aux positions physiques des aiguilles
         *
         * Retour :
         *   DirectionState (voieActive, ok, erreur, geometry)
         *
         * Cette fonction :
         *   - ne modifie rien
         *   - ne dépend que de ses paramètres
         *   - ne contient aucune logique cachée
         *   - est totalement déterministe
         */
        static DirectionState compute(const std::string &codeBarre,
                                      uint8_t voieDemandee,
                                      bool cantonOccupe,
                                      const IAiguillesPhysiques &aiguilles);
    };
}
