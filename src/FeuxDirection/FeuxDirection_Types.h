#pragma once
/**
 * ============================================================================
 *  FeuxDirection_Types.h — Gestion Canton 2026
 * ============================================================================
 * Définition de TOUS les types de base utilisés par le module FeuxDirection.
 *
 * Ce fichier constitue la fondation commune pour :
 *   - FeuxDirection_CodeBarre (décodage du code‑barres)
 *   - FeuxDirection_Conditions (vérification des aiguilles physiques)
 *   - FeuxDirection (API publique pour le CC)
 *
 * IMPORTANT :
 *   - aucune logique métier
 *   - aucune dépendance au Canton, Settings, CAN, UART, etc.
 *   - uniquement la représentation géométrique du faisceau
 *
 * Les structures définies ici décrivent :
 *   - le type du faisceau (V2/V3/V4)
 *   - le nombre de voies
 *   - les blocs voie (P0/P1/P2/T1/T2)
 *   - la géométrie complète décodée
 *
 * Elles NE décrivent PAS :
 *   - les voisins
 *   - les feux
 *   - la sécurité
 *   - les aiguilles physiques
 *
 * Ces éléments sont traités dans :
 *   - FeuxDirection_Conditions
 *   - FeuxDirection
 *   - GestionReseau
 * ============================================================================
 */

#include <stdint.h>
#include <array>
#include <string>

namespace FeuxDirection
{
    /* =========================================================================
     *  Constantes générales
     * ========================================================================= */
    constexpr uint8_t MAX_VOIES = 4; ///< V2=2, V3=3, V4=4

    /* =========================================================================
     *  Type de faisceau (suffixe du code‑barres)
     * -------------------------------------------------------------------------
     *  01 → V2 (2 voies, 1 aiguille)
     *  10 → V3 (3 voies, 2 aiguilles)
     *  11 → V4 (4 voies, 3 aiguilles)
     * ========================================================================= */
    enum class FaisceauType : uint8_t
    {
        Invalide = 0,
        V2,
        V3,
        V4
    };

    /* =========================================================================
     *  Description d’une voie (bloc de 5 bits)
     * -------------------------------------------------------------------------
     *  Format selon le type :
     *
     *    V2 : 000 P0 0 0
     *    V3 : 0 P1 P0 0 T1
     *    V4 : P2 P1 P0 T2 T1
     *
     *  Signification :
     *    - P0 : position attendue de l’aiguille 0
     *    - P1 : position attendue de l’aiguille 1
     *    - P2 : position attendue de l’aiguille 2
     *    - T1 : topologie aig1 (voie impliquée)
     *    - T2 : topologie aig2 (voie impliquée)
     *
     *  Ce struct décrit UNIQUEMENT la géométrie du faisceau.
     * ========================================================================= */
    struct VoieCodeBarre // 🟢
    {
        uint8_t P0{0};
        uint8_t P1{0};
        uint8_t P2{0};
        uint8_t T1{0};
        uint8_t T2{0};

        bool active{false}; ///< true si cette voie existe réellement
    };

    /* =========================================================================
     *  Résultat complet du décodage du code‑barres
     * -------------------------------------------------------------------------
     *  Utilisé par :
     *    - FeuxDirection_CodeBarre::decode()
     *    - FeuxDirection_Conditions::voieOuverte()
     *    - FeuxDirection::compute()
     * ========================================================================= */
    struct CodeBarreDecoded // 🟢
    {
        FaisceauType type{FaisceauType::Invalide};
        uint8_t nbVoies{0};
        std::array<VoieCodeBarre, MAX_VOIES> voies{};

        bool valide{false};
        std::string erreur;
    };

    /* =========================================================================
     *  Résultat final pour le CC : état du feu directionnel
     * -------------------------------------------------------------------------
     *  Répond à la question :
     *      « Quel feu dois‑je allumer ? »
     *
     *  Règles :
     *    - voieActive = 0 → aucun feu
     *    - voieActive = N → feu de la voie N
     *
     *  ok = false → erreur de configuration ou code‑barres invalide
     * ========================================================================= */
    struct DirectionState // 🟢
    {
        uint8_t voieActive{0};     ///< 0 = aucun feu, sinon 1..nbVoies
        CodeBarreDecoded geometry; ///< Code‑barres décodé (debug)
        bool ok{false};            ///< true = état cohérent
        std::string erreur;        ///< message si ok=false
    };
}

/* ============================================================================
 *  Fin de FeuxDirection_Types.h
 * ============================================================================
 */
