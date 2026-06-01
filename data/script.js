// -------------------------------------------------------------
// WebSocket
// -------------------------------------------------------------

const gateway = 'ws://' + window.location.hostname + '/ws';
let websocket;

function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

function onOpen() {
  console.log('Connection opened');
  document.getElementById('messages').innerHTML = "Connected";
}

function onClose() {
  console.log('Connection closed');
  document.getElementById('messages').innerHTML = "Connection closed";
  setTimeout(initWebSocket, 2000);
}

// -------------------------------------------------------------
// Réception WebSocket
// -------------------------------------------------------------

function onMessage(event) {
  console.log('Server:', event.data);
  let data = JSON.parse(event.data);

  // --- Commande spécifique ---
  if (data.cmd === "roleUpdate") {
    document.getElementById('typeCanton').value = data.role;
    return;
  }

  // --- Champs généraux ---
  document.getElementById('idNode').value = data.idNode;

  document.getElementById('p00').value = data.p00;
  document.getElementById('p01').value = data.p01;
  document.getElementById('p10').value = data.p10;
  document.getElementById('p11').value = data.p11;

  document.getElementById('m00').value = data.m00;
  document.getElementById('m01').value = data.m01;
  document.getElementById('m10').value = data.m10;
  document.getElementById('m11').value = data.m11;

  // --- Aiguilles ---
  for (let i = 0; i < 6; i++) {
    document.getElementById('s' + i).value = data['s' + i];
    document.getElementById('s' + i + '0').value = data['s' + i + '0'];
    document.getElementById('s' + i + '1').value = data['s' + i + '1'];
    document.getElementById('s' + i + '2').value = data['s' + i + '2'];
  }

  // --- Vitesse ---
  document.getElementById('maxSpeed').value = data.maxSpeed;

  // --- Sens de marche ---
  switch (data.sensMarche) {
    case 0: document.getElementById('indifferent').checked = true; break;
    case 1: document.getElementById('horaire').checked = true; break;
    case 2: document.getElementById('antihoraire').checked = true; break;
  }

  // --- Cibles ---
  document.getElementById('imageHoraire').src = 'cible_' + data.cibleHoraire + '.jpg';
  document.getElementById('imageAntiHor').src = 'cible_' + data.cibleAntiHor + '.jpg';

  // --- Switchs ---
  document.getElementById('discovery_on').checked = data.discovery_on;
  document.getElementById('wifi_on').checked = data.wifi_on;

  // --- Type de canton ---
  if (data.role !== undefined)
    document.getElementById('typeCanton').value = data.role;

  // --- Debug brut ---
  document.getElementById('rawData').innerHTML = event.data;
}

window.addEventListener('load', initWebSocket);

// -------------------------------------------------------------
// Envoi WebSocket
// -------------------------------------------------------------

function sendJson() {
  const msg = {
    idNode: document.getElementById('idNode').value,
    comptAig: document.getElementById('comptAig').value,
    p00: document.getElementById('p00').value,
    p01: document.getElementById('p01').value,
    p10: document.getElementById('p10').value,
    p11: document.getElementById('p11').value,
    m00: document.getElementById('m00').value,
    m01: document.getElementById('m01').value,
    m10: document.getElementById('m10').value,
    m11: document.getElementById('m11').value
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
  websocket.send(JSON.stringify({ wifi_on: [obj.checked] }));
}

function discovery_on(obj) {
  websocket.send(JSON.stringify({ discovery_on: [obj.checked] }));
}

function restartEsp(obj) {
  websocket.send(JSON.stringify({ restartEsp: [obj.id] }));
}

function save(obj) {
  websocket.send(JSON.stringify({ save: [obj.id] }));
}

// -----------------------------
// Changement du type de canton (rôle ferroviaire)
// -----------------------------
function typeCantonChange(obj) {
  websocket.send(JSON.stringify({ cmd: "setRole", value: parseInt(obj.value) }));
}

function saveBooster() {
  const msg = {
    booster_seuils: [
      parseInt(document.getElementById('booster_seuil_libre').value),
      parseInt(document.getElementById('booster_seuil_occupe').value)
    ]
  };
  websocket.send(JSON.stringify(msg));
}

function calibBooster() {
  websocket.send(JSON.stringify({ cmd: "calibBooster" }));
}
