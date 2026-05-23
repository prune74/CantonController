/*
 * Node_Capteurs.cpp — Gestion des capteurs ponctuels H / AH
 * ---------------------------------------------------------------------------
 * Version Discovery 2026
 */

#include "Node_Internal.h"

/* ============================================================================
 * readCapteurAH() / readCapteurH()
 * ---------------------------------------------------------------------------
 * Lecture simple des capteurs ponctuels.
 * ============================================================================
 */

bool Node::readCapteurAH()
{
    bool v = sensor[IDX_CAPT_ANTIHORAIRE].state();
    SA_LOG_TRACE("[Node %u] Capteur AH = %d\n", m_id, v);
    return v;
}

bool Node::readCapteurH()
{
    bool v = sensor[IDX_CAPT_HORAIRE].state();
    SA_LOG_TRACE("[Node %u] Capteur H = %d\n", m_id, v);
    return v;
}

/* ============================================================================
 * capteurActif() — Helper générique
 * ---------------------------------------------------------------------------
 * Version Discovery 2026 :
 *   sens = SensHoraire     → capteur H
 *   sens = SensAntiHoraire → capteur AH
 * ============================================================================
 */

bool Node::capteurActif(SensDeMarche sens)
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

void Node::overrideCapteur(SensDeMarche sens, bool etat)
{
    if (sens == SensHoraire)
    {
        sensor[IDX_CAPT_HORAIRE].overrideState(etat);
        SA_LOG_WARN("[Node %u] Override capteur H → %d\n", m_id, etat);
    }
    else
    {
        sensor[IDX_CAPT_ANTIHORAIRE].overrideState(etat);
        SA_LOG_WARN("[Node %u] Override capteur AH → %d\n", m_id, etat);
    }
}

/* ============================================================================
 * resetOverrideCapteurs()
 * ---------------------------------------------------------------------------
 * Désactive tous les overrides.
 * ============================================================================
 */

void Node::resetOverrideCapteurs()
{
    sensor[IDX_CAPT_HORAIRE].overrideState(false);
    sensor[IDX_CAPT_ANTIHORAIRE].overrideState(false);

    SA_LOG_TRACE("[Node %u] Override capteurs réinitialisé\n", m_id);
}
