# 🧩 Module `AspectSignal`

## 📌 Rôle

Déduit les aspects dynamiques à afficher en fonction des cantons aval (SP1 / SM1) et de la position des aiguilles. Envoie ces aspects via UART et CAN (trame 0xE7) uniquement si changement détecté et temporisation respectée.

---

## 🔧 API

```cpp
void mettreAJourAspectSignal(Canton* canton, uint8_t* signalValue);
```
- `canton` : pointeur vers le nœud courant
- `signalValue` : tableau des aspects à afficher (0 = horaire, 1 = anti-horaire)*

---

## 🧪 Exemple d’usage

---

```cpp
// À placer dans loopTask après supervision cantonale
mettreAJourAspectSignal(canton, signalValue);
```

---

## 📁 Dépendances

---

**Canton.h** : structure du nœud
**DeductionAspect.h** : fonction deduireAspectDepuisAval(...)
**CanMsg.h** : envoi des trames CAN