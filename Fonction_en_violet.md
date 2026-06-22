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

### 📌 void Canton::SM2_busy(bool v) // 🟡

📌 Analyse
Cette méthode est appelée uniquement dans :
✔ CAN exploitation
→ mise à jour de l’état d’occupation du voisin secondaire SM2
→ typiquement envoyé par EXCC dans les trames d’exploitation

Elle n’apparaît pas dans :
SupervisionCanton
SupervisionCAN
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
SM2_busy est reçu,
mais jamais utilisé dans la logique interne.

📌 Utilité réelle
SM2_busy() devrait indiquer :
si le voisin secondaire SM2 est occupé
pour éviter d’envoyer un train dans un chemin secondaire occupé
pour la sécurité
pour la signalisation
pour la supervision

Mais dans l’état actuel du code :
❗ L’information est stockée… mais jamais exploitée.
Aucun module ne lit m_SM2_busy via le setter.

Pourquoi ?
Elle est appelée dans CAN exploitation → donc vivante
Mais elle n’est jamais lue ailleurs
Elle n’a aucun effet sur la supervision, les signaux, la topologie
Elle pourrait être supprimée sans rien casser
…sauf si tu prévois d’utiliser SM2_busy plus tard

📌 Le paradoxe SM2_busy
👉 Le setter est douteux (valeur reçue mais jamais exploitée ailleurs).
👉 Le getter est vivant (valeur lue dans la supervision).

Cela signifie :
soit la valeur vient d’ailleurs (ex : fusion occupation, propagation interne)
soit la logique est incomplète
soit SM2_busy est un vestige partiellement utilisé
soit EXSA envoie l’info mais Discovery ne l’exploite pas encore complètement
Mais le getter est bel et bien utilisé, donc il doit rester.

### 📌 CantonPeriph *Canton::voisinSP2() // 🟡

📌 Analyse immédiate
👉 Ce getter ne renvoie PAS m_SP2_idx.  
👉 Il renvoie toujours getCantonP(2).

Donc :
SP2 n’est pas configurable
SP2 n’est pas lu depuis la topologie
SP2 n’est pas lu depuis le JSON
SP2 est hardcodé à l’index 2 dans le tableau cantonP[]

C’est un choix d’architecture historique :
SP1 = index 0, SM1 = index 1, SP2 = index 2, SM2 = index 3.

Mais attention :
SP2_idx n’existe même pas dans la classe Canton.

📌 Utilité réelle
voisinSP2() sert à :
naviguer vers le voisin secondaire SP2
gérer les feux directionnels
gérer les transitions d’aspects
gérer la logique de circulation secondaire
gérer computeRole() dans les topologies complexes

Même si SP2 est secondaire, il est réellement utilisé dans :
✔ Feux directionnels
→ pour déterminer la direction aval
✔ Logique de circulation
→ pour déterminer les sorties possibles
✔ Signaux transitions
→ pour calculer les transitions d’aspects
✔ Topologie
→ pour vérifier la cohérence

📌 Problème structurel
Le fait que SP2 soit hardcodé à 2 signifie :
impossible d’avoir une topologie où SP2 ≠ 2
impossible de reconfigurer SP2 via JSON
impossible de reconfigurer SP2 via TopologieSat
incohérence avec SP1/SM1 qui eux sont configurables
incohérence avec SP2_acces / SP2_busy qui eux sont dynamiques

C’est un design asymétrique :

Élément	SP1	SM1	SP2	SM2
idx configurable	✔	✔	❌	❌
acces configurable	✔	✔	✔	✔
busy dynamique	✔	✔	✔	✔
voisin via idx	✔	✔	❌	❌


Donc SP2/SM2 sont semi‑dynamiques :
→ occupation OK
→ accessibilité OK
→ mais topologie figée.

📌 Code couleur
voisinSP2() est appelé dans la logique interne, donc :

Pourquoi ?
utilisée dans FeuxDirection
utilisée dans Logic
utilisée dans SignauxTransitions
utilisée dans Topologie
indispensable pour la circulation secondaire

