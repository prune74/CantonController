/*
 * Settings_SPIFFS.cpp — Montage SPIFFS robuste (Discovery 2026)
 */

#include "settings/Settings_Internal.h"
#include <SPIFFS.h>
#include "debug_sa.h"

bool Settings::mountSPIFFS()
{
    SA_LOG_INFO("[Settings][SPIFFS] Montage du système de fichiers...\n");

    if (!SPIFFS.begin(true))   // true = format si échec
    {
        SA_LOG_ERROR("[Settings][SPIFFS] ❌ Erreur : impossible de monter SPIFFS\n");
        SA_LOG_ERROR("[Settings][SPIFFS] SPIFFS.begin() a échoué\n");
        return false;
    }

    SA_LOG_INFO("[Settings][SPIFFS] ✔ SPIFFS monté avec succès (%u octets disponibles)\n",
                SPIFFS.totalBytes());

    return true;
}
