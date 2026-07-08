#pragma once

#include <Arduino.h>
#include "Canton.h"
#include "SensEnum.h"
#include <Protocol.h> // pour ExccAspect

/*
 * SupervisionCanton.h — Gestion Canton 2026
 * ------------------------------------------------------------
 * Détermination de l’aspect ferroviaire à afficher pour le
 * Canton Controller (CC), en fonction du sens (horaire ou
 * anti‑horaire) et de l’état du canton aval.
 *
 * Logique intégrée :
 *   - Accessibilité et occupation du canton aval
 *   - Réservation locomotive (RailCom via EXCC)
 *   - Position des aiguilles (voie directe / déviée)
 *   - Masquage ou désactivation du signal (prévision)
 *   - Transitions douces ou temporisation (prévision)
 *   - Propagation passive d’un aspect reçu (prévision)
 *
 * Paramètres :
 *   - canton : pointeur vers le Canton Controller (CC)
 *   - i      : 0 = sens horaire, 1 = sens anti‑horaire
 *
 * Retour :
 *   - ExccAspect : aspect SNCF à afficher sur le signal
 */

ExccAspect mettreAJourAspectCanton(Canton *canton, uint8_t i);
