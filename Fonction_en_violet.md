### void Canton::validateTopology() // 🟣:
🎯 Pourquoi je la mets en 🟣 (morte mais à réfléchir)
Parce que cette fonction :
valide SP1_idx / SM1_idx
protège contre des erreurs de topologie
évite des crashs si un JSON ou une trame EXCC est mal formée
était utile dans l’ancienne architecture
pourrait redevenir utile si tu ajoutes une validation dynamique
Et surtout :

👉 Gestion Canton 2026 repose énormément sur la topologie dynamique.  
👉 Une validation de cohérence n’est jamais une mauvaise idée.

Donc oui, elle est morte dans le code actuel, mais elle a un rôle conceptuel important.

### void Canton::SP2_busy(bool v) // 🟡

📌 Analyse
Cette méthode est appelée uniquement dans :

✔ CAN exploitation
→ mise à jour de l’état d’occupation du voisin secondaire SP2
→ typiquement envoyé par EXCC dans les trames d’exploitation

Elle n’apparaît pas dans :
SupervisionCanton
SupervisionCAN
SupervisionEssieux
AspectSignal
PilotageDistribue
TopologieSat
JSON save
JSON debug
Navigation topologique
Propagation d’occupation
Logique de rôle
Signaux SNCF

Donc :
SP2_busy est reçu,
mais jamais utilisé dans la logique interne.

📌 Utilité réelle
SP2_busy() devrait indiquer :
si le voisin secondaire SP2 est occupé
pour éviter d’envoyer un train dans un chemin secondaire occupé
pour la sécurité
pour la signalisation
pour la supervision

Mais dans l’état actuel du code :
❗ L’information est stockée… mais jamais exploitée.
Aucun module ne lit m_SP2_busy.

Donc :
l’occupation SP2 n’est jamais utilisée pour la sécurité
les signaux ne l’utilisent pas
la supervision ne l’utilise pas
la topologie ne l’utilise pas
l’UI ne l’affiche pas
JSON ne la sauvegarde pas

### void Aig::speed(uint16_t v)

📌 Utilité réelle
Aig::speed() sert à :
charger la vitesse du servo depuis les paramètres JSON
permettre à l’utilisateur de configurer la vitesse dans l’UI (si exposée)
stocker la valeur dans l’objet Aig

Mais attention :
❗ Elle n’est pas utilisée ailleurs dans le code
→ ni dans WebHandler
→ ni dans Supervision
→ ni dans l’exécution du mouvement
→ ni dans l’UART
→ ni dans l’EXCC
→ ni dans la logique interne

Donc :
la valeur est chargée,
mais jamais utilisée pour piloter quoi que ce soit.

Pourquoi ?
Elle est appelée dans Settings JSON → donc pas morte
Mais elle n’est jamais utilisée pour piloter un servo
Elle n’a aucun impact fonctionnel dans l’état actuel du code
Elle pourrait être supprimée sans rien casser
…sauf si tu prévois d’utiliser la vitesse plus tard

### 📌 uint16_t Aig::speed() const // 🟡

📌 Utilité réelle
Aig::speed() const sert à :
exporter la vitesse du servo dans le JSON
permettre à l’UI ou à un autre module de la relire (si un jour utilisé)

Mais attention :
❗ Elle n’est jamais utilisée dans la logique interne
→ pas dans WebHandler
→ pas dans l’UART
→ pas dans la supervision
→ pas dans l’exécution du mouvement
→ pas dans EXSA
→ pas dans la topologie
→ pas dans les signaux

Donc :
la valeur est sauvegardée,
mais jamais utilisée pour quoi que ce soit.

Pourquoi ?
Elle est appelée dans Settings JSON → donc pas morte
Mais elle n’est jamais utilisée dans la logique du système
Elle n’a aucun effet réel
Elle pourrait être supprimée sans rien casser
…sauf si tu prévois d’utiliser la vitesse plus tard

📌 Le paradoxe SP2_busy
👉 Le setter est douteux (valeur reçue mais jamais exploitée ailleurs).
👉 Le getter est vivant (valeur lue dans la supervision).

Cela signifie :
soit la valeur vient d’ailleurs (ex : calcul interne, fusion occupation)
soit la logique est incomplète
soit SP2_busy est un vestige partiellement utilisé
soit EXCC envoie l’info mais CC ne l’exploite pas encore
Mais le getter est bel et bien utilisé, donc il doit rester.


### envoyerAiguillesDepuisEtatCourant() // 🟣

📌 1. Fonction analysée
cpp
void envoyerAiguillesDepuisEtatCourant()
{
    for (uint8_t idx = 0; idx < aigSize; ++idx)
    {
        Aig *aig = Settings::canton->getAig(idx);
        if (!aig)
            continue;

        uint8_t direction = aig->estDroit() ? 0 : 1;

        Settings::canton->aigRun(idx);

        CC_LOG_INFO("[TopoUART][CC] F0 → servo=%u direction=%u (estDroit=%u)\n",
                    idx, direction, aig->estDroit());
    }

    CC_LOG_INFO("[TopoUART][CC] Aiguilles renvoyées (F0) après reboot EXCC\n");
}
📌 2. Occurrences trouvées
Code
src/SatTopologieUART.h
src/SatTopologieUART/SatTopo_UART_Aiguilles.cpp
⚠️ Aucune autre occurrence.
Elle n’est JAMAIS appelée.

📌 3. Analyse fonctionnelle
Cette fonction :
parcourt toutes les aiguilles logiques
récupère leur état logique (estDroit())
appelle aigRun(idx) → envoie la trame 0x06 à EXCC
donc déclenche le mouvement physique du servo
logge la synchronisation

C’est exactement la fonction prévue pour :
✔ resynchroniser EXCC après un reboot
✔ renvoyer l’état logique complet
✔ réaligner les servos physiques
✔ garantir la cohérence logique ↔ physique

📌 4. Importance théorique
Si EXCC ne garde rien en mémoire, alors :
au reboot, les servos sont dans un état aléatoire
CC croit qu’ils sont dans l’état logique
incohérence totale
danger pour la sécurité directionnelle
Conditions::voieOuverte() devient fausse
FeuxDirection::compute() devient incohérent
Donc cette fonction est structurellement importante.

📌 5. Importance réelle dans le code actuel
👉 Elle n’est jamais appelée.

Donc :
aucune resynchronisation après reboot
EXCC démarre dans un état inconnu
les servos ne sont jamais remis dans l’état logique
la fonction est orpheline

📌 6. Impact si on la supprime
Techniquement :
→ aucun impact immédiat, car elle n’est jamais utilisée.

Architecturalement :
→ gros risque, car elle est censée être utilisée.

✔ potentiellement indispensable → si EXCC reboot, elle DOIT être appelée

### 