#ifndef NODE_INTERNAL_H
#define NODE_INTERNAL_H

/*
 * Node_Internal.h — Déclarations internes du module Node (Discovery 2026)
 * -----------------------------------------------------------------------
 * Ce fichier est destiné uniquement aux fichiers .cpp du dossier /Node/.
 *
 * Il contient :
 *   - les includes communs
 *   - les déclarations forward nécessaires
 *   - quelques constantes internes au module
 *
 * IMPORTANT :
 *   - Ne pas inclure ce fichier en dehors de /Node/.
 *   - L’API publique reste définie dans Node.h.
 */

#include <Arduino.h>
#include "Node.h"
#include "ConsoCourant.h"
#include "SatTopologieUART.h"   // envoyerServoMove() vers EXSA
#include "debug_sa.h"

/*
 * Forward declarations internes si besoin
 * ---------------------------------------
 * (Ici, Node et NodePeriph sont déjà connus via Node.h)
 *
 * class Node;
 * class NodePeriph;
 */

/*
 * Constantes internes Node
 * ------------------------
 * Elles peuvent être utilisées par plusieurs sous-modules Node/*.cpp
 * sans polluer l’API publique.
 */

// ID spécial pour indiquer qu’aucun voisin / canton n’est configuré
static const uint8_t NODE_UNUSED_ID = UNUSED_ID;

// Index des capteurs ponctuels (cohérent avec ton Node.cpp existant)
static const uint8_t IDX_CAPT_ANTIHORAIRE = 0;
static const uint8_t IDX_CAPT_HORAIRE    = 1;

/*
 * Helpers internes (si besoin plus tard)
 * --------------------------------------
 * On peut ajouter ici des fonctions inline d’aide au debug ou à la
 * cohérence interne, sans les exposer à l’extérieur.
 */

#endif // NODE_INTERNAL_H
