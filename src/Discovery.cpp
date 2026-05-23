/*
  Discovery.cpp — Version 2026 (CLEAN)
  ------------------------------------------------------------
  Rôle :
  - Détecter les satellites voisins via les boutons physiques.
  - Construire la topologie SP1/SP2/SM1/SM2.
  - Créer les aiguilles LOGIQUES (Aig) en fonction des voisins.
  - Associer chaque aiguille à un EXSA (H/AH).
  - Déduire les signaux (cibles) en fonction de la topologie.
  - Sauvegarder settings.json et envoyer la topologie à la Main.

  Notes 2026 :
  - Le SA ne pilote plus aucun servo.
  - Les aiguilles sont 100 % logiques.
  - EXSA pilote physiquement les servos via PCA9685.
  ------------------------------------------------------------
*/

#include "Discovery.h"

// ------------------------------------------------------------
// Variables statiques
// ------------------------------------------------------------

// État des boutons (4 bits)
byte Discovery::m_btnState{0};

// ID du satellite voisin détecté via CAN
byte Discovery::m_ID_satPeriph{UNUSED_ID};

// Nombre d’aiguilles logiques détectées
byte Discovery::m_comptAig{0};

// Indique que Discovery doit s’arrêter
bool Discovery::m_stopProcess{false};

// Référence vers le Node principal
Node *Discovery::node = nullptr;

// Entrées physiques (boutons)
const gpio_num_t Discovery::m_pinIn[] = {
  BTN_SAT_MOINS, BTN_SAT_PLUS, INTER_DEV_2, INTER_DEV_1
};

// LED d’état Discovery
const gpio_num_t Discovery::m_pinLed = LED_PIN_DISCOV;


// ------------------------------------------------------------
// Accesseurs
// ------------------------------------------------------------
void Discovery::ID_satPeriph(byte val) { m_ID_satPeriph = val; }
byte Discovery::ID_satPeriph() { return m_ID_satPeriph; }

void Discovery::comptAig(byte val) { m_comptAig = val; }
byte Discovery::comptAig() { return m_comptAig; }

void Discovery::btnState(byte val) { m_btnState = val; }
byte Discovery::btnState() { return m_btnState; }

void Discovery::stopProcess(bool stop) { m_stopProcess = stop; }


// ------------------------------------------------------------
// begin() : initialisation Discovery
// ------------------------------------------------------------
void Discovery::begin(Node *nd)
{
  node = nd;

  // Initialisation des boutons
  for (byte i = 0; i < 4; i++)
    pinMode(m_pinIn[i], INPUT);

  pinMode(m_pinLed, OUTPUT);

  // Lancement des tâches FreeRTOS
  xTaskCreatePinnedToCore(process, "Process", 4096, (void *)node, 7, NULL, 1);
  xTaskCreatePinnedToCore(createAigEtCibles, "CreateAiguilles", 4096, (void *)node, 2, NULL, 0);
}


// ------------------------------------------------------------
// process() : gestion des boutons + envoi CAN
// ------------------------------------------------------------
void Discovery::process(void *p)
{
  bool ledAllumee{LOW};

  auto clignoterLED = [&]() {
    digitalWrite(m_pinLed, ledAllumee ? LOW : HIGH);
    ledAllumee = !ledAllumee;
  };

  auto allumerLED = [&]() { digitalWrite(m_pinLed, HIGH); };
  auto eteindreLED = [&]() { digitalWrite(m_pinLed, LOW); };

  auto btnPush = [&](uint8_t btnNum)
  {
    // Demande d’ID du voisin via CAN
    CanMsg::sendMsg(0, 0xC0, 0, node->ID(), UNUSED_ID, 0);

    if (m_ID_satPeriph < 253)
    {
      if (node->nodeP[btnNum] == nullptr)
        node->nodeP[btnNum] = new NodePeriph;

      node->nodeP[btnNum]->ID(m_ID_satPeriph);
      allumerLED();
      m_ID_satPeriph = UNUSED_ID;
    }
    else
      clignoterLED();
  };

  Node *node = (Node *)p;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    // Lecture des boutons
    for (byte i = 0; i < 4; i++)
    {
      if (!digitalRead(m_pinIn[i]))
        m_btnState |= (1 << i);
      else
        m_btnState &= ~(1 << i);
    }

    // Gestion des actions
    switch (m_btnState & 0x03)
    {
    case 0x01: // Btn -
      btnPush(m_btnState >> 2);
      break;

    case 0x02: // Btn +
      btnPush((m_btnState >> 2) + 4);
      break;

    case 0x03: // Reset complet
      if (m_comptAig > 0)
      {
        for (byte i = 0; i < 5; i++)
        {
          clignoterLED();
          vTaskDelay(pdMS_TO_TICKS(100));
        }
      }

      // Suppression des voisins
      for (byte i = 0; i < nodePsize; i++)
      {
        delete node->nodeP[i];
        node->nodeP[i] = nullptr;
      }

      // Suppression des aiguilles
      for (byte i = 0; i < aigSize; i++)
      {
        delete node->aig[i];
        node->aig[i] = nullptr;
      }

      // Suppression des signaux
      for (byte i = 0; i < signalSize; i++)
      {
        delete node->signal[i];
        node->signal[i] = nullptr;
      }

      m_comptAig = 0;
      allumerLED();
      break;

    default:
      eteindreLED();
      break;
    }

    // Envoi du masque d’aiguilles sur le bus CAN
    CanMsg::sendMsg(0, 0xC1, 0, node->ID(), UNUSED_ID, 0, node->masqueAig());

    if (m_stopProcess)
      vTaskDelete(NULL);

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
  }
}


