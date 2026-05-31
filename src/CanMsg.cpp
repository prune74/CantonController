/*
  CanMsg.cpp - Gestion des messages CAN (squelette + dispatch)
  ----------------------------------------------------------------
  - crée la tâche FreeRTOS de réception
  - lit les trames CAN
  - décode l’en-tête (commande, ID expéditeur, rtr/response)
  - route la trame vers le bon handler (System, Discovery, Exploitation)
*/

#include "CanMsg.h"
#include "debug_sa.h" // ← système de logs Discovery 2026

// Handlers déclarés dans les autres fichiers .cpp
void handleSystemCommand(uint8_t commande, const CANMessage &frame, Node *node, uint16_t idSatExpediteur);
void handleDiscoveryCommand(uint8_t commande, const CANMessage &frame, Node *node, uint16_t idSatExpediteur);
void handleExploitCommand(uint8_t commande, const CANMessage &frame, Node *node, uint16_t idSatExpediteur);

// -----------------------------------------------------------------------------
// setup() : création de la tâche de réception CAN
// -----------------------------------------------------------------------------
void CanMsg::setup(Node *node)
{
  SA_LOG_INFO("[CanMsg %d] : setup\n", __LINE__);

  TaskHandle_t canReceiveHandle = NULL;

  xTaskCreatePinnedToCore(
      canReceiveMsg,
      "CanReceiveMsg",
      4 * 1024,
      (void *)node,
      6,
      &canReceiveHandle,
      0);

#ifdef TEST_MEMORY_TASK
  xTaskCreate(
      testMemory,
      "TestMemory",
      2 * 1024,
      (void *)canReceiveHandle,
      2,
      NULL);
#endif
}

#ifdef TEST_MEMORY_TASK
// -----------------------------------------------------------------------------
// testMemory() : surveille la mémoire de la tâche canReceiveMsg
// -----------------------------------------------------------------------------
void CanMsg::testMemory(void *pvParameters)
{
  TaskHandle_t canReceiveHandle = (TaskHandle_t)pvParameters;
  for (;;)
  {
    UBaseType_t freeStack = uxTaskGetStackHighWaterMark(canReceiveHandle);
    Serial.printf("canReceiveMsg free memory = %d bytes\n", freeStack);
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
#endif

// -----------------------------------------------------------------------------
// canReceiveMsg() : tâche FreeRTOS de réception / dispatch CAN
// -----------------------------------------------------------------------------
void CanMsg::canReceiveMsg(void *pvParameters)
{
  Node *node = (Node *)pvParameters;

  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    CANMessage frameIn;
    if (ACAN_ESP32::can.receive(frameIn))
    {
      // ---------------------------------------------------------------------
      // 🔥 STOP global Discovery 2026 (ID = 0x201, 11 bits, DLC = 0)
      // ---------------------------------------------------------------------
      if (frameIn.id == DISCOVERY_CAN_ID_EMERGENCY_STOP) {
        node->setStopActive(true);
        SA_LOG_ERROR("[Node %u] STOP global reçu depuis Master\n", node->ID());
        continue; // on ne traite rien d'autre
      }

      // ---------------------------------------------------------------------
      // ⚠️ Décodage standard Discovery (29 bits)
      // ---------------------------------------------------------------------
      const uint8_t commande = (uint8_t)((frameIn.id & 0x1FE0000) >> 17);
      const uint16_t idSatExpediteur = (uint16_t)(frameIn.id & 0xFFFF);
      const bool response = (frameIn.id & 0x10000) >> 16;
      (void)response;

      if (frameIn.rtr)
      {
        switch (commande)
        {
        case 0x0F:
          ACAN_ESP32::can.tryToSend(frameIn);
          break;
        default:
          break;
        }
      }
      else
      {
        if (commande >= CMD_SAT_TEST_BUS_REPLY && commande <= CMD_SAVE_ALL)
        {
          handleSystemCommand(commande, frameIn, node, idSatExpediteur);
        }
        else if (commande >= 0xC0 && commande <= 0xC1)
        {
          handleDiscoveryCommand(commande, frameIn, node, idSatExpediteur);
        }
        else if (commande >= 0xE0 && commande <= 0xE9)
        {
          handleExploitCommand(commande, frameIn, node, idSatExpediteur);
        }
        else
        {
          SA_LOG_INFO("[CanMsg] Commande 0x%X non gérée\n", commande);
        }
      }
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
  }
}
