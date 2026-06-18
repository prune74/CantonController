/*
 * SupervisionAiguilles.cpp — Gestion Canton 2026
 * ------------------------------------------------------------
 * Supervision des aiguilles pour le Canton Controller (CC).
 *
 * Rôle :
 *   - recevoir les positions d’aiguilles envoyées par l’EXCC
 *   - mettre à jour l’état logique des aiguilles dans le CC
 *
 * IMPORTANT 2026 :
 *   - aucun masque d’aiguilles n’est maintenu côté Canton
 *   - ce module ne pilote pas les servos
 *   - il reflète uniquement l’état réel remonté par l’EXCC
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
// onPosition()
// Mise à jour d’une aiguille suite à une notification EXCC
// ---------------------------------------------------------------------------
void SupervisionAiguilles::onPosition(uint8_t idAig,
                                      uint8_t etat,
                                      uint8_t masque /* ignoré en 2026 */)
{
    if (!s_canton)
        return;

    Aig *aig = s_canton->getAig(idAig);
    if (!aig)
    {
        CC_LOG_WARN("[Aiguilles][CC] Aiguille %u inconnue\n", idAig);
        return;
    }

    bool droit = (etat == 1);
    aig->estDroit(droit);

    CC_LOG_INFO("[Aiguilles][CC] Aig %u = %s\n",
                idAig,
                droit ? "DROIT" : "DEVIE");
}
