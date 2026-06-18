/*
 * SupervisionAiguilles.cpp — Gestion Canton 2026
 * ------------------------------------------------------------
 * Supervision des aiguilles pour le Canton Controller (CC).
 *
 * Rôle :
 *   - recevoir les positions d’aiguilles envoyées par l’EXCC
 *   - mettre à jour l’état logique des aiguilles dans le CC
 *   - mettre à jour le masque global des aiguilles
 *
 * Ce module ne pilote pas les servos : il reflète uniquement
 * l’état réel remonté par l’Extension Canton Controller (EXCC).
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
void SupervisionAiguilles::onPosition(uint8_t index_exsa,
                                      uint8_t idAig,
                                      uint8_t etat,
                                      uint8_t masque)
{
    if (!s_canton)
        return;

    Aig *aig = s_canton->getAig(idAig);
    if (!aig)
    {
        CC_LOG_WARN("[Aiguilles][CC] Aiguille %u inconnue (EXCC %u)\n",
                    idAig, index_exsa);
        return;
    }

    bool droit = (etat == 1);
    aig->estDroit(droit);

    // Mise à jour du masque global
    s_canton->masqueAig(masque);

    CC_LOG_INFO("[Aiguilles][CC] EXCC %u → Aig %u = %s (masque=0x%02X)\n",
                index_exsa,
                idAig,
                droit ? "DROIT" : "DEVIE",
                masque);
}
