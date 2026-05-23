#ifndef RAILCOM_H
#define RAILCOM_H

/*
 * Railcom.h — Interface publique du module Railcom (Discovery 2026)
 * -----------------------------------------------------------------
 * Ce fichier définit :
 *   - la classe Railcom (API publique)
 *   - les signatures des tâches FreeRTOS internes
 *   - les variables globales nécessaires aux sous-modules
 *
 * Architecture du module :
 *
 *   Railcom.cpp                → façade (reste vide, inclus par les autres)
 *   Railcom_Init.cpp           → initialisation UART + queues + buffer
 *   Railcom_Receive.cpp        → lecture brute UART → queue1
 *   Railcom_Parse.cpp          → décodage Railcom 4/8 + stabilisation
 *   Railcom_Address.cpp        → mise à jour de l’adresse publique
 *
 * Pourquoi cette structure ?
 *   → Séparation claire des responsabilités
 *   → Onboarding facile pour les contributeurs
 *   → Robustesse FreeRTOS (chaque tâche dans son fichier)
 *   → Railcom.h reste stable et visible partout dans le SA
 */

#include <Arduino.h>
#include <driver/gpio.h>
#include "RingBuf.h"
#include "Config.h"

/*
 * Classe Railcom
 * ---------------------------------------------------------------------------
 * Rôle :
 *   - Fournir une API simple :
 *        * begin()   → lance les tâches Railcom
 *        * address() → retourne l’adresse DCC stabilisée
 *
 *   - Encapsuler :
 *        * les queues FreeRTOS
 *        * le port UART Railcom
 *        * l’adresse courante
 *
 *   - Les tâches FreeRTOS sont statiques car FreeRTOS impose un pointeur C.
 */
class Railcom
{
public:
    /*
     * Constructeur
     * -----------------------------------------------------------------------
     * Paramètres :
     *   - rxPin : GPIO d’entrée Railcom
     *   - txPin : GPIO de sortie Railcom (souvent inutilisé)
     */
    Railcom(const gpio_num_t rxPin, const gpio_num_t txPin);

    /*
     * begin()
     * -----------------------------------------------------------------------
     * Lance les 3 tâches FreeRTOS :
     *   - receiveData()
     *   - parseData()
     *   - setAddress()
     */
    void begin();

    /*
     * address()
     * -----------------------------------------------------------------------
     * Retourne l’adresse DCC Railcom stabilisée.
     * Cette valeur est mise à jour exclusivement par setAddress().
     */
    uint16_t address() const;

    /*
     * Tâches FreeRTOS internes
     * -----------------------------------------------------------------------
     * Elles doivent être statiques pour être compatibles avec xTaskCreate().
     */
    static void IRAM_ATTR receiveData(void* p);
    static void IRAM_ATTR parseData(void* p);
    static void IRAM_ATTR setAddress(void* p);

private:
    // Pins Railcom
    gpio_num_t m_rxPin;
    gpio_num_t m_txPin;

    // UART Railcom (Serial1)
    HardwareSerial* mySerial;

    // Queues FreeRTOS :
    //   - xQueue1 : octets bruts (UART → parseur)
    //   - xQueue2 : adresses stabilisées (parseur → setAddress)
    QueueHandle_t xQueue1;
    QueueHandle_t xQueue2;

    // Adresse DCC stabilisée
    volatile uint16_t m_address;
};

/*
 * Variables globales Railcom
 * ---------------------------------------------------------------------------
 * Elles sont définies dans Railcom_Init.cpp et utilisées dans les sous-modules.
 */

// Bits indiquant si un octet Railcom contient la partie haute/basse de l’adresse
extern const byte CH1_ADR_LOW;
extern const byte CH1_ADR_HIGH;

// Tailles des queues FreeRTOS
extern const byte QUEUE_1_SIZE;
extern const byte QUEUE_2_SIZE;

// Buffer circulaire global pour stabiliser l’adresse
extern RingBuf<uint16_t, NB_ADDRESS_TO_COMPARE> buffer;

#endif // RAILCOM_H
