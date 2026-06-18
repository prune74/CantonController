#pragma once
#include <stdint.h>

/*
 * CC_UartRx.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Réception UART RS485 depuis l’EXCC.
 *
 * Rôle :
 *   - lancer la tâche FreeRTOS de réception
 *   - parser les trames EXCC → CC
 *   - dispatcher vers les modules concernés :
 *        • Sensor (ponctuels)
 *        • ConsoCourant (occupation)
 *        • Canton (compteur essieux)
 *        • SupervisionAiguilles (positions)
 *        • EXCC_Link (booster + pong)
 *        • Railcom (adresse loco)
 *        • Booster (calibration)
 *
 * Ce module ne contient aucune logique métier :
 *   → il distribue simplement les messages reçus.
 *
 * NOTE 2026 :
 *   - les trames EXCC contiennent toujours un INDEX_EXCC
 *   - certains modules n’utilisent plus cet index (occupation, railcom, booster…)
 *   - mais il reste indispensable pour EXCC_Link (PING/PONG, supervision)
 */

class CC_UartRx
{
public:
    // -----------------------------------------------------------------------
    // Lance la tâche FreeRTOS de réception RS485
    // -----------------------------------------------------------------------
    static void begin();

private:
    // -----------------------------------------------------------------------
    // Boucle de réception (tâche FreeRTOS)
    // -----------------------------------------------------------------------
    static void uartTask(void *param);

public:
    // -----------------------------------------------------------------------
    // Dispatch : opcode + index EXCC + pointeur data + longueur
    // -----------------------------------------------------------------------
    static void dispatch(uint8_t opcode, uint8_t index, uint8_t *data, uint8_t len);
};
