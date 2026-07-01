/* ============================================================================
 * WebSocket Canton — Gestion Canton 2026
 * ==========================================================================*/

const gateway = "ws://" + window.location.hostname + "/ws";
let websocket = null;

function initWebSocket() {
  console.log("[WS] Tentative de connexion…");
  websocket = new WebSocket(gateway);

  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

function onOpen() {
  console.log("[WS] Connecté");
  safeSetHTML("messages", "Connected");
}

function onClose() {
  console.log("[WS] Déconnecté — nouvelle tentative dans 2s");
  safeSetHTML("messages", "Connection closed");
  setTimeout(initWebSocket, 2000);
}

window.addEventListener("load", () => {
  initWebSocket();
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
   * Champs généraux
   * ------------------------------------------------------------------------ */
  safeSetValue("idCanton", data.idCanton);

  safeSetValue("p00", data.p00);
  safeSetValue("p01", data.p01);
  safeSetValue("p10", data.p10);
  safeSetValue("p11", data.p11);

  safeSetValue("m00", data.m00);
  safeSetValue("m01", data.m01);
  safeSetValue("m10", data.m10);
  safeSetValue("m11", data.m11);

  /* ------------------------------------------------------------------------
   * Pilotage Distribué
   * ------------------------------------------------------------------------ */
  const pdFields = [
    "longueur_canton_mm", "zone_ralentissement_mm",
    "ecart_r30_n", "ecart_r30_ho",
    "ecart_r60_n", "ecart_r60_ho",
    "ecart_avert_n", "ecart_avert_ho",
    "ecart_man_n", "ecart_man_ho",
    "ecart_carre_n", "ecart_carre_ho",
    "ecart_default_n", "ecart_default_ho"
  ];

  pdFields.forEach(f => safeSetValue(f, data[f]));

  /* ------------------------------------------------------------------------
   * Aiguilles
   * ------------------------------------------------------------------------ */
  for (let i = 0; i < 6; i++) {
    safeSetValue("s" + i, data["s" + i]);
    safeSetValue("s" + i + "0", data["s" + i + "0"]);
    safeSetValue("s" + i + "1", data["s" + i + "1"]);
    safeSetValue("s" + i + "2", data["s" + i + "2"]);
  }

  /* ------------------------------------------------------------------------
   * Vitesse
   * ------------------------------------------------------------------------ */
  safeSetValue("maxSpeed", data.maxSpeed);

  /* ------------------------------------------------------------------------
   * Sens de marche
   * ------------------------------------------------------------------------ */
  const sens = data.sensMarche;
  if (sens === 0) safeCheck("indifferent", true);
  if (sens === 1) safeCheck("horaire", true);
  if (sens === 2) safeCheck("antihoraire", true);

  /* ------------------------------------------------------------------------
   * Cibles
   * ------------------------------------------------------------------------ */
  safeSetImage("imageHoraire", "cible_" + data.cibleHoraire + ".jpg");
  safeSetImage("imageAntiHor", "cible_" + data.cibleAntiHor + ".jpg");

  /* ------------------------------------------------------------------------
   * Switchs
   * ------------------------------------------------------------------------ */
  safeCheck("exploration_on", data.exploration_on);
  safeCheck("wifi_on", data.wifi_on);

  /* ------------------------------------------------------------------------
   * Debug brut
   * ------------------------------------------------------------------------ */
  safeSetHTML("rawData", event.data);
}

/* ============================================================================
 * Helpers UI — Version robuste Discovery 2026
 * ==========================================================================*/

function safeSetValue(id, value) {
  const el = document.getElementById(id);
  if (!el) return;
  if (value === null || value === undefined) return;
  el.value = value;
}

function safeSetHTML(id, html) {
  const el = document.getElementById(id);
  if (!el) return;
  el.innerHTML = html;
}

function safeCheck(id, state) {
  const el = document.getElementById(id);
  if (!el) return;
  el.checked = !!state;
}

function safeSetImage(id, src) {
  const el = document.getElementById(id);
  if (!el) return;
  if (!src) return;
  el.src = src;
}

/* ============================================================================
 * Envoi WebSocket
 * ==========================================================================*/

function sendJson() {
  const msg = {
    idCanton: safeGet("idCanton"),
    comptAig: safeGet("comptAig"),
    p00: safeGet("p00"),
    p01: safeGet("p01"),
    p10: safeGet("p10"),
    p11: safeGet("p11"),
    m00: safeGet("m00"),
    m01: safeGet("m01"),
    m10: safeGet("m10"),
    m11: safeGet("m11")
  };
  websocket.send(JSON.stringify(msg));
}

function safeGet(id) {
  const el = document.getElementById(id);
  return el ? el.value : null;
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
      parseInt(safeGet("booster_seuil_libre")),
      parseInt(safeGet("booster_seuil_occupe"))
    ]
  };
  websocket.send(JSON.stringify(msg));
}

function calibBooster() {
  websocket.send(JSON.stringify({ cmd: "calibBooster" }));
}

function savePilotageDistribue() {
  const msg = {
    pilotage_distribue: {
      longueur_canton_mm: parseInt(safeGet("longueur_canton_mm")),
      zone_ralentissement_mm: parseInt(safeGet("zone_ralentissement_mm")),
      ecart_r30_n: parseInt(safeGet("ecart_r30_n")),
      ecart_r30_ho: parseInt(safeGet("ecart_r30_ho")),
      ecart_r60_n: parseInt(safeGet("ecart_r60_n")),
      ecart_r60_ho: parseInt(safeGet("ecart_r60_ho")),
      ecart_avert_n: parseInt(safeGet("ecart_avert_n")),
      ecart_avert_ho: parseInt(safeGet("ecart_avert_ho")),
      ecart_man_n: parseInt(safeGet("ecart_man_n")),
      ecart_man_ho: parseInt(safeGet("ecart_man_ho")),
      ecart_carre_n: parseInt(safeGet("ecart_carre_n")),
      ecart_carre_ho: parseInt(safeGet("ecart_carre_ho")),
      ecart_default_n: parseInt(safeGet("ecart_default_n")),
      ecart_default_ho: parseInt(safeGet("ecart_default_ho"))
    }
  };

  websocket.send(JSON.stringify(msg));
}
