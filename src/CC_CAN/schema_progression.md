Pipeline Gestion Canton 2026 — Vérification finale
Voici ce que ton système fait réellement maintenant, du Booster EXCC jusqu’à l’ERM :

# 1. EXCC détecte l’adresse RailCom
→ t.railcomAddress  
→ détectée physiquement dans le cutout

# 2. EXCC envoie l’adresse au CC
→ EXCC_CAN_CC::envoyerRailcom()  
→ trame : type, low, high

# 3. CC reçoit RAILCOM_ADRESSE
→ Railcom::onRailcom(d[1], d[2])  
→ reconstruction correcte de l’adresse
→ Railcom::address() fiable

# 4. CC0 vérifie estMesurable()
→ occupation
→ aiguilles
→ aval libre
→ aspect ferroviaire
→ loco connue via RailCom
→ OK

# 5. CC0 envoie MESURE_PREPARE à CCx
→ Cmd_CC_to_CC::MESURE_PREPARE  
→ locoID inclus

# 6. CCx reçoit MESURE_PREPARE
→ handleExploit_EB()  
→ canton->armer(trainID)  
→ mesureActive = true

# 7. CCx détecte entrée / sortie
→ onEntree() → t0
→ onSortie() → t1
→ dt = t1 - t0
→ v = L / dt
→ vitesse calculée

# 8. CCx envoie MESURE_VITESSE à l’ERM
→ CC_CAN::sendMsg()  
→ locoID
→ vitesse × 1000
→ ID du CC source
→ trame envoyée

# 9. ERM reçoit la vitesse réelle du train
→ pipeline complet
→ boucle fermée