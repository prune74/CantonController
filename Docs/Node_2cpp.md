/*
 * Canton.cpp — Façade publique du module Canton (Exploration 2026)
 * ---------------------------------------------------------------------------
 * Rôle :
 *   Ce fichier ne contient AUCUNE logique métier.
 *   Il inclut simplement les sous-modules internes situés dans /Canton/.
 *
 * Objectif :
 *   - garder une façade propre et stable
 *   - séparer l’API publique (Canton.h) de l’implémentation interne
 *   - faciliter l’onboarding et la maintenance
 */

#include "Canton.h"

// Sous-modules internes
#include "Canton/Canton_Init.cpp"
#include "Canton/Canton_Periph.cpp"
#include "Canton/Canton_Topologie.cpp"
#include "Canton/Canton_Aiguilles.cpp"
#include "Canton/Canton_Signaux.cpp"
#include "Canton/Canton_SignauxTransitions.cpp"
#include "Canton/Canton_Occupation.cpp"
#include "Canton/Canton_Capteurs.cpp"
#include "Canton/Canton_Role.cpp"
#include "Canton/Canton_Logic.cpp"
#include "Canton/Canton_Debug.cpp"
