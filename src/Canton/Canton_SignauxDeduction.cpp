/*
 * Canton_SignauxDeduction.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction du type de mât SNCF selon les besoins topologiques.
 */

#include "Canton.h"
#include "Exploration.h"
#include <Protocol.h>
#include "debug_cc.h"

/* ============================================================================
 *  Helpers topologiques
 * ==========================================================================*/

bool Canton::estImpasse() const
{
    bool aSP1 = (m_SP1_idx != UNUSED_ID);
    bool aSM1 = (m_SM1_idx != UNUSED_ID);
    return (aSP1 ^ aSM1); // XOR : un seul voisin → impasse
}

bool Canton::estZoneAiguilles() const
{
    return (Exploration::comptAig() > 0);
}

bool Canton::prochainCantonEstDangereux(SensDeMarche sens) const
{
    CantonPeriph *v = (sens == SensHoraire)
                          ? getCantonP(m_SP1_idx)
                          : getCantonP(m_SM1_idx);

    return v && (v->masqueAigTopo() != 0);
}

bool Canton::aBifurcation(SensDeMarche sens) const
{
    if (sens == SensHoraire)
        return voisinSP2() != nullptr;

    return voisinSM2() != nullptr;
}

bool Canton::cantonPrecedentEstEnRalentissement(SensDeMarche sens) const
{
    CantonPeriph *v = (sens == SensHoraire)
                          ? getCantonP(m_SM1_idx)
                          : getCantonP(m_SP1_idx);

    if (!v)
        return false;

    // Conversion des octets reçus en enum class ExccAspect
    ExccAspect aH = static_cast<ExccAspect>(v->aspectRecu[0]);
    ExccAspect aAH = static_cast<ExccAspect>(v->aspectRecu[1]);

    return (aH == ExccAspect::ASPECT_RALENTISSEMENT_30 ||
            aH == ExccAspect::ASPECT_RALENTISSEMENT_60 ||
            aAH == ExccAspect::ASPECT_RALENTISSEMENT_30 ||
            aAH == ExccAspect::ASPECT_RALENTISSEMENT_60);
}

/* ============================================================================
 *  Besoins topologiques
 * ==========================================================================*/

bool Canton::besoinRappel(SensDeMarche sens) const
{
    return cantonPrecedentEstEnRalentissement(sens);
}

bool Canton::besoinRalentissement(SensDeMarche sens) const
{
    return prochainCantonEstDangereux(sens) || aBifurcation(sens);
}

bool Canton::besoinCarre(SensDeMarche sens) const
{
    return estZoneAiguilles() || prochainCantonEstDangereux(sens);
}

/* ============================================================================
 *  Déduction du type de mât (VERSION 2026)
 * ==========================================================================*/

uint8_t Canton::deduireTypeSignal(SensDeMarche sens) const
{
    if (estImpasse())
        return TYPE_M;

    if (besoinRappel(sens))
        return TYPE_G;

    if (besoinRalentissement(sens))
        return TYPE_E;

    if (besoinCarre(sens))
        return TYPE_C;

    return TYPE_A;
}
