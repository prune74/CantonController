/*
 * SupervisionCanton.cpp — Gestion Canton 2026
 * ------------------------------------------------------------
 * Calcul de l’aspect ferroviaire du canton pour le
 * Canton Controller (CC).
 *
 * Ce module détermine l’aspect à afficher en fonction :
 *   - de la topologie locale (SP1 / SM1)
 *   - de l’état du canton aval (CantonPeriph aval)
 *   - de l’accessibilité ferroviaire secondaire (SP2 / SM2)
 *   - de l’état des aiguilles
 *   - de l’adresse RailCom reçue de l’EXCC
 *
 * L’objectif est de produire un aspect cohérent :
 *   - Carré
 *   - Sémaphore
 *   - Avertissement
 *   - Ralentissement 30 / 60
 *   - Rappel 30 / 60
 *   - Voie libre
 */

#include "SupervisionCanton.h"
#include "Protocol.h"
#include "Canton.h"
#include "Aig.h"
#include "Railcom.h"
#include "debug_cc.h"

/*
 * Recherche l’aiguille correspondant au sens donné
 * en fonction de l’index du canton aval.
 */
static Aig *trouverAiguillePourSens(Canton *canton, uint8_t indexAval) // 🟢
{
    for (uint8_t k = 0; k < aigSize; ++k)
    {
        Aig *a = canton->getAig(k);
        if (!a)
            continue;

        if (a->cantonPdroitIdx() == indexAval || a->cantonPdevieIdx() == indexAval)
            return a;
    }
    return nullptr;
}

/*
 * Calcul de l’aspect ferroviaire du canton
 * i = 0 → sens horaire
 * i = 1 → sens anti‑horaire
 */
ExccAspect mettreAJourAspectCanton(Canton *canton, uint8_t i) // 🟢
{
    CantonPeriph *aval = nullptr; // voisin principal (SP1 / SM1)
    CantonPeriph *s2 = nullptr;   // voisin secondaire (SP2 / SM2)

    uint8_t indexAval = 0;

    switch (i)
    {
    case 0: // sens horaire
        aval = canton->voisinSP1();
        s2 = canton->voisinSP2();
        indexAval = canton->SP1_idx();
        break;

    case 1: // sens anti‑horaire
        aval = canton->voisinSM1();
        s2 = canton->voisinSM2();
        indexAval = canton->SM1_idx();
        break;

    default:
        CC_LOG_ERROR("[Canton][CC] Sens invalide (%u) → Carré\n", i);
        return ASPECT_CARRE;
    }

    if (!aval)
    {
        CC_LOG_WARN("[Canton][CC] Aval inexistant (idx=%u) → Carré\n", indexAval);
        return ASPECT_CARRE;
    }

    if (!aval->acces())
    {
        CC_LOG_WARN("[Canton][CC] Aval %u inaccessible → Carré\n", indexAval);
        return ASPECT_CARRE;
    }

    Aig *aigSens = trouverAiguillePourSens(canton, indexAval);
    bool voieDevie = aigSens ? !aigSens->estDroit() : false;

    CC_LOG_TRACE("[Canton][CC] Sens=%u aval=%u voieDevie=%d\n",
                 i, indexAval, voieDevie);

    /*
     * CAS 1 : canton aval occupé
     */
    if (aval->busy())
    {
        CC_LOG_TRACE("[Canton][CC] Aval occupé\n");

        // Adresse RailCom reçue de l’EXCC
        uint16_t adr = Railcom::address();

        if (adr == 0)
        {
            CC_LOG_INFO("[Canton][CC] Aucune loco connue → Avertissement\n");
            return ASPECT_AVERTISSEMENT;
        }

        if (adr != aval->reserved())
        {
            CC_LOG_INFO("[Canton][CC] Loco différente → Sémaphore\n");
            return ASPECT_SEMAPHORE;
        }

        CC_LOG_INFO("[Canton][CC] Même loco → Ralentissement %s\n",
                    voieDevie ? "30" : "60");

        return voieDevie ? ASPECT_RALENTISSEMENT_30
                         : ASPECT_RALENTISSEMENT_60;
    }

    /*
     * CAS 2 : canton aval libre → vérifier SP2/SM2
     *
     * S2 est considéré :
     *   - inaccessible si s2 == nullptr ou !s2->acces()
     *   - occupé si s2 && s2->busy()
     */
    bool s2access = (s2 && s2->acces());
    bool s2busy = (s2 && s2->busy());

    if (!s2access)
    {
        CC_LOG_INFO("[Canton][CC] S2 inexistant ou inaccessible → Avertissement\n");
        return ASPECT_AVERTISSEMENT;
    }

    if (s2busy)
    {
        CC_LOG_INFO("[Canton][CC] S2 occupé → Rappel %s\n",
                    voieDevie ? "30" : "60");

        return voieDevie ? ASPECT_RAPPEL_30
                         : ASPECT_RAPPEL_60;
    }

    /*
     * CAS 3 : tout est libre
     */
    if (voieDevie)
    {
        CC_LOG_INFO("[Canton][CC] Voie déviée → Ralentissement 30\n");
        return ASPECT_RALENTISSEMENT_30;
    }

    CC_LOG_INFO("[Canton][CC] Voie libre\n");
    return ASPECT_VOIE_LIBRE;
}
