#include "SupervisionEssieux.h"
#include "Canton.h"
#include "debug_sa.h"

/*
 * Variables internes statiques
 */

Canton *SupervisionEssieux::s_canton = nullptr;
bool SupervisionEssieux::s_rebootDetecte = false;
uint16_t SupervisionEssieux::s_incoherenceTimer = 0;
const uint16_t SupervisionEssieux::INCOHERENCE_TIMEOUT;

void SupervisionEssieux::begin(Canton *canton)
{
    s_canton = canton;
    s_incoherenceTimer = 0;

    SA_LOG_INFO("[Essieux] Supervision initialisée pour Canton %d\n", canton->ID());
}

void SupervisionEssieux::notifierRebootEXCC()
{
    s_rebootDetecte = true;
    SA_LOG_WARN("[Essieux] Reboot EXCC détecté → compteur invalide\n");
}

void SupervisionEssieux::loop()
{
    if (!s_canton)
        return;

    verifierCoherence();
}

void SupervisionEssieux::verifierCoherence()
{
    // ------------------------------------------------------------------------
    // Récupération des états ferroviaires
    // ------------------------------------------------------------------------
    int compteur = s_canton->compteurEssieux();   // ⬅️ NOUVEAU
    bool occLocal = s_canton->busy();
    bool occAmont = false;
    bool occAval = s_canton->SP2_busy() || s_canton->SM2_busy();

    // Canton amont occupé ?
    CantonPeriph *sp1 = s_canton->getCantonP(s_canton->SP1_idx());
    if (sp1)
        occAmont |= sp1->busy();

    CantonPeriph *sm1 = s_canton->getCantonP(s_canton->SM1_idx());
    if (sm1)
        occAmont |= sm1->busy();

    /* ========================================================================
       1) Reboot EXCC → reset immédiat
       ======================================================================== */
    if (s_rebootDetecte)
    {
        s_canton->resetCompteurEssieux();   // ⬅️ NOUVEAU
        s_rebootDetecte = false;
        s_incoherenceTimer = 0;

        SA_LOG_WARN("[Essieux] Reset compteur (reboot EXCC)\n");
        return;
    }

    /* ========================================================================
       2) compteur < 0 → impossible physiquement → reset immédiat
       ======================================================================== */
    if (compteur < 0)
    {
        s_canton->resetCompteurEssieux();   // ⬅️ NOUVEAU
        s_incoherenceTimer = 0;

        SA_LOG_ERROR("[Essieux] Reset compteur (compteur < 0)\n");
        return;
    }

    /* ========================================================================
       3) compteur > 0 mais tout est libre → incohérence forte
       ======================================================================== */
    bool incoherenceForte = (compteur > 0 && !occLocal && !occAmont && !occAval);

    if (incoherenceForte)
    {
        s_incoherenceTimer++;

        SA_LOG_WARN("[Essieux] Incohérence forte (%d/%d)\n",
                    s_incoherenceTimer, INCOHERENCE_TIMEOUT);

        if (s_incoherenceTimer >= INCOHERENCE_TIMEOUT)
        {
            s_canton->resetCompteurEssieux();   // ⬅️ NOUVEAU
            s_incoherenceTimer = 0;

            SA_LOG_ERROR("[Essieux] Reset compteur (incohérence persistante)\n");
        }

        return;
    }

    /* ========================================================================
       4) compteur > 0 mais canton local libre → incohérence faible
       ======================================================================== */
    bool incoherenceFaible = (compteur > 0 && !occLocal);

    if (incoherenceFaible)
    {
        s_incoherenceTimer++;

        SA_LOG_WARN("[Essieux] Incohérence faible (%d/%d)\n",
                    s_incoherenceTimer, INCOHERENCE_TIMEOUT);

        if (s_incoherenceTimer >= INCOHERENCE_TIMEOUT)
        {
            s_canton->resetCompteurEssieux();   // ⬅️ NOUVEAU
            s_incoherenceTimer = 0;

            SA_LOG_ERROR("[Essieux] Reset compteur (incohérence faible persistante)\n");
        }

        return;
    }

    /* ========================================================================
       5) Tout est cohérent → reset du timer
       ======================================================================== */
    s_incoherenceTimer = 0;

    SA_LOG_TRACE("[Essieux] OK (local=%d, amont=%d, aval=%d, essieux=%d)\n",
                 occLocal, occAmont, occAval, compteur);
}
