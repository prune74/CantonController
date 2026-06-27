/*
 * GestionReseau.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Tâche principale de supervision du Canton Controller (CC).
 *
 * Rôle :
 *   - orchestrer la logique ferroviaire locale
 *   - coordonner les modules spécialisés :
 *        • capteurs (occupation, IR, courant…)
 *        • déduction du sens de roulage
 *        • topologie SP1 / SM1
 *        • supervision CAN (diffusion réseau)
 *        • supervision cantonale (aspects SNCF)
 *        • pilotage distribué (ralentissement, arrêt…)
 *        • commande DCC++ (vitesse, sens)
 *        • pilotage des signaux via EXCC (AspectSignal)
 *
 * Ce module ne contient :
 *   - que la création de la tâche FreeRTOS
 *   - la boucle de supervision (loopTask)
 *   - le buffer des aspects envoyés aux signaux
 *
 * Toute la logique métier est déléguée aux modules spécialisés.
 */

#include "GestionReseau.h"
#include "GestionLoco.h"
#include "FeuxDirection.h"
#include "Protocol.h"
#include "AspectSignal.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Valeurs d’aspect envoyées aux signaux (horaire / anti‑horaire)
// ---------------------------------------------------------------------------
ExccAspect GestionReseau::signalValue[2] = {
    ExccAspect::ASPECT_CARRE,
    ExccAspect::ASPECT_CARRE
};

// ---------------------------------------------------------------------------
// Création de la tâche FreeRTOS
// ---------------------------------------------------------------------------
void GestionReseau::setup(Canton *canton) // 🟢
{
    xTaskCreatePinnedToCore(
        loopTask,
        "LoopTask",
        8 * 1024,
        (void *)canton,
        10,
        NULL,
        0);
}

// ---------------------------------------------------------------------------
// Boucle principale de supervision
// ---------------------------------------------------------------------------
void IRAM_ATTR GestionReseau::loopTask(void *pvParameters) // 🟢
{
    Canton *canton = static_cast<Canton *>(pvParameters);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        // -------------------------------------------------------------------
        // 1) Mise à jour des capteurs
        // -------------------------------------------------------------------
        gestionLoco_update(canton);

        // -------------------------------------------------------------------
        // 2) Déduction du sens de roulage
        // -------------------------------------------------------------------
        deduireSensRoulage(canton);

        // -------------------------------------------------------------------
        // 3) Mise à jour de la topologie SP1 / SM1
        // -------------------------------------------------------------------
        mettreAJourTopologie(canton);

        // -------------------------------------------------------------------
        // 4) Mise à jour interne des feux directionnels
        // -------------------------------------------------------------------
        canton->updateFeuDirection(SensHoraire);
        canton->updateFeuDirection(SensAntiHoraire);

        uint8_t feuH = canton->getFeuDirection(SensHoraire);
        uint8_t feuAH = canton->getFeuDirection(SensAntiHoraire);

        // -------------------------------------------------------------------
        // 5) Diffusion de l’état sur le bus CAN
        // -------------------------------------------------------------------
        envoyerEtatCAN(canton);

        // -------------------------------------------------------------------
        // 6) Supervision cantonale → calcul des aspects locaux
        // -------------------------------------------------------------------
        for (uint8_t i = 0; i < 2; i++)
        {
            signalValue[i] = mettreAJourAspectCanton(canton, i);
        }

        // -------------------------------------------------------------------
        // 7) Pilotage distribué de la locomotive
        // -------------------------------------------------------------------
        executerPilotageDistribue(canton);

        // -------------------------------------------------------------------
        // 8) Envoi des commandes DCC++ (trame 0x04)
        // -------------------------------------------------------------------
        envoyerCommandeDCC(canton);

        // -------------------------------------------------------------------
        // 9) Déduction + envoi des aspects dynamiques aux signaux EXCC
        // -------------------------------------------------------------------
        mettreAJourAspectSignal(
            canton,
            reinterpret_cast<uint8_t *>(signalValue));

        // -------------------------------------------------------------------
        // 10) Temporisation fixe (100 ms)
        // -------------------------------------------------------------------
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}
