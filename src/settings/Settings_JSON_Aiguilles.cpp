/*
 * Settings_JSON_Aiguilles.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Chargement / sauvegarde des AIGUILLES LOGIQUES depuis/vers settings.json.
 *
 * Rôle :
 *   - créer ou supprimer les objets Aig selon le JSON
 *   - charger les paramètres logiques :
 *        • ID
 *        • posDroit / posDevie
 *        • index EXCC côté H / AH
 *        • vitesse (stockée dans servoCfg, plus dans Aig)
 *   - sauvegarder ces mêmes paramètres dans settings.json
 *
 * Ce module ne contient aucune logique métier :
 *   → il synchronise simplement l’état logique avec le JSON.
 */

#include "Settings.h"
#include "Canton.h"
#include "Aig.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Chargement des aiguilles LOGIQUES
 * ==========================================================================*/
void Settings_JSON_loadAiguilles(Canton *canton, JsonDocument &doc)
{
    for (byte i = 0; i < aigSize; i++)
    {
        String base = "aig" + String(i);

        // -------------------------------------------------------------------
        // Aiguille absente → nullptr
        // -------------------------------------------------------------------
        if (!doc.containsKey(base) ||
            (doc[base].is<const char *>() && strcmp(doc[base], "null") == 0))
        {
            canton->setAig(i, nullptr);
            continue;
        }

        // -------------------------------------------------------------------
        // Création si nécessaire
        // -------------------------------------------------------------------
        Aig *a = canton->getAig(i);
        if (!a)
        {
            a = new Aig;
            canton->setAig(i, a);
        }

        // -------------------------------------------------------------------
        // Chargement des champs logiques
        // -------------------------------------------------------------------
        a->ID(doc[base + "id"] | 0);
        a->posDroit(doc[base + "posDroit"] | 1500);
        a->posDevie(doc[base + "posDevie"] | 1500);

        // -------------------------------------------------------------------
        // Vitesse (stockée dans servoCfg, plus dans Aig)
        // -------------------------------------------------------------------
        canton->getServoCfg(i).speed = doc[base + "speed"] | 0;

        // -------------------------------------------------------------------
        // Index EXCC côté H / AH
        // -------------------------------------------------------------------
        a->cantonPdroitIdx(doc[base + "cantonPdroitIdx"] | 0);
        a->cantonPdevieIdx(doc[base + "cantonPdevieIdx"] | 0);
    }

    CC_LOG_INFO("[Settings][Aiguilles][CC] Aiguilles logiques chargées (speed via servoCfg)\n");
}

/* ============================================================================
 *  Sauvegarde des aiguilles LOGIQUES
 * ==========================================================================*/
void Settings_JSON_saveAiguilles(Canton *canton, JsonDocument &doc)
{
    for (byte i = 0; i < aigSize; i++)
    {
        String base = "aig" + String(i);
        Aig *a = canton->getAig(i);

        // -------------------------------------------------------------------
        // Aiguille absente
        // -------------------------------------------------------------------
        if (!a)
        {
            doc[base] = "null";
            continue;
        }

        // -------------------------------------------------------------------
        // Aiguille active
        // -------------------------------------------------------------------
        doc[base] = "Actif";
        doc[base + "id"]              = a->ID();
        doc[base + "posDroit"]        = a->posDroit();
        doc[base + "posDevie"]        = a->posDevie();

        // -------------------------------------------------------------------
        // Vitesse logique (désormais dans servoCfg)
        // -------------------------------------------------------------------
        doc[base + "speed"]           = canton->getServoCfg(i).speed;

        // -------------------------------------------------------------------
        // Index EXCC côté H / AH
        // -------------------------------------------------------------------
        doc[base + "cantonPdroitIdx"] = a->cantonPdroitIdx();
        doc[base + "cantonPdevieIdx"] = a->cantonPdevieIdx();
    }
}
