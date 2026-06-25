/*
 * Canton_SignauxDeduction.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction du type de mât SNCF selon les besoins topologiques :
 *
 *   - Impasse → SIG_MANOEUVRE
 *   - Besoin de RAPPEL → SIG_RAPPEL (9 feux)
 *   - Besoin de RALENTISSEMENT → SIG_RAL (7 feux)
 *   - Besoin de CARRE → SIG_CARRE (5 feux)
 *   - Sinon → SIG_BAL (3 feux)
 *
 * IMPORTANT :
 *   En 2026, l’aspect détermine le mât, et le mât est toujours compatible
 *   avec l’aspect. Aucune adaptation d’aspect n’est nécessaire.
 */

#include "Canton.h"
#include "Exploration.h"
#include "Protocol.h"
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

    return v && (v->masqueAig() != 0);
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

    ExccAspect aH = static_cast<ExccAspect>(v->aspectRecu[0]);
    ExccAspect aAH = static_cast<ExccAspect>(v->aspectRecu[1]);

    return (aH == ASPECT_RALENTISSEMENT_30 ||
            aH == ASPECT_RALENTISSEMENT_60 ||
            aAH == ASPECT_RALENTISSEMENT_30 ||
            aAH == ASPECT_RALENTISSEMENT_60);
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
        return SIG_MANOEUVRE;

    if (besoinRappel(sens))
        return SIG_RAPPEL;

    if (besoinRalentissement(sens))
        return SIG_RAL;

    if (besoinCarre(sens))
        return SIG_CARRE;

    return SIG_BAL;
}
