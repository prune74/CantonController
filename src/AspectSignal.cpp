/*
 * AspectSignal.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction et envoi des aspects SNCF + feux directionnels.
 */

#include "AspectSignal.h"
#include "FeuxDirection.h"
#include "CC_CAN_EXCC.h"
#include "CC_CAN.h"
#include "Protocol.h"
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
    ExccAspect aspectAval = ExccAspect::ASPECT_CARRE;   // côté H
    ExccAspect aspectAvalAH = ExccAspect::ASPECT_CARRE; // côté AH

    // -----------------------------------------------------------------------
    // 2) Récupération des aspects aval SP1 / SM1
    // -----------------------------------------------------------------------
    CantonPeriph *sp1 = canton->voisinSP1();
    CantonPeriph *sm1 = canton->voisinSM1();

    if (sp1)
        aspectAval = static_cast<ExccAspect>(sp1->aspectRecu[1]);

    if (sm1)
        aspectAvalAH = static_cast<ExccAspect>(sm1->aspectRecu[0]);

    // -----------------------------------------------------------------------
    // 3) Vérification des aiguilles locales
    // -----------------------------------------------------------------------
    Aig *aigSP1 = canton->getAig(0); // côté H
    Aig *aigSM1 = canton->getAig(3); // côté AH

    bool voieDevieSP1 = (aigSP1 && !aigSP1->estDroit());
    bool voieDevieSM1 = (aigSM1 && !aigSM1->estDroit());

    // -----------------------------------------------------------------------
    // 4) Déduction des aspects locaux (BAL)
    // -----------------------------------------------------------------------
    signalValue[0] = static_cast<uint8_t>(
        deduireAspectDepuisAval(aspectAval, voieDevieSP1));

    signalValue[1] = static_cast<uint8_t>(
        deduireAspectDepuisAval(aspectAvalAH, voieDevieSM1));

    // -----------------------------------------------------------------------
    // 5) Application du MODE MANOEUVRE (voie de service)
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
    // 6) Déduction du type de mât + mise à jour des objets Signal
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
    // 7) Calcul des feux directionnels
    // -----------------------------------------------------------------------
    canton->updateFeuDirection(SensHoraire);
    canton->updateFeuDirection(SensAntiHoraire);

    uint8_t dirValue0 = canton->getFeuDirection(SensHoraire);
    uint8_t dirValue1 = canton->getFeuDirection(SensAntiHoraire);

    // -----------------------------------------------------------------------
    // 8) Envoi conditionnel (anti‑spam + changement)
    // -----------------------------------------------------------------------
    if ((signalValue[0] != oldSignalValue0 ||
         signalValue[1] != oldSignalValue1 ||
         dirValue0 != oldDirValue0 ||
         dirValue1 != oldDirValue1) &&
        (now - lastEnvoi > tempoEnvoi))
    {
        // ---------------------------------------------------------------
        // 8A) ASPECT HORAIRE
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
        // 8B) ASPECT ANTI‑HORAIRE
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
        // 8C) FEUX DIRECTIONNELS
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
        // 8D) OCCUPATION DES CANTONS VOISINS
        // ---------------------------------------------------------------
        uint8_t occVoisins = 0;

        if (sp1 && sp1->busy())
            occVoisins |= 0x01;
        if (sm1 && sm1->busy())
            occVoisins |= 0x02;

        CC_CAN_EXCC::sendOccupationVoisins(canton, occVoisins);

        lastEnvoi = now;
    }
}
