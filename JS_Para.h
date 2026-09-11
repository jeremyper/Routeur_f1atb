//***************************************************
// Page Javascript de gestion des Paramètres (Soleo)
// Logique d'origine conservée : SetParaFixe, SetParaVar,
// SendValues, checkDisabled — IDs HTML inchangés.
//***************************************************
const char * ParaJS1 = R"====(
var refTempIP = [];
var dirty = false;
var toastT;

//Conversion date stockée "AAAAMMJJ" -> "AAAA-MM-JJ" pour <input type=date>
function isoDate(d){
  if(!d || (""+d).length!=8) return "";
  d=""+d; return d.substr(0,4)+"-"+d.substr(4,2)+"-"+d.substr(6,2);
}

//---------- Thème clair / sombre ----------
// Bascule de thème : /theme.js (partagé par toutes les pages)

//---------- Accordéons : un seul ouvert à la fois ----------
document.querySelectorAll("details.acc").forEach(d=>{
  d.addEventListener("toggle",()=>{
    if(d.open)document.querySelectorAll("details.acc").forEach(o=>{if(o!==d)o.open=false;});
  });
});

//---------- Détection des modifications (bouton flottant) ----------
function markDirty(){
  if(!dirty){dirty=true;GID("saveBar").classList.add("show");}
}
document.addEventListener("input",e=>{if(e.target.closest("#LesParas"))markDirty();});
document.addEventListener("change",e=>{if(e.target.closest("#LesParas"))markDirty();});

//---------- Toast de notification ----------
function Toast(m){
  const t=GID("toast");
  t.textContent=m;t.classList.add("show");
  clearTimeout(toastT);
  toastT=setTimeout(()=>t.classList.remove("show"),4000);
}

//---------- Mot de passe d'accès : affichage et déconnexion ----------
function VoirCle(){
  const c = GID("CleAcces");
  const cache = c.type === "password";
  c.type = cache ? "text" : "password";
  GH("btnVoirCle", cache ? "🙈" : "👁");
}

// Le mot de passe est conservé en cookie par le navigateur. Sans moyen de l'effacer,
// il restait indéfiniment sur tout appareil ayant servi à se connecter une fois.
function Deconnexion(){
  document.cookie = "CleAcces=;path=/;max-age=0";
  Toast("Déconnecté — rechargement…");
  setTimeout(() => { location.href = "/Para"; }, 900);
}

//---------- Port MQTT selon le chiffrement ----------
// 1883 avec TLS ne se connecte nulle part : les brokers cloud n'exposent que 8883.
// On ne corrige que les deux ports usuels, pour ne pas écraser un réglage délibéré.
function PortSelonTLS(){
  const p = GID("MQTTPort"), tls = GID("MQTTSecure").checked;
  if (tls && p.value == "1883") { p.value = "8883"; Toast("Port passé à 8883, requis par le chiffrement"); }
  else if (!tls && p.value == "8883") { p.value = "1883"; Toast("Port revenu à 1883"); }
  markDirty();
}

//---------- Test de notification ----------
function TestNotif(){
  GH("btnTestNotif","⏳ Envoi…");
  fetch("/ajax_testNotif")
    .then(r=>{ if(r.status===401) throw new Error("clé d'accès refusée"); return r.text(); })
    .then(t=>{
      GH("btnTestNotif","🔔 Envoyer un test");
      if(t.indexOf("OK")>=0) Toast("Test envoyé — il arrive dans quelques secondes");
      else Toast("Activez les notifications et enregistrez avant de tester");
    })
    .catch(e=>{ GH("btnTestNotif","🔔 Envoyer un test"); Toast("Échec : "+e.message); });
}

//---------- Géolocalisation ----------
function DetectePosition(){
  if(!navigator.geolocation){Toast("Géolocalisation non disponible sur ce navigateur");return;}
  GH("btnGeo","⏳ Recherche…");
  navigator.geolocation.getCurrentPosition(function(p){
    GID("MeteoLat").value=p.coords.latitude.toFixed(4);
    GID("MeteoLon").value=p.coords.longitude.toFixed(4);
    GH("btnGeo","📍 Position détectée ✓");
    markDirty();
  },function(){
    GH("btnGeo","📍 Détecter ma position");
    Toast("Position refusée — autorisez la localisation ou saisissez lat/lon à la main");
  });
}

