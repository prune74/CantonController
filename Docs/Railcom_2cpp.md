/*
 * Railcom.cpp — Façade du module Railcom (Discovery 2026)
 * ---------------------------------------------------------------------------
 * Ce fichier reste volontairement minimal.
 *
 * Pourquoi ?
 *   - D’autres modules du SA incluent "Railcom.h" et s’attendent à ce que
 *     "Railcom.cpp" existe à la racine du projet.
 *
 *   - Toute la logique Railcom est désormais répartie dans :
 *        /Railcom/Railcom_Init.cpp
 *        /Railcom/Railcom_Receive.cpp
 *        /Railcom/Railcom_Parse.cpp
 *        /Railcom/Railcom_Address.cpp
 *
 *   - Railcom.cpp sert donc uniquement de point d’entrée unique pour
 *     l’édition, la compilation et la lisibilité globale du module.
 */

#include "Railcom.h"

// Inclusion des sous-modules internes
#include "Railcom/Railcom_Init.cpp"
#include "Railcom/Railcom_Receive.cpp"
#include "Railcom/Railcom_Parse.cpp"
#include "Railcom/Railcom_Address.cpp"

/*
 * Rien d’autre ici.
 *
 * Toute la logique est dans les fichiers internes.
 * Railcom.cpp reste la façade publique, visible par le reste du SA.
 */
