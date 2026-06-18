#pragma once

#include "Canton.h"

/*
 * CapteursEtat.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Mise à jour locale des capteurs ponctuels et de la locomotive
 * en fonction de l’occupation du canton.
 *
 * Rôle :
 *   - si le canton est libre :
 *        • reset des capteurs ponctuels (H / AH)
 *        • reset complet de la locomotive (vitesse, sens, adresse)
 *
 *   - si le canton est occupé :
 *        • clamp de la vitesse si > maxSpeed()
 *
 * Ce module ne contient aucune logique globale :
 *   → il applique uniquement les règles locales du canton.
 */

/*
 * mettreAJourCapteurs()
 * ---------------------------------------------------------------------------
 * Applique les règles locales d’état des capteurs et de la locomotive.
 *
 * Paramètre :
 *   - canton : pointeur vers le canton local
 */
void mettreAJourCapteurs(Canton *canton);
