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
void SupervisionAiguilles::onPosition(uint8_t index_excc,
                                      uint8_t idAig,
                                      uint8_t etat,
                                      uint8_t masque /* ignoré en 2026 */)
{
    if (!s_canton)
        return;

    Aig *aig = s_canton->getAig(idAig);
    if (!aig)
    {
        CC_LOG_WARN("[Aiguilles][CC] Aiguille %u inconnue (EXCC %u)\n",
                    idAig, index_excc);
        return;
    }

    bool droit = (etat == 1);
    aig->estDroit(droit);

    // En 2026 : aucun masque n’est maintenu côté Canton
    // Le paramètre 'masque' est ignoré volontairement.

    CC_LOG_INFO("[Aiguilles][CC] EXCC %u → Aig %u = %s\n",
                index_excc,
                idAig,
                droit ? "DROIT" : "DEVIE");
}
