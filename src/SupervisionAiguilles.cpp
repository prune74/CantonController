#include "SupervisionAiguilles.h"
#include "Canton.h"
#include "debug_sa.h"

Canton *SupervisionAiguilles::s_canton = nullptr;

void SupervisionAiguilles::begin(Canton *canton)
{
    s_canton = canton;
    SA_LOG_INFO("[Aiguilles] Supervision initialisée pour Canton %d\n", canton->ID());
}

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
        SA_LOG_WARN("[Aiguilles] Aiguille %u inconnue (EXSA %u)\n",
                    idAig, index_exsa);
        return;
    }

    bool droit = (etat == 1);
    aig->estDroit(droit);

    s_canton->masqueAig(masque);

    SA_LOG_INFO("[Aiguilles] EXSA %u → Aig %u = %s (masque=0x%02X)\n",
                index_exsa,
                idAig,
                droit ? "DROIT" : "DEVIE",
                masque);
}
