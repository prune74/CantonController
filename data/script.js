/* ============================================================================
 * WebSocket Canton — Gestion Canton 2026
 * ----------------------------------------------------------------------------
 * Rôle :
 *   - établir la connexion WebSocket
 *   - recevoir l’état complet du canton
 *   - mettre à jour l’UI
 *   - envoyer les commandes utilisateur
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune logique ferroviaire
 *   - ce module ne fait que relayer UI ↔ CC
 * ==========================================================================*/

/* ============================================================================
 * Connexion WebSocket
 * ==========================================================================*/

const gateway = "ws://" + window.location.hostname + "/ws";
let websocket = null;

function initWebSocket() {
  console.log("[WS] Tentative de connexion…");
  websocket = new WebSocket(gateway);

  websocket.onopen    = onOpen;
  websocket.onclose   = onClose;
  websocket.onmessage = onMessage;
}

function onOpen() {
  console.log("[WS] Connecté");
  document.getElementById("messages").innerHTML = "Connected";
}

function onClose() {
  console.log("[WS] Déconnecté — nouvelle tentative dans 2s");
  document.getElementById("messages").innerHTML = "Connection closed";
  setTimeout(initWebSocket, 2000);
}

window.addEventListener("load", () => {
  initWebSocket();
  // Désactivation du rôle (legacy)
  const typeCanton = document.getElementById("typeCanton");
  if (typeCanton) typeCanton.disabled = true;
});

/* ============================================================================
 * Réception WebSocket
 * ==========================================================================*/

function onMessage(event) {
  console.log("[WS] Reçu :", event.data);

  let data = {};
  try {
    data = JSON.parse(event.data);
  } catch (e) {
    console.warn("[WS] JSON invalide :", e);
    return;
  }

  /* ------------------------------------------------------------------------
   * Commande spécifique : mise à jour du rôle (désactivé)
   * ------------------------------------------------------------------------ */
  if (data.cmd === "roleUpdate") {
    document.getElementById("typeCanton").value = 0;
    return;
  }

  /* ------------------------------------------------------------------------
   * Champs généraux
   * ------------------------------------------------------------------------ */
  setValue("idCanton", data.idCanton);

  setValue("p00", data.p00);
  setValue("p01", data.p01);
  setValue("p10", data.p10);
  setValue("p11", data.p11);

  setValue("m00", data.m00);
  setValue("m01", data.m01);
  setValue("m10", data.m10);
  setValue("m11", data.m11);

  /* ------------------------------------------------------------------------
   * Aiguilles
   * ------------------------------------------------------------------------ */
  for (let i = 0; i < 6; i++) {
    setValue("s" + i, data["s" + i]);
    setValue("s" + i + "0", data["s" + i + "0"]);
    setValue("s" + i + "1", data["s" + i + "1"]);
    setValue("s" + i + "2", data["s" + i + "2"]);
  }

  /* ------------------------------------------------------------------------
   * Vitesse
   * ------------------------------------------------------------------------ */
  setValue("maxSpeed", data.maxSpeed);

  /* ------------------------------------------------------------------------
   * Sens de marche
   * ------------------------------------------------------------------------ */
  switch (data.sensMarche) {
    case 0: document.getElementById("indifferent").checked = true; break;
    case 1: document.getElementById("horaire").checked = true; break;
    case 2: document.getElementById("antihoraire").checked = true; break;
  }

  /* ------------------------------------------------------------------------
   * Cibles
   * ------------------------------------------------------------------------ */
  document.getElementById("imageHoraire").src  = "cible_" + data.cibleHoraire + ".jpg";
  document.getElementById("imageAntiHor").src  = "cible_" + data.cibleAntiHor + ".jpg";

  /* ------------------------------------------------------------------------
   * Switchs
   * ------------------------------------------------------------------------ */
  document.getElementById("exploration_on").checked = data.exploration_on;
  document.getElementById("wifi_on").checked        = data.wifi_on;

  /* ------------------------------------------------------------------------
   * Type de canton (désactivé)
   * ------------------------------------------------------------------------ */
  document.getElementById("typeCanton").value = 0;

  /* ------------------------------------------------------------------------
   * Debug brut
   * ------------------------------------------------------------------------ */
  document.getElementById("rawData").innerText = event.data;
}

/* ============================================================================
 * Helpers UI
 * ==========================================================================*/

function setValue(id, value) {
  const el = document.getElementById(id);
  if (el !== null && value !== undefined)
    el.value = value;
}

/* ============================================================================
 * Envoi WebSocket
 * ==========================================================================*/

function sendJson() {
  const msg = {
    idCanton: document.getElementById("idCanton").value,
    comptAig: document.getElementById("comptAig").value,
    p00: document.getElementById("p00").value,
    p01: document.getElementById("p01").value,
    p10: document.getElementById("p10").value,
    p11: document.getElementById("p11").value,
    m00: document.getElementById("m00").value,
    m01: document.getElementById("m01").value,
    m10: document.getElementById("m10").value,
    m11: document.getElementById("m11").value
  };
  websocket.send(JSON.stringify(msg));
}

function servoSettings(obj) {
  websocket.send(JSON.stringify({ servoSettings: [obj.id, obj.value, obj.name] }));
}

function servoTest(obj) {
  websocket.send(JSON.stringify({ servoTest: [obj.name] }));
}

function wifi_on(obj) {
  websocket.send(JSON.stringify({ wifi_on: obj.checked }));
}

function exploration_on(obj) {
  websocket.send(JSON.stringify({ exploration_on: obj.checked }));
}

function restartEsp(obj) {
  websocket.send(JSON.stringify({ restartEsp: true }));
}

function save(obj) {
  websocket.send(JSON.stringify({ save: true }));
}

/* ============================================================================
 * Booster
 * ==========================================================================*/
function saveBooster() {
  const msg = {
    booster_seuils: [
      parseInt(document.getElementById("booster_seuil_libre").value),
      parseInt(document.getElementById("booster_seuil_occupe").value)
    ]
  };
  websocket.send(JSON.stringify(msg));
}

function calibBooster() {
  websocket.send(JSON.stringify({ cmd: "calibBooster" }));
}
