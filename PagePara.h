//*****************************************************
// Page Réglages "Soleo" - accordéons thématiques
// Tous les IDs de champs sont inchangés (POST /ParaNew)
//*****************************************************
const char *ParaHtml = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="icon" href="/favicon.ico">
<title>Réglages</title>
<link rel="stylesheet" href="/commun.css">
<script src="/theme.js"></script>
<style>
/* Shell Soleo : voir /commun.css — surcharges propres a la page */
.app,header.top,.botnav{max-width:760px}
body{padding-bottom:150px}
h1,h2{margin:0;font-weight:700}
.online{display:flex;align-items:center;gap:5px;font-size:11.5px;font-weight:700;color:var(--grid-exp)}
.online.off{color:var(--txt-dim)}
.odot{width:9px;height:9px;border-radius:50%;background:currentColor;box-shadow:0 0 8px currentColor;animation:blink 2.4s infinite}
.modecard{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);padding:14px 16px;margin:14px 0;display:flex;align-items:center;justify-content:space-between;gap:12px;flex-wrap:wrap}
.mt{font-weight:700;font-size:14.5px}
.seg{display:flex;background:var(--card-2);border:1px solid var(--line);border-radius:999px;padding:3px}
.seg input{display:none}
.seg label{padding:7px 18px;border-radius:999px;font-size:13px;font-weight:700;color:var(--txt-soft);cursor:pointer;transition:.2s}
.seg input:checked+label{background:var(--accent);color:#08221a}
details.acc{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);margin:12px 0;box-shadow:var(--shadow)}
summary{display:flex;align-items:center;gap:12px;padding:15px 16px;cursor:pointer;list-style:none}
summary::-webkit-details-marker{display:none}
.aico{flex-shrink:0;width:38px;height:38px;border-radius:11px;display:grid;place-items:center;font-size:19px;background:var(--card-2)}
.atit{flex:1;font-weight:700;font-size:15.5px;min-width:0}
.atit small{display:block;font-weight:500;font-size:12px;color:var(--txt-dim)}
.chev{color:var(--txt-dim);transition:transform .25s;font-size:14px}
details[open] .chev{transform:rotate(180deg)}
.abody{padding:2px 16px 16px;border-top:1px solid var(--line)}
.row,.rowh{padding:12px 0;border-bottom:1px solid var(--line)}
.row{display:flex;flex-direction:column;gap:7px}
.rowh{display:flex;align-items:center;justify-content:space-between;gap:14px}
.rowh>label:first-child{flex:1}
.row:last-child,.rowh:last-child{border-bottom:none}
label{font-size:14px;font-weight:600}
.hint{display:block;font-size:12px;font-weight:400;color:var(--txt-dim);margin-top:2px}
.subt{font-size:11.5px;font-weight:800;color:var(--accent);text-transform:uppercase;letter-spacing:.07em;margin:16px 0 2px}
input[type=text],input[type=number],input[type=password],select{width:100%;background:var(--card-2);border:1px solid var(--line);border-radius:10px;color:var(--txt);font-size:16px;padding:10px 12px;outline:none}
input:focus,select:focus{border-color:var(--accent)}
input.ro{opacity:.55}
.duo{display:flex;gap:8px}
.info{font-size:13.5px;color:var(--txt-soft);background:var(--card-2);border-radius:10px;padding:10px 12px}
.sw{position:relative;display:inline-block;width:46px;height:26px;flex-shrink:0}
.sw input{opacity:0;width:0;height:0;position:absolute}
.kn{position:absolute;inset:0;background:var(--card-2);border:1px solid var(--line);border-radius:999px;transition:.25s;cursor:pointer}
.kn:before{content:"";position:absolute;width:18px;height:18px;left:3px;top:3px;border-radius:50%;background:var(--txt-dim);transition:.25s}
.sw input:checked+.kn{background:rgba(110,231,199,.22);border-color:var(--accent)}
.sw input:checked+.kn:before{transform:translateX(20px);background:var(--accent)}
.sw input:disabled+.kn{opacity:.35;cursor:default}
.ghost,.danger{border:1px solid var(--line);background:var(--card-2);color:var(--txt);border-radius:10px;padding:9px 14px;font-size:13.5px;font-weight:700;cursor:pointer}
.ghost:active,.danger:active{transform:scale(.96)}
.danger{border-color:rgba(229,96,77,.5);color:var(--grid-imp)}
.links{display:grid;grid-template-columns:1fr 1fr;gap:8px;padding:10px 0}
.lnk{display:flex;align-items:center;gap:8px;background:var(--card-2);border:1px solid var(--line);border-radius:10px;padding:11px 12px;font-size:13.5px;font-weight:600}
.lnk:active{transform:scale(.97)}
.tcanal{border:1px solid var(--line);border-radius:10px;padding:2px 12px;margin:10px 0;background:rgba(0,0,0,.07)}
.rrow input{margin-top:4px}
#saveBar{position:fixed;left:50%;transform:translate(-50%,30px);bottom:78px;z-index:60;display:flex;align-items:center;gap:12px;background:var(--card);border:1px solid var(--accent);border-radius:16px;padding:10px 14px;box-shadow:var(--shadow);transition:transform .3s,opacity .3s;width:min(92%,560px);opacity:0;pointer-events:none}
#saveBar.show{transform:translate(-50%,0);opacity:1;pointer-events:auto}
.sv-txt{flex:1;font-size:12.5px;color:var(--txt-soft)}
.save{background:var(--accent);color:#08221a;border:none;border-radius:11px;padding:11px 20px;font-size:14.5px;font-weight:800;cursor:pointer}
.save:active{transform:scale(.96)}
#attente{visibility:hidden;width:17px;height:17px;border:3px solid var(--line);border-top-color:var(--accent);border-radius:50%;animation:spin .8s linear infinite;flex-shrink:0}
@keyframes spin{to{transform:rotate(360deg)}}
#toast{position:fixed;top:14px;left:50%;transform:translate(-50%,-180%);z-index:80;background:var(--card);border:1px solid var(--accent);color:var(--txt);border-radius:14px;padding:12px 20px;font-size:14px;font-weight:600;box-shadow:var(--shadow);transition:transform .35s;max-width:92%}
#toast.show{transform:translate(-50%,0)}
.foot{text-align:center;font-size:12px;color:var(--txt-dim);margin:24px 0 8px}
.foot a{color:var(--accent)}
#donEnv{display:none;text-align:center;margin:10px auto;background:#fff;border-radius:14px;padding:4px;width:fit-content}
.donNone{display:none}
@media(min-width:700px){
body{padding-bottom:90px}
.botnav{position:static;border-top:none;border-bottom:1px solid var(--line);max-width:760px;margin:0 auto;justify-content:center;gap:20px;background:transparent;backdrop-filter:none}
.botnav a{flex-direction:row;font-size:13px;gap:7px}
.botnav a .ni{font-size:17px}
#saveBar{bottom:18px}
.row{flex-direction:row;align-items:center;justify-content:space-between}
.row>label,.row>.hint{flex:1;padding-right:14px}
.row>input,.row>select,.row>.duo,.row>.info,.row>button{width:46%}
.duo input{width:50%}
}
</style>
</head>
<body onload="Init();">
<header class="top">
<div class="brand"><span class="logo"></span><span id="nom_R">Routeur solaire</span></div>
<div class="top-actions"><span class="badge" id="tarifBadge">—</span><span class="online off" id="online"><span class="odot"></span><span id="onlineTxt">…</span></span><button class="iconbtn" id="btnTheme" title="Thème">🌙</button></div>
</header>
<nav class="botnav">
<a href="/"><span class="ni">⚡</span>Accueil</a>
<a href="/mesures"><span class="ni">📊</span>Mesures</a>
<a href="/Actions"><span class="ni">✨</span>Actions</a>
<a class="active" href="/Para"><span class="ni">🛠️</span>Réglages</a>
</nav>
<main class="app" id="LesParas" style="display:none">

<div class="modecard">
<div><div class="mt">Niveau de réglage</div><span class="hint">Le mode Expert révèle tous les réglages techniques.</span></div>
<div class="seg">
<input type="radio" name="ModeP" id="ModeP0" value="0" checked onclick="checkDisabled();"><label for="ModeP0">Standard</label>
<input type="radio" name="ModeP" id="ModeP1" value="1" onclick="checkDisabled();"><label for="ModeP1">Expert</label>
</div>
</div>

<details class="acc" id="accEssentiel" open>
<summary><span class="aico">⭐</span><span class="atit">L'essentiel<small>Nom, tarifs, solaire, eau chaude</small></span><span class="chev">▼</span></summary>
<div class="abody">
<div class="row"><label for="nomRouteur">Nom du routeur</label><input type="text" id="nomRouteur" name="nomRouteur"></div>
<div class="subt">Votre tarif électrique</div>
<div class="row ligneTarifBase"><label for="PrixHP">Tarif Heures Pleines (€/kWh)<span class="hint">ou tarif unique si vous n'avez pas d'heures creuses</span></label><input type="number" id="PrixHP" name="PrixHP" step="0.001" min="0" max="2"></div>
<div class="row ligneTarifBase"><label for="PrixHC">Tarif Heures Creuses (€/kWh)</label><input type="number" id="PrixHC" name="PrixHC" step="0.001" min="0" max="2"></div>
<div class="ligneTarifTempo" style="display:none"><span class="hint">Tarif Tempo activé : saisissez vos 6 prix (€/kWh). Activez/désactivez Tempo dans « Automatismes avancés ».</span>
<div class="subt" style="color:#4ab3f4">🔵 Jour Bleu</div>
<div class="row"><label for="PrixBleuHP">Bleu — Heures Pleines</label><input type="number" id="PrixBleuHP" name="PrixBleuHP" step="0.0001" min="0" max="2"></div>
<div class="row"><label for="PrixBleuHC">Bleu — Heures Creuses</label><input type="number" id="PrixBleuHC" name="PrixBleuHC" step="0.0001" min="0" max="2"></div>
<div class="subt" style="color:#9aa6b8">⚪ Jour Blanc</div>
<div class="row"><label for="PrixBlancHP">Blanc — Heures Pleines</label><input type="number" id="PrixBlancHP" name="PrixBlancHP" step="0.0001" min="0" max="2"></div>
<div class="row"><label for="PrixBlancHC">Blanc — Heures Creuses</label><input type="number" id="PrixBlancHC" name="PrixBlancHC" step="0.0001" min="0" max="2"></div>
<div class="subt" style="color:#e5604d">🔴 Jour Rouge</div>
<div class="row"><label for="PrixRougeHP">Rouge — Heures Pleines</label><input type="number" id="PrixRougeHP" name="PrixRougeHP" step="0.0001" min="0" max="2"></div>
<div class="row"><label for="PrixRougeHC">Rouge — Heures Creuses</label><input type="number" id="PrixRougeHC" name="PrixRougeHC" step="0.0001" min="0" max="2"></div>
</div>
<div class="subt">Prévision solaire</div>
<div class="rowh"><label for="MeteoOn">Activer la prévision solaire<span class="hint">Le routeur anticipe la production de vos panneaux (Open-Meteo, gratuit, sans clé)</span></label><label class="sw"><input type="checkbox" id="MeteoOn" name="MeteoOn" onclick="checkDisabled();"><span class="kn"></span></label></div>
<div class="row ligneMeteo" style="display:none"><label for="MeteoPVcrete">Puissance crête de vos panneaux (kWc)</label><input type="number" id="MeteoPVcrete" name="MeteoPVcrete" step="0.1" min="0.1" max="100"></div>
<div class="row ligneMeteo" style="display:none"><label for="MeteoLat">Localisation de la maison<span class="hint">latitude / longitude</span></label>
<div class="duo"><input type="number" id="MeteoLat" name="MeteoLat" step="0.0001" min="-90" max="90" placeholder="Latitude"><input type="number" id="MeteoLon" name="MeteoLon" step="0.0001" min="-180" max="180" placeholder="Longitude"></div>
</div>
<div class="row ligneMeteo" style="display:none"><label>&nbsp;</label><button type="button" class="ghost" id="btnGeo" onclick="DetectePosition();">📍 Détecter ma position</button></div>
<div class="row ligneMeteo" style="display:none"><label>Prévision de production</label><div class="info" id="previsionMeteo">—</div></div>
<div class="subt">Onduleur SMA Sunny Boy</div>
<div class="rowh"><label for="SmaOn">Lire la production de l'onduleur<span class="hint">Activez d'abord Modbus TCP sur l'onduleur : Paramètres &gt; Communication externe &gt; Modbus &gt; TCP on</span></label><label class="sw"><input type="checkbox" id="SmaOn" name="SmaOn" onclick="checkDisabled();"><span class="kn"></span></label></div>
<div class="row ligneSma" style="display:none"><label for="SmaIP">Adresse de votre onduleur SMA<span class="hint">ex : 192.168.1.50</span></label><input type="text" id="SmaIP" name="SmaIP"></div>
<div class="row ligneSma" style="display:none"><label>Production actuelle / du jour</label><div class="info" id="productionSma">—</div></div>
<div class="subt">Ballon d'eau chaude</div>
<div class="row"><label for="BallonCanal">Sonde de température du ballon<span class="hint">Permet la chauffe intelligente : forçage la nuit seulement si le soleil du lendemain ne suffira pas</span></label>
<select id="BallonCanal" name="BallonCanal" onchange="checkDisabled();" onclick="checkDisabled();">
<option value="-1">Non exploité</option><option value="0">Canal 0</option><option value="1">Canal 1</option><option value="2">Canal 2</option><option value="3">Canal 3</option>
</select></div>
<div class="row ligneBallon" style="display:none"><label for="BallonVolume">Capacité du ballon (litres)</label><input type="number" id="BallonVolume" name="BallonVolume" step="10" min="50" max="500"></div>
<div class="row ligneBallon" style="display:none"><label for="BallonTcible">Température souhaitée (°C)</label><input type="number" id="BallonTcible" name="BallonTcible" step="1" min="40" max="80"></div>
<div class="row ligneBallon" style="display:none"><label for="BallonPuissance">Puissance de la résistance (W)</label><input type="number" id="BallonPuissance" name="BallonPuissance" step="100" min="500" max="6000"></div>
<div class="rowh ligneBallon" style="display:none"><label for="BallonModeIntel">Chauffe prédictive (intelligente)<span class="hint">Apprend votre consommation réelle d'eau chaude et n'entretient que la réserve nécessaire, au lieu de toujours viser la température cible. Une chauffe anti-légionelle de sécurité reste assurée.</span></label><label class="sw"><input type="checkbox" id="BallonModeIntel" name="BallonModeIntel" onclick="checkDisabled();"><span class="kn"></span></label></div>
<div class="row ligneBallonIntel" style="display:none"><label for="BallonTmin">Température minimale utile (°C)<span class="hint">en dessous, l'eau n'est plus considérée comme exploitable — typiquement 40°C</span></label><input type="number" id="BallonTmin" name="BallonTmin" step="1" min="20" max="55"></div>
<div class="row ligneBallon" style="display:none"><label>Diagnostic du jour</label><div class="info" id="etatBallon">—</div></div>
</div>
</details>

<details class="acc" id="accEquip">
<summary><span class="aico">🔌</span><span class="atit">Équipements &amp; mesures<small>Capteurs, sondes, matériel</small></span><span class="chev">▼</span></summary>
<div class="abody">
<div class="row"><label for="sources">Comment mesurez-vous votre consommation ?<span class="hint">Interface de mesure installée — redémarrage nécessaire</span></label>
<select id="sources" onclick="checkDisabled();" onchange="checkDisabled();">
<option value="NotDef" selected>Non définie</option>
<option value="UxI">UxI (transfo + pince ampèremétrique)</option>
<option value="UxIx2">UxIx2 (module JSY-MK-194T)</option>
<option value="UxIx3">UxIx3 (module JSY-MK-333 triphasé)</option>
</select></div>
<div class="row" id="ligneMobile"><label for="nomSondeMobile">Nom de la mesure principale<span class="hint">puissance à l'entrée de la maison</span></label><input type="text" id="nomSondeMobile" name="nomSondeMobile"></div>
<div class="row" id="ligneFixe" style="display:none"><label for="nomSondeFixe">Nom de la seconde sonde<span class="hint">obligatoire si vous avez 2 sondes</span></label><input type="text" id="nomSondeFixe" name="nomSondeFixe"></div>
<div class="row" id="ligneFixe1" style="display:none"><label for="nomSfixePpos">Nom si puissance seconde sonde &gt; 0<span class="hint">vide = colonne masquée</span></label><input type="text" id="nomSfixePpos" name="nomSfixePpos"></div>
<div class="row" id="ligneFixe2" style="display:none"><label for="nomSfixePneg">Nom si puissance seconde sonde &lt; 0<span class="hint">vide = colonne masquée</span></label><input type="text" id="nomSfixePneg" name="nomSfixePneg"></div>
<div id="LesSourcesTemp" style="display:none"><div class="subt">Sondes de température</div><div id="Sources_Temp"></div></div>
<div class="subt">Matériel (redémarrage nécessaire)</div>
<div class="row"><label for="ESP">Modèle de carte ESP32<span class="hint">définit les broches GPIO disponibles</span></label>
<select id="ESP" onclick="checkDisabled();" onchange="checkDisabled();">
<option value="0" selected>Non défini</option><option value="1">Wroom seul</option><option value="2">Carte 1 relais</option><option value="3">Carte 4 relais</option>
</select></div>
<div class="row" id="port_serie"><label for="Serie">Port série 2<span class="hint">pour module JSY (UxIx2, UxIx3)</span></label>
<select id="Serie" onclick="checkDisabled();" onchange="checkDisabled();">
<option value="0" selected>Non utilisé</option>
<option value="1">RX=gpio 16, TX=gpio 17</option>
<option value="2">RX=gpio 26, TX=gpio 27</option>
<option value="3">RX=gpio 18, TX=gpio 19</option>
<option value="4">RX=gpio 5, TX=gpio 17</option>
<option value="5">RX=gpio 21, TX=gpio 22</option>
</select></div>
<div class="row" id="Vport_serie" style="display:none"><label for="Serial2V">Vitesse du port série 2<span class="hint">défaut : UxIx2=4800, UxIx3=9600</span></label>
<select id="Serial2V" onclick="checkDisabled();">
<option value="0" selected>Pas utilisé</option><option value="4800">4800 bauds</option><option value="9600">9600 bauds</option><option value="19200">19200 bauds</option><option value="38400">38400 bauds</option>
</select></div>
<div class="row" id="Analog" style="display:none"><label for="pUxI">Entrées analogiques (GPIO)</label>
<select id="pUxI" onclick="checkDisabled();">
<option value="0" selected>Pas d'entrée</option>
<option value="1">Commun(35), U(32), I(33)</option>
<option value="2">Commun(35), U(32), I(34)</option>
<option value="3">Commun(34), U(32), I(33)</option>
<option value="4">Commun(35), U(36), I(39)</option>
</select></div>
<div class="row"><label for="pLED">LEDs d'état</label>
<select id="pLED" onclick="checkDisabled();">
<option value="0" selected>Pas d'affichage</option>
<option value="1">LEDs gpio 18, gpio 19</option>
<option value="2">LEDs gpio 4, gpio 16</option>
<option value="3">LEDs gpio 2, gpio 4</option>
<option value="4">LEDs gpio 4, gpio 17</option>
</select></div>
<div class="row" id="pTemp" style="display:none"><label for="PTemp">Broche du capteur de température</label>
<select id="PTemp" onclick="checkDisabled();">
<option value="0" selected>Pas de capteur</option>
<option value="10">DS18B20 gpio 4</option>
<option value="5">DS18B20 gpio 5</option>
<option value="1">DS18B20 gpio 13</option>
<option value="11">DS18B20 gpio 16</option>
<option value="12">DS18B20 gpio 17</option>
<option value="6">DS18B20 gpio 18</option>
<option value="7">DS18B20 gpio 19</option>
<option value="4">DS18B20 gpio 21</option>
<option value="8">DS18B20 gpio 22</option>
<option value="9">DS18B20 gpio 23</option>
<option value="2">DS18B20 gpio 27</option>
<option value="3">DS18B20 gpio 33</option>
</select></div>
</div>
</details>

<details class="acc" id="accVentil">
<summary><span class="aico">🌀</span><span class="atit">Ventilateur SSR<small>Refroidissement thermorégulé par sonde</small></span><span class="chev">▼</span></summary>
<div class="abody">
<div class="row"><label for="FanGpio">Broche GPIO du ventilateur<span class="hint">Broche câblée sur le connecteur FAN de la carte (PWM via transistor)</span></label>
<select id="FanGpio" name="FanGpio" onchange="checkDisabled();" onclick="checkDisabled();">
<option value="0">Désactivé</option>
<option value="4">GPIO 4</option>
<option value="5">GPIO 5</option>
<option value="12">GPIO 12</option>
<option value="13">GPIO 13</option>
<option value="14">GPIO 14</option>
<option value="25">GPIO 25</option>
<option value="26">GPIO 26</option>
<option value="27">GPIO 27</option>
<option value="32">GPIO 32</option>
<option value="33">GPIO 33</option>
</select></div>
<div class="row ligneFan" style="display:none"><label for="FanCanalTemp">Sonde de température à surveiller<span class="hint">Sonde posée sur le dissipateur du SSR</span></label>
<select id="FanCanalTemp" name="FanCanalTemp">
<option value="-1">Non configurée</option>
<option value="0">Canal 0</option>
<option value="1">Canal 1</option>
<option value="2">Canal 2</option>
<option value="3">Canal 3</option>
</select></div>
<div class="row ligneFan" style="display:none"><label for="FanTdemarrage">Température de démarrage (°C)<span class="hint">Le ventilateur démarre à vitesse minimale dès ce seuil</span></label><input type="number" id="FanTdemarrage" name="FanTdemarrage" step="1" min="20" max="80"></div>
<div class="row ligneFan" style="display:none"><label for="FanTmax">Température pleine vitesse (°C)<span class="hint">Au-delà, le ventilateur tourne à 100%</span></label><input type="number" id="FanTmax" name="FanTmax" step="1" min="30" max="100"></div>
<div class="row ligneFan" style="display:none"><label for="FanVitesseMin">Vitesse minimale (%)<span class="hint">Évite le calage au démarrage — typiquement 20 à 40%</span></label><input type="number" id="FanVitesseMin" name="FanVitesseMin" step="5" min="0" max="80"></div>
</div>
</details>

<details class="acc" id="accAbsence">
<summary><span class="aico">🏨</span><span class="atit">Mode absence<small>Coupe les actions quand vous êtes parti</small></span><span class="chev">▼</span></summary>
<div class="abody">
<div class="info">Quand le mode absence est actif, toutes les actions (chauffe-eau…) sont coupées. Une chauffe de sécurité anti-légionelle est déclenchée automatiquement si le ballon reste froid trop longtemps. Activez aussi le mode d'un simple bouton depuis l'accueil.</div>
<div class="rowh"><label for="AbsenceManuel">Je suis absent maintenant<span class="hint">activation manuelle immédiate</span></label><label class="sw"><input type="checkbox" id="AbsenceManuel" name="AbsenceManuel"><span class="kn"></span></label></div>
<div class="subt">Absence programmée (optionnel)</div>
<div class="row"><label for="AbsenceDebut">Date de départ</label><input type="date" id="AbsenceDebut" name="AbsenceDebut"></div>
<div class="row"><label for="AbsenceFin">Date de retour</label><input type="date" id="AbsenceFin" name="AbsenceFin"></div>
<div class="subt">Sécurité sanitaire</div>
<div class="row"><label for="AbsenceAntiLegio">Chauffe anti-légionelle<span class="hint">force une chauffe complète du ballon si sa température cible n'est pas atteinte depuis ce nombre de jours (0 = désactivé). Nécessite une sonde ballon configurée.</span></label>
<select id="AbsenceAntiLegio" name="AbsenceAntiLegio">
<option value="0">Désactivée</option>
<option value="3">Tous les 3 jours</option>
<option value="5">Tous les 5 jours</option>
<option value="7">Toutes les semaines</option>
<option value="14">Toutes les 2 semaines</option>
</select></div>
</div>
</details>

<details class="acc" id="accReseau">
<summary><span class="aico">🌐</span><span class="atit">Réseau &amp; domotique<small>WiFi, adresse IP, MQTT</small></span><span class="chev">▼</span></summary>
<div class="abody">
<div class="row"><label for="ModeW">Accès réseau</label>
<select id="ModeW" onclick="checkDisabled();" onchange="checkDisabled();">
<option value="0" selected>Internet</option><option value="1">Réseau local uniquement</option><option value="2">Point isolé</option>
</select></div>
<div class="row"><label for="hostname">Nom d'hôte<span class="hint">accès via hostname.local — caractères a-z, 0-9, tiret</span></label><input type="text" id="hostname" name="hostname"></div>
<div class="rowh" id="rowDhcp"><label for="dhcp">Adresse IP automatique (DHCP)<span class="hint" id="localIP"></span><span class="hint" id="ipreset">Redémarrage nécessaire</span></label><label class="sw"><input type="checkbox" id="dhcp" name="dhcp" onclick="checkDisabled();"><span class="kn"></span></label></div>
<div id="infoIP" style="display:none">
<div class="row"><label for="adrIP">IP fixe de votre routeur (optionnel)<span class="hint">ex : 192.168.1.245</span></label><input type="text" id="adrIP" name="adrIP" minlength=7 maxlength=15 placeholder="xxx.xxx.xxx.xxx"></div>
<div class="row"><label for="gateway">Passerelle<span class="hint">en général l'adresse de votre box internet</span></label><input type="text" id="gateway" name="gateway" minlength=7 maxlength=15 placeholder="xxx.xxx.xxx.xxx"></div>
<div class="row"><label for="masque">Masque de sous-réseau<span class="hint">ex : 255.255.255.0</span></label><input type="text" id="masque" name="masque" minlength=7 maxlength=15 placeholder="xxx.xxx.xxx.xxx"></div>
<div class="row"><label for="dns">DNS<span class="hint">en général l'adresse de votre box internet</span></label><input type="text" id="dns" name="dns" minlength=7 maxlength=15 placeholder="xxx.xxx.xxx.xxx"></div>
</div>
<div class="subt" id="subtMqtt" style="display:none">Serveur domotique (MQTT)</div>
<div class="row" id="l_wifi_1" style="display:none"><label for="MQTTRepet">Envoi des mesures à la domotique<span class="hint">période en secondes entre deux envois (0 = pas d'envoi, 10 mini conseillé)</span></label><input type="number" id="MQTTRepet" name="MQTTRepet" onclick="checkDisabled();" onchange="checkDisabled();"></div>
<div id="Zmqtt" style="display:none">
<div class="row"><label for="MQTTIP">Serveur domotique (adresse IP)<span class="hint">Home Assistant, Domoticz…</span></label><input type="text" id="MQTTIP" name="MQTTIP"></div>
<div class="row"><label for="MQTTPort">Port MQTT</label><input type="number" id="MQTTPort" name="MQTTPort"></div>
<div class="row"><label for="MQTTUser">Identifiant de connexion</label><input type="text" id="MQTTUser" name="MQTTUser" autocomplete="on"></div>
<div class="row"><label for="MQTTPwd">Mot de passe de connexion</label><input type="password" id="MQTTPwd" name="MQTTPwd" autocomplete="on"></div>
<div class="row"><label for="MQTTPrefix">Préfixe de découverte</label><input type="text" id="MQTTPrefix" name="MQTTPrefix"></div>
<div class="row"><label for="MQTTPrefixEtat">Préfixe de publication</label><input type="text" id="MQTTPrefixEtat" name="MQTTPrefixEtat"></div>
<div class="row"><label for="MQTTdeviceName">Nom de l'appareil dans la domotique</label><input type="text" id="MQTTdeviceName" name="MQTTdeviceName"></div>
<div class="rowh"><label for="subMQTT">Piloter les actions depuis la domotique<span class="hint">souscription au forçage via MQTT</span></label><label class="sw"><input type="checkbox" id="subMQTT" name="subMQTT" onclick="checkDisabled();"><span class="kn"></span></label></div>
</div>
<div id="listerouteurs" style="display:none"><div class="subt">Autres routeurs sur le réseau</div><span class="hint">Même version de routeur recommandée</span><div id="Routeurs"></div></div>
</div>
</details>

<details class="acc" id="accAvance">
<summary><span class="aico">⚙️</span><span class="atit">Automatismes avancés<small>Tarif Tempo, WiFi, calibration</small></span><span class="chev">▼</span></summary>
<div class="abody">
<div class="rowh" id="l_wifi_0"><label for="TempoRTEon">Type de tarif électrique : Tempo<span class="hint">affiche la couleur du jour Tempo RTE et débloque les 6 tarifs — redémarrage nécessaire</span></label><label class="sw"><input type="checkbox" id="TempoRTEon" name="TempoRTEon" onclick="checkDisabled();"><span class="kn"></span></label></div>
<div class="rowh" id="l_wifi_2" style="display:none"><label for="WifiSleep">Veille WiFi<span class="hint">réduit la consommation mais ralentit la communication — redémarrage nécessaire</span></label><label class="sw"><input type="checkbox" id="WifiSleep" name="WifiSleep"><span class="kn"></span></label></div>
<div class="row" id="SurvCom" style="display:none"><label for="ComSurv">Redémarrer si coupure WiFi de plus de</label>
<select id="ComSurv" onclick="checkDisabled();">
<option value="6" selected>3mn</option><option value="60">30mn</option><option value="360">3h</option><option value="1440">12h</option><option value="2880">24h</option>
</select></div>
<div id="Zcalib" style="display:none"><div class="subt" id="Tui">Calibration des mesures (UxI)</div>
<div class="row" id="CUi"><label for="CalibU">Coefficient multiplicateur Ueff<span class="hint">typique : 1000</span></label><input type="number" id="CalibU" name="CalibU"></div>
<div class="row" id="CuI"><label for="CalibI">Coefficient multiplicateur Ieff<span class="hint">typique : 1000</span></label><input type="number" id="CalibI" name="CalibI"></div>
</div>
</div>
</details>

<details class="acc" id="accSysteme" style="display:none">
<summary><span class="aico">🧰</span><span class="atit">Système<small>Sécurité, mises à jour, maintenance</small></span><span class="chev">▼</span></summary>
<div class="abody">
<div class="row"><label for="CleAcces">Mot de passe d'accès<span class="hint">protège les réglages — pas de « ; », vide = accès libre</span></label><input type="text" id="CleAcces" name="CleAcces"></div>
<div class="subt">Outils</div>
<div class="links">
<a class="lnk" href="/OTA">⬆️ Mise à jour du logiciel</a>
<a class="lnk" href="/Export">💾 Export des données</a>
<a class="lnk" href="/Heure">🕒 Réglage de l'heure</a>
<a class="lnk" href="/Brute">🔬 Données brutes</a>
<a class="lnk" href="/Wifi">📶 Configuration WiFi</a>
<a class="lnk" href="/Couleurs">🎨 Couleurs (anciennes pages)</a>
</div>
<div class="subt">Maintenance</div>
<div class="rowh"><label>Remise à zéro de l'historique des mesures<span class="hint">efface définitivement les historiques enregistrés</span></label><button type="button" class="danger" id="RAZdata" onclick="SendRAZ();">RAZ</button></div>
<div class="rowh" id="BoutonsBas"><label>Redémarrer le routeur</label><button type="button" class="ghost" onclick="Reset();">Redémarrer</button></div>
</div>
</details>

<div class="foot"><span id="version"></span> · <a href="https://f1atb.fr/fr" target="_blank">F1ATB.fr</a></div>
<div id="donEnv">
<form action="https://www.paypal.com/donate" method="post" target="_top">
<input type="hidden" name="hosted_button_id" value="Z35E9D5D9N9DN">
<input type="image" src="https://pics.paypal.com/00/s/MGY1NzdhY2YtYTRkNi00YzIwLWI2YzQtNWI3YjM3ZmFiNWUx/file.PNG" style="border:0;height:42px;" name="submit" title="PayPal" alt="Faites un don avec PayPal">
<img alt="" class="donNone" style="border:0;" src="https://www.paypal.com/fr_FR/i/scr/pixel.gif" width="1" height="1">
</form>
</div>
</main>

<div id="saveBar"><span class="sv-txt">Modifications non enregistrées</span><div id="attente"></div><button class="save" onclick="SendValues();">💾 Sauvegarder</button></div>
<div id="toast"></div>

<script src="/ParaCommunJS"></script>
<script src="/ParaJS1"></script>
<script src="/ParaJS2"></script>
</body>
</html>
)====";
