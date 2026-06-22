/*
 * FeuxDirection.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Module de décision des feux directionnels (0..4) pour un faisceau donné.
 *
 * Rôle :
 *   - décoder le code‑barres décrivant la géométrie du faisceau
 *   - vérifier la cohérence de la voie demandée
 *   - vérifier l’ouverture réelle des aiguilles physiques
 *   - déterminer la voie active (ou 0 si aucun feu)
 *
 * Ce module est volontairement :
 *   - 100 % déterministe
 *   - sans logique de voisinage
 *   - sans SP/SM
 *   - sans sécurité globale
 *   - sans interprétation d’image
 *
 * Toute la logique métier externe (choix de voieDemandee, occupation,
 * topologie, sécurité) est gérée ailleurs.
 */

#include "FeuxDirection.h"

namespace FeuxDirection
{
    // -----------------------------------------------------------------------
    // compute() — Fonction principale appelée par GestionReseau
    // -----------------------------------------------------------------------
    DirectionState FeuxDirection::compute(const std::string &codeBarre, uint8_t voieDemandee, bool cantonOccupe, const IAiguillesPhysiques &aiguilles) // 🟢
    {
        DirectionState st;

        // -------------------------------------------------------------------
        // 1) Canton non occupé → aucun feu
        // -------------------------------------------------------------------
        if (!cantonOccupe)
        {
            st.voieActive = 0;
            st.ok = true;   // cohérent : pas de train → pas de feu
            return st;
        }

        // -------------------------------------------------------------------
        // 2) Décodage du code‑barres
        // -------------------------------------------------------------------
        st.geometry = CodeBarre::decode(codeBarre);

        if (!st.geometry.valide)
        {
            st.voieActive = 0;
            st.ok = false;
            st.erreur = "Code-barres invalide : " + st.geometry.erreur;
            return st;
        }

        // -------------------------------------------------------------------
        // 3) Vérification de la voie demandée
        // -------------------------------------------------------------------
        if (voieDemandee == 0 || voieDemandee > st.geometry.nbVoies)
        {
            st.voieActive = 0;
            st.ok = false;
            st.erreur = "Voie demandée hors limites pour ce faisceau";
            return st;
        }

        // -------------------------------------------------------------------
        // 4) Vérification des aiguilles physiques
        // -------------------------------------------------------------------
        bool ouverte = Conditions::voieOuverte(st.geometry,
                                               voieDemandee,
                                               aiguilles);

        if (!ouverte)
        {
            // Chemin non ouvert → pas de feu
            st.voieActive = 0;
            st.ok = true;   // cohérent : aiguilles mal orientées
            return st;
        }

        // -------------------------------------------------------------------
        // 5) Tout est OK → la voie demandée est active
        // -------------------------------------------------------------------
        st.voieActive = voieDemandee;
        st.ok = true;
        return st;
    }
}
