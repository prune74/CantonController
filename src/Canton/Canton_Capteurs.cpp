/*
 * Canton_Capteurs.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des capteurs ponctuels H / AH du canton.
 *
 * Rôle :
 *   - lire l’état des capteurs ponctuels (H / AH)
 *   - fournir un helper générique capteurActif()
 *   - permettre un override manuel (mode debug)
 *
 * IMPORTANT :
 *   - aucune logique métier
 *   - aucune interprétation RailCom / essieux
 *   - aucune décision de sécurité
 *
 * Ce module fournit UNIQUEMENT l’état brut des capteurs ponctuels.
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  readCapteurAH() / readCapteurH()
 * ---------------------------------------------------------------------------
 *  Lecture simple des capteurs ponctuels.
 * ==========================================================================*/
bool Canton::readCapteurAH()
{
    bool v = sensor[IDX_CAPT_ANTIHORAIRE].state();
    CC_LOG_TRACE("[Canton %u][Capteurs][CC] AH = %d\n", m_id, v);
    return v;
}

bool Canton::readCapteurH()
{
    bool v = sensor[IDX_CAPT_HORAIRE].state();
    CC_LOG_TRACE("[Canton %u][Capteurs][CC] H = %d\n", m_id, v);
    return v;
}

/* ============================================================================
 *  capteurActif() — Helper générique
 * ---------------------------------------------------------------------------
 *  sens = SensHoraire     → capteur H
 *  sens = SensAntiHoraire → capteur AH
 * ==========================================================================*/
bool Canton::capteurActif(SensDeMarche sens)
{
    return (sens == SensHoraire)
           ? readCapteurH()
           : readCapteurAH();
}

/* ============================================================================
 *  overrideCapteur() — Mode debug
 * ---------------------------------------------------------------------------
 *  Force l’état d’un capteur ponctuel.
 *  Utilisé uniquement pour tests / diagnostics.
 * ==========================================================================*/
void Canton::overrideCapteur(SensDeMarche sens, bool etat)
{
    if (sens == SensHoraire)
    {
        sensor[IDX_CAPT_HORAIRE].overrideState(etat);
        CC_LOG_WARN("[Canton %u][Capteurs][CC] Override H → %d\n", m_id, etat);
    }
    else
    {
        sensor[IDX_CAPT_ANTIHORAIRE].overrideState(etat);
        CC_LOG_WARN("[Canton %u][Capteurs][CC] Override AH → %d\n", m_id, etat);
    }
}

/* ============================================================================
 *  resetOverrideCapteurs()
 * ---------------------------------------------------------------------------
 *  Désactive tous les overrides.
 * ==========================================================================*/
void Canton::resetOverrideCapteurs()
{
    sensor[IDX_CAPT_HORAIRE].overrideState(false);
    sensor[IDX_CAPT_ANTIHORAIRE].overrideState(false);

    CC_LOG_TRACE("[Canton %u][Capteurs][CC] Overrides réinitialisés\n", m_id);
}
