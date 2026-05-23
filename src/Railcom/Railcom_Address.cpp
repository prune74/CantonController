/*
 * Railcom_Address.cpp — Mise à jour de l’adresse Railcom (Discovery 2026)
 * ---------------------------------------------------------------------------
 * Rôle de cette tâche :
 *   - Lire les adresses stabilisées produites par parseData() dans xQueue2.
 *   - Mettre à jour m_address, la valeur consultée par le reste du SA.
 *
 * Idée d’architecture :
 *   - parseData() se concentre sur la logique Railcom (décodage, filtrage).
 *   - setAddress() se concentre sur la mise à disposition de l’information.
 *
 * Avantage :
 *   - On isole l’accès en écriture à m_address dans une seule tâche.
 *   - On évite des accès concurrents complexes à cette variable.
 */

#include "Railcom.h"
#include "debug_sa.h"

/*
 * setAddress()
 * ---------------------------------------------------------------------------
 * Boucle FreeRTOS :
 *
 *   1) Lit xQueue2, qui contient des adresses DCC déjà stabilisées par
 *      la tâche parseData().
 *
 *   2) Si une adresse est disponible :
 *        - on la lit dans "address"
 *        - on l’affecte à m_address
 *        - on loggue l’événement pour le debug Discovery 2026
 *
 *   3) La tâche se rendort pour 25 ms avant de recommencer.
 *
 * Remarque :
 *   - xQueueReceive() est appelé avec un timeout de 0 ms :
 *       → on ne bloque jamais cette tâche.
 *       → si aucune adresse n’est prête, on ne fait rien pour ce tour.
 */
void IRAM_ATTR Railcom::setAddress(void *p)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    uint16_t address(0);
    Railcom *pThis = static_cast<Railcom*>(p);

    SA_LOG_TRACE("[Railcom] Tâche setAddress() démarrée\n");

    for (;;)
    {
        address = 0;

        /*
         * Lecture non bloquante dans xQueue2 :
         *   - pdMS_TO_TICKS(0) → la tâche ne bloque pas.
         *   - Si une adresse est disponible → pdPASS.
         */
        if (pThis->xQueue2 &&
            xQueueReceive(pThis->xQueue2, &address, pdMS_TO_TICKS(0)) == pdPASS)
        {
            /*
             * Mise à jour de l’adresse Railcom :
             *   - C’est le seul endroit où m_address est modifiée.
             *   - Les autres modules doivent utiliser address().
             */
            pThis->m_address = address;

            SA_LOG_INFO("[Railcom] Adresse Railcom mise à jour : %u\n", address);
        }

        /*
         * Périodicité :
         *   - Tâche réveillée toutes les 25 ms.
         *   - Cela suffit largement pour suivre les changements d’adresse.
         */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(25)); // toutes les x ms
    }
}
