#pragma once
#include <Arduino.h>
#include "Protocol.h" // ExccAspect

/*
 * DeductionAspect.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction de l’aspect amont à partir de l’aspect aval.
 *
 * Rôle :
 *   - appliquer les règles SNCF locales :
 *        • si l’aval est restrictif, l’amont doit l’être aussi
 *        • si la voie est déviée, on limite l’ouverture (ralentissement → rappel)
 *        • gestion optionnelle des clignotants (USE_CLIGNOTANTS)
 *
 * Ce module ne contient :
 *   - aucune logique de voisinage
 *   - aucune logique de topologie
 *   - aucune sécurité globale
 *   - aucune dépendance au Canton
 *
 * Il est 100 % déterministe.
 */

// Alias pratique (optionnel mais cohérent avec ton code)
using Aspect = ExccAspect;

/*
 * Déduit l’aspect amont à partir :
 *   - de l’aspect aval (enum ExccAspect)
 *   - de l’état de la voie (voieDevie = true si déviation)
 *
 * Retour :
 *   - un aspect SNCF cohérent avec les règles locales
 */
Aspect deduireAspectDepuisAval(ExccAspect aval, bool voieDevie);