/**
 * Initialise les éléments HTML dynamiques (sondes de température, routeurs).
 */
function Init() {
    let S = "";

    // --- Sources de température (4 canaux) ---
    const NbCanauxTemp = 4;
    for (let i = 0; i < NbCanauxTemp; i++) {
        S += `<div class="tcanal"><div class="row"><label for="srcTemp${i}">Sonde ${i}</label>`;
        S += `<select id="srcTemp${i}" onclick="checkDisabled();" onchange="checkDisabled();">
                  <option value="tempNo" selected>Pas de mesure</option>
                  <option value="tempInt">Capteur DS18B20 Interne</option>
                  <option value="tempExt">Capteur DS18B20 ESP Externe</option>
                  <option value="tempMqtt">MQTT</option>
              </select></div>`;
        S += `<div id='ligneTemperature${i}' style='display:none'>`;
        S += `<div class='row'><label for='nomTemperature${i}'>Nom de la sonde</label><input type='text' id='nomTemperature${i}'></div>`;
        S += `<div class='row' id='ligneoffsetTemp${i}' style='display:none'><label for='offsetTemp${i}'>Correction de mesure (°C)</label><input type='number' id='offsetTemp${i}'></div>`;
        S += `<div class='row' id='ligneTopicT${i}' style='display:none'><label for='TopicT${i}'>MQTT Topic température</label><input type='text' id='TopicT${i}'></div>`;
        S += `<div class='row' id='ligneIPtemp${i}' style='display:none'><label for='refTempIP${i}'>Routeur externe (ESP-RMS)</label><select id='refTempIP${i}'></select></div>`;
        S += `<div class='row' id='canalEXTtemp${i}' style='display:none'><label for='canalEXT${i}'>Canal sur le routeur externe</label><select id='canalEXT${i}'>
                  <option value="0">Canal 0</option>
                  <option value="1">Canal 1</option>
                  <option value="2">Canal 2</option>
                  <option value="3">Canal 3</option>
              </select></div>`;
        S += "</div></div>";
    }
    GH("Sources_Temp", S);

    // --- Routeurs/RMS externes (8 entrées) ---
    S = "";
    const NbRouteurs = 8;
    for (let i = 0; i < NbRouteurs; i++) {
        S += `<div class='row rrow' id='Routeur_${i}' style='display:none'><span class='hint' id='RMS_Nom${i}'></span><input type='text' id='RMS_IP${i}' title='adresse au format IPv4 : xxx.xxx.xxx.xxx'></div>`;
    }
    GH("Routeurs", S);

    LoadParaFixe();
}

/**
 * Implémente les paramètres après chargement depuis le serveur via AJAX.
 */
