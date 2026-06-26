/*
 * SupervisionAiguilles.cpp — Gestion Canton 2026
 * ------------------------------------------------------------
 * Supervision des aiguilles pour le Canton Controller (CC).
 */

#include "SupervisionAiguilles.h"
#include "Canton.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Variables statiques internes
// ---------------------------------------------------------------------------
Canton *SupervisionAiguilles::s_canton = nullptr;

// ---------------------------------------------------------------------------
// begin()
// ---------------------------------------------------------------------------
void SupervisionAiguilles::begin(Canton *canton)
{
    s_canton = canton;
    CC_LOG_INFO("[Aiguilles][CC] Supervision initialisée pour Canton %d\n", canton->ID());
}

// ---------------------------------------------------------------------------
// onPosition() — Mise à jour d’une aiguille suite à une notification EXCC
// ---------------------------------------------------------------------------
void SupervisionAiguilles::onPosition(uint8_t idAig, uint8_t pos, uint8_t etat)
{
    if (!s_canton)
        return;

    Aig *aig = s_canton->getAig(idAig);
    if (!aig)
    {
        CC_LOG_WARN("[Aiguilles][CC] Aiguille %u inconnue\n", idAig);
        return;
    }

    // --- Sécurité : état envoyé par l’EXCC ---
    if (etat == static_cast<uint8_t>(ExccCode::ETAT_BLOQUE))
    {
        CC_LOG_ERROR("[Aiguilles][CC] Aig %u BLOQUÉ → STOP GLOBAL !\n", idAig);
        s_canton->setStopActive(true);
        return;
    }

    if (etat == static_cast<uint8_t>(ExccCode::ETAT_ERREUR))
    {
        CC_LOG_ERROR("[Aiguilles][CC] Aig %u ERREUR SWITCH → STOP GLOBAL !\n", idAig);
        s_canton->setStopActive(true);
        return;
    }

    // --- Position physique ---
    switch (pos)
    {
    case static_cast<uint8_t>(ExccCode::POS_DROIT):
        aig->estDroit(true);
        CC_LOG_INFO("[Aiguilles][CC] Aig %u = DROIT\n", idAig);
        break;

    case static_cast<uint8_t>(ExccCode::POS_DEVIE):
        aig->estDroit(false);
        CC_LOG_INFO("[Aiguilles][CC] Aig %u = DEVIE\n", idAig);
        break;

    case static_cast<uint8_t>(ExccCode::POS_INDET):
        CC_LOG_ERROR("[Aiguilles][CC] Aig %u = INDET → STOP GLOBAL !\n", idAig);
        s_canton->setStopActive(true);
        break;

    case static_cast<uint8_t>(ExccCode::POS_INCOHERENT):
        CC_LOG_ERROR("[Aiguilles][CC] Aig %u = INCOHERENT → STOP GLOBAL !\n", idAig);
        s_canton->setStopActive(true);
        break;

    default:
        CC_LOG_ERROR("[Aiguilles][CC] Aig %u = valeur invalide (%u) → STOP GLOBAL !\n",
                     idAig, pos);
        s_canton->setStopActive(true);
        break;
    }
}
