#include "FeuxDirection_CodeBarre.h"

namespace FeuxDirection
{
    /* =========================================================================
     *  Détection du type de faisceau via le suffixe (2 derniers bits)
     * -------------------------------------------------------------------------
     *  01 → V2
     *  10 → V3
     *  11 → V4
     * ========================================================================= */
    static FaisceauType detectTypeFromSuffix(const std::string &code) // 🟢
    {
        if (code.size() < 2)
            return FaisceauType::Invalide;

        const char b1 = code[code.size() - 2];
        const char b0 = code[code.size() - 1];

        if (b1 == '0' && b0 == '1') return FaisceauType::V2;
        if (b1 == '1' && b0 == '0') return FaisceauType::V3;
        if (b1 == '1' && b0 == '1') return FaisceauType::V4;

        return FaisceauType::Invalide;
    }

    /* =========================================================================
     *  decode() — Décodage complet du code‑barres directionnel
     * -------------------------------------------------------------------------
     *  Étapes :
     *    1. Vérifier que la chaîne est binaire
     *    2. Détecter le type (V2/V3/V4)
     *    3. Vérifier la longueur attendue
     *    4. Découper les blocs voie (5 bits)
     *    5. Extraire P0/P1/P2/T1/T2 selon le type
     *    6. Remplir CodeBarreDecoded
     *
     *  En cas d’erreur :
     *    - result.valide = false
     *    - result.erreur contient un message explicite
     * ========================================================================= */
    CodeBarreDecoded CodeBarre::decode(const std::string &code) // 🟢
    {
        CodeBarreDecoded result;

        // ---------------------------------------------------------------------
        // 1) Vérification binaire
        // ---------------------------------------------------------------------
        for (char c : code)
        {
            if (c != '0' && c != '1')
            {
                result.erreur = "Le code-barres contient des caractères non binaires";
                return result;
            }
        }

        // ---------------------------------------------------------------------
        // 2) Détection du type via suffixe
        // ---------------------------------------------------------------------
        result.type = detectTypeFromSuffix(code);
        if (result.type == FaisceauType::Invalide)
        {
            result.erreur = "Suffixe du code-barres invalide (attendu : 01, 10 ou 11)";
            return result;
        }

        // ---------------------------------------------------------------------
        // 3) Vérification de la longueur attendue
        // ---------------------------------------------------------------------
        size_t expectedLength = 0;

        switch (result.type)
        {
            case FaisceauType::V2: result.nbVoies = 2; expectedLength = 12; break;
            case FaisceauType::V3: result.nbVoies = 3; expectedLength = 17; break;
            case FaisceauType::V4: result.nbVoies = 4; expectedLength = 22; break;

            default:
                result.erreur = "Type de faisceau inconnu";
                return result;
        }

        if (code.size() != expectedLength)
        {
            result.erreur = "Longueur du code-barres incorrecte pour ce type de faisceau";
            return result;
        }

        // ---------------------------------------------------------------------
        // 4) Découpage des blocs voie (5 bits)
        // ---------------------------------------------------------------------
        const size_t suffixLen = 2;
        size_t offset = expectedLength - suffixLen;

        for (uint8_t voieIdx = 0; voieIdx < result.nbVoies; ++voieIdx)
        {
            if (offset < 5)
            {
                result.erreur = "Découpage des blocs voie impossible (offset invalide)";
                return result;
            }

            size_t start = offset - 5;
            std::string bloc = code.substr(start, 5);
            offset -= 5;

            VoieCodeBarre vb;

            // -----------------------------------------------------------------
            // 5) Extraction des bits selon le type
            // -----------------------------------------------------------------
            switch (result.type)
            {
                case FaisceauType::V2:
                    // Format : 000 P0 0 0
                    // Index :  0 1 2 3 4
                    vb.P0 = (bloc[2] == '1');
                    vb.P1 = vb.P2 = vb.T1 = vb.T2 = 0;
                    vb.active = true;
                    break;

                case FaisceauType::V3:
                    // Format : 0 P1 P0 0 T1
                    // Index :  0 1 2 3 4
                    vb.P1 = (bloc[1] == '1');
                    vb.P0 = (bloc[2] == '1');
                    vb.P2 = 0;
                    vb.T1 = (bloc[4] == '1');
                    vb.T2 = 0;
                    vb.active = true;
                    break;

                case FaisceauType::V4:
                    // Format : P2 P1 P0 T2 T1
                    // Index :  0 1 2 3 4
                    vb.P2 = (bloc[0] == '1');
                    vb.P1 = (bloc[1] == '1');
                    vb.P0 = (bloc[2] == '1');
                    vb.T2 = (bloc[3] == '1');
                    vb.T1 = (bloc[4] == '1');
                    vb.active = true;
                    break;

                default:
                    result.erreur = "Type de faisceau inconnu lors du parsing";
                    return result;
            }

            // On remplit dans l’ordre voie1..voieN
            result.voies[voieIdx] = vb;
        }

        // ---------------------------------------------------------------------
        // 6) Succès
        // ---------------------------------------------------------------------
        result.valide = true;
        return result;
    }
}
