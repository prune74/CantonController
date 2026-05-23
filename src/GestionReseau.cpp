/*
  GestionReseau.cpp
  ------------------------------------------------------------
  Tâche principale de supervision du réseau ferroviaire distribué.
*/

#include "GestionReseau.h"
#include "FeuxDirection.h"      // LED blanches de direction
#include "SA_EXSA_Protocol.h"   // ExsaAspect + opcodes UART/CAN
#include "AspectSignal.h"       // pour mettreAJourAspectSignal
/*
  ------------------------------------------------------------
  Rôle global :
  - Orchestrer la logique ferroviaire locale du canton
  - Coordonner les modules spécialisés :
        * Capteurs (occupation, IR, courant…)
        * Déduction du sens de roulage
        * Topologie SP1 / SM1
        * Supervision CAN (diffusion réseau)
        * Supervision cantonale (aspects)
        * Pilotage distribué (ralentissement, arrêt…)
        * Commande DCC++ (vitesse, sens)
        * AspectSignal (pilotage des signaux via EXSA)

  Cette classe ne contient que :
  - la création de la tâche FreeRTOS principale
  - la boucle de supervision (loopTask)
  - le buffer des aspects à transmettre aux signaux

  Toute la logique est déléguée aux modules spécialisés.
  ------------------------------------------------------------*/

// Valeurs d’aspect envoyées aux signaux (horaire / anti-horaire)
ExsaAspect GestionReseau::signalValue[2] = {ASPECT_CARRE, ASPECT_CARRE};

void GestionReseau::setup(Node* node)
{
    xTaskCreatePinnedToCore(
        loopTask,
        "LoopTask",
        8 * 1024,
        (void*)node,
        10,
        NULL,
        0
    );
}

void IRAM_ATTR GestionReseau::loopTask(void* pvParameters)
{
    Node* node = static_cast<Node*>(pvParameters);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        // 1) Mise à jour des capteurs
        mettreAJourCapteurs(node);

        // 2) Déduction du sens de roulage
        deduireSensRoulage(node);

        // 3) Mise à jour de la topologie SP1 / SM1
        mettreAJourTopologie(node);

        // 4)Mise à jour interne des feux directionnels
        node->updateFeuDirection(SensHoraire);
        node->updateFeuDirection(SensAntiHoraire);

        // Lecture des valeurs calculées (0..4)
        uint8_t feuH  = node->getFeuDirection(SensHoraire);
        uint8_t feuAH = node->getFeuDirection(SensAntiHoraire);

        // 5) Envoi des états sur le bus CAN
        envoyerEtatCAN(node);

        // 6) Supervision cantonale → mise à jour des aspects locaux
        for (uint8_t i = 0; i < 2; i++)
        {
            signalValue[i] = mettreAJourAspectCanton(node, i);
        }

        // 7) Pilotage loco selon aspect reçu
        executerPilotageDistribue(node);

        // 8) Envoi des commandes DCC++ (trame 0x04)
        envoyerCommandeDCC(node);

        // 9) Déduction + envoi des aspects dynamiques aux signaux EXSA
        mettreAJourAspectSignal(node,
                                reinterpret_cast<uint8_t*>(signalValue));

        // 10) Temporisation fixe (100 ms)
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}
