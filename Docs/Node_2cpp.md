/*
 * Node.cpp — Façade publique du module Node (Discovery 2026)
 * ---------------------------------------------------------------------------
 * Rôle :
 *   Ce fichier ne contient AUCUNE logique métier.
 *   Il inclut simplement les sous-modules internes situés dans /Node/.
 *
 * Objectif :
 *   - garder une façade propre et stable
 *   - séparer l’API publique (Node.h) de l’implémentation interne
 *   - faciliter l’onboarding et la maintenance
 */

#include "Node.h"

// Sous-modules internes
#include "Node/Node_Init.cpp"
#include "Node/Node_Periph.cpp"
#include "Node/Node_Topologie.cpp"
#include "Node/Node_Aiguilles.cpp"
#include "Node/Node_Signaux.cpp"
#include "Node/Node_SignauxTransitions.cpp"
#include "Node/Node_Occupation.cpp"
#include "Node/Node_Capteurs.cpp"
#include "Node/Node_Role.cpp"
#include "Node/Node_Logic.cpp"
#include "Node/Node_Debug.cpp"
