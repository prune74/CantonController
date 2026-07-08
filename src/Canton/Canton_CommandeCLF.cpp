#include "Canton.h"
#include "Protocol.h"
#include "CC_CAN.h"
#include "CC_CAN_CLF.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Commandes CLF → CC
// ---------------------------------------------------------------------------
void Canton::onCommandeCLF(uint8_t commande, const CanMsg &msg)
{
    switch ((Cmd_CLF_to_CC)commande)
    {
    case Cmd_CLF_to_CC::TRAIN_VALIDE:
    {
        if (msg.dlc < 5)
            return;

        uint16_t trainID = msg.data[0];

        uint32_t expirationEpoch =
            ((uint32_t)msg.data[1] << 24) |
            ((uint32_t)msg.data[2] << 16) |
            ((uint32_t)msg.data[3] << 8) |
            ((uint32_t)msg.data[4]);

        activateSilenceEpoch(trainID, expirationEpoch);

        CC_LOG_INFO("[CLF][CC] TRAIN_VALIDE : train=%u silence jusqu'à epoch=%u",
                    trainID, expirationEpoch);
        break;
    }

    case Cmd_CLF_to_CC::TRAIN_REARMER:
    {
        if (msg.dlc < 1)
            return;

        uint16_t trainID = msg.data[0];

        deactivateSilence(trainID);

        CC_LOG_INFO("[CLF][CC] TRAIN_REARMER : train=%u", trainID);
        break;
    }

    case Cmd_CLF_to_CC::DEMANDE_MESURE:
    {
        if (msg.dlc < 1)
            return;

        uint16_t trainID = msg.data[0];

        // 1) Vérifier qu'une mesure est disponible
        if (!mesureVitesseDisponible())
        {
            CC_LOG_WARN("[CLF][CC] DEMANDE_MESURE ignorée : aucune mesure disponible");
            break;
        }

        // 2) Vérifier que la mesure correspond au train demandé
        if (locoMesuree() != trainID)
        {
            CC_LOG_WARN("[CLF][CC] DEMANDE_MESURE ignorée : train %u ≠ mesure %u",
                        trainID, locoMesuree());
            break;
        }

        // 3) Envoi vitesse
        float v = vitesseMesuree();
        uint16_t v1000 = (uint16_t)(v * 1000.0f);

        CC_LOG_INFO("[CLF][CC] DEMANDE_MESURE : renvoi vitesse %.3f pour train %u",
                    v, trainID);

        CC_CAN::sendMsg(
            0,
            (uint8_t)Cmd_CC_to_CLF::MESURE_VITESSE,
            CLF_ID,
            ID(),
            trainID >> 8,
            trainID & 0xFF,
            v1000 >> 8,
            v1000 & 0xFF);

        // 4) Envoi essieux
        uint8_t essieux = essieuxMesures();

        CC_CAN::sendMsg(
            0,
            (uint8_t)Cmd_CC_to_CLF::ESSIEUX_TRAIN,
            CLF_ID,
            ID(),
            trainID >> 8,
            trainID & 0xFF,
            essieux);

        break;
    }

    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// Gestion du silence CC pour un train
// ---------------------------------------------------------------------------
void Canton::activateSilenceEpoch(uint16_t trainID, uint32_t expirationEpoch)
{
    m_silenceTrainID = trainID;
    m_silenceExpireAtEpoch = expirationEpoch;
    m_silenceActive = true;

    CC_LOG_INFO("[CC] Silence activé pour train %u jusqu'à epoch=%u",
                trainID, expirationEpoch);
}

void Canton::deactivateSilence(uint16_t trainID)
{
    if (m_silenceTrainID == trainID)
    {
        m_silenceActive = false;
        CC_LOG_INFO("[CLF][CC] Silence désactivé pour train %u", trainID);
    }
}

bool Canton::isSilent(uint16_t trainID) const
{
    if (!m_silenceActive)
        return false;

    if (m_silenceTrainID != trainID)
        return false;

    uint32_t now = CC_CAN_CLF::getCLFTime();

    return now < m_silenceExpireAtEpoch;
}
