/*
 * Settings_SPIFFS.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Montage robuste du système de fichiers SPIFFS.
 *
 * Rôle :
 *   - monter SPIFFS au démarrage
 *   - formater automatiquement en cas d’échec (option true)
 *   - logguer clairement chaque étape
 *
 * Ce module ne contient aucune logique métier :
 *   → il fournit simplement un accès fiable au stockage flash.
 */

#include "Settings.h"
#include <SPIFFS.h>
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// mountSPIFFS()
// ---------------------------------------------------------------------------
bool Settings::mountSPIFFS() // 🟢
{
    CC_LOG_INFO("[Settings][SPIFFS][CC] Montage du système de fichiers...\n");

    // true = format automatique si montage impossible
    if (!SPIFFS.begin(true))
    {
        CC_LOG_ERROR("[Settings][SPIFFS][CC] ❌ Erreur : impossible de monter SPIFFS\n");
        CC_LOG_ERROR("[Settings][SPIFFS][CC] SPIFFS.begin() a échoué\n");
        return false;
    }

    CC_LOG_INFO("[Settings][SPIFFS][CC] ✔ SPIFFS monté (%u octets disponibles)\n",
                SPIFFS.totalBytes());

    return true;
}