Mais…
SP2_idx n’existe pas
SP2 est hardcodé
incohérence avec SP1/SM1
incohérence avec SP2_acces / SP2_busy

🔥 Résumé clair
Élément	Statut	Raison
voisinSP2()	🟢 vivante	utilisée dans la logique interne
Design SP2	⚠️ bancal	index hardcodé, pas configurable

Si tu veux, je peux t’aider à :
réparer la symétrie SP1/SM1/SP2/SM2,
réécrire la topologie pour rendre SP2/SM2 configurables

### 📌 CantonPeriph *Canton::voisinSM2() // 🟡

📌 Analyse immédiate
👉 Comme voisinSP2(), cette méthode ne renvoie pas un index configuré.
👉 Elle renvoie toujours getCantonP(3).

Donc :
SM2 n’est pas configurable
SM2 n’est pas lu depuis la topologie
SM2 n’est pas lu depuis le JSON
SM2 est hardcodé à l’index 3 dans cantonP[]

C’est la symétrie exacte de SP2 → index 2.

📌 Analyse détaillée
✔ Canton_FeuxDirection.cpp
→ utilisé comme fallback si SP1/SM1/SP2 ne donnent rien
→ donc utilisé dans la logique des feux directionnels

✔ Canton_Topologie.cpp
→ utilisé dans computeRole() et les vérifications internes

📌 Importance fonctionnelle
Même si SM2 est un voisin secondaire, il est utilisé dans :
la logique des feux directionnels
la logique topologique interne
la navigation secondaire
les transitions d’aspects indirectes (via computeRole)

Sans ce getter :
les feux directionnels pourraient être incohérents
les chemins secondaires côté M deviendraient inaccessibles
la topologie serait incomplète

📌 Problème structurel (comme SP2)
Le design SP2/SM2 est asymétrique :

Élément	SP1	SM1	SP2	SM2
idx configurable	✔	✔	❌	❌
acces configurable	✔	✔	✔	✔
busy dynamique	✔	✔	✔	✔
voisin via idx	✔	✔	❌	❌


Donc SP2/SM2 sont :
dynamiques pour acces/busy
figés pour la topologie
C’est un design hérité des versions 2023–2024.

📌 Code couleur
voisinSM2() est appelé dans la logique interne, donc :

Pourquoi ?
utilisée dans FeuxDirection
utilisée dans Topologie
indispensable pour la circulation secondaire

Mais…
SM2_idx n’existe pas
SM2 est hardcodé
incohérence avec SP1/SM1
incohérence avec SM2_acces / SM2_busy

🔥 Résumé clair
Élément	Statut	Raison
voisinSM2()	🟢 vivante	utilisée dans FeuxDirection + Topologie
Design SM2	⚠️ bancal	index hardcodé, pas configurable

Si tu veux, je peux t’aider à :
rendre SP2/SM2 symétriques avec SP1/SM1,
réécrire la topologie pour rendre SP2/SM2 configurables,

### 📌 bool Canton::SP1_estAccessible() // 🟣

📌 Analyse fonctionnelle
Cette méthode :
récupère le voisin SP1 via voisinSP1()

vérifie :
que le voisin existe
que v->acces() est vrai
que v->busy() est faux

C’est donc une méthode utilitaire qui encapsule :

C’est exactement la même logique que :
peutEntrerDansVoisin()
estAccesAutorise()
SP2_estAccessible() (si elle existait)

Mais…

📌 Problème : elle n’est jamais utilisée
Tu m’as donné toutes les occurrences, et :
SupervisionCanton → ne l’utilise pas
SupervisionEssieux → ne l’utilise pas
AspectSignal → ne l’utilise pas
Canton_Logic → ne l’utilise pas
TopologieSat → ne l’utilise pas
JSON → ne l’utilise pas
CAN → ne l’utilise pas

Donc :

