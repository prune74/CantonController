#include "CC_CAN_CLF.h"
#include <Protocol.h>
#include "debug_cc.h"
#include "Canton.h"
#include "CC_CAN.h"

static uint32_t g_CLF_timeEpoch = 0;

void CC_CAN_CLF::updateCLFTime(uint32_t epoch)
{
    g_CLF_timeEpoch = epoch;
}

uint32_t CC_CAN_CLF::getCLFTime()
{
    return g_CLF_timeEpoch;
}

namespace CC_CAN_CLF
{

    void handleCLFCommand(uint8_t commande, const CanMsg &msg, Canton *canton, uint16_t idExpediteur)
    {
        switch ((Cmd_CLF_to_CC)commande)
        {
        case Cmd_CLF_to_CC::TRAIN_VALIDE:
        {
            if (msg.dlc < 5)
                return;

            uint16_t trainID = msg.data[0];

            // Reconstruction du timestamp epoch (4 octets)
            uint32_t expirationEpoch =
                ((uint32_t)msg.data[1] << 24) |
                ((uint32_t)msg.data[2] << 16) |
                ((uint32_t)msg.data[3] << 8) |
                ((uint32_t)msg.data[4]);

            canton->activateSilenceEpoch(trainID, expirationEpoch);

            CC_LOG_INFO("[CLF][CC] TRAIN_VALIDE : train=%u silence jusqu'à epoch=%u",
                        trainID, expirationEpoch);
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

        case Cmd_CLF_to_CC::CLF_TIME:
        {
            if (msg.dlc < 4)
                return;

            uint32_t epoch =
                ((uint32_t)msg.data[0] << 24) |
                ((uint32_t)msg.data[1] << 16) |
                ((uint32_t)msg.data[2] << 8) |
                ((uint32_t)msg.data[3]);

            CC_CAN_CLF::updateCLFTime(epoch);

            CC_LOG_INFO("[CLF][CC] CLF_TIME reçu : epoch=%u", epoch);
            return;
        }

        default:
            return;
        }
    }

}
