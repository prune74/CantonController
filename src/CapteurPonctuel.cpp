#include "CapteurPonctuel.h"
#include "Canton.h"
#include "debug_cc.h"

#include "CapteurPonctuel.h"
#include "Canton.h"
#include "debug_cc.h"

void CapteurPonctuel::onPonctuelH(uint8_t etat)
{
    Canton *c = Canton::s_instance;
    if (!c) return;

    bool actif = (etat != 0);
    c->setPonctuelH(actif);

    CC_LOG_TRACE("[Ponctuel] H = %u\n", actif);
}

void CapteurPonctuel::onPonctuelAH(uint8_t etat)
{
    Canton *c = Canton::s_instance;
    if (!c) return;

    bool actif = (etat != 0);
    c->setPonctuelAH(actif);

    CC_LOG_TRACE("[Ponctuel] AH = %u\n", actif);
}