function SetParaFixe() {
    // Lecture du cookie de clé d'accès
    const cookieName = "CleAcces=";
    const ca = document.cookie.split(';');
    for (let i = 0; i < ca.length; i++) {
        let c = decodeURIComponent(ca[i]).trim();
        if (c.indexOf(cookieName) === 0) {
            GID("CleAcces").value = c.substring(cookieName.length);
            break;
        }
    }

    // --- Paramètres Généraux ---
    GID("dhcp").checked = F.dhcpOn == 1;
    GID("gateway").value = int2ip(F.Gateway);
    GID("masque").value = int2ip(F.masque);
    GID("dns").value = int2ip(F.dns);
    GID("hostname").value = F.hostname;
    GID("ModeP" + F.ModePara).checked = true;
    GID("ModeW").value = F.ModeReseau;
    GID("sources").value = F.Source;
    GID("MQTTRepet").value = F.MQTTRepet;
    GID("MQTTIP").value = int2ip(F.MQTTIP);
    GID("MQTTHost").value = F.MQTTHost !== undefined ? F.MQTTHost : "";
    GID("MQTTSecure").checked = F.MQTTSecure == 1;
    GID("MQTTPort").value = F.MQTTPort;
    GID("MQTTUser").value = F.MQTTUser;
    GID("MQTTPwd").value = F.MQTTPwd;
    GID("MQTTPrefix").value = F.MQTTPrefix;
    GID("MQTTPrefixEtat").value = F.MQTTPrefixEtat;
    GID("MQTTdeviceName").value = F.MQTTdeviceName;
    GID("subMQTT").checked = F.subMQTT == 1;
    GID("nomRouteur").value = F.nomRouteur;
    GID("nomSondeFixe").value = F.nomSondeFixe;
    GID("nomSondeMobile").value = F.nomSondeMobile;
    GID("nomSfixePpos").value = F.nomSfixePpos;
    GID("nomSfixePneg").value = F.nomSfixePneg;
    GID("CalibU").value = F.CalibU;
    GID("CalibI").value = F.CalibI;
    GID("TempoRTEon").checked = F.TempoRTEon == 1;
    GID("WifiSleep").checked = F.WifiSleep == 1;
    GID("ComSurv").value = F.ComSurv;
    GID("Serie").value = F.pSerial;
    GID("Serial2V").value = F.Serial2V;
    GID("ESP").value = F.ESP32_Type;
    GID("pLED").value = F.LEDgroupe;
    GID("pUxI").value = F.pUxI;
    GID("PTemp").value = F.pTemp;
    GID("PrixHP").value = F.PrixHP !== undefined ? F.PrixHP : 0.25;
    GID("PrixHC").value = F.PrixHC !== undefined ? F.PrixHC : 0.15;
    GID("PrixBleuHP").value = F.PrixBleuHP !== undefined ? F.PrixBleuHP : 0.1609;
    GID("PrixBleuHC").value = F.PrixBleuHC !== undefined ? F.PrixBleuHC : 0.1296;
    GID("PrixBlancHP").value = F.PrixBlancHP !== undefined ? F.PrixBlancHP : 0.1894;
    GID("PrixBlancHC").value = F.PrixBlancHC !== undefined ? F.PrixBlancHC : 0.1486;
    GID("PrixRougeHP").value = F.PrixRougeHP !== undefined ? F.PrixRougeHP : 0.7562;
    GID("PrixRougeHC").value = F.PrixRougeHC !== undefined ? F.PrixRougeHC : 0.1568;
    GID("MeteoOn").checked = F.MeteoOn == 1;
    GID("MeteoLat").value = F.MeteoLat !== undefined ? F.MeteoLat : 46.5;
    GID("MeteoLon").value = F.MeteoLon !== undefined ? F.MeteoLon : 2.4;
    GID("MeteoPVcrete").value = F.MeteoPVcrete !== undefined ? F.MeteoPVcrete : 3.0;
    GID("SmaOn").checked = F.SmaOn == 1;
    GID("SmaIP").value = F.SmaIP !== undefined ? F.SmaIP : "";
    GID("BallonCanal").value = F.BallonCanal !== undefined ? F.BallonCanal : -1;
    GID("BallonVolume").value = F.BallonVolume !== undefined ? F.BallonVolume : 200;
    GID("BallonTcible").value = F.BallonTcible !== undefined ? F.BallonTcible : 55;
    GID("BallonPuissance").value = F.BallonPuissance !== undefined ? F.BallonPuissance : 2400;
    GID("BallonModeIntel").checked = F.BallonModeIntel == 1;
    GID("BallonTmin").value = F.BallonTmin !== undefined ? F.BallonTmin : 40;
    GID("FanGpio").value = F.FanGpio !== undefined ? F.FanGpio : 0;
    GID("FanCanalTemp").value = F.FanCanalTemp !== undefined ? F.FanCanalTemp : -1;
    GID("FanTdemarrage").value = F.FanTdemarrage !== undefined ? F.FanTdemarrage : 40;
    GID("FanTmax").value = F.FanTmax !== undefined ? F.FanTmax : 60;
    GID("FanVitesseMin").value = F.FanVitesseMin !== undefined ? F.FanVitesseMin : 30;
    GID("NotifOn").checked = F.NotifOn == 1;
    GID("NotifUrl").value = F.NotifUrl !== undefined ? F.NotifUrl : "";
    GID("DelestageOn").checked = F.DelestageOn == 1;
    GID("DelestagePuissance").value = F.DelestagePuissance !== undefined ? F.DelestagePuissance : 6000;
    GID("DelestageMarge").value = F.DelestageMarge !== undefined ? F.DelestageMarge : 10;
    GID("AbsenceManuel").checked = F.AbsenceManuel == 1;
    GID("AbsenceDebut").value = isoDate(F.AbsenceDebut);
    GID("AbsenceFin").value = isoDate(F.AbsenceFin);
    GID("AbsenceAntiLegio").value = F.AbsenceAntiLegio !== undefined ? F.AbsenceAntiLegio : 7;

    // --- Paramètres de Température ---
    const NbCanauxTemp = 4;
    for (let c = 0; c < NbCanauxTemp; c++) {
        GID(`nomTemperature${c}`).value = F[`nomTemperature${c}`];
        GID(`srcTemp${c}`).value = F[`Source_Temp${c}`];
        GID(`TopicT${c}`).value = F[`TopicT${c}`];
        refTempIP[c] = F[`refTempIP${c}`];
        GID(`canalEXT${c}`).value = F[`canalTempExterne${c}`];
        GID(`offsetTemp${c}`).value = F[`offsetTemp${c}`] / 100;
    }
    LoadParaVar();
    GID("LesParas").style.display = "block";
    dirty = false;
    GID("saveBar").classList.remove("show");
}

