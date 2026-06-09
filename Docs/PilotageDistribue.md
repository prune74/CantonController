# PilotageDistribue.md

## 🧠 Rôle

Le module `PilotageDistribue.cpp` pilote la locomotive en fonction de l’aspect reçu depuis le canton aval. Il applique la logique SNCF en tenant compte du sens de marche et des signaux reçus par les satellites voisins.

---

## 🔧 Fonction principale

```cpp
void executerPilotageDistribue(Canton* canton);
```

- **Entrée** : pointeur vers le `Canton` courant
- **Sortie** : appel à `pilotageDepuisAspect()` avec l’aspect à appliquer

---

## 🔄 Logique de pilotage

```plaintext
┌────────────┐
│  Canton      │
└────┬───────┘
     │
     ▼
┌────────────┐
│ sens()     │
└────┬───────┘
     │
     ▼
┌────────────┐
│ CantonPeriph │
│ aspectRecu │
└────┬───────┘
     │
     ▼
┌────────────────────────────┐
│ pilotageDepuisAspect()     │
└────────────────────────────┘
```

---

## 🧩 Détail du comportement

| Sens de marche | Voisin consulté         | Aspect utilisé           | Direction logique |
|----------------|--------------------------|---------------------------|-------------------|
| `horaire`      | `cantonP[SP1_idx()]`       | `aspectRecu[0]`           | vers l’avant      |
| `antiHor`      | `cantonP[SM1_idx()]`       | `aspectRecu[1]`           | vers l’arrière    |
| `inconnu`      | —                        | Aucun pilotage            | —                 |

---

## 📦 Dépendances

- `PilotageDistribue.h` : déclaration de `executerPilotageDistribue`
- `PilotageLoco.h` : contient `pilotageDepuisAspect()`
- `SensEnum.h` : enum `horaire`, `antiHor`
- `Canton.h` : structure du satellite courant
- `CantonPeriph.h` : structure des voisins

---

## 🧠 Notes pédagogiques

- Le pilotage est **réactif** : il dépend uniquement de l’aspect reçu du canton aval
- La logique est **modulaire** : chaque `Canton` agit selon son propre contexte
- Le système est **extensible** : on peut ajouter des cas pour `ralentissement`, `masquage`, etc.
