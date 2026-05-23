#include "FeuxDirection.h"

namespace FeuxDirection
{
    /**
     * ------------------------------------------------------------
     * compute()
     * ------------------------------------------------------------
     * Fonction principale appelée par GestionReseau.
     *
     * Elle orchestre :
     *   1. Décodage du code-barres
     *   2. Vérification des aiguilles physiques
     *   3. Décision du feu (voieActive)
     *
     * Elle ne fait AUCUNE logique cachée :
     *   - pas de voisin
     *   - pas de SP/SM
     *   - pas d’interprétation d’image
     *   - pas de sécurité globale
     *
     * Elle est 100% déterministe.
     * ------------------------------------------------------------
     */
    DirectionState FeuxDirection::compute(const std::string& codeBarre,
                                          uint8_t voieDemandee,
                                          bool cantonOccupe,
                                          const IAiguillesPhysiques& aiguilles)
    {
        DirectionState st;

        // --------------------------------------------------------
        // 1) Si le canton n’est pas occupé → aucun feu
        // --------------------------------------------------------
        if (!cantonOccupe)
        {
            st.voieActive = 0;
            st.ok = true;  // état cohérent : pas de train → pas de feu
            return st;
        }

        // --------------------------------------------------------
        // 2) Décodage du code-barres
        // --------------------------------------------------------
        st.geometry = CodeBarre::decode(codeBarre);

        if (!st.geometry.valide)
        {
            st.voieActive = 0;
            st.ok = false;
            st.erreur = "Code-barres invalide : " + st.geometry.erreur;
            return st;
        }

        // --------------------------------------------------------
        // 3) Vérification de la voie demandée
        // --------------------------------------------------------
        if (voieDemandee == 0 || voieDemandee > st.geometry.nbVoies)
        {
            st.voieActive = 0;
            st.ok = false;
            st.erreur = "Voie demandée hors limites pour ce faisceau";
            return st;
        }

        // --------------------------------------------------------
        // 4) Vérification des aiguilles physiques
        // --------------------------------------------------------
        bool ouverte = Conditions::voieOuverte(st.geometry,
                                               voieDemandee,
                                               aiguilles);

        if (!ouverte)
        {
            // Chemin non ouvert → pas de feu
            st.voieActive = 0;
            st.ok = true;  // état cohérent : aiguilles mal orientées
            return st;
        }

        // --------------------------------------------------------
        // 5) Tout est OK → on allume la voie demandée
        // --------------------------------------------------------
        st.voieActive = voieDemandee;
        st.ok = true;
        return st;
    }
}