/**
 * Envoie les valeurs des paramètres au serveur via AJAX (POST /ParaNew).
 */
function SendValues() {
  GID("attente").style = "visibility: visible;";
  F.dhcpOn = GID("dhcp").checked ? 1 : 0;
  F.TempoRTEon = GID("TempoRTEon").checked ? 1 : 0;
  F.ComSurv = GID("ComSurv").value;
  F.Serial2V = GID("Serial2V").value;
  F.subMQTT = GID("subMQTT").checked ? 1 : 0;
  F.WifiSleep = GID("WifiSleep").checked ? 1 : 0;
  F.hostname = GID("hostname").value;
  F.pSerial = GID("Serie").value;
  F.ModePara = document.querySelector('input[name="ModeP"]:checked').value;
  F.ModeReseau = GID("ModeW").value;
  F.ESP32_Type = GID("ESP").value;
  F.LEDgroupe = GID("pLED").value;
  F.pUxI = GID("pUxI").value;
  F.pTemp = GID("PTemp").value;
  F.Source = GID("sources").value;
  if (F.ModePara == 0) { //Non Expert
    F.subMQTT = 0; F.WifiSleep = 1;
  }
  F.Gateway = ip2int(GID("gateway").value);
  F.masque = ip2int(GID("masque").value);
  F.dns = ip2int(GID("dns").value);

  F.PrixHP = parseFloat(GID("PrixHP").value) || 0.25;
  F.PrixHC = parseFloat(GID("PrixHC").value) || 0.15;
  F.PrixBleuHP = parseFloat(GID("PrixBleuHP").value) || 0.1609;
  F.PrixBleuHC = parseFloat(GID("PrixBleuHC").value) || 0.1296;
  F.PrixBlancHP = parseFloat(GID("PrixBlancHP").value) || 0.1894;
  F.PrixBlancHC = parseFloat(GID("PrixBlancHC").value) || 0.1486;
  F.PrixRougeHP = parseFloat(GID("PrixRougeHP").value) || 0.7562;
  F.PrixRougeHC = parseFloat(GID("PrixRougeHC").value) || 0.1568;
  F.MeteoOn = GID("MeteoOn").checked ? 1 : 0;
  F.MeteoLat = parseFloat(GID("MeteoLat").value) || 0;
  F.MeteoLon = parseFloat(GID("MeteoLon").value) || 0;
  F.MeteoPVcrete = parseFloat(GID("MeteoPVcrete").value) || 3;
  F.SmaOn = GID("SmaOn").checked ? 1 : 0;
  F.SmaIP = GID("SmaIP").value.trim();
  F.BallonCanal = parseInt(GID("BallonCanal").value, 10);
  F.BallonVolume = parseInt(GID("BallonVolume").value, 10) || 200;
  F.BallonTcible = parseInt(GID("BallonTcible").value, 10) || 55;
  F.BallonPuissance = parseInt(GID("BallonPuissance").value, 10) || 2400;
  F.BallonModeIntel = GID("BallonModeIntel").checked ? 1 : 0;
  F.BallonTmin = parseInt(GID("BallonTmin").value, 10) || 40;
  F.FanGpio = parseInt(GID("FanGpio").value, 10);
  F.FanCanalTemp = parseInt(GID("FanCanalTemp").value, 10);
  F.FanTdemarrage = parseInt(GID("FanTdemarrage").value, 10) || 40;
  F.FanTmax = parseInt(GID("FanTmax").value, 10) || 60;
  F.FanVitesseMin = parseInt(GID("FanVitesseMin").value, 10) || 30;
  F.NotifOn = GID("NotifOn").checked ? 1 : 0;
  F.NotifUrl = GID("NotifUrl").value.trim();
  F.DelestageOn = GID("DelestageOn").checked ? 1 : 0;
  F.DelestagePuissance = parseInt(GID("DelestagePuissance").value, 10) || 6000;
  F.DelestageMarge = parseInt(GID("DelestageMarge").value, 10);
  if (isNaN(F.DelestageMarge)) F.DelestageMarge = 10;  //0 % est une valeur valide
  F.AbsenceManuel = GID("AbsenceManuel").checked ? 1 : 0;
  F.AbsenceDebut = GID("AbsenceDebut").value.replace(/-/g, "");
  F.AbsenceFin = GID("AbsenceFin").value.replace(/-/g, "");
  F.AbsenceAntiLegio = parseInt(GID("AbsenceAntiLegio").value, 10);

  F.nomRouteur = GID("nomRouteur").value.trim();
  F.nomSondeFixe = GID("nomSondeFixe").value.trim();
  F.nomSfixePpos = GID("nomSfixePpos").value.trim();
  F.nomSfixePneg = GID("nomSfixePneg").value.trim();
  F.nomSondeMobile = GID("nomSondeMobile").value.trim();

  F.MQTTRepet = GID("MQTTRepet").value;
  F.MQTTIP = ip2int(GID("MQTTIP").value);
  F.MQTTHost = GID("MQTTHost").value.trim();
  F.MQTTSecure = GID("MQTTSecure").checked ? 1 : 0;
  F.MQTTPort = GID("MQTTPort").value;
  F.MQTTUser = GID("MQTTUser").value.trim();
  F.MQTTPwd = GID("MQTTPwd").value.trim();
  F.MQTTPrefix = GID("MQTTPrefix").value.trim();
  F.MQTTPrefixEtat = GID("MQTTPrefixEtat").value.trim();
  F.MQTTdeviceName = GID("MQTTdeviceName").value.trim();
  F.CalibU = GID("CalibU").value;
  F.CalibI = GID("CalibI").value;
  F.CleAccesRef = GID("CleAcces").value.trim();

  for (var c = 0; c < 4; c++) {
    var QS = GID(`srcTemp${c}`).value;
    F["Source_Temp" + c] = (F.ModePara == 0) ? "tempNo" : QS;
    F["nomTemperature" + c] = GID(`nomTemperature${c}`).value.trim();
    F["TopicT" + c] = GID(`TopicT${c}`).value.trim();
    F["refTempIP" + c] = GID(`refTempIP${c}`).value;
    F["canalTempExterne" + c] = GID(`canalEXT${c}`).value;
    F["offsetTemp" + c] = GID(`offsetTemp${c}`).value * 100;
  }
  F.RMS_IP0 = ip2int(GID("adrIP").value);
  F.IP_Fixe = F.RMS_IP0;
  for (let i = 1; i < 8; i++) {
    let Vip = (F.ModePara == 0) ? 0 : ip2int(GID("RMS_IP" + i).value.trim());
    F["RMS_IP" + i] = Vip;
  }

  // Le cookie n'est mis à jour qu'APRÈS la sauvegarde : la requête doit s'authentifier
  // avec la clé encore connue du serveur, sinon changer sa clé d'accès serait impossible.
  if ((GID("dhcp").checked || checkIP("adrIP") && checkIP("gateway")) && (!GID("MQTTRepet").checked || checkIP("MQTTIP"))) {
    fetch("/ParaNew", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(F)
    })
      .then(response => {
        if (response.status === 401) throw new Error("clé d'accès refusée, rechargez la page et ressaisissez-la");
        if (!response.ok) throw new Error("Erreur HTTP " + response.status);
        return response.json();
      })
      .then(resultat => {
        document.cookie = "CleAcces=" + encodeURIComponent(F.CleAccesRef) + ";max-age=31536000;path=/";
        dirty = false;
        GID("saveBar").classList.remove("show");
        GID("attente").style = "visibility: hidden;";
        Toast("✓ Paramètres sauvegardés — l'ESP redémarre");
        setTimeout(() => location.reload(), 2500);
      })
      .catch(error => {
        GID("attente").style = "visibility: hidden;";
        Toast("✗ Erreur d'envoi : " + error.message);
        console.error("Erreur d'envoi :", error);
      });
  } else {
    GID("attente").style = "visibility: hidden;";
    Toast("✗ Adresse IP invalide — vérifiez les champs en rouge");
  }
}
)====";


