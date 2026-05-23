#pragma once
#include <stdint.h>

/* =============================================================================
 *  SatTopologieUART.h — API publique du module de communication UART → EXSA
 *  =============================================================================
 *
 *  Ce module constitue la *couche de transport* entre le firmware Discovery
 *  (côté SA) et les modules EXSA (côté terrain). Il encapsule l’intégralité
 *  des trames UART du protocole EXSA :
 *
 *      - E4 : Topologie SP/SM (voisins CAN)
 *      - E5 : Configuration des signaux
 *      - E6/E7 : Aspects SNCF (H / AH)
 *      - E8/E9 : Feux directionnels (H / AH)
 *      - EA : Occupation SP1 / SM1
 *      - F0/F1/F2 : Commandes servos (mouvement / configuration / test)
 *
 *  L’objectif de ce header est de fournir une API claire, stable et
 *  documentée, indépendante de l’implémentation interne. Les .cpp associés
 *  sont rangés dans le dossier `SatTopologieUART/` et ne doivent contenir
 *  *aucune logique métier* : uniquement la traduction SA → EXSA.
 *
 *  Ce module est volontairement découplé de la logique ferroviaire :
 *  - Il ne décide rien.
 *  - Il ne calcule rien.
 *  - Il ne valide rien.
 *
 *  Il se contente d’**envoyer** à EXSA ce que les autres modules ont décidé.
 *
 *  =============================================================================
 *  SECTION 1 — TOPOLOGIE (opcode E4)
 *  =============================================================================
 *
 *  La topologie représente la structure locale du réseau vue par EXSA :
 *
 *      - Les voisins "précédents" (m00/m01/m10/m11)
 *      - Les voisins "suivants"   (p00/p01/p10/p11)
 *
 *  Ces informations sont stockées dans settings.json et envoyées à EXSA
 *  sous forme d’une trame E4 :
 *
 *      [SYNC] [E4] [idLocal] [nbPrecedents] [liste] [nbSuivants] [liste]
 *
 *  EXSA utilise cette topologie pour :
 *      - router les aspects
 *      - router les feux directionnels
 *      - router les occupations
 *      - reconstruire le graphe ferroviaire local
 *
 *  IMPORTANT :
 *      La topologie n’est envoyée qu’une seule fois après le boot EXSA.
 *      Le SA attend que settings.json contienne au moins un voisin valide.
 *
 *  Les fonctions ci‑dessous sont les *seules* à devoir être appelées depuis
 *  l’extérieur. Toute la logique interne (lecture JSON, vérifications, etc.)
 *  est encapsulée dans SatTopo_UART_Topologie.cpp.
 *
 * =============================================================================
 */


/**
 * @brief Envoie la topologie SP/SM à EXSA (opcode E4).
 *
 * Cette fonction :
 *   1. Lit settings.json
 *   2. Extrait les voisins précédents (m00..m11)
 *   3. Extrait les voisins suivants   (p00..p11)
 *   4. Construit la trame E4
 *   5. L’envoie sur UART vers EXSA
 *
 * Elle ne réalise *aucune* validation métier : elle transmet simplement
 * ce que settings.json contient.
 *
 * À appeler uniquement lorsque la topologie est prête.
 */
void envoyerTopologieDepuisSettings();


/**
 * @brief Déclenche automatiquement l’envoi de la topologie dès qu’elle est prête.
 *
 * Fonction à appeler régulièrement (ex : dans la boucle de supervision).
 *
 * Comportement :
 *   - Vérifie si settings.json contient au moins un voisin valide
 *   - Si oui :
 *        * Envoie la topologie (E4)
 *        * Envoie la configuration des signaux (E5)
 *        * Se désactive définitivement (ne renvoie plus rien)
 *
 * Cette fonction garantit que EXSA reçoit la topologie *exactement une fois*
 * après un reboot, même si le SA redémarre plusieurs fois.
 */
void envoyerTopologieSiPret();

/* =============================================================================
 *  SECTION 2 — SIGNAUX (E5 / E6 / E7)
 * =============================================================================
 *
 *  Cette section gère l’ensemble des trames EXSA liées aux signaux :
 *
 *    - E5 : Configuration des signaux
 *           (type + position physique sur le module)
 *
 *    - E6 : Aspect SNCF horaire
 *    - E7 : Aspect SNCF anti-horaire
 *
 *  Le SA ne calcule pas les aspects ici : il se contente d’envoyer
 *  à EXSA ce que la logique métier (SupervisionCanton) a déterminé.
 *
 *  Le but est de séparer clairement :
 *      - la logique ferroviaire (SupervisionCanton)
 *      - la logique de transport (ce module)
 *
 *  Ce découpage rend le protocole EXSA lisible, testable et maintenable.
 * =============================================================================
 */

