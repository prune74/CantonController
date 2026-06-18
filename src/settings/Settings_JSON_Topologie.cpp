/*
 * Settings_JSON_Topologie.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion de la topologie ferroviaire du Canton :
 *   - SP1 / SM1 (voies principales)
 *   - SP2 / SM2 (voies secondaires)
 *
 * IMPORTANT 2026 :
 *   - aucun masque d’aiguilles n’est stocké dans la topologie
 *   - les masques utiles sont uniquement ceux des CantonPeriph (voisins)
 *   - ce module ne contient aucune logique métier
 */

#include "Settings.h"
#include "Canton.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Chargement de la topologie ferroviaire
 * ==========================================================================*/
void Settings_JSON_loadTopologie(Canton *canton, JsonDocument &doc)
{
    // SP1 / SM1 : indices des voisins principaux
    canton->SP1_idx(doc["SP1_idx"] | 0);
    canton->SM1_idx(doc["SM1_idx"] | 0);

    // SP2 / SM2 : accès secondaires
    canton->SP2_acces(doc["SP2_acces"] | true);
    canton->SM2_acces(doc["SM2_acces"] | true);

    CC_LOG_INFO("[Settings][Topo][CC] SP1=%d SM1=%d | SP2_acces=%d SM2_acces=%d\n",
                canton->SP1_idx(),
                canton->SM1_idx(),
                canton->SP2_acces(),
                canton->SM2_acces());
}

/* ============================================================================
 *  Sauvegarde de la topologie ferroviaire
 * ==========================================================================*/
void Settings_JSON_saveTopologie(Canton *canton, JsonDocument &doc)
{
    // SP1 / SM1
    doc["SP1_idx"] = canton->SP1_idx();
    doc["SM1_idx"] = canton->SM1_idx();

    // SP2 / SM2
    doc["SP2_acces"] = canton->SP2_acces();
    doc["SM2_acces"] = canton->SM2_acces();
}