const char * ParaJS2 = R"====(
/**
 * Envoie une requête au serveur pour réinitialiser les historiques.
 */
function SendRAZ() {
    if (confirm("Confirmez la remise à zéro des historiques")) {
        const xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState === 4 && this.status === 200) {
                Toast("✓ Historiques remis à zéro");
            }
        };
        xhttp.open('GET', '/ajaxRAZhisto', true);
        xhttp.send();
    }
}

/**
 * Active/désactive et montre/masque les champs selon le mode (Standard/Expert)
 * et l'état du réseau. Même logique que la page d'origine.
 */
function checkDisabled() {
    GID("RMS_IP0").readOnly = true;
    GID("RMS_IP0").classList.add("ro");

    F.ModeReseau = GID("ModeW").value;
    const selectedMode = document.querySelector('input[name="ModeP"]:checked');
    F.ModePara = selectedMode ? selectedMode.value : '0';
    const isExpertMode = (F.ModePara > 0);
    const isDisabledAP = (F.ModeReseau == 2);
    const SH = (id, show, mode) => { const e = GID(id); if (e) e.style.display = show ? (mode || "") : "none"; };

    SH("Vport_serie", GID("Serie").value > 0);

    // Paramètres IP statiques
    SH("infoIP", !(GID("dhcp").checked || isDisabledAP));
    SH("rowDhcp", !isDisabledAP);
    SH("ipreset", !isDisabledAP);

    // Lignes conditionnelles réseau / expert
    SH("l_wifi_0", !isDisabledAP);
    SH("l_wifi_1", !isDisabledAP);   //Période d'envoi : indispensable à la supervision distante
    SH("subtMqtt", !isDisabledAP);
    //Le pilotage depuis la domotique reste réservé au mode Expert : en Standard il est
    //forcé à 0 à l'enregistrement, une case qui se décoche seule induirait en erreur.
    SH("rowSubMqtt", !(isDisabledAP || !isExpertMode));
    SH("l_wifi_2", !(isDisabledAP || !isExpertMode));
    SH("listerouteurs", !(isDisabledAP || !isExpertMode));

    // Météo : champs visibles seulement si activée, indisponible en point isolé
    GID("MeteoOn").disabled = isDisabledAP;
    if (isDisabledAP) GID("MeteoOn").checked = false;
    const meteoVisible = GID("MeteoOn").checked;
    document.querySelectorAll(".ligneMeteo").forEach(l => { l.style.display = meteoVisible ? "" : "none"; });

    // SMA
    GID("SmaOn").disabled = isDisabledAP;
    if (isDisabledAP) GID("SmaOn").checked = false;
    const smaVisible = GID("SmaOn").checked;
    document.querySelectorAll(".ligneSma").forEach(l => { l.style.display = smaVisible ? "" : "none"; });

    // Ballon : champs visibles seulement si une sonde est choisie
    const ballonVisible = GID("BallonCanal").value >= 0;
    document.querySelectorAll(".ligneBallon").forEach(l => { l.style.display = ballonVisible ? "" : "none"; });
    // Température minimale utile : seulement si le mode prédictif est activé
    const intelVisible = ballonVisible && GID("BallonModeIntel").checked;
    document.querySelectorAll(".ligneBallonIntel").forEach(l => { l.style.display = intelVisible ? "" : "none"; });

    // Ventilateur SSR : champs détaillés visibles seulement si un GPIO est sélectionné
    const fanVisible = GID("FanGpio").value != "0";
    document.querySelectorAll(".ligneFan").forEach(l => { l.style.display = fanVisible ? "" : "none"; });

    // Notifications : adresse et test visibles seulement si le service est activé
    const notifVisible = GID("NotifOn").checked;
    document.querySelectorAll(".ligneNotif").forEach(l => { l.style.display = notifVisible ? "" : "none"; });

    // Délestage : puissance souscrite et marge visibles seulement si la protection est active
    const delestVisible = GID("DelestageOn").checked;
    document.querySelectorAll(".ligneDelestage").forEach(l => { l.style.display = delestVisible ? "" : "none"; });

    // Tarifs : si Tempo activé, on affiche les 6 prix Tempo et on masque le tarif Base HP/HC
    const tempoOn = GID("TempoRTEon").checked;
    document.querySelectorAll(".ligneTarifTempo").forEach(l => { l.style.display = tempoOn ? "" : "none"; });
    document.querySelectorAll(".ligneTarifBase").forEach(l => { l.style.display = tempoOn ? "none" : ""; });

    // Options de température désactivées en mode point isolé
    for (let i = 0; i < 4; i++) {
        const srcTemp = GID('srcTemp' + i);
        if (srcTemp) {
            const optExt = srcTemp.querySelector('option[value="tempExt"]');
            if (optExt) optExt.disabled = isDisabledAP;
            const optMqtt = srcTemp.querySelector('option[value="tempMqtt"]');
            if (optMqtt) optMqtt.disabled = isDisabledAP;
        }
    }

    SH("donEnv", !isDisabledAP, "block");

    // IP affichée
    if (isDisabledAP) {
        GH("localIP", "IP : 192.168.4.1");
    } else {
        GH("localIP", "IP actuelle (DHCP) : " + ((typeof V !== "undefined" && V) ? V.localIP : "…"));
    }

    // Correction de l'horloge si point isolé
    if (F.ModeReseau == 2 && F.Horloge == 0) {
        F.Horloge = 2;
    }

    // Bloc MQTT
    // La section MQTT n'est plus réservée au mode Expert : la supervision à distance
    // en dépend, et elle vise justement ceux qui ne quittent pas le mode Standard.
    const isMqttVisible = GID("MQTTRepet").value != 0 || GID("sources").value == "Pmqtt" || GID("subMQTT").checked;
    SH("Zmqtt", isMqttVisible);

    // Blocs réservés au mode expert
    SH("LesSourcesTemp", isExpertMode);
    SH("pTemp", isExpertMode);
    SH("SurvCom", isExpertMode);
    SH("accSysteme", isExpertMode);
    if (!isExpertMode && GID("accSysteme").open) GID("accSysteme").open = false;

    // Lignes de paramètres de température par canal
    for (let c = 0; c < 4; c++) {
        const srcValue = GID(`srcTemp${c}`).value;
        const isMqtt = srcValue === "tempMqtt";
        const isExt = srcValue === "tempExt";
        const isInt = srcValue === "tempInt";
        const isNo = srcValue === "tempNo";

        SH('ligneTemperature' + c, !isNo);
        SH('ligneTopicT' + c, isMqtt);
        SH('ligneIPtemp' + c, isExt);
        SH('canalEXTtemp' + c, isExt);
        SH('ligneoffsetTemp' + c, isInt);

        // Si une source est MQTT, force l'affichage du bloc MQTT
        if (isMqtt) GID("Zmqtt").style.display = "";
    }

    F.Source = GID("sources").value;
    if (typeof V !== "undefined" && V) V.Source_data = F.Source;
    AdaptationSource();
}

