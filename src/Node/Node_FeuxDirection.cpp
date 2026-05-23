/*
 * Node_FeuxDirection.cpp — Gestion des feux directionnels du canton (Node)
 * ---------------------------------------------------------------------------
 * Ce fichier regroupe la logique liée aux feux de direction (LED blanches),
 * distincte des signaux SNCF classiques.
 *
 * Il utilise désormais le module FeuxDirection (nouvelle architecture) :
 *   - FeuxDirection::compute()
 *   - FeuxDirection_CodeBarre
 *   - FeuxDirection_Conditions
 */

#include "Node.h"
#include "debug_sa.h"

#include "FeuxDirection/FeuxDirection_Types.h"
#include "FeuxDirection/FeuxDirection_CodeBarre.h"
#include "FeuxDirection/FeuxDirection_Conditions.h"
#include "FeuxDirection.h"

// ---------------------------------------------------------------------------
// Implémentation concrète de IAiguillesPhysiques pour un Node
// ---------------------------------------------------------------------------

class AiguillesPhysiquesFromNode : public FeuxDirection::IAiguillesPhysiques
{
public:
    explicit AiguillesPhysiquesFromNode(const Node* n) : m_node(n) {}

    uint8_t getPositionAig(uint8_t indexAig) const override
    {
        // Node fournit la position physique (0=droit, 1=devie)
        return m_node->getAiguillePosition(indexAig);
    }

private:
    const Node* m_node;
};


// ---------------------------------------------------------------------------
// Fonction principale : mise à jour du feu directionnel pour un sens donné
// ---------------------------------------------------------------------------

void Node::updateFeuDirection(SensDeMarche sens)
{
    // 1) Sélection du bloc directionnel (H ou AH)
    const DirectionConfig& cfg =
        (sens == SensHoraire) ? direction.H : direction.AH;

    // Si ce sens n'est pas actif → pas de feu
    if (!cfg.active)
    {
        setFeuDirection(sens, 0);
        return;
    }

    // 2) Code-barres
    const std::string& codeBarre = cfg.codeBarre;

    // 3) Détermination du voisin réel (SP1/SP2 ou SM1/SM2)
    NodePeriph* voisin = nullptr;

    if (sens == SensHoraire)
    {
        voisin = voisinSP1();
        if (!voisin)
            voisin = voisinSP2();
    }
    else
    {
        voisin = voisinSM1();
        if (!voisin)
            voisin = voisinSM2();
    }

    // 4) Détermination de la voie demandée
    uint8_t voieDemandee = 0;

    if (voisin)
    {
        uint16_t idVoisin = voisin->ID();

        auto it = cfg.voieDuVoisin.find(idVoisin);
        if (it != cfg.voieDuVoisin.end())
            voieDemandee = it->second;
    }

    // 5) Occupation du canton
    bool occupe = estOccupe();

    // 6) Accès aux aiguilles physiques
    AiguillesPhysiquesFromNode aiguilles(this);

    // 7) Calcul du feu directionnel via la nouvelle API
    FeuxDirection::DirectionState st =
        FeuxDirection::FeuxDirection::compute(codeBarre,
                                              voieDemandee,
                                              occupe,
                                              aiguilles);

    // 8) Application du résultat
    setFeuDirection(sens, st.voieActive);

    // 9) Logs
    if (!st.ok)
    {
        SA_LOG_ERROR("[Node %u] FeuxDirection ERROR (%s) : %s\n",
                     m_id,
                     (sens == SensHoraire ? "H" : "AH"),
                     st.erreur.c_str());
    }
    else
    {
        SA_LOG_TRACE("[Node %u] FeuxDirection %s → voieActive=%u (ok)\n",
                     m_id,
                     (sens == SensHoraire ? "H" : "AH"),
                     st.voieActive);
    }
}


// ---------------------------------------------------------------------------
// Fonctions manquantes : setFeuDirection() et getFeuDirection()
// ---------------------------------------------------------------------------

uint8_t Node::getFeuDirection(SensDeMarche sens) const
{
    return m_feuDirection[sens];
}

void Node::setFeuDirection(SensDeMarche sens, uint8_t valeur)
{
    m_feuDirection[sens] = valeur;
}


/* ------------------------------------------------------------
  Fin de Node_FeuxDirection.cpp
  ------------------------------------------------------------
*/
