#pragma once
#include <Arduino.h>
#include "Canton.h"
#include "SensEnum.h"
#include "Exploration_Protocol.h" // pour ExsaAspect

/*************************************************************************************
 * Supervision cantonale
 * Déduction de l’aspect à afficher sur le signal du canton courant (horaire ou anti-horaire)
 * en fonction de l’état du canton aval et du canton suivant.
 *
 * Logique intégrée :
 * - Accessibilité et occupation du canton aval
 * - Réservation locomotive
 * - Position des aiguilles (voie déviée)
 * - Masquage ou désactivation du signal (prévision)
 * - Temporisation ou transition douce (prévision)
 * - Propagation passive d’un aspect reçu (prévision)
 *
 * Paramètres :
 * - canton : pointeur vers le satellite courant
 * - i    : 0 = horaire, 1 = anti-horaire
 *
 * Retour :
 * - ExsaAspect : aspect SNCF à afficher
 *************************************************************************************/
ExsaAspect mettreAJourAspectCanton(Canton *canton, uint8_t i);
