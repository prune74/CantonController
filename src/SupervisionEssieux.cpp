#include "SupervisionEssieux.h"
#include "CompteurEssieuxUart.h"
#include "Node.h"
#include "debug_sa.h"

/*
 * Variables internes statiques
 */

Node*     SupervisionEssieux::s_node = nullptr;
bool      SupervisionEssieux::s_rebootDetecte = false;
uint16_t  SupervisionEssieux::s_incoherenceTimer = 0;
const uint16_t SupervisionEssieux::INCOHERENCE_TIMEOUT;

void SupervisionEssieux::begin(Node* node)
{
    s_node = node;
    s_incoherenceTimer = 0;

    SA_LOG_INFO("[Essieux] Supervision initialisée pour Node %d\n", node->ID());
}

void SupervisionEssieux::notifierRebootEXSA()
{
    s_rebootDetecte = true;
    SA_LOG_WARN("[Essieux] Reboot EXSA détecté → compteur invalide\n");
}

void SupervisionEssieux::loop()
{
    if (!s_node)
        return;

    verifierCoherence();
}

void SupervisionEssieux::verifierCoherence()
{
    // ------------------------------------------------------------------------
    // Récupération des états ferroviaires
    // ------------------------------------------------------------------------
    int  compteur  = CompteurEssieuxUart::compteurGlobal();
    bool occLocal  = s_node->busy();  // Occupation logique du canton local
    bool occAmont  = false;
    bool occAval   = s_node->SP2_busy() || s_node->SM2_busy();

    // Canton amont occupé ?
    NodePeriph* sp1 = s_node->getNodeP(s_node->SP1_idx());
    if (sp1)
        occAmont |= sp1->busy();

    NodePeriph* sm1 = s_node->getNodeP(s_node->SM1_idx());
    if (sm1)
        occAmont |= sm1->busy();

    /* ========================================================================
       1) Reboot EXSA → reset immédiat
       ======================================================================== */
    if (s_rebootDetecte)
    {
        CompteurEssieuxUart::reset();
        s_rebootDetecte     = false;
        s_incoherenceTimer  = 0;

        SA_LOG_WARN("[Essieux] Reset compteur (reboot EXSA)\n");
        return;
    }

    /* ========================================================================
       2) compteur < 0 → impossible physiquement → reset immédiat
       ======================================================================== */
    if (compteur < 0)
    {
        CompteurEssieuxUart::reset();
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
            CompteurEssieuxUart::reset();
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
            CompteurEssieuxUart::reset();
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
