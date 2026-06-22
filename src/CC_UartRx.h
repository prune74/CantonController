#pragma once
#include <stdint.h>

/*
 * CC_UartRx.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Ce module lit les trames envoyées par l’unique EXCC via RS485.
 *
 * Format des trames :
 *   [SYNC=0xAA][OPCODE][DATA...]
 *
 * Rôle :
 *   - lancer la tâche FreeRTOS de réception
 *   - parser les trames reçues
 *   - dispatcher les données vers les modules concernés :
 *        • Sensor (ponctuels)
 *        • Occupation (occupation)
 *        • Canton (compteur essieux)
 *        • SupervisionAiguilles (positions)
 *        • EXCC_Link (booster + pong)
 *        • Railcom (adresse loco)
 *        • Booster (calibration)
 *
 * Ce module ne contient aucune logique métier :
 *   → il distribue simplement les messages reçus.
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
    // Dispatch : opcode + pointeur data + longueur
    // -----------------------------------------------------------------------
    static void dispatch(uint8_t opcode, uint8_t *data, uint8_t len);
};
