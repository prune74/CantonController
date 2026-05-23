/*
 * Railcom_Init.cpp — Initialisation du module Railcom (Discovery 2026)
 * --------------------------------------------------------------------
 * Rôle de ce fichier :
 *   - Configurer le port UART dédié Railcom (RX / TX)
 *   - Créer les queues FreeRTOS pour échanger les données entre tâches
 *   - Initialiser le buffer circulaire utilisé pour stabiliser l’adresse
 *   - Fournir l’API publique :
 *       - constructeur Railcom(rxPin, txPin)
 *       - begin()
 *       - address()
 *
 * IMPORTANT :
 *   La logique “métier” de décodage Railcom est dans Railcom_Parse.cpp.
 *   Ce fichier est purement “initialisation et orchestration”.
 */

#include "Railcom.h"
#include "debug_sa.h"

extern HardwareSerial Serial1;

// Identifiants des données du canal 1 (Railcom canal 1)
const byte CH1_ADR_LOW  = 1 << 2;   // bit 2 → indique que l’octet contient la partie basse de l’adresse
const byte CH1_ADR_HIGH = 1 << 3;   // bit 3 → indique que l’octet contient la partie haute de l’adresse

// Tailles des queues FreeRTOS pour Railcom
const byte QUEUE_1_SIZE = 10;  // File des octets bruts (UART → parseur)
const byte QUEUE_2_SIZE = 3;   // File des adresses stabilisées

// Buffer circulaire global : utilisé pour filtrer / stabiliser l’adresse Railcom
RingBuf<uint16_t, NB_ADDRESS_TO_COMPARE> buffer;

/*
 * Constructeur
 * ---------------------------------------------------------------------------
 * Paramètres :
 *   - rxPin : GPIO d’entrée Railcom (RX)
 *   - txPin : GPIO de sortie Railcom (TX) si nécessaire
 *
 * Actions :
 *   - sauvegarde des pins
 *   - initialisation de l’adresse courante à 0
 *   - création des queues FreeRTOS
 *   - configuration du port UART (250 000 bauds)
 *   - remplissage du buffer circulaire avec des zéros
 */
Railcom::Railcom(const gpio_num_t rxPin, const gpio_num_t txPin)
    : m_rxPin(rxPin),
      m_txPin(txPin),
      m_address(0),
      mySerial(nullptr),
      xQueue1(nullptr),
      xQueue2(nullptr)
{
    SA_LOG_INFO("[Railcom] Constructeur : RX=%d TX=%d\n", m_rxPin, m_txPin);

    // Création des queues FreeRTOS
    xQueue1 = xQueueCreate(QUEUE_1_SIZE, sizeof(uint8_t));
    xQueue2 = xQueueCreate(QUEUE_2_SIZE, sizeof(uint16_t));

    if (!xQueue1 || !xQueue2)
        SA_LOG_WARN("[Railcom] ⚠ Impossible de créer les queues FreeRTOS\n");

    // Association au port UART matériel (Serial1) et configuration
    mySerial = &Serial1;
    mySerial->begin(250000, SERIAL_8N1, m_rxPin, m_txPin);

    /*
     * Buffer de stabilisation :
     *   On remplit le buffer circulaire avec des zéros.
     *   Objectif : disposer d’un historique des dernières adresses lues
     *   pour éviter de prendre en compte un “glitch” Railcom.
     */
    const uint16_t x = 0;
    for (uint8_t i = 0; i < NB_ADDRESS_TO_COMPARE; i++)
        buffer.push(x);

    SA_LOG_TRACE("[Railcom] Buffer de stabilisation initialisé (%u entrées)\n",
                 NB_ADDRESS_TO_COMPARE);
}

/*
 * begin()
 * ---------------------------------------------------------------------------
 * Lancement des 3 tâches FreeRTOS :
 *
 *   1) receiveData() :
 *        - lit les octets sur l’UART
 *        - tague le début de trame avec '\0'
 *        - pousse les octets dans xQueue1
 *
 *   2) parseData() :
 *        - reconstruit la trame Railcom
 *        - décode le format 4/8
 *        - calcule l’adresse DCC
 *        - stabilise l’adresse via le buffer circulaire
 *        - pousse l’adresse stable dans xQueue2
 *
 *   3) setAddress() :
 *        - lit xQueue2
 *        - met à jour m_address
 */
void Railcom::begin()
{
    SA_LOG_INFO("[Railcom] Lancement des tâches FreeRTOS\n");

    xTaskCreatePinnedToCore(this->receiveData, "ReceiveData",
                            4 * 1024, this, 5, NULL, 1);

    xTaskCreatePinnedToCore(this->parseData, "ParseData",
                            4 * 1024, this, 7, NULL, 1);

    xTaskCreatePinnedToCore(this->setAddress, "SetAddress",
                            2 * 1024, this, 9, NULL, 1);
}

/*
 * address()
 * ---------------------------------------------------------------------------
 * Retourne l’adresse DCC Railcom stabilisée.
 *
 * Remarque :
 *   - Cette valeur est mise à jour exclusivement par la tâche setAddress().
 *   - Elle ne doit PAS être modifiée ailleurs.
 */
uint16_t Railcom::address() const
{
    return m_address;
}
