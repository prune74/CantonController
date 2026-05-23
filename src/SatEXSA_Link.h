#pragma once
#include <stdint.h>

/*
 * SatEXSA_Link.h
 * ------------------------------------------------------------
 * Gestion de la "vie" des modules EXSA vue par le SA.
 *
 * Rôle :
 *   - Envoyer périodiquement des trames PING à chaque EXSA
 *   - Lire les trames PONG renvoyées par les EXSA
 *   - Maintenir un état ONLINE / OFFLINE par EXSA
 *   - Déclencher des hooks de resynchronisation quand un EXSA
 *     revient ONLINE après un reboot
 *
 * Ce module ne fait AUCUNE logique ferroviaire :
 *   - Il ne calcule pas d’aspect
 *   - Il ne gère pas les itinéraires
 *   - Il ne gère pas les aiguilles
 *
 * Il se contente de :
 *   - Superviser la liaison UART SA ↔ EXSA
 *   - Informer le reste du SA via onExsaOnline()/onExsaOffline()
 */

namespace SatEXSA_Link
{
    /*
     * begin()
     * ---------------------------------------------------------
     * À appeler au démarrage du SA (setup()).
     * Initialise l’état interne des EXSA (ONLINE/OFFLINE).
     */
    void begin();

    /*
     * loop()
     * ---------------------------------------------------------
     * À appeler régulièrement dans la loop principale du SA.
     *
     * Rôle :
     *   - envoyer périodiquement des PING
     *   - lire les réponses PONG
     *   - détecter les timeouts (EXSA OFFLINE)
     *   - appeler onExsaOnline() / onExsaOffline()
     */
    void loop();

    /*
     * onExsaOnline(index)
     * ---------------------------------------------------------
     * Hook appelé quand un EXSA passe de OFFLINE → ONLINE,
     * typiquement après un reboot EXSA détecté.
     *
     * index :
     *   0 = EXSA côté HORAIRE
     *   1 = EXSA côté ANTI-HORAIRE
     *
     * Rôle typique :
     *   - renvoyer la topologie (E4)
     *   - renvoyer la config signaux (E5)
     *   - renvoyer la config servos (F1)
     *   - renvoyer occupation voisins (EA)
     *   - renvoyer aspects et directions en cours (E6/E7/E8/E9)
     *
     * L’implémentation par défaut est fournie dans le .cpp,
     * mais tu peux l’adapter selon tes besoins.
     */
    void onExsaOnline(uint8_t index);

    /*
     * onExsaOffline(index)
     * ---------------------------------------------------------
     * Hook appelé quand un EXSA passe de ONLINE → OFFLINE.
     *
     * Rôle typique :
     *   - marquer le module comme indisponible
     *   - empêcher l’envoi de nouvelles commandes vers cet EXSA
     *   - (optionnel) afficher une alerte dans l’UI
     */
    void onExsaOffline(uint8_t index);

    /*
     * isOnline(index)
     * ---------------------------------------------------------
     * Permet au reste du code de savoir si un EXSA est vu
     * comme ONLINE par la supervision PING/PONG.
     */
    bool isOnline(uint8_t index);

    /*
     * Helpers lisibles Discovery 2026
     * ---------------------------------------------------------
     * Fournissent une API claire pour savoir quel EXSA est actif.
     *
     * isHoraireOnline()     → EXSA 0 (côté H)
     * isAntiHoraireOnline() → EXSA 1 (côté AH)
     *
     * Ces helpers ne changent rien à la logique interne :
     * ils appellent simplement isOnline(index).
     */
    inline bool isHoraireOnline()     { return isOnline(0); }
    inline bool isAntiHoraireOnline() { return isOnline(1); }
}
