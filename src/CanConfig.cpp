/*
 * CanConfig.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Configuration du contrôleur CAN intégré de l’ESP32 (TWAI).
 *
 * Rôle :
 *   - configurer les pins RX/TX
 *   - initialiser le contrôleur CAN interne
 *   - vérifier le code retour
 *
 * Ce module ne contient aucune logique métier :
 *   → il prépare simplement le bus CAN Exploration 2026.
 */

#include "CanConfig.h"
#include "debug_cc.h"

void CanConfig::setup()
{
    CC_LOG_INFO("[CanConfig][CC] Initialisation du CAN interne\n");

    // -----------------------------------------------------------------------
    // 1) Création de la configuration CAN
    // -----------------------------------------------------------------------
    ACAN_ESP32_Settings settings(CAN_BITRATE);
    settings.mRxPin = CAN_RX;
    settings.mTxPin = CAN_TX;

    uint32_t errorCode = 0;

    // -----------------------------------------------------------------------
    // 2) Initialisation du contrôleur CAN
    // -----------------------------------------------------------------------
    errorCode = ACAN_ESP32::can.begin(settings);

    CC_LOG_INFO("[CanConfig][CC] Configuration sans filtre\n");

    // -----------------------------------------------------------------------
    // 3) Vérification du résultat
    // -----------------------------------------------------------------------
    if (errorCode == 0)
    {
        CC_LOG_INFO("[CanConfig][CC] CAN configuré avec succès\n");
    }
    else
    {
        CC_LOG_ERROR("[CanConfig][CC] Erreur configuration CAN : 0x%X\n", errorCode);
        vTaskDelay(pdMS_TO_TICKS(1000));
        return;
    }
}
