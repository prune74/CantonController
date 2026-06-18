/*
 * Canton_FeuxDirection.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des feux directionnels (LED blanches) du canton.
 *
 * Rôle :
 *   - récupérer la configuration directionnelle (H / AH)
 *   - déterminer le voisin réel (SP1/SP2 ou SM1/SM2)
 *   - identifier la voie demandée
 *   - lire l’occupation du canton
 *   - fournir l’accès aux aiguilles physiques
 *   - déléguer la décision finale à FeuxDirection::compute()
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune interprétation du code‑barres
 *   - aucune décision d’ouverture de voie
 *
 * Toute la logique directionnelle est dans :
 *   - FeuxDirection_CodeBarre
 *   - FeuxDirection_Conditions
 *   - FeuxDirection::compute()
 */

#include "Canton.h"
#include "debug_cc.h"

#include "FeuxDirection/FeuxDirection_Types.h"
#include "FeuxDirection/FeuxDirection_CodeBarre.h"
#include "FeuxDirection/FeuxDirection_Conditions.h"
#include "FeuxDirection.h"

/* ============================================================================
 *  Implémentation IAiguillesPhysiques pour un Canton
 * ==========================================================================*/
class AiguillesPhysiquesFromCanton : public FeuxDirection::IAiguillesPhysiques
{
public:
    explicit AiguillesPhysiquesFromCanton(const Canton *c) : m_canton(c) {}

    uint8_t getPositionAig(uint8_t indexAig) const override
    {
        // Canton fournit la position logique (0=droit, 1=devie)
        return m_canton->getAiguillePosition(indexAig);
    }

private:
    const Canton *m_canton;
};

/* ============================================================================
 *  updateFeuDirection() — Mise à jour du feu directionnel pour un sens donné
 * ==========================================================================*/
void Canton::updateFeuDirection(SensDeMarche sens)
{
    /* ------------------------------------------------------------------------
     * 1) Sélection du bloc directionnel (H ou AH)
     * ------------------------------------------------------------------------ */
    const DirectionConfig &cfg =
        (sens == SensHoraire) ? direction.H : direction.AH;

    if (!cfg.active)
    {
        setFeuDirection(sens, 0);
        return;
    }

    /* ------------------------------------------------------------------------
     * 2) Code‑barres (chaîne binaire brute)
     * ------------------------------------------------------------------------ */
    const std::string &codeBarre = cfg.codeBarre;

    /* ------------------------------------------------------------------------
     * 3) Détermination du voisin réel (SP1/SP2 ou SM1/SM2)
     * ------------------------------------------------------------------------ */
    CantonPeriph *voisin = nullptr;

    if (sens == SensHoraire)
    {
        voisin = voisinSP1();
        if (!voisin) voisin = voisinSP2();
    }
    else
    {
        voisin = voisinSM1();
        if (!voisin) voisin = voisinSM2();
    }

    /* ------------------------------------------------------------------------
     * 4) Détermination de la voie demandée
     * ------------------------------------------------------------------------ */
    uint8_t voieDemandee = 0;

    if (voisin)
    {
        uint16_t idVoisin = voisin->ID();

        auto it = cfg.voieDuVoisin.find(idVoisin);
        if (it != cfg.voieDuVoisin.end())
            voieDemandee = it->second;
    }

    /* ------------------------------------------------------------------------
     * 5) Occupation du canton
     * ------------------------------------------------------------------------ */
    bool occupe = estOccupe();

    /* ------------------------------------------------------------------------
     * 6) Accès aux aiguilles physiques
     * ------------------------------------------------------------------------ */
    AiguillesPhysiquesFromCanton aiguilles(this);

    /* ------------------------------------------------------------------------
     * 7) Calcul du feu directionnel via FeuxDirection::compute()
     * ------------------------------------------------------------------------ */
    FeuxDirection::DirectionState st =
        FeuxDirection::FeuxDirection::compute(codeBarre,
                                              voieDemandee,
                                              occupe,
                                              aiguilles);

    /* ------------------------------------------------------------------------
     * 8) Application du résultat
     * ------------------------------------------------------------------------ */
    setFeuDirection(sens, st.voieActive);

    /* ------------------------------------------------------------------------
     * 9) Logs
     * ------------------------------------------------------------------------ */
    if (!st.ok)
    {
        CC_LOG_ERROR("[Canton %u][FeuxDir][CC] ERROR (%s) : %s\n",
                     m_id,
                     (sens == SensHoraire ? "H" : "AH"),
                     st.erreur.c_str());
    }
    else
    {
        CC_LOG_TRACE("[Canton %u][FeuxDir][CC] %s → voieActive=%u\n",
                     m_id,
                     (sens == SensHoraire ? "H" : "AH"),
                     st.voieActive);
    }
}

/* ============================================================================
 *  Accesseurs
 * ==========================================================================*/
uint8_t Canton::getFeuDirection(SensDeMarche sens) const
{
    return m_feuDirection[sens];
}

void Canton::setFeuDirection(SensDeMarche sens, uint8_t valeur)
{
    m_feuDirection[sens] = valeur;
}
