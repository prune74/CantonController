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
 *   - créer / supprimer les objets Signal selon le type
 *   - charger type + position
 *   - appliquer les defaults selon le rôle ferroviaire
 *
 * Ce module ne contient aucune logique métier :
 *   → il synchronise simplement JSON ↔ structures Canton.
 */

#include "Settings.h"
#include "Canton.h"
#include "Signal.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Chargement des signaux
 * ==========================================================================*/
void Settings_JSON_loadSignaux(Canton *canton, JsonDocument &doc)
{
    for (byte i = 0; i < signalSize; i++)
    {
        String baseType     = "sign" + String(i) + "type";
        String basePosition = "sign" + String(i) + "position";

        uint8_t type     = doc[baseType]     | 0;
        uint8_t position = doc[basePosition] | 0;

        // -------------------------------------------------------------------
        // Aucun signal → suppression
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
        // Chargement des paramètres
        // -------------------------------------------------------------------
        s->type(type);
        s->position(position);
    }

    // Ajustement automatique selon le rôle ferroviaire
    canton->applyRoleDefaults();

    CC_LOG_INFO("[Settings][Signaux][CC] Signaux chargés + applyRoleDefaults()\n");
}

/* ============================================================================
 *  Sauvegarde des signaux
 * ==========================================================================*/
void Settings_JSON_saveSignaux(Canton *canton, JsonDocument &doc)
{
    for (byte i = 0; i < signalSize; i++)
    {
        String baseType     = "sign" + String(i) + "type";
        String basePosition = "sign" + String(i) + "position";

        Signal *s = canton->getSignal(i);

        if (!s)
        {
            doc[baseType]     = 0;
            doc[basePosition] = 0;
        }
        else
        {
            doc[baseType]     = s->type();
            doc[basePosition] = s->position();
        }
    }

    // Sauvegarde du rôle ferroviaire
    doc["role"] = (uint8_t)canton->getRole();
}