// ------------------------------------------------------------
// createAigEtCibles() : création des aiguilles + signaux
// ------------------------------------------------------------
void Discovery::createAigEtCibles(void *p)
{
  Node *node = (Node *)p;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    // Réinitialisation
    node->masqueAig(0x00);
    m_comptAig = 0;

    for (byte i = 0; i < aigSize; i++)
      node->aig[i] = nullptr;

    // Fonction de création d’une aiguille logique
    auto createAig = [&](uint8_t index, uint8_t nodP0, uint8_t nodP1)
    {
      if (m_comptAig < aigSize)
      {
        if (node->aig[index] == nullptr)
          node->aig[index] = new Aig;

        node->aig[index]->ID(index);

        // Association EXSA H/AH
        node->aig[index]->nodePdroitIdx(nodP0);
        node->aig[index]->nodePdevieIdx(nodP1);

        // Mise à jour du masque
        byte masque = node->masqueAig();
        masque |= (1 << index);
        node->masqueAig(masque);

        m_comptAig++;
      }
    };

    // 6 patterns logiques SP/SM
    const byte aigConditions[aigSize][2] = {
      {p00, p01}, {p00, p10}, {p01, p11},
      {m00, m01}, {m00, m10}, {m01, m11}
    };

    // Création des aiguilles logiques
    for (uint8_t i = 0; i < aigSize; i++)
    {
      auto condition = aigConditions[i];
      if (node->nodeP[condition[0]] && node->nodeP[condition[1]])
        createAig(i, condition[0], condition[1]);
    }

    // --------------------------------------------------------
    // Détermination des signaux (H / AH)
    // --------------------------------------------------------
    for (byte sens = 0; sens < 2; sens++)
    {
      uint8_t x = (sens == 0 ? 0 : 3);
      uint8_t y = (sens == 0 ? 3 : 0);
      uint8_t index = (sens == 0 ? p00 : m00);

      byte typeCible = 0;

      if (node->masqueAig() & (1 << x))
        typeCible = 3; // Carré + Ralentissement
      else if (node->nodeP[index])
      {
        if (node->nodeP[index]->masqueAig() & (1 << y))
          typeCible = 1; // Carré
        else if (node->nodeP[index]->masqueAig() & (1 << x))
          typeCible = 2; // Ralentissement
      }
      else
        typeCible = 1; // Carré par défaut

      // Création du signal
      uint8_t sigIdx = (sens == 0 ? 0 : 1);

      if (node->signal[sigIdx] == nullptr)
        node->signal[sigIdx] = new Signal;

      node->signal[sigIdx]->type(typeCible);
      node->signal[sigIdx]->position(sigIdx);
      node->signal[sigIdx]->setup();
    }

    // Fin du Discovery
    if (m_stopProcess)
    {
      Settings::writeFile(Settings::node);

      vTaskDelay(pdMS_TO_TICKS(200));
      envoyerTopologieSiPret();
      vTaskDelay(pdMS_TO_TICKS(800));
      ESP.restart();
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(2000));
  }
}
/* ------------------------------------------------------------
  Fin de Discovery.cpp
  ------------------------------------------------------------
*/