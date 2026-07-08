#include "Canton.h"
#include "Protocol.h"
#include "CC_CAN.h"
#include "CC_CAN_CLF.h"
#include "debug_cc.h"

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