/**
 * @brief Envoie la configuration des signaux (opcode E5).
 *
 * Lit dans settings.json :
 *    - sign0type / sign1type
 *    - sign0position / sign1position
 *
 * Puis transmet ces informations telles quelles à EXSA.
 */
void envoyerConfigurationSignauxDepuisSettings();

/**
 * @brief Envoie l’aspect SNCF horaire (opcode E6).
 *
 * L’aspect est un enum ExsaAspect (1 octet).
 * La logique métier est effectuée dans SupervisionCanton.
 */
void envoyerAspectSignalHoraire(uint8_t aspect);

/**
 * @brief Envoie l’aspect SNCF anti-horaire (opcode E7).
 *
 * Même principe que pour l’aspect horaire.
 */
void envoyerAspectSignalAntiHoraire(uint8_t aspect);

/* =============================================================================
 *  SECTION 3 — FEUX DIRECTIONNELS (E8 / E9)
 * =============================================================================
 *
 *  Les feux directionnels sont une innovation Discovery 2026 :
 *  ils indiquent à EXSA la direction logique du trafic sur le canton.
 *
 *  Le calcul est effectué dans :
 *      Node_FeuxDirection.cpp → updateFeuDirection()
 *
 *  Ce module n’a qu’un rôle :
 *      → transmettre à EXSA les codes calculés (0..4)
 *
 *  - E8 : Feu directionnel horaire
 *  - E9 : Feu directionnel anti-horaire
 *
 *  Les valeurs envoyées sont des entiers 0..4 correspondant aux états :
 *      0 = Aucun feu
 *      1 = Feu blanc fixe
 *      2 = Feu blanc clignotant
 *      3 = Feu double
 *      4 = Erreur / incohérence
 *
 *  Ce découpage garantit une séparation stricte :
 *      - logique métier (Node)
 *      - transport protocolaire (ce module)
 * =============================================================================
 */

/**
 * @brief Envoie les feux directionnels calculés (opcode E8 + E9).
 *
 * Met à jour les feux dans le Node, lit les codes (0..4),
 * puis les transmet à EXSA.
 */
void envoyerFeuxDepuisEtatCourant();

/**
 * @brief Envoie le feu directionnel horaire (opcode E8).
 */
void envoyerFeuDirectionHoraire(uint8_t code);

/**
 * @brief Envoie le feu directionnel anti-horaire (opcode E9).
 */
void envoyerFeuDirectionAntiHoraire(uint8_t code);

/* =============================================================================
 *  SECTION 4 — AIGUILLES (F0)
 * =============================================================================
 *
 *  Les aiguilles logiques sont gérées dans :
 *      - Aig.cpp
 *      - Node_Aiguilles.cpp
 *
 *  Ce module ne fait qu’une seule chose :
 *      → transmettre à EXSA l’ordre de mouvement réel (opcode F0)
 *
 *  Le SA ne décide pas de la position servo :
 *      - estDroit() / estDevie() sont déjà déterminés par la logique métier
 *      - EXSA choisit la position réelle (posDroit / posDevie)
 *
 *  Le rôle de F0 est donc :
 *      “Synchroniser l’état logique des aiguilles avec l’état physique EXSA”
 *
 *  Ce mécanisme est essentiel après :
 *      - un reboot EXSA
 *      - un changement de rôle
 *      - une reconfiguration
 * =============================================================================
 */

/**
 * @brief Envoie l’état logique des aiguilles (opcode F0).
 *
 * Pour chaque aiguille logique (0..5) :
 *   - détermine quel EXSA la pilote (horaire ou anti-horaire)
 *   - envoie F0 pour demander le mouvement réel
 *
 * Cette fonction est appelée :
 *   - après un reboot EXSA
 *   - lors d’une resynchronisation
 *   - lors d’un changement de rôle
 */
void envoyerAiguillesDepuisEtatCourant();

/* =============================================================================
 *  SECTION 5 — SERVOS (F0 / F1 / F2)
 * =============================================================================
 *
 *  Cette section regroupe l’ensemble des commandes EXSA liées aux servos.
 *
 *  - F0 : Mouvement réel du servo
 *         (EXSA applique posDroit ou posDevie selon l’état logique)
 *
 *  - F1 : Configuration physique du servo
 *         (posDroit / posDevie / speed)
 *
 *  - F2 : Test manuel du servo
 *
 *  RÔLE DE CHAQUE COMMANDE :
 *  --------------------------
 *
 *  • F0 (servoMove)
 *      → Synchronise l’état logique (estDroit / estDevie) avec l’état physique.
 *      → Utilisé par envoyerAiguillesDepuisEtatCourant().
 *      → EXSA choisit automatiquement posDroit ou posDevie.
 *
 *  • F1 (servoConfig)
 *      → Transmet à EXSA les paramètres physiques définis dans settings.json :
 *            aigXposDroit  = position PWM en voie directe
 *            aigXposDevie  = position PWM en voie déviée
 *            aigXspeed     = slider 0..10 (converti en 11000 → 1000)
 *
 *  • F2 (servoTest)
 *      → Permet de tester un servo individuellement.
 *
 *  ARCHITECTURE :
 *  --------------
 *  Ce module garantit une séparation stricte :
 *
 *      - logique métier (Aig.cpp / Node_Aiguilles.cpp)
 *      - configuration physique (settings.json)
 *      - protocole EXSA (ce module)
 *
 *  Aucun calcul ferroviaire n’est effectué ici.
 * =============================================================================
 */

