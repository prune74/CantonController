#pragma once
/**
 * ------------------------------------------------------------
 * FeuxDirection_CodeBarre.h
 * ------------------------------------------------------------
 * Ce fichier déclare la classe responsable du DÉCODAGE du
 * code-barres directionnel.
 *
 * Rôle du décodeur :
 *   - lire une chaîne binaire (ex : "01101001010000010")
 *   - vérifier sa longueur
 *   - identifier le type de faisceau (V2 / V3 / V4)
 *   - découper les blocs voie (5 bits chacun)
 *   - extraire P0, P1, P2, T1, T2 pour chaque voie
 *   - remplir un CodeBarreDecoded
 *
 * IMPORTANT :
 *   - Ce module ne fait AUCUNE logique métier.
 *   - Il ne regarde PAS les aiguilles physiques.
 *   - Il ne décide PAS si un feu doit s’allumer.
 *   - Il ne connaît PAS les voisins.
 *
 * Il fournit uniquement une représentation géométrique
 * complète et fiable du faisceau.
 * ------------------------------------------------------------
 */

#include <string>
#include "FeuxDirection_Types.h"

namespace FeuxDirection
{
    class CodeBarre
    {
    public:
        /**
         * --------------------------------------------------------
         * decode()
         * --------------------------------------------------------
         * Décode un code-barres binaire complet.
         *
         * Paramètre :
         *   - code : string binaire (ex : "001000000001")
         *
         * Retour :
         *   - CodeBarreDecoded :
         *       .valide = true  → décodage OK
         *       .valide = false → erreur (longueur, suffixe, etc.)
         *
         * En cas d’erreur :
         *   - .erreur contient un message explicite
         *   - .valide = false
         *   - les autres champs ne doivent pas être utilisés
         *
         * Cette fonction est la SEULE porte d’entrée pour
         * décoder un code-barres dans tout le module.
         * --------------------------------------------------------
         */
        static CodeBarreDecoded decode(const std::string& code);
    };
}
