#pragma once
#include "CanMsg.h"
#include "Canton.h"
#include <Protocol.h>
#include <vector>
#include <stdint.h>

namespace CC_CAN_CLF
{
    void handleCLFCommand(uint8_t commande, const CanMsg &msg, Canton *canton, uint16_t idExpediteur);

    // Gestion du temps CLF
    void updateCLFTime(uint32_t epoch);
    uint32_t getCLFTime();

    // ⭐ Gestion de la composition des trains
    void setComposition(uint16_t trainID, const std::vector<uint16_t> &wagons);
    const std::vector<uint16_t> &getComposition(uint16_t trainID);
}
