/*
 * RalentissementInterneCanton.cpp — Discovery 2026
 * ---------------------------------------------------------------------------
 * Gestion du ralentissement interne dans un canton :
 *   - point zéro du canton défini par le ponctuel d’entrée
 *   - calcul de la distance parcourue
 *   - déclenchement du ralentissement à :
 *         longueur_canton_mm - zone_ralentissement_mm
 *   - rampe de ralentissement (linéaire)
 *   - vitesse finale = vitesseAspect + Δ
 *   - vitesse stricte au ponctuel de sortie via PilotageDistribue
 */

#include "RalentissementInterneCanton.h"
#include "Settings.h"
#include "PilotageDistribue.h"
#include "SensRoulage.h"
#include "Canton.h"
#include "Loco.h"
#include "debug_cc.h"

/* ============================================================================
 *  Écart Δ selon l’aspect et le profil N/HO
 * ==========================================================================*/
static uint8_t calculerEcart(ExccAspect aspect)
{
    bool isHO = Settings::TRACK_PROFILE;

    switch (aspect)
    {
    case ExccAspect::ASPECT_RALENTISSEMENT_30:
        return isHO ? Settings::ecartR30_HO() : Settings::ecartR30_N();

    case ExccAspect::ASPECT_RALENTISSEMENT_60:
        return isHO ? Settings::ecartR60_HO() : Settings::ecartR60_N();

    case ExccAspect::ASPECT_AVERTISSEMENT:
    case ExccAspect::ASPECT_SEMAPHORE:
        return isHO ? Settings::ecartAvert_HO() : Settings::ecartAvert_N();

    case ExccAspect::ASPECT_MANOEUVRE:
        return isHO ? Settings::ecartMan_HO() : Settings::ecartMan_N();

    case ExccAspect::ASPECT_CARRE:
    case ExccAspect::ASPECT_CARRE_VIOLET:
        return isHO ? Settings::ecartCarre_HO() : Settings::ecartCarre_N();

    default:
        return isHO ? Settings::ecartDefault_HO() : Settings::ecartDefault_N();
    }
}

/* ============================================================================
 *  Vitesse finale (aspect + Δ)
 * ==========================================================================*/
static uint16_t calculerVitesseFinale(ExccAspect aspect, Loco *loco, uint16_t ecart)
{
    switch (aspect)
    {
    case ExccAspect::ASPECT_VOIE_LIBRE:
        return loco->vitesseVoieLibre;

    case ExccAspect::ASPECT_AVERTISSEMENT:
    case ExccAspect::ASPECT_SEMAPHORE:
        return loco->vitesseAvertissement + ecart;

    case ExccAspect::ASPECT_RALENTISSEMENT_60:
        return loco->vitesseRalentissement60 + ecart;

    case ExccAspect::ASPECT_RALENTISSEMENT_30:
        return loco->vitesseRalentissement30 + ecart;

    case ExccAspect::ASPECT_RAPPEL_60:
        return loco->vitesseRappel60 + ecart;

    case ExccAspect::ASPECT_RAPPEL_30:
        return loco->vitesseRappel30 + ecart;

    case ExccAspect::ASPECT_MANOEUVRE:
        return loco->vitesseManoeuvre + ecart;

    case ExccAspect::ASPECT_CARRE:
    case ExccAspect::ASPECT_CARRE_VIOLET:
        return 0 + ecart;

    case ExccAspect::ASPECT_MASQUE:
        return loco->vitesseAvertissement + ecart;

    default:
        return 0;
    }
}

/* ============================================================================
 *  Distance parcourue depuis t0
 * ==========================================================================*/
static uint32_t calculerDistanceParcourue(uint32_t t0, uint32_t now, uint16_t vitesseInitiale)
{
    uint32_t dt = now - t0;  // ms
    return (vitesseInitiale * dt) / 1000;  // mm
}

/* ============================================================================
 *  Rampe linéaire
 * ==========================================================================*/
static uint16_t calculerVitesseRampe(uint32_t distanceRestante,
                                     uint32_t zone,
                                     uint16_t v0,
                                     uint16_t vFin)
{
    if (distanceRestante >= zone)
        return v0;

    return map(distanceRestante, zone, 0, v0, vFin);
}

/* ============================================================================
 *  Classe RalentissementInterneCanton
 * ==========================================================================*/

RalentissementInterneCanton::RalentissementInterneCanton()
    : actif(false),
      t0(0),
      vitesseInitiale(0)
{
}

void RalentissementInterneCanton::executer(Canton *canton, Loco *loco)
{
    if (!canton || !loco)
        return;

    // Déduire le sens
    deduireSensRoulage(canton);

    // Ponctuel d’entrée
    bool ponctuelEntree = false;

    if (loco->sens() == SensHoraire && canton->ponctuelH())
        ponctuelEntree = true;

    if (loco->sens() == SensAntiHoraire && canton->ponctuelAH())
        ponctuelEntree = true;

    // Début du canton
    if (ponctuelEntree && !actif)
    {
        CC_LOG_INFO("[Ralentissement][CC] Début du canton\n");

        actif = true;
        t0 = millis();
        vitesseInitiale = loco->speed();

        return;
    }

    if (!actif)
        return;

    uint32_t now = millis();

    uint32_t distanceParcourue =
        calculerDistanceParcourue(t0, now, vitesseInitiale);

    uint32_t distanceRestante =
        Settings::longueurCantonMM() - distanceParcourue;

    uint32_t seuilRalentissement =
        Settings::longueurCantonMM() - Settings::zoneRalentissementMM();

    ExccAspect aspect = canton->aspectLocal(loco->sens());

    uint16_t v;

    if (distanceParcourue < seuilRalentissement)
    {
        v = vitesseInitiale;
    }
    else
    {
        uint8_t ecart = calculerEcart(aspect);
        uint16_t vFin = calculerVitesseFinale(aspect, loco, ecart);

        v = calculerVitesseRampe(distanceRestante,
                                 Settings::zoneRalentissementMM(),
                                 vitesseInitiale,
                                 vFin);
    }

    loco->speed(v);

    CC_LOG_TRACE("[Ralentissement][CC] dist=%lu rest=%lu v=%u aspect=%u\n",
                 distanceParcourue, distanceRestante, v, aspect);

    // Ponctuel de sortie
    bool ponctuelSortie = false;

    if (loco->sens() == SensHoraire && canton->ponctuelAH())
        ponctuelSortie = true;

    if (loco->sens() == SensAntiHoraire && canton->ponctuelH())
        ponctuelSortie = true;

    if (ponctuelSortie)
    {
        CC_LOG_INFO("[Ralentissement][CC] Fin du canton → PilotageDistribue\n");

        canton->pilotageDistribue();
        actif = false;
    }
}
