#pragma once
/**
 * ============================================================================
 *  FeuxDirection_CodeBarre.h — Gestion Canton 2026
 * ============================================================================
 * Déclaration du décodeur de code‑barres directionnel.
 *
 * Rôle du décodeur :
 *   - lire une chaîne binaire (ex : "01101001010000010")
 *   - vérifier qu’elle est bien binaire
 *   - identifier le type de faisceau (V2 / V3 / V4) via le suffixe
 *   - vérifier la longueur attendue selon le type
 *   - découper les blocs voie (5 bits chacun)
 *   - extraire P0, P1, P2, T1, T2 pour chaque voie
 *   - remplir un CodeBarreDecoded complet
 *
 * IMPORTANT :
 *   - aucune logique métier
 *   - aucune interaction avec les aiguilles physiques
 *   - aucune décision d’allumage des feux
 *   - aucune connaissance des voisins ou de la topologie
 *
 * Ce module fournit UNIQUEMENT une représentation géométrique
 * fiable et complète du faisceau directionnel.
 * ============================================================================
 */

#include <string>
#include "FeuxDirection_Types.h"

namespace FeuxDirection
{
    class CodeBarre
    {
    public:
        /**
         * --------------------------------------------------------------------
         * decode()
         * --------------------------------------------------------------------
         * Décode un code‑barres directionnel complet.
         *
         * Paramètre :
         *   - code : chaîne binaire (ex : "001000000001")
         *
         * Retour :
         *   - CodeBarreDecoded :
         *       .valide = true  → décodage réussi
         *       .valide = false → erreur (longueur, suffixe, caractère non binaire…)
         *       .erreur contient alors un message explicite
         *
         * Cette fonction est la SEULE porte d’entrée pour décoder un
         * code‑barres dans tout le module FeuxDirection.
         * --------------------------------------------------------------------
         */
        static CodeBarreDecoded decode(const std::string &code);
    };
}
