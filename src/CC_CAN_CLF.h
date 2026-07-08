#pragma once
#include "CanMsg.h"
#include "Canton.h"
#include "Protocol.h"

namespace CC_CAN_CLF
{
    void handleCLFCommand(uint8_t commande, const CanMsg &msg, Canton *canton, uint16_t idExpediteur);
}

namespace CC_CAN_CLF
{
    void updateCLFTime(uint32_t epoch);
    uint32_t getCLFTime();
}
