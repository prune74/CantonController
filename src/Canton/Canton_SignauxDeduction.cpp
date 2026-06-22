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
#include "Exploration_Protocol.h"
#include "debug_cc.h"

/* ============================================================================
 *  Helpers topologiques
 * ==========================================================================*/

bool Canton::estImpasse() const // 🟢
{
    bool aSP1 = (m_SP1_idx != UNUSED_ID);
    bool aSM1 = (m_SM1_idx != UNUSED_ID);
    return (aSP1 ^ aSM1); // XOR : un seul voisin → impasse
}

bool Canton::estZoneAiguilles() const // 🟢
{
    return (Exploration::comptAig() > 0);
}

bool Canton::prochainCantonEstDangereux(SensDeMarche sens) const // 🟢
{
    CantonPeriph *v = (sens == SensHoraire)
                        ? getCantonP(m_SP1_idx)
                        : getCantonP(m_SM1_idx);

    return v && (v->masqueAig() != 0);
}

bool Canton::aBifurcation(SensDeMarche sens) const // 🟢
{
    return (sens == SensHoraire) ? m_SP2_acces : m_SM2_acces;
}

bool Canton::cantonPrecedentEstEnRalentissement(SensDeMarche sens) const // 🟢
{
    CantonPeriph *v = (sens == SensHoraire)
                        ? getCantonP(m_SM1_idx)
                        : getCantonP(m_SP1_idx);

    if (!v)
        return false;

    ExccAspect aH  = static_cast<ExccAspect>(v->aspectRecu[0]);
    ExccAspect aAH = static_cast<ExccAspect>(v->aspectRecu[1]);

    return (aH == ASPECT_RALENTISSEMENT_30 ||
            aH == ASPECT_RALENTISSEMENT_60 ||
            aAH == ASPECT_RALENTISSEMENT_30 ||
            aAH == ASPECT_RALENTISSEMENT_60);
}

/* ============================================================================
 *  Besoins topologiques
 * ==========================================================================*/

bool Canton::besoinRappel(SensDeMarche sens) const // 🟢
{
    return cantonPrecedentEstEnRalentissement(sens);
}

bool Canton::besoinRalentissement(SensDeMarche sens) const // 🟢
{
    return prochainCantonEstDangereux(sens) || aBifurcation(sens);
}

bool Canton::besoinCarre(SensDeMarche sens) const // 🟢
{
    return estZoneAiguilles() || prochainCantonEstDangereux(sens);
}

/* ============================================================================
 *  Déduction du type de mât (VERSION 2026)
 * ==========================================================================*/

uint8_t Canton::deduireTypeSignal(SensDeMarche sens) const // 🟢
{
    // 1) Impasse → manœuvre obligatoire
    if (estImpasse())
        return SIG_MANOEUVRE;

    // 2) Besoin de rappel → mât 9 feux
    if (besoinRappel(sens))
        return SIG_RAPPEL;

    // 3) Besoin de ralentissement → mât 7 feux
    if (besoinRalentissement(sens))
        return SIG_RAL;

    // 4) Besoin de carré → mât 5 feux
    if (besoinCarre(sens))
        return SIG_CARRE;

    // 5) Sinon → BAL (3 feux)
    return SIG_BAL;
}
