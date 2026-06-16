#include "AspectSignal.h"
#include "FeuxDirection.h"
#include "SatTopologieUART.h"
#include "CanMsg.h"
#include "Exploration_Protocol.h"
#include "DeductionAspect.h"

/*************************************************************************************
 *  Module AspectSignal — Version ENUM (Option A)
 *************************************************************************************/

// Derniers aspects envoyés (permet d’éviter les envois inutiles)
static uint8_t oldSignalValue0 = 255; // Horaire
static uint8_t oldSignalValue1 = 255; // Anti-horaire

// Derniers feux directionnels envoyés
static uint8_t oldDirValue0 = 255; // Horaire
static uint8_t oldDirValue1 = 255; // Anti-horaire

// Temporisation entre deux envois (anti-spam)
static TickType_t lastEnvoi = 0;
const TickType_t tempoEnvoi = pdMS_TO_TICKS(300);

/*************************************************************************************
 *  Fonction principale : mettreAJourAspectSignal()
 *************************************************************************************/
void mettreAJourAspectSignal(Canton *canton, uint8_t *signalValue)
{
    TickType_t now = xTaskGetTickCount();

    /**************************************************************************
     * 1) Valeurs par défaut : carré
     **************************************************************************/
    ExsaAspect aspectAval = ASPECT_CARRE;   // côté horaire
    ExsaAspect aspectAvalAH = ASPECT_CARRE; // côté anti-horaire

    /**************************************************************************
     * 2) Récupération des voisins SP1 / SM1 via getters Exploration 2026
     **************************************************************************/
    CantonPeriph *sp1 = canton->getCantonP(canton->SP1_idx());
    CantonPeriph *sm1 = canton->getCantonP(canton->SM1_idx());

    if (sp1)
        aspectAval = static_cast<ExsaAspect>(sp1->aspectRecu[0]);
    if (sm1)
        aspectAvalAH = static_cast<ExsaAspect>(sm1->aspectRecu[1]);

    /**************************************************************************
     * 3) Vérification des aiguilles via getters Exploration 2026
     **************************************************************************/
    Aig *aigSP1 = canton->getAig(0); // côté horaire
    Aig *aigSM1 = canton->getAig(3); // côté anti-horaire

    bool voieDevieSP1 = (aigSP1 && !aigSP1->estDroit());
    bool voieDevieSM1 = (aigSM1 && !aigSM1->estDroit());

    /**************************************************************************
     * 4) Déduction des aspects locaux (SNCF)
     **************************************************************************/
    signalValue[0] = static_cast<uint8_t>(
        deduireAspectDepuisAval(aspectAval, voieDevieSP1));

    signalValue[1] = static_cast<uint8_t>(
        deduireAspectDepuisAval(aspectAvalAH, voieDevieSM1));

    /**************************************************************************
     * 5) Calcul des feux directionnels (nouvelle architecture Exploration 2026)
     **************************************************************************/

    // Mise à jour interne du Canton (code-barres + aiguilles + occupation)
    canton->updateFeuDirection(SensHoraire);
    canton->updateFeuDirection(SensAntiHoraire);

    // Lecture du résultat (0..4)
    uint8_t dirValue0 = canton->getFeuDirection(SensHoraire);     // H
    uint8_t dirValue1 = canton->getFeuDirection(SensAntiHoraire); // AH

    /**************************************************************************
     * 6) Envoi conditionnel (anti-spam + changement)
     **************************************************************************/
    if ((signalValue[0] != oldSignalValue0 ||
         signalValue[1] != oldSignalValue1 ||
         dirValue0 != oldDirValue0 ||
         dirValue1 != oldDirValue1) &&
        (now - lastEnvoi > tempoEnvoi))
    {
        /**********************************************************************
         * 6A) ASPECT HORAIRE
         **********************************************************************/
        if (signalValue[0] != oldSignalValue0)
        {
            envoyerAspectSignalHoraire(signalValue[0]);
            oldSignalValue0 = signalValue[0];

            CanMsg::sendMsg(
                1, PROTO_E6_ASPECT_HORAIRE, 0, canton->ID(),
                0, 0,
                signalValue[0], 0);
        }

        /**********************************************************************
         * 6B) ASPECT ANTI-HORAIRE
         **********************************************************************/
        if (signalValue[1] != oldSignalValue1)
        {
            envoyerAspectSignalAntiHoraire(signalValue[1]);
            oldSignalValue1 = signalValue[1];

            CanMsg::sendMsg(
                1, PROTO_E7_ASPECT_ANTIHORAIRE, 0, canton->ID(),
                1, 1,
                signalValue[1], 0);
        }

        /**********************************************************************
         * 6C) FEUX DIRECTIONNELS
         **********************************************************************/
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

        /**********************************************************************
         * 6D) OCCUPATION DES CANTONS VOISINS
         **********************************************************************/
        uint8_t occVoisins = 0;

        if (sp1 && sp1->busy())
            occVoisins |= 0x01;
        if (sm1 && sm1->busy())
            occVoisins |= 0x02;

        envoyerOccupationVoisins(occVoisins);

        lastEnvoi = now;
    }
}
