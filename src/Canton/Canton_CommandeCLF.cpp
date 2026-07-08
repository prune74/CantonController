#include "Canton.h"
#include "Protocol.h"
#include "CC_CAN.h"
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
        if (msg.dlc < 3)
            return;

        uint16_t trainID = msg.data[0];
        uint16_t duree   = msg.data[1] | (msg.data[2] << 8);

        activateSilence(trainID, duree);

        CC_LOG_INFO("[CLF][CC] TRAIN_VALIDE : train=%u silence=%u ms",
                    trainID, duree);
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
            v1000 & 0xFF
        );

        // 4) Envoi essieux
        uint8_t essieux = essieuxMesures();

        CC_CAN::sendMsg(
            0,
            (uint8_t)Cmd_CC_to_CLF::ESSIEUX_TRAIN,
            CLF_ID,
            ID(),
            trainID >> 8,
            trainID & 0xFF,
            essieux
        );

        break;
    }

    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// Gestion du silence CC pour un train
// ---------------------------------------------------------------------------
void Canton::activateSilence(uint16_t trainID, uint32_t dureeMinutes)
{
    m_silenceTrainID  = trainID;
    m_silenceExpireAt = millis() + (dureeMinutes * 60000UL);  // minutes → ms
    m_silenceActive   = true;

    CC_LOG_INFO("[CLF][CC] Silence activé pour train %u (%u minutes)",
                trainID, dureeMinutes);
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

    if (millis() >= m_silenceExpireAt)
        return false;

    return true;
}
