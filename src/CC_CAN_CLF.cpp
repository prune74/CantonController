#include "CC_CAN_CLF.h"
#include "Protocol.h"
#include "debug_cc.h"
#include "Canton.h"
#include "CC_CAN.h"

namespace CC_CAN_CLF
{

    void handleCLFCommand(uint8_t commande, const CanMsg &msg, Canton *canton, uint16_t idExpediteur)
    {
        switch ((Cmd_CLF_to_CC)commande)
        {
        case Cmd_CLF_to_CC::TRAIN_VALIDE:
        {
            if (msg.dlc < 3)
                return;

            uint16_t trainID = msg.data[0];
            uint16_t dureeMinutes = msg.data[1] | (msg.data[2] << 8);

            canton->activateSilence(trainID, dureeMinutes);

            CC_LOG_INFO("[CLF][CC] TRAIN_VALIDE : train=%u silence=%u minutes",
                        trainID, dureeMinutes);
            return;
        }

        case Cmd_CLF_to_CC::TRAIN_REARMER:
        {
            if (msg.dlc < 1)
                return;

            uint16_t trainID = msg.data[0];

            canton->deactivateSilence(trainID);

            CC_LOG_INFO("[CLF][CC] TRAIN_REARMER : train=%u", trainID);
            return;
        }

        case Cmd_CLF_to_CC::DEMANDE_MESURE:
        {
            if (msg.dlc < 1)
                return;

            uint16_t trainID = msg.data[0];

            // ---------------------------------------------------------------
            // 1) Le CC doit être armé via EB
            // ---------------------------------------------------------------
            if (!canton->mesureVitesseDisponible())
            {
                CC_LOG_WARN("[CLF][CC] DEMANDE_MESURE ignorée : aucune mesure disponible");
                return;
            }

            // ---------------------------------------------------------------
            // 2) La mesure doit correspondre au train demandé
            // ---------------------------------------------------------------
            if (canton->locoMesuree() != trainID)
            {
                CC_LOG_WARN("[CLF][CC] DEMANDE_MESURE ignorée : train %u ≠ mesure %u",
                            trainID, canton->locoMesuree());
                return;
            }

            // ---------------------------------------------------------------
            // 3) Envoi immédiat de la mesure déjà calculée
            // ---------------------------------------------------------------
            float v = canton->vitesseMesuree();
            uint16_t v1000 = (uint16_t)(v * 1000.0f);

            CC_LOG_INFO("[CLF][CC] DEMANDE_MESURE : renvoi vitesse %.3f pour train %u",
                        v, trainID);

            CC_CAN::sendMsg(
                0,
                (uint8_t)Cmd_CC_to_CLF::MESURE_VITESSE,
                CLF_ID,
                canton->ID(),
                trainID >> 8,
                trainID & 0xFF,
                v1000 >> 8,
                v1000 & 0xFF);

            // ---------------------------------------------------------------
            // 4) Renvoi du nombre d'essieux
            // ---------------------------------------------------------------
            uint8_t essieux = canton->essieuxMesures();

            CC_CAN::sendMsg(
                0,
                (uint8_t)Cmd_CC_to_CLF::ESSIEUX_TRAIN,
                CLF_ID,
                canton->ID(),
                trainID >> 8,
                trainID & 0xFF,
                essieux);

            return;
        }

        default:
            return;
        }
    }

}
