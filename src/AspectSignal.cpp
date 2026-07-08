/*
 * AspectSignal.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction et envoi des aspects SNCF + feux directionnels.
 */

#include "AspectSignal.h"
#include "FeuxDirection.h"
#include "CC_CAN_EXCC.h"
#include "CC_CAN.h"
#include <Protocol.h>
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
    // 1) Valeurs par défaut : CARRÉ
    // -----------------------------------------------------------------------
    ExccAspect aspectAvalH = ExccAspect::ASPECT_CARRE;  // côté H
    ExccAspect aspectAvalAH = ExccAspect::ASPECT_CARRE; // côté AH

    // -----------------------------------------------------------------------
    // 2) Récupération des aspects aval SP1 / SP2  (côté H)
    // -----------------------------------------------------------------------
    CantonPeriph *voisinH = canton->voisinSP1();
    if (!voisinH)
        voisinH = canton->voisinSP2(); // fallback si SP1 absent

    if (voisinH)
        aspectAvalH = static_cast<ExccAspect>(voisinH->aspectRecu[1]);

    // -----------------------------------------------------------------------
    // 3) Récupération des aspects aval SM1 / SM2 (côté AH)
    // -----------------------------------------------------------------------
    CantonPeriph *voisinAH = canton->voisinSM1();
    if (!voisinAH)
        voisinAH = canton->voisinSM2(); // fallback si SM1 absent

    if (voisinAH)
        aspectAvalAH = static_cast<ExccAspect>(voisinAH->aspectRecu[0]);

    // -----------------------------------------------------------------------
    // 4) Vérification des aiguilles locales
    // -----------------------------------------------------------------------
    Aig *aigSP = canton->getAig(0); // côté H
    Aig *aigSM = canton->getAig(3); // côté AH

    bool voieDevieSP = (aigSP && !aigSP->estDroit());
    bool voieDevieSM = (aigSM && !aigSM->estDroit());

    // -----------------------------------------------------------------------
    // 5) Déduction des aspects locaux (BAL)
    // -----------------------------------------------------------------------
    signalValue[0] = static_cast<uint8_t>(
        deduireAspectDepuisAval(aspectAvalH, voieDevieSP));

    signalValue[1] = static_cast<uint8_t>(
        deduireAspectDepuisAval(aspectAvalAH, voieDevieSM));

    // -----------------------------------------------------------------------
    // 5bis) Stockage de l’aspect local dans le canton (pour PilotageDistribue)
    // -----------------------------------------------------------------------
    canton->setAspectLocal(SensHoraire, signalValue[0]);
    canton->setAspectLocal(SensAntiHoraire, signalValue[1]);

    // -----------------------------------------------------------------------
    // 6) Application du MODE MANOEUVRE (voie de service)
    // -----------------------------------------------------------------------
    if (canton->modeManoeuvre())
    {
        for (int i = 0; i < 2; i++)
        {
            ExccAspect asp = static_cast<ExccAspect>(signalValue[i]);

            if (asp == ExccAspect::ASPECT_CARRE)
            {
                signalValue[i] = static_cast<uint8_t>(ExccAspect::ASPECT_CARRE_VIOLET);
                continue;
            }

            if (asp == ExccAspect::ASPECT_VOIE_LIBRE)
            {
                signalValue[i] = static_cast<uint8_t>(ExccAspect::ASPECT_MANOEUVRE);
                continue;
            }

            signalValue[i] = static_cast<uint8_t>(asp);
        }
    }

    // -----------------------------------------------------------------------
    // 7) Déduction du type de mât + mise à jour des objets Signal
    // -----------------------------------------------------------------------
    uint8_t typeMatH = canton->deduireTypeSignal(SensHoraire);
    uint8_t typeMatAH = canton->deduireTypeSignal(SensAntiHoraire);

    Signal *sH = canton->getSignal(0);
    if (sH)
        sH->type(typeMatH);

    Signal *sAH = canton->getSignal(1);
    if (sAH)
        sAH->type(typeMatAH);

    // -----------------------------------------------------------------------
    // 8) Calcul des feux directionnels
    // -----------------------------------------------------------------------
    canton->updateFeuDirection(SensHoraire);
    canton->updateFeuDirection(SensAntiHoraire);

    uint8_t dirValue0 = canton->getFeuDirection(SensHoraire);
    uint8_t dirValue1 = canton->getFeuDirection(SensAntiHoraire);

    // -----------------------------------------------------------------------
    // 9) Envoi conditionnel (anti‑spam + changement)
    // -----------------------------------------------------------------------
    if ((signalValue[0] != oldSignalValue0 ||
         signalValue[1] != oldSignalValue1 ||
         dirValue0 != oldDirValue0 ||
         dirValue1 != oldDirValue1) &&
        (now - lastEnvoi > tempoEnvoi))
    {
        // ---------------------------------------------------------------
        // 9A) ASPECT HORAIRE
        // ---------------------------------------------------------------
        if (signalValue[0] != oldSignalValue0)
        {
            CC_CAN_EXCC::sendAspectHoraire(
                canton,
                static_cast<ExccAspect>(signalValue[0]));

            oldSignalValue0 = signalValue[0];

            CC_CAN::sendMsg(
                1,
                (uint16_t)Cmd_CC_to_EXCC::ASPECT_HORAIRE,
                0,
                canton->ID(),
                0, 0,
                signalValue[0], 0);
        }

        // ---------------------------------------------------------------
        // 9B) ASPECT ANTI‑HORAIRE
        // ---------------------------------------------------------------
        if (signalValue[1] != oldSignalValue1)
        {
            CC_CAN_EXCC::sendAspectAntiHoraire(
                canton,
                static_cast<ExccAspect>(signalValue[1]));

            oldSignalValue1 = signalValue[1];

            CC_CAN::sendMsg(
                1,
                (uint16_t)Cmd_CC_to_EXCC::ASPECT_ANTIHORAIRE,
                0,
                canton->ID(),
                1, 1,
                signalValue[1], 0);
        }

        // ---------------------------------------------------------------
        // 9C) FEUX DIRECTIONNELS
        // ---------------------------------------------------------------
        if (dirValue0 != oldDirValue0)
        {
            CC_CAN_EXCC::sendFeuDirectionHoraire(canton, dirValue0);
            oldDirValue0 = dirValue0;
        }

        if (dirValue1 != oldDirValue1)
        {
            CC_CAN_EXCC::sendFeuDirectionAntiHoraire(canton, dirValue1);
            oldDirValue1 = dirValue1;
        }

        // ---------------------------------------------------------------
        // 9D) OCCUPATION DES CANTONS VOISINS
        // ---------------------------------------------------------------
        uint8_t occVoisins = 0;

        if (voisinH && voisinH->busy())
            occVoisins |= 0x01;
        if (voisinAH && voisinAH->busy())
            occVoisins |= 0x02;

        CC_CAN_EXCC::sendOccupationVoisins(canton, occVoisins);

        lastEnvoi = now;
    }
}