/**
 * @brief Envoie une commande de mouvement réel du servo (opcode F0).
 *
 * EXSA applique automatiquement posDroit ou posDevie selon l’état logique
 * de l’aiguille. Utilisé notamment par envoyerAiguillesDepuisEtatCourant().
 */
void envoyerServoMove(uint8_t exsaAdresse, uint8_t servoIndex);

/**
 * @brief Envoie la configuration des servos (opcode F1).
 *
 * Lit dans settings.json :
 *    - aigXposDroit
 *    - aigXposDevie
 *    - aigXspeed (slider 0..10)
 *
 * Convertit le slider en vitesse EXSA (11000 → 1000),
 * puis transmet la configuration à EXSA.
 */
void envoyerConfigurationServosDepuisSettings();

/**
 * @brief Envoie une configuration servo individuelle (opcode F1).
 */
void envoyerServoConfig(uint8_t exsaAdresse,
                        uint8_t servoIndex,
                        uint16_t posDroit,
                        uint16_t posDevie,
                        uint16_t speed);

/**
 * @brief Envoie une commande de test servo (opcode F2).
 */
void envoyerServoTest(uint8_t exsaAdresse, uint8_t servoIndex);


/* =============================================================================
 *  SECTION 6 — OCCUPATION (EA)
 * =============================================================================
 *
 *  Cette section transmet à EXSA l’état d’occupation des deux voisins
 *  directs du canton :
 *
 *      - SP1 (sens horaire)
 *      - SM1 (sens anti-horaire)
 *
 *  Ces informations proviennent de :
 *      NodePeriph::busy()
 *
 *  Le SA ne fait aucun calcul ici :
 *      → il lit l’état réel
 *      → il encode SP1/SM1 dans un octet (2 bits)
 *      → il transmet via opcode EA
 *
 *  Format envoyé :
 *
 *      bit 1 = SP1 occupé (1 = occupé)
 *      bit 0 = SM1 occupé (1 = occupé)
 *
 *  Ce mécanisme permet à EXSA de :
 *      - gérer les protections
 *      - anticiper les mouvements
 *      - synchroniser les cantons voisins
 * =============================================================================
 */

/**
 * @brief Envoie l’occupation SP1 / SM1 (opcode EA).
 *
 * Lit l’état d’occupation des deux voisins directs via NodePeriph,
 * encode les deux bits, puis transmet la valeur à EXSA.
 */
void envoyerOccupationDepuisEtatCourant();

/**
 * @brief Envoie directement la valeur d’occupation (2 bits).
 *
 * Utilisé par envoyerOccupationDepuisEtatCourant().
 */
void envoyerOccupationVoisins(uint8_t valeur);

/* =============================================================================
 *  SECTION 7 — ASPECTS SNCF (E6 / E7)
 * =============================================================================
 *
 *  Les aspects SNCF représentent l’état du signal ferroviaire :
 *
 *      - Voie libre
 *      - Avertissement
 *      - Ralentissement
 *      - Carré
 *      - Sémaphore
 *      - Etc.
 *
 *  Le calcul des aspects est effectué dans :
 *      SupervisionCanton.cpp → mettreAJourAspectCanton()
 *
 *  Ce module ne fait qu’une seule chose :
 *      → transmettre à EXSA les aspects calculés
 *
 *  - E6 : Aspect horaire
 *  - E7 : Aspect anti-horaire
 *
 *  Les aspects sont envoyés sous forme d’un enum ExsaAspect (1 octet).
 *
 *  Ce découpage garantit une séparation stricte :
 *      - logique ferroviaire (SupervisionCanton)
 *      - protocole EXSA (ce module)
 * =============================================================================
 */

/**
 * @brief Envoie les aspects SNCF calculés (opcode E6 + E7).
 *
 * Appelle mettreAJourAspectCanton() pour les deux sens,
 * puis transmet les valeurs à EXSA.
 */
void envoyerAspectsDepuisEtatCourant();
