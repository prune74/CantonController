/*
 * Railcom_Receive.cpp — Tâche de lecture UART (Railcom)
 * ---------------------------------------------------------------------------
 * Rôle de cette tâche :
 *   - Lire les octets bruts en provenance du décodeur Railcom.
 *   - Marquer le début de trame avec un octet spécial '\0'.
 *   - Pousser les premiers octets dans la queue xQueue1.
 *
 * Pourquoi ce marqueur '\0' ?
 *   → Ce caractère spécial sert de "début de trame" interne.
 *   → parseData() attend précisément ce marqueur pour
 *     synchroniser son état.
 *
 * Ce n’est PAS un octet Railcom réel :
 *   → c’est un protocole interne SA pour découper les séquences.
 */

#include "Railcom.h"
#include "debug_sa.h"

void IRAM_ATTR Railcom::receiveData(void *p)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    uint8_t inByte(0);
    uint8_t count(0);
    Railcom *pThis = static_cast<Railcom*>(p);

    SA_LOG_TRACE("[Railcom] Tâche receiveData() démarrée\n");

    for (;;)
    {
        while (pThis->mySerial && pThis->mySerial->available() > 0)
        {
            /*
             * Premier octet d’une rafale UART :
             *   → remplacé par '\0'
             *   → sert de marqueur de début de trame pour parseData()
             */
            if (count == 0)
                inByte = '\0';
            else
                inByte = static_cast<uint8_t>(pThis->mySerial->read());

            /*
             * On ne garde que les 3 premiers octets :
             *   - '\0'
             *   - octet Railcom 1
             *   - octet Railcom 2
             *
             * Le parseur n’a besoin que de ces 3 éléments.
             */
            if (count < 3 && pThis->xQueue1)
                xQueueSend(pThis->xQueue1, &inByte, 0);

            count++;
        }

        count = 0;

        // Tâche périodique : toutes les 25 ms
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(25));
    }
}
