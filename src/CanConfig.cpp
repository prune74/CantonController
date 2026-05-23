/*
  CanConfig.cpp — Configuration du contrôleur CAN intégré de l’ESP32
  ------------------------------------------------------------------
*/

#include "CanConfig.h"
#include "debug_sa.h"   // ← système de log Discovery 2026

void CanConfig::setup()
{
    SA_LOG_INFO("[CanConfig %d] : Configure ESP32 CAN\n", __LINE__);

    /**************************************************************************
     * 1) Création de la configuration CAN
     **************************************************************************/
    ACAN_ESP32_Settings settings(CAN_BITRATE);

    settings.mRxPin = CAN_RX;
    settings.mTxPin = CAN_TX;

    uint32_t errorCode;

    /**************************************************************************
     * 2) Initialisation du contrôleur CAN
     **************************************************************************/
    errorCode = ACAN_ESP32::can.begin(settings);

    SA_LOG_INFO("[CanConfig %d] : config without filter\n", __LINE__);

    /**************************************************************************
     * 3) Vérification du résultat
     **************************************************************************/
    if (errorCode == 0)
    {
        SA_LOG_INFO("[CanConfig %d] : configuration OK !\n", __LINE__);
    }
    else
    {
        SA_LOG_ERROR("[CanConfig %d] : configuration error 0x%x\n",
                     __LINE__, errorCode);

        vTaskDelay(pdMS_TO_TICKS(1000));
        return;
    }
}
