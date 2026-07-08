#include "Canton.h"
#include "Aig.h"
#include "SupervisionCanton.h"
#include "Railcom.h"
#include "CC_CAN.h"
#include <Protocol.h>
#include "debug_cc.h"

bool Canton::estMesurable(Canton *canton, uint8_t sens)
{
    CantonPeriph *aval = nullptr;
    CantonPeriph *s2 = nullptr;
    uint8_t indexAval = 0;

    // --- Détermination du voisin suivant ---
    switch (sens)
    {
    case 0: // sens horaire
        aval = canton->voisinSP1();
        s2 = canton->voisinSP2();
        indexAval = canton->SP1_idx();
        break;

    case 1: // sens anti-horaire
        aval = canton->voisinSM1();
        s2 = canton->voisinSM2();
        indexAval = canton->SM1_idx();
        break;

    default:
        return false;
    }

    if (!aval || !aval->acces())
        return false;

    // --- Aiguillage : voie droite ou déviée ---
    Aig *aigSens = nullptr;
    for (uint8_t k = 0; k < aigSize; ++k)
    {
        Aig *a = canton->getAig(k);
        if (!a)
            continue;

        if (a->cantonPdroitIdx() == indexAval || a->cantonPdevieIdx() == indexAval)
        {
            aigSens = a;
            break;
        }
    }

    bool voieDevie = aigSens ? !aigSens->estDroit() : false;
    if (voieDevie)
        return false;

    // --- Canton aval doit être libre ---
    if (aval->busy())
        return false;

    // --- Canton S2 doit être accessible ---
    if (!s2 || !s2->acces())
        return false;

    // --- Aspect ferroviaire du canton aval ---
    ExccAspect aspect = mettreAJourAspectCanton(canton, sens);
    if (aspect != ExccAspect::ASPECT_VOIE_LIBRE)
        return false;

    // --- Loco connue ---
    uint16_t trainID = Railcom::address();
    if (trainID == 0)
        return false;

    // ----------------------------------------------------------------------
    // ⭐ Ici : toutes les conditions sont OK → on envoie MESURE_PREPARE
    // ----------------------------------------------------------------------
    uint8_t idCible = aval->ID();

    CC_LOG_INFO("[CAN][Mesure][CC] CC%u → CC%u : ENVOI MESURE_PREPARE train=%u\n",
                canton->ID(), idCible, trainID);

    CC_CAN::sendMsg(
        0,                                     // prio
        (uint8_t)Cmd_CC_to_CC::MESURE_PREPARE, // commande 0xEB
        idCible,                               // resp = CCx cible
        canton->ID(),                          // ID CC source
        trainID >> 8,                          // d0
        trainID & 0xFF                         // d1
    );

    return true;
}
