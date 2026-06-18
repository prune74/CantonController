/*
 * SupervisionEssieux.cpp — Gestion Canton 2026
 * ------------------------------------------------------------
 * Module de supervision de la cohérence essieux pour le
 * Canton Controller (CC).
 *
 * Ce module vérifie la cohérence entre :
 *   - l’occupation logique du canton (busy)
 *   - le compteur global d’essieux (fourni par l’EXCC)
 *   - l’état des cantons voisins (amont / aval)
 *   - les reboot EXCC (perte d’état interne)
 *
 * Objectifs :
 *   - détecter les incohérences ferroviaires
 *   - remettre le compteur à zéro dans les cas sûrs
 *   - éviter les faux positifs grâce à un timer interne
 *
 * Remarque :
 *   Ce module NE MODIFIE JAMAIS Canton::busy()
 *   (l’occupation logique reste gérée par ConsoCourant).
 */

#include "SupervisionEssieux.h"
#include "Canton.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Variables statiques internes
// ---------------------------------------------------------------------------
Canton *SupervisionEssieux::s_canton = nullptr;
bool SupervisionEssieux::s_rebootDetecte = false;
uint16_t SupervisionEssieux::s_incoherenceTimer = 0;
const uint16_t SupervisionEssieux::INCOHERENCE_TIMEOUT;

// ---------------------------------------------------------------------------
// begin()
// ---------------------------------------------------------------------------
void SupervisionEssieux::begin(Canton *canton)
{
    s_canton = canton;
    s_incoherenceTimer = 0;

    CC_LOG_INFO("[Essieux][CC] Supervision initialisée pour Canton %d\n", canton->ID());
}

// ---------------------------------------------------------------------------
// notifierRebootEXCC()
// ---------------------------------------------------------------------------
void SupervisionEssieux::notifierRebootEXCC()
{
    s_rebootDetecte = true;
    CC_LOG_WARN("[Essieux][CC] Reboot EXCC détecté → compteur invalide\n");
}

// ---------------------------------------------------------------------------
// loop()
// ---------------------------------------------------------------------------
void SupervisionEssieux::loop()
{
    if (!s_canton)
        return;

    verifierCoherence();
}

// ---------------------------------------------------------------------------
// verifierCoherence()
// Vérifie toutes les règles de cohérence ferroviaire
// ---------------------------------------------------------------------------
void SupervisionEssieux::verifierCoherence()
{
    // ------------------------------------------------------------------------
    // Récupération des états ferroviaires
    // ------------------------------------------------------------------------
    int compteur = s_canton->compteurEssieux(); // compteur global EXCC
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

    // ------------------------------------------------------------------------
    // 1) Reboot EXCC → reset immédiat
    // ------------------------------------------------------------------------
    if (s_rebootDetecte)
    {
        s_canton->resetCompteurEssieux();
        s_rebootDetecte = false;
        s_incoherenceTimer = 0;

        CC_LOG_WARN("[Essieux][CC] Reset compteur (reboot EXCC)\n");
        return;
    }

    // ------------------------------------------------------------------------
    // 2) compteur < 0 → impossible physiquement → reset immédiat
    // ------------------------------------------------------------------------
    if (compteur < 0)
    {
        s_canton->resetCompteurEssieux();
        s_incoherenceTimer = 0;

        CC_LOG_ERROR("[Essieux][CC] Reset compteur (compteur < 0)\n");
        return;
    }

    // ------------------------------------------------------------------------
    // 3) compteur > 0 mais tout est libre → incohérence forte
    // ------------------------------------------------------------------------
    bool incoherenceForte = (compteur > 0 && !occLocal && !occAmont && !occAval);

    if (incoherenceForte)
    {
        s_incoherenceTimer++;

        CC_LOG_WARN("[Essieux][CC] Incohérence forte (%d/%d)\n",
                    s_incoherenceTimer, INCOHERENCE_TIMEOUT);

        if (s_incoherenceTimer >= INCOHERENCE_TIMEOUT)
        {
            s_canton->resetCompteurEssieux();
            s_incoherenceTimer = 0;

            CC_LOG_ERROR("[Essieux][CC] Reset compteur (incohérence persistante)\n");
        }

        return;
    }

    // ------------------------------------------------------------------------
    // 4) compteur > 0 mais canton local libre → incohérence faible
    // ------------------------------------------------------------------------
    bool incoherenceFaible = (compteur > 0 && !occLocal);

    if (incoherenceFaible)
    {
        s_incoherenceTimer++;

        CC_LOG_WARN("[Essieux][CC] Incohérence faible (%d/%d)\n",
                    s_incoherenceTimer, INCOHERENCE_TIMEOUT);

        if (s_incoherenceTimer >= INCOHERENCE_TIMEOUT)
        {
            s_canton->resetCompteurEssieux();
            s_incoherenceTimer = 0;

            CC_LOG_ERROR("[Essieux][CC] Reset compteur (incohérence faible persistante)\n");
        }

        return;
    }

    // ------------------------------------------------------------------------
    // 5) Tout est cohérent → reset du timer
    // ------------------------------------------------------------------------
    s_incoherenceTimer = 0;

    CC_LOG_TRACE("[Essieux][CC] OK (local=%d, amont=%d, aval=%d, essieux=%d)\n",
                 occLocal, occAmont, occAval, compteur);
}
