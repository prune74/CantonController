/*
 * Canton_Capteurs.cpp — Gestion des capteurs ponctuels H / AH
 * ---------------------------------------------------------------------------
 * Version Exploration 2026
 */

#include "Canton.h"
#include "Config.h"
#include "debug_sa.h"

/* ============================================================================
 * readCapteurAH() / readCapteurH()
 * ---------------------------------------------------------------------------
 * Lecture simple des capteurs ponctuels.
 * ============================================================================
 */

bool Canton::readCapteurAH()
{
    bool v = sensor[IDX_CAPT_ANTIHORAIRE].state();
    SA_LOG_TRACE("[Canton %u] Capteur AH = %d\n", m_id, v);
    return v;
}

bool Canton::readCapteurH()
{
    bool v = sensor[IDX_CAPT_HORAIRE].state();
    SA_LOG_TRACE("[Canton %u] Capteur H = %d\n", m_id, v);
    return v;
}

/* ============================================================================
 * capteurActif() — Helper générique
 * ---------------------------------------------------------------------------
 * Version Exploration 2026 :
 *   sens = SensHoraire     → capteur H
 *   sens = SensAntiHoraire → capteur AH
 * ============================================================================
 */

bool Canton::capteurActif(SensDeMarche sens)
{
    if (sens == SensHoraire)
        return readCapteurH();

    return readCapteurAH();
}

/* ============================================================================
 * overrideCapteur() — Mode debug
 * ---------------------------------------------------------------------------
 * Permet de forcer l’état d’un capteur ponctuel.
 * ============================================================================
 */

void Canton::overrideCapteur(SensDeMarche sens, bool etat)
{
    if (sens == SensHoraire)
    {
        sensor[IDX_CAPT_HORAIRE].overrideState(etat);
        SA_LOG_WARN("[Canton %u] Override capteur H → %d\n", m_id, etat);
    }
    else
    {
        sensor[IDX_CAPT_ANTIHORAIRE].overrideState(etat);
        SA_LOG_WARN("[Canton %u] Override capteur AH → %d\n", m_id, etat);
    }
}

/* ============================================================================
 * resetOverrideCapteurs()
 * ---------------------------------------------------------------------------
 * Désactive tous les overrides.
 * ============================================================================
 */

void Canton::resetOverrideCapteurs()
{
    sensor[IDX_CAPT_HORAIRE].overrideState(false);
    sensor[IDX_CAPT_ANTIHORAIRE].overrideState(false);

    SA_LOG_TRACE("[Canton %u] Override capteurs réinitialisé\n", m_id);
}
