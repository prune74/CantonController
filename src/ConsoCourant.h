#pragma once

#include <driver/gpio.h>   // pour gpio_num_t
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class Node;

/*
 * ============================================================
 *  ConsoCourant.h — SA 2026
 * ------------------------------------------------------------
 *  Rôle :
 *    - Représenter le "capteur de courant" d’un canton côté SA.
 *    - En 2026, ce capteur n’est plus physique : 
 *      l’info vient à 100% d’EXSA via UART.
 *
 *    - ConsoCourant reçoit les trames :
 *          [AA][04][30] → OCCUPÉ  (PROTO_OCC_ACTIVE)
 *          [AA][04][31] → LIBRE   (PROTO_OCC_LIBRE)
 *
 *    - Il fusionne cette présence physique avec le compteur
 *      global d’essieux (H + AH) maintenu par CompteurEssieuxUart.
 *
 *    - Il met ensuite à jour l’état logique du canton via :
 *          m_node->busy(bool occupeLogique)
 *
 *  Ce module est donc le "point d’entrée" officiel de
 *  l’occupation dans le SA.
 * ============================================================
 */

class ConsoCourant
{
public:
    ConsoCourant();
    ~ConsoCourant();

    // Initialisation du capteur de courant virtuel pour un canton
    void setup(Node* node); //void setup(Node* node, const gpio_num_t pinIn);

    // Démarrage de la tâche FreeRTOS qui lit l’UART EXSA → SA
    void startReceptionUART();

    // Mise à jour de l’état à partir d’une info physique reçue
    // (appelée par la tâche UART lorsqu’une trame 0x04 arrive)
    void updateEtat(bool occupePhysique);

private:
    // Tâche FreeRTOS pour la réception UART (fonction statique C-style)
    static void tacheReceptionUART(void* pvParameters);

    // Pointeur vers le canton concerné
    Node* m_node = nullptr;

    // Anciennement utilisé pour un GPIO physique, conservé pour compat
    gpio_num_t m_pinIn = GPIO_NUM_NC;
};
