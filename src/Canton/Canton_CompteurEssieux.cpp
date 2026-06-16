#include "Canton.h"
#include "Exploration_Protocol.h"
#include "debug_sa.h"

void Canton::updateCompteurDepuisTrame(uint8_t code, uint8_t valeur)
{
    if (code == PROTO_05_COMPTEUR_ESSIEUX)
    {
        setCompteurEssieux(valeur);
        SA_LOG_TRACE("[Canton] Compteur essieux mis à jour : %d\n", valeur);
    }
}
