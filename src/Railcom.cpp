#include "Railcom.h"
#include "debug_sa.h"

volatile uint16_t Railcom::s_address = 0;

void Railcom::begin()
{
    s_address = 0;
    SA_LOG_INFO("[Railcom] Initialisé (mode EXSA → SA)\n");
}

uint16_t Railcom::address()
{
    return s_address;
}

void Railcom::onRailcom(uint8_t index_exsa, uint8_t low, uint8_t high)
{
    uint16_t adr = (uint16_t(high) << 8) | low;

    s_address = adr;

    SA_LOG_INFO("[Railcom] EXSA %u → adresse RailCom = %u\n",
                index_exsa, adr);
}