/**
 * Valide le format d'une adresse IP dans un champ donné.
 */
function checkIP(id) {
    const S = GID(id).value;
    const Table = S.split(".");
    let valide = true;
    if (Table.length !== 4) {
        valide = false;
    } else {
        for (const part of Table) {
            const num = parseInt(part, 10);
            if (isNaN(num) || num > 255 || num < 0) {
                valide = false;
                break;
            }
        }
    }
    GID(id).style.color = valide ? "" : "red";
    GID(id).style.borderColor = valide ? "" : "red";
    return valide;
}

/**
 * Adapte l'affichage des champs selon la source de mesure sélectionnée.
 */
function AdaptationSource() {
    const src = (typeof V !== "undefined" && V) ? V.Source_data : F.Source;
    // UxIx2 a une seconde sonde
    const isSourceDual = (src === 'UxIx2');
    GID('ligneFixe').style.display = isSourceDual ? "" : "none";
    GID('ligneFixe1').style.display = isSourceDual ? "" : "none";
    GID('ligneFixe2').style.display = isSourceDual ? "" : "none";
    // Calibration seulement si UxI
    const isSourceUxI = (src === 'UxI' && F.Source === 'UxI');
    GID('Zcalib').style.display = isSourceUxI ? "" : "none";
    GID('Analog').style.display = isSourceUxI ? "" : "none";
}

