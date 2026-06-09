#pragma once
/**
 * ------------------------------------------------------------
 * FeuxDirection_Types.h
 * ------------------------------------------------------------
 * Ce fichier définit TOUS les types de base utilisés par le
 * module FeuxDirection.
 *
 * Il ne contient :
 *   - aucune logique métier
 *   - aucune dépendance au Canton, Settings, CAN, etc.
 *
 * Il sert de fondation commune à :
 *   - FeuxDirection_CodeBarre.h/.cpp     (décodage du code-barres)
 *   - FeuxDirection_Conditions.h/.cpp    (vérification des aiguilles physiques)
 *   - FeuxDirection.h/.cpp               (API publique pour le SA)
 *
 * ------------------------------------------------------------
 * NOTE IMPORTANTE :
 * ------------------------------------------------------------
 * Les structures définies ici décrivent UNIQUEMENT la géométrie
 * du faisceau telle qu'encodée dans le code-barres :
 *
 *   - ordre des voies
 *   - positions logiques des aiguilles (P0, P1, P2)
 *   - topologie des aiguilles (T1, T2)
 *   - type du faisceau (V2, V3, V4)
 *
 * Elles NE décrivent PAS :
 *   - les voisins
 *   - les feux
 *   - la sécurité
 *   - les aiguilles physiques
 *
 * Ces éléments sont traités dans d’autres modules :
 *   - Conditions : comparaison géométrie ↔ aiguilles physiques
 *   - FeuxDirection : décision du feu (voieActive)
 *   - GestionReseau : intégration dans le SA
 *
 * Cette séparation stricte est essentielle pour éviter toute
 * ambiguïté et garantir un comportement déterministe.
 * ------------------------------------------------------------
 */

#include <stdint.h>
#include <array>
#include <string>

namespace FeuxDirection
{
    // --------------------------------------------------------
    // Constantes générales
    // --------------------------------------------------------

    /**
     * Nombre maximal de voies dans un faisceau directionnel.
     *
     * Exploration supporte :
     *   - V2 → 2 voies
     *   - V3 → 3 voies
     *   - V4 → 4 voies
     *
     * On fixe à 4 pour simplifier les tableaux internes.
     */
    constexpr uint8_t MAX_VOIES = 4;

    // --------------------------------------------------------
    // Type de faisceau (suffixe du code-barres)
    // --------------------------------------------------------

    /**
     * Le suffixe du code-barres (2 bits) indique le type :
     *
     *   01 → V2 (2 voies, 1 aiguille)
     *   10 → V3 (3 voies, 2 aiguilles)
     *   11 → V4 (4 voies, 3 aiguilles)
     *
     * Si le suffixe est invalide → Invalide.
     */
    enum class FaisceauType : uint8_t
    {
        Invalide = 0,
        V2,
        V3,
        V4
    };

    // --------------------------------------------------------
    // Description d’une voie (bloc de 5 bits)
    // --------------------------------------------------------

    /**
     * VoieCodeBarre
     * --------------------------------------------------------
     * Représente un bloc de 5 bits extrait du code-barres.
     *
     * Format selon le type :
     *
     *   V2 : 000 P0 0 0
     *   V3 : 0 P1 P0 0 T1
     *   V4 : P2 P1 P0 T2 T1
     *
     * Signification des bits :
     *   - P0 : position attendue de l’aiguille 0
     *   - P1 : position attendue de l’aiguille 1
     *   - P2 : position attendue de l’aiguille 2
     *   - T1 : topologie aig1 (indique si aig1 est impliquée)
     *   - T2 : topologie aig2 (indique si aig2 est impliquée)
     *
     * IMPORTANT :
     *   Ce struct décrit UNIQUEMENT la géométrie du faisceau.
     *   Il ne dit rien sur :
     *     - les voisins
     *     - les feux
     *     - la sécurité
     *     - les aiguilles physiques
     *
     * Ces éléments seront traités plus tard dans :
     *   - FeuxDirection_Conditions
     *   - FeuxDirection
     *   - GestionReseau
     */
    struct VoieCodeBarre
    {
        uint8_t P0{0};
        uint8_t P1{0};
        uint8_t P2{0};
        uint8_t T1{0};
        uint8_t T2{0};

        bool active{false}; ///< true si cette voie existe réellement dans ce faisceau
    };

    // --------------------------------------------------------
    // Résultat complet du décodage du code-barres
    // --------------------------------------------------------

    /**
     * CodeBarreDecoded
     * --------------------------------------------------------
     * Contient :
     *   - le type du faisceau (V2/V3/V4)
     *   - le nombre de voies
     *   - les blocs voie décodés
     *   - un indicateur de validité
     *   - un message d’erreur éventuel
     *
     * Ce type est utilisé par :
     *   - FeuxDirection_CodeBarre::decode()
     *   - FeuxDirection_Conditions::voieOuverte()
     *   - FeuxDirection::compute()
     */
    struct CodeBarreDecoded
    {
        FaisceauType type{FaisceauType::Invalide};
        uint8_t nbVoies{0};
        std::array<VoieCodeBarre, MAX_VOIES> voies{};

        bool valide{false};
        std::string erreur;
    };

    // --------------------------------------------------------
    // Résultat final pour le SA : état du feu directionnel
    // --------------------------------------------------------

    /**
     * DirectionState
     * --------------------------------------------------------
     * C’est le type de retour principal du module FeuxDirection.
     *
     * Il répond à la question :
     *   → "Quel feu dois-je allumer ?"
     *
     * Règles :
     *   - voieActive = 0 :
     *       aucun feu (canton vide, aiguilles mal orientées, etc.)
     *
     *   - voieActive = N (1..nbVoies) :
     *       allumer la LED correspondant à la voie N
     *
     * Le champ "ok" indique si l’état est cohérent :
     *   - ok = true :
     *       même si voieActive = 0, l’état est normal
     *
     *   - ok = false :
     *       erreur de configuration ou code-barres invalide
     */
    struct DirectionState
    {
        uint8_t voieActive{0};     ///< 0 = aucun feu, sinon 1..nbVoies
        CodeBarreDecoded geometry; ///< Code-barres décodé (debug)
        bool ok{false};            ///< true = état cohérent
        std::string erreur;        ///< message si ok=false
    };
}
/* ------------------------------------------------------------
  Fin de FeuxDirection_Types.h
  ------------------------------------------------------------
*/