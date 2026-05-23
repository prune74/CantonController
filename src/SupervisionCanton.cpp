/*
 * SupervisionCanton.cpp — Version ENUM (Option A)
 */

#include "SupervisionCanton.h"
#include "SA_EXSA_Protocol.h"
#include "Node.h"
#include "Aig.h"
#include "debug_sa.h"

/**
 * trouverAiguillePourSens(node, indexAval)
 */
static Aig* trouverAiguillePourSens(Node* node, uint8_t indexAval)
{
    for (uint8_t k = 0; k < aigSize; ++k)
    {
        Aig* a = node->getAig(k);
        if (!a)
            continue;

        if (a->nodePdroitIdx() == indexAval || a->nodePdevieIdx() == indexAval)
            return a;
    }
    return nullptr;
}

/**
 * mettreAJourAspectCanton(node, i)
 */
ExsaAspect mettreAJourAspectCanton(Node* node, uint8_t i)
{
    uint8_t indexAval = 0;
    bool s2access = false;
    bool s2busy   = false;

    switch (i)
    {
        case 0: // sens horaire
            indexAval = node->SP1_idx();
            s2access  = node->SP2_acces();
            s2busy    = node->SP2_busy();
            break;

        case 1: // sens anti-horaire
            indexAval = node->SM1_idx();
            s2access  = node->SM2_acces();
            s2busy    = node->SM2_busy();
            break;

        default:
            SA_LOG_ERROR("[Canton] Sens invalide (%u) → Carré\n", i);
            return ASPECT_CARRE;
    }

    NodePeriph* aval = node->getNodeP(indexAval);

    if (!aval)
    {
        SA_LOG_WARN("[Canton] Aval inexistant (idx=%u) → Carré\n", indexAval);
        return ASPECT_CARRE;
    }

    if (!aval->acces())
    {
        SA_LOG_WARN("[Canton] Aval %u inaccessible → Carré\n", indexAval);
        return ASPECT_CARRE;
    }

    Aig* aigSens = trouverAiguillePourSens(node, indexAval);
    bool voieDevie = aigSens ? !aigSens->estDroit() : false;

    SA_LOG_TRACE("[Canton] Sens=%u aval=%u voieDevie=%d\n",
                 i, indexAval, voieDevie);

    /*
     * CAS 1 : canton aval occupé
     */
    if (aval->busy())
    {
        SA_LOG_TRACE("[Canton] Aval occupé\n");

        Loco* loco = node->getLoco();

        if (!loco || loco->address() == 0)
        {
            SA_LOG_INFO("[Canton] Aucune loco connue → Avertissement\n");
            return ASPECT_AVERTISSEMENT;
        }

        if (loco->address() != aval->reserved())
        {
            SA_LOG_INFO("[Canton] Loco différente → Sémaphore\n");
            return ASPECT_SEMAPHORE;
        }

        SA_LOG_INFO("[Canton] Même loco → Ralentissement %s\n",
                    voieDevie ? "30" : "60");

        return voieDevie ? ASPECT_RALENTISSEMENT_30
                         : ASPECT_RALENTISSEMENT_60;
    }

    /*
     * CAS 2 : canton aval libre → vérifier SP2/SM2
     */

    if (!s2access)
    {
        SA_LOG_INFO("[Canton] S2 inaccessible → Avertissement\n");
        return ASPECT_AVERTISSEMENT;
    }

    if (s2busy)
    {
        SA_LOG_INFO("[Canton] S2 occupé → Rappel %s\n",
                    voieDevie ? "30" : "60");

        return voieDevie ? ASPECT_RAPPEL_30
                         : ASPECT_RAPPEL_60;
    }

    /*
     * CAS 3 : tout est libre
     */

    if (voieDevie)
    {
        SA_LOG_INFO("[Canton] Voie déviée → Ralentissement 30\n");
        return ASPECT_RALENTISSEMENT_30;
    }

    SA_LOG_INFO("[Canton] Voie libre\n");
    return ASPECT_VOIE_LIBRE;
}