👉 Elle ne sert à rien dans l’état actuel du code.
📌 Pourquoi existe‑t‑elle ?
Probablement pour :
simplifier la lecture du code
préparer une API plus propre
remplacer un jour les appels directs à v->acces() et v->busy()
être symétrique avec une future SM1_estAccessible() / SP2_estAccessible() / SM2_estAccessible()

Mais aujourd’hui :
❗ Elle n’a aucun impact fonctionnel.

Pourquoi ?
Elle n’est appelée nulle part
Elle n’a aucun impact sur la logique
Elle pourrait être supprimée sans rien casser
Elle semble être un vestige ou une API prévue mais jamais utilisée

### 📌 bool Canton::SM1_estAccessible() // 🟣

📌 Analyse fonctionnelle
Cette méthode encapsule :
existence du voisin SM1
accessibilité (v->acces())
non‑occupation (!v->busy())

C’est exactement la même logique que :
SP1_estAccessible()
la logique interne de peutEntrerDansVoisin()
la logique interne de estAccesAutorise()

Mais contrairement à ces deux dernières, elle n’est jamais utilisée.

📌 Pourquoi existe‑t‑elle ?
Probablement pour :
préparer une API plus propre
offrir une méthode simple pour vérifier l’accessibilité d’un voisin
être symétrique avec SP1_estAccessible()
simplifier un futur refactoring de la logique de circulation

Mais aujourd’hui :
👉 Elle ne sert à rien dans la logique Discovery 2026.

Pourquoi ?
jamais appelée
aucun impact fonctionnel
peut être supprimée sans risque
semble être un vestige ou une API prévue mais non utilisée

### 📌 bool Canton::SP2_estAccessible() // 🟣

⚠️ Aucune autre occurrence dans tout le projet.  
Donc elle n’est jamais appelée.

📌 Analyse fonctionnelle
Cette méthode encapsule :
l’accessibilité SP2 (m_SP2_acces)
la non‑occupation SP2 (!m_SP2_busy)
C’est la version “simple” de :
SP1_estAccessible()
SM1_estAccessible()
et ce que devraient être SM2_estAccessible() (si elle existait)

Mais contrairement aux getters SP2/SM2 utilisés dans la supervision :

👉 Cette méthode n’est utilisée nulle part.
📌 Pourquoi existe‑t‑elle ?
Probablement pour :
préparer une API propre et symétrique
offrir un raccourci lisible pour la logique SP2
être cohérente avec SP1/SM1
simplifier un futur refactoring de la logique de circulation

Mais aujourd’hui :
❗ Elle n’a aucun impact fonctionnel.
Donc :
👉 Elle peut être supprimée sans rien casser.

Pourquoi ?
jamais appelée
aucun impact
peut être supprimée sans risque
semble être un vestige ou une API prévue mais non utilisée

### 📌 bool Canton::SM2_estAccessible() // 🟣

⚠️ Aucune autre occurrence dans tout le projet.  
Donc elle n’est jamais appelée.

📌 Analyse fonctionnelle
Cette méthode encapsule :
l’accessibilité SM2 (m_SM2_acces)
la non‑occupation SM2 (!m_SM2_busy)
C’est la version “simple” de :
SP1_estAccessible()
SM1_estAccessible()
SP2_estAccessible()

Mais contrairement aux getters SM2_acces() et SM2_busy() :
👉 Cette méthode n’est utilisée nulle part dans la logique Discovery 2026.
📌 Pourquoi existe‑t‑elle ?
Probablement pour :
compléter la symétrie SP1 / SM1 / SP2 / SM2
préparer une API plus propre
simplifier un futur refactoring de la logique de circulation
offrir un raccourci lisible pour la logique SM2

Mais aujourd’hui :
❗ Elle n’a aucun impact fonctionnel.

Donc :
👉 Elle peut être supprimée sans rien casser.

Pourquoi ?
jamais appelée
aucun impact
peut être supprimée sans risque
semble être un vestige ou une API prévue mais non utilisée

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
SA croit qu’ils sont dans l’état logique
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