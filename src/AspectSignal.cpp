/*
 * AspectSignal.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction et envoi des aspects SNCF + feux directionnels.
 *
 * Rôle :
 *   - récupérer les aspects aval SP1 / SM1
 *   - vérifier les aiguilles locales
 *   - déduire l’aspect SNCF local (via DeductionAspect)
 *   - déduire automatiquement le type de mât (topologie / aspect 2026)
 *   - mettre à jour les objets Signal (type) pour sauvegarde JSON
 *   - calculer les feux directionnels (FeuxDirection)
 *   - envoyer les aspects / feux / occupation voisins (anti‑spam)
 *
 * IMPORTANT :
 *   Ce module ne contient aucune logique ferroviaire globale :
 *     → il applique uniquement les règles locales du canton.
 */

#include "AspectSignal.h"
#include "FeuxDirection.h"
#include "SatTopologieUART.h"
#include "CanMsg.h"
#include "Exploration_Protocol.h"
#include "DeductionAspect.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Anti‑spam : mémorisation des derniers envois
// ---------------------------------------------------------------------------
static uint8_t oldSignalValue0 = 255; // H
static uint8_t oldSignalValue1 = 255; // AH

static uint8_t oldDirValue0 = 255; // H
static uint8_t oldDirValue1 = 255; // AH

static TickType_t lastEnvoi = 0;
const TickType_t tempoEnvoi = pdMS_TO_TICKS(300);

// ---------------------------------------------------------------------------
// Fonction principale : mettreAJourAspectSignal()
// ---------------------------------------------------------------------------
void mettreAJourAspectSignal(Canton *canton, uint8_t *signalValue)
{
    TickType_t now = xTaskGetTickCount();

    // -----------------------------------------------------------------------
    // 1) Valeurs par défaut : carré
    // -----------------------------------------------------------------------
    ExccAspect aspectAval   = ASPECT_CARRE; // côté H
    ExccAspect aspectAvalAH = ASPECT_CARRE; // côté AH

    // -----------------------------------------------------------------------
    // 2) Récupération des voisins SP1 / SM1
    // -----------------------------------------------------------------------
    CantonPeriph *sp1 = canton->getCantonP(canton->SP1_idx());
    CantonPeriph *sm1 = canton->getCantonP(canton->SM1_idx());

    if (sp1)
        aspectAval = static_cast<ExccAspect>(sp1->aspectRecu[0]);
    if (sm1)
        aspectAvalAH = static_cast<ExccAspect>(sm1->aspectRecu[1]);

    // -----------------------------------------------------------------------
    // 3) Vérification des aiguilles locales
    // -----------------------------------------------------------------------
    Aig *aigSP1 = canton->getAig(0); // côté H
    Aig *aigSM1 = canton->getAig(3); // côté AH

    bool voieDevieSP1 = (aigSP1 && !aigSP1->estDroit());
    bool voieDevieSM1 = (aigSM1 && !aigSM1->estDroit());

    // -----------------------------------------------------------------------
    // 4) Déduction des aspects locaux (SNCF)
    // -----------------------------------------------------------------------
    signalValue[0] = static_cast<uint8_t>(
        deduireAspectDepuisAval(aspectAval, voieDevieSP1));

    signalValue[1] = static_cast<uint8_t>(
        deduireAspectDepuisAval(aspectAvalAH, voieDevieSM1));

    // -----------------------------------------------------------------------
    // 4bis) Déduction du type de mât + mise à jour des objets Signal
    // -----------------------------------------------------------------------
    uint8_t typeMatH  = canton->deduireTypeSignal(SensHoraire);
    uint8_t typeMatAH = canton->deduireTypeSignal(SensAntiHoraire);

    // Mise à jour du type dans les objets Signal (pour sauvegarde JSON)
    Signal *sH = canton->getSignal(0);
    if (sH) sH->type(typeMatH);

    Signal *sAH = canton->getSignal(1);
    if (sAH) sAH->type(typeMatAH);

    // -----------------------------------------------------------------------
    // 5) Calcul des feux directionnels (Exploration 2026)
    // -----------------------------------------------------------------------
    canton->updateFeuDirection(SensHoraire);
    canton->updateFeuDirection(SensAntiHoraire);

    uint8_t dirValue0 = canton->getFeuDirection(SensHoraire);
    uint8_t dirValue1 = canton->getFeuDirection(SensAntiHoraire);

    // -----------------------------------------------------------------------
    // 6) Envoi conditionnel (anti‑spam + changement)
    // -----------------------------------------------------------------------
    if ((signalValue[0] != oldSignalValue0 ||
         signalValue[1] != oldSignalValue1 ||
         dirValue0 != oldDirValue0 ||
         dirValue1 != oldDirValue1) &&
        (now - lastEnvoi > tempoEnvoi))
    {
        // ---------------------------------------------------------------
        // 6A) ASPECT HORAIRE
        // ---------------------------------------------------------------
        if (signalValue[0] != oldSignalValue0)
        {
            envoyerAspectSignalHoraire(signalValue[0]);
            oldSignalValue0 = signalValue[0];

            CanMsg::sendMsg(
                1, PROTO_E6_ASPECT_HORAIRE, 0, canton->ID(),
                0, 0,
                signalValue[0], 0);
        }

        // ---------------------------------------------------------------
        // 6B) ASPECT ANTI‑HORAIRE
        // ---------------------------------------------------------------
        if (signalValue[1] != oldSignalValue1)
        {
            envoyerAspectSignalAntiHoraire(signalValue[1]);
            oldSignalValue1 = signalValue[1];

            CanMsg::sendMsg(
                1, PROTO_E7_ASPECT_ANTIHORAIRE, 0, canton->ID(),
                1, 1,
                signalValue[1], 0);
        }

        // ---------------------------------------------------------------
        // 6C) FEUX DIRECTIONNELS
        // ---------------------------------------------------------------
        if (dirValue0 != oldDirValue0)
        {
            envoyerFeuDirectionHoraire(dirValue0);
            oldDirValue0 = dirValue0;
        }

        if (dirValue1 != oldDirValue1)
        {
            envoyerFeuDirectionAntiHoraire(dirValue1);
            oldDirValue1 = dirValue1;
        }

        // ---------------------------------------------------------------
        // 6D) OCCUPATION DES CANTONS VOISINS
        // ---------------------------------------------------------------
        uint8_t occVoisins = 0;

        if (sp1 && sp1->busy())
            occVoisins |= 0x01;
        if (sm1 && sm1->busy())
            occVoisins |= 0x02;

        envoyerOccupationVoisins(occVoisins);

        lastEnvoi = now;
    }
}
