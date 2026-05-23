#include "AspectSignal.h"
#include "FeuxDirection.h"
#include "SatTopologieUART.h"
#include "CanMsg.h"
#include "SA_EXSA_Protocol.h"      // pour ExsaAspect (enum)
#include "DeductionAspect.h"       // déduction SNCF (version enum)

/*************************************************************************************
 *  Module AspectSignal — Version ENUM (Option A)
 *************************************************************************************/

// Derniers aspects envoyés (permet d’éviter les envois inutiles)
static uint8_t oldSignalValue0 = 255;  // Horaire
static uint8_t oldSignalValue1 = 255;  // Anti-horaire

// Derniers feux directionnels envoyés
static uint8_t oldDirValue0 = 255;     // Horaire
static uint8_t oldDirValue1 = 255;     // Anti-horaire

// Temporisation entre deux envois (anti-spam)
static TickType_t lastEnvoi = 0;
const TickType_t tempoEnvoi = pdMS_TO_TICKS(300);


/*************************************************************************************
 *  Fonction principale : mettreAJourAspectSignal()
 *************************************************************************************/
void mettreAJourAspectSignal(Node* node, uint8_t* signalValue)
{
    TickType_t now = xTaskGetTickCount();

    /**************************************************************************
     * 1) Valeurs par défaut : carré
     **************************************************************************/
    ExsaAspect aspectAval   = ASPECT_CARRE;  // côté horaire
    ExsaAspect aspectAvalAH = ASPECT_CARRE;  // côté anti-horaire


    /**************************************************************************
     * 2) Récupération des voisins SP1 / SM1 via getters Discovery 2026
     **************************************************************************/
    NodePeriph* sp1 = node->getNodeP(node->SP1_idx());
    NodePeriph* sm1 = node->getNodeP(node->SM1_idx());

    if (sp1) aspectAval   = static_cast<ExsaAspect>(sp1->aspectRecu[0]);
    if (sm1) aspectAvalAH = static_cast<ExsaAspect>(sm1->aspectRecu[1]);


    /**************************************************************************
     * 3) Vérification des aiguilles via getters Discovery 2026
     **************************************************************************/
    Aig* aigSP1 = node->getAig(0);   // côté horaire
    Aig* aigSM1 = node->getAig(3);   // côté anti-horaire

    bool voieDevieSP1 = (aigSP1 && !aigSP1->estDroit());
    bool voieDevieSM1 = (aigSM1 && !aigSM1->estDroit());


    /**************************************************************************
     * 4) Déduction des aspects locaux (SNCF)
     **************************************************************************/
    signalValue[0] = static_cast<uint8_t>(
        deduireAspectDepuisAval(aspectAval, voieDevieSP1)
    );

    signalValue[1] = static_cast<uint8_t>(
        deduireAspectDepuisAval(aspectAvalAH, voieDevieSM1)
    );


    /**************************************************************************
     * 5) Calcul des feux directionnels (nouvelle architecture Discovery 2026)
     **************************************************************************/

    // Mise à jour interne du Node (code-barres + aiguilles + occupation)
    node->updateFeuDirection(SensHoraire);
    node->updateFeuDirection(SensAntiHoraire);

    // Lecture du résultat (0..4)
    uint8_t dirValue0 = node->getFeuDirection(SensHoraire);     // H
    uint8_t dirValue1 = node->getFeuDirection(SensAntiHoraire); // AH
    

    /**************************************************************************
     * 6) Envoi conditionnel (anti-spam + changement)
     **************************************************************************/
    if ((signalValue[0] != oldSignalValue0 ||
         signalValue[1] != oldSignalValue1 ||
         dirValue0      != oldDirValue0   ||
         dirValue1      != oldDirValue1) &&
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
                1, PROTO_E6_ASPECT_HORAIRE, 0, node->ID(),
                0, 0,
                signalValue[0], 0
            );
        }

        /**********************************************************************
         * 6B) ASPECT ANTI-HORAIRE
         **********************************************************************/
        if (signalValue[1] != oldSignalValue1)
        {
            envoyerAspectSignalAntiHoraire(signalValue[1]);
            oldSignalValue1 = signalValue[1];

            CanMsg::sendMsg(
                1, PROTO_E7_ASPECT_ANTIHORAIRE, 0, node->ID(),
                1, 1,
                signalValue[1], 0
            );
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

        if (sp1 && sp1->busy()) occVoisins |= 0x01;
        if (sm1 && sm1->busy()) occVoisins |= 0x02;

        envoyerOccupationVoisins(occVoisins);

        lastEnvoi = now;
    }
}