/**
 * Termine le chargement des paramètres des routeurs et sélecteurs de température externe.
 */
function SetParaVar() {
    GID("adrIP").value = int2ip(V.IP_Fixe); //Valeur actuelle, pas F.IP_Fixe
    let Soptions = "";
    for (let c = 0; c < nb_ESP; c++) {
        GID(`RMS_IP${c}`).value = V.IP_RMS[c];
        GH(`RMS_Nom${c}`, nomRMS[c]);
        GID(`Routeur_${c}`).style.display = "";
        if (c > 0) {
            Soptions += `<option value=${c}>${V.IP_RMS[c]} ${nomRMS[c]}</option>`;
        }
        const d = c + 1;
        if (c < 7) {
            GH("RMS_Nom" + d, "<small>Ajoutez une adresse IP de routeur :</small>");
            GID("Routeur_" + d).style.display = "";
        }
    }
    for (let i = 0; i < 4; i++) {
        GH("refTempIP" + i, Soptions);
        GID("refTempIP" + i).value = refTempIP[i];
    }

    // Prévision solaire
    if (V.MeteoOn == 1) {
        if (V.PrevisionJour >= 0) {
            GH("previsionMeteo", "Aujourd'hui : <strong>" + V.PrevisionJour + " kWh</strong> &nbsp;|&nbsp; Demain : <strong>" + V.PrevisionDemain + " kWh</strong>");
        } else {
            GH("previsionMeteo", "En attente de données Open-Meteo…");
        }
    }
    // Production SMA
    if (V.SmaOn == 1) {
        if (V.EnergieJourPV !== undefined && V.PuissancePV !== undefined) {
            GH("productionSma", "<strong>" + V.PuissancePV + " W</strong> &nbsp;|&nbsp; " + (V.EnergieJourPV / 1000).toFixed(2) + " kWh aujourd'hui");
        } else {
            GH("productionSma", "En attente de l'onduleur…");
        }
    }
    // État du forçage adaptatif ballon
    if (V.BallonCanal >= 0) {
        if (V.BallonBesoin >= 0) {
            let decision = (V.BallonBesoin - V.BallonSurplus > 0.05) ? "<span style='color:var(--grid-imp);'>forçage autorisé</span>" : "<span style='color:var(--grid-exp);'>le soleil suffira</span>";
            let intel = "";
            if (V.BallonModeIntel == 1) {
                intel = "<br>Mode prédictif : réserve d'eau chaude <strong>" + V.BallonReserve + " kWh</strong>, usage moyen appris <strong>" + V.BallonUsageMoyen + " kWh/jour</strong>";
            }
            GH("etatBallon", "Besoin : <strong>" + V.BallonBesoin + " kWh</strong> &nbsp;|&nbsp; Surplus prévu : <strong>" + V.BallonSurplus + " kWh</strong> &nbsp;|&nbsp; Coef : " + V.BallonCoefAuto + "% &mdash; " + decision + intel);
        } else {
            GH("etatBallon", "En attente (sonde et météo nécessaires)…");
        }
    }
    MajBadge(V);
    lastOk = Date.now();
    checkDisabled();
}

