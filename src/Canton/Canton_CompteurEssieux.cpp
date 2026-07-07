#include "Canton.h"
#include "debug_cc.h"

void Canton::onEssieux(Canton *canton, uint8_t essieux)
{
    // 1) Stockage du nombre d’essieux
    canton->m_compteurEssieux = essieux;

    // 2) Log
    CC_LOG_INFO("[Canton %u] Essieux reçus du EXCC : %u\n",
                canton->ID(), essieux);

    // 3) Rien d’autre ici.
    // Le CC n’a pas de champ RailCom dans Canton,
    // donc aucune association RailCom ↔ essieux ne peut être faite ici.
    //
    // L’envoi vers l’ERM se fera dans onSortie(),
    // en même temps que MESURE_VITESSE.
}
