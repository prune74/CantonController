#pragma once
#include <stdint.h>

class SA_UartRx
{
public:
    // Lance la tâche FreeRTOS de réception RS485
    static void begin();

private:
    // Boucle de réception (tâche FreeRTOS)
    static void uartTask(void *param);

public:
    // opcode + index EXSA + pointeur data + longueur
    static void dispatch(uint8_t opcode, uint8_t index, uint8_t *data, uint8_t len);
};