//---------- Badge tarif + indicateur En ligne (header) ----------
var lastOk = 0;
function MajBadge(v) {
    if (!v) return;
    const b = parseInt(v.LTARFbin) || 0;
    const tempo = (b & 4) ? "Tempo Bleu" : (b & 8) ? "Tempo Blanc" : (b & 16) ? "Tempo Rouge" : "";
    const h = (b & 2) ? "HC" : (b & 1) ? "HP" : "";
    const el = GID("tarifBadge");
    el.textContent = tempo ? (tempo + (h ? " · " + h : "")) : (h || "Tarif —");
    const c = (b & 16) ? "var(--grid-imp)" : (b & 8) ? "var(--txt)" : (b & 4) ? "var(--home)" : (b & 2) ? "var(--grid-exp)" : (b & 1) ? "var(--sun)" : "var(--txt-dim)";
    el.style.color = c; el.style.borderColor = c;
}
async function PollBadge() {
    try {
        const r = await fetch("/ParaVar");
        if (r.ok) { const v = await r.json(); lastOk = Date.now(); MajBadge(v); }
    } catch (e) {}
    setTimeout(PollBadge, 10000);
}
function Tick() {
    const ok = (Date.now() - lastOk) < 25000;
    GID("online").className = ok ? "online" : "online off";
    GH("onlineTxt", ok ? "En ligne" : "Hors ligne");
}
setInterval(Tick, 2000);
setTimeout(PollBadge, 12000); //le premier /ParaVar vient de LoadParaVar
)====";
