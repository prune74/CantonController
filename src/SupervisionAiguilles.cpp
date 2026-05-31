#include "SupervisionAiguilles.h"
#include "Node.h"
#include "debug_sa.h"

Node* SupervisionAiguilles::s_node = nullptr;

void SupervisionAiguilles::begin(Node* node)
{
    s_node = node;
    SA_LOG_INFO("[Aiguilles] Supervision initialisée pour Node %d\n", node->ID());
}

void SupervisionAiguilles::onPosition(uint8_t index_exsa,
                                      uint8_t idAig,
                                      uint8_t etat,
                                      uint8_t masque)
{
    if (!s_node)
        return;

    Aig* aig = s_node->getAig(idAig);
    if (!aig)
    {
        SA_LOG_WARN("[Aiguilles] Aiguille %u inconnue (EXSA %u)\n",
                    idAig, index_exsa);
        return;
    }

    bool droit = (etat == 1);
    aig->estDroit(droit);

    s_node->masqueAig(masque);

    SA_LOG_INFO("[Aiguilles] EXSA %u → Aig %u = %s (masque=0x%02X)\n",
                index_exsa,
                idAig,
                droit ? "DROIT" : "DEVIE",
                masque);
}
