/*
 * Settings_JSON_Signaux.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Chargement / sauvegarde des signaux ferroviaires du canton.
 *
 * Format JSON (2026) :
 *   sign0type, sign0position
 *   sign1type, sign1position
 *
 * Rôle :
 *   - créer / supprimer les objets Signal selon la présence et le type
 *   - charger type + position
 *   - sauvegarder le type déterminé automatiquement (SIG_ABSENT → SIG_xxx)
 *
 * IMPORTANT 2026 :
 *   - La logique métier (aspect → mât) est faite ailleurs.
 *   - Ici : aucune logique ferroviaire, uniquement JSON ↔ Canton.
 */

#include "Settings.h"
#include "Canton.h"
#include "Signal.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Chargement des signaux depuis settings.json
 * ==========================================================================*/
void Settings_JSON_loadSignaux(Canton *canton, JsonDocument &doc) // 🟢
{
    for (uint8_t i = 0; i < signalSize; i++)
    {
        String baseType = "sign" + String(i) + "type";
        String basePosition = "sign" + String(i) + "position";

        uint8_t type = doc[baseType] | 0;
        uint8_t position = doc[basePosition] | 0;

        // -------------------------------------------------------------------
        // Aucun signal physiquement présent → suppression
        // -------------------------------------------------------------------
        if (type == 0)
        {
            canton->setSignal(i, nullptr);
            continue;
        }

        // -------------------------------------------------------------------
        // Création si nécessaire
        // -------------------------------------------------------------------
        Signal *s = canton->getSignal(i);
        if (!s)
        {
            s = new Signal;
            canton->setSignal(i, s);
        }

        // -------------------------------------------------------------------
        // Chargement du type et de la position
        // -------------------------------------------------------------------
        s->type(type);         // SIG_ABSENT ou SIG_xxx
        s->position(position); // orientation physique
    }

    CC_LOG_INFO("[Settings][Signaux][CC] Signaux chargés \n");
}

/* ============================================================================
 *  Sauvegarde des signaux vers settings.json
 * ==========================================================================*/
void Settings_JSON_saveSignaux(Canton *canton, JsonDocument &doc) // 🟢
{
    for (uint8_t i = 0; i < signalSize; i++)
    {
        String baseType = "sign" + String(i) + "type";
        String basePosition = "sign" + String(i) + "position";

        Signal *s = canton->getSignal(i);

        // -------------------------------------------------------------------
        // Aucun signal → écrire 0 (SIG_ABSENT)
        // -------------------------------------------------------------------
        if (!s)
        {
            doc[baseType] = 0;
            doc[basePosition] = 0;
            continue;
        }

        // -------------------------------------------------------------------
        // Signal présent → sauvegarde du type réel + position
        // -------------------------------------------------------------------
        doc[baseType] = s->type();         // SIG_BAL / SIG_CARRE / SIG_RAL / SIG_RAPPEL / SIG_MANOEUVRE
        doc[basePosition] = s->position(); // Position H ou AH
    }
}
