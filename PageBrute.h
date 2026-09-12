//******************************************
// Page données RMS Brutes — design Soleo
//******************************************
const char *PageBrute = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="icon" href="/favicon.ico">
  <title>Données brutes</title>
  <link rel="stylesheet" href="/commun.css">
  <script src="/theme.js"></script>
<style>
/* Shell Soleo : voir /commun.css — surcharges propres a la page */
.app,header.top,.botnav{max-width:900px}
  @media(min-width:700px){body{padding-bottom:20px}.botnav{position:static;border-top:none;border-bottom:1px solid var(--line);max-width:900px;margin:0 auto;justify-content:center;gap:20px;background:transparent;backdrop-filter:none}.botnav a{flex-direction:row;font-size:13px;gap:7px}.botnav a .ni{font-size:17px}}

  /* === Contenu données brutes — reskin Soleo === */
  .ri { text-align: right; }
  /* Secours CSS : LoadDataESP32() reinjecte son tableau toutes les 5 s sans
     relancer setCouleur(), les cellules retombent donc sur ces fonds pastel.
     Ils sont clairs : le texte doit l'etre aussi, sinon il devient illisible
     des que la palette du tableau passe en clair sur fond sombre. */
  .Wh, .A, .W, .phi, .V, .VA, .Hz, .Enph { color: #101418; }
  .Wh { background-color: #fdd; }
  .A { background-color: #ddf; }
  .W { background-color: #f88; }
  .phi { background-color: #ffd; }
  .V { background-color: #ee8; }
  .VA { background-color: #dfd; }
  .Hz, .Enph { background-color: #eeb; }
  /* tableaux : Soleo card */
  .tableau { overflow: hidden; margin-bottom: 14px; }
  .titre { background: rgba(74,179,244,.22); color: inherit; text-align: center; font-weight: 700; }
  /* La palette (page Couleurs) pose sa couleur de texte sur .tableau ;
     forcer var(--txt) ici la masquait et laissait du pale sur du pale. */
  td { text-align: left; padding: 6px 8px; color: inherit; }
  .dataIn { text-align: left; overflow: hidden; word-wrap: break-word; font-size: 13px; }
  svg { border: 1px solid var(--line); border-radius: var(--radius-s); }
  .Bbrut { border: 2px solid var(--accent); border-radius: var(--radius-s); }
  .dispT { display: none; }
  .ce { text-align: center; position: relative; }
  #LED {
    position: fixed;
    top: 70px;
    right: 16px;
    width: 10px;
    height: 10px;
    border-radius: 50%;
    background: var(--grid-exp);
    box-shadow: 0 0 8px var(--grid-exp);
    display: none;
    z-index: 30;
  }
  #infoUxIx2,#infoUxIx3,#infoUxI,#infoNotDef,#infoLinky,
  #infoEnphase,#infoSmartG,#infoHomeW,#infoShellyEm,#infoPmqtt{display:none}
  #DataLinky{font-size:13px;tab-size:10;color:inherit}
  #donneeDistante{font-size:50%;color:var(--txt-soft);text-align:center;margin-bottom:10px;display:none}
  .bloc a:link,.bloc a:visited{color:var(--accent);text-decoration:none}
  .sec-title{font-size:12px;font-weight:700;color:var(--txt-soft);margin:18px 0 6px;text-transform:uppercase;letter-spacing:.08em}
  </style>
</head>
<body onload="SetHautBas(); LoadParaFixe();">

<div id="LED" title="Réception des données"></div>

<header class="top">
<div class="brand"><span class="logo"></span><span id="nom_R">Routeur solaire</span></div>
<div class="top-actions"><span class="badge" id="tarifBadge">—</span><button class="iconbtn" id="btnTheme" title="Thème">🌙</button></div>
</header>
<nav class="botnav">
<a href="/"><span class="ni">⚡</span>Accueil</a>
<a class="active" href="/mesures"><span class="ni">📊</span>Mesures</a>
<a href="/Actions"><span class="ni">✨</span>Actions</a>
<a href="/Para"><span class="ni">🛠️</span>Réglages</a>
</nav>

<div class="app" style="margin-top:16px">

  <div id="date" style="font-size:13px;color:var(--txt-dim);margin-bottom:8px">Date</div>

  <!-- Source non définie -->
  <div id="infoNotDef">
    <div class="tableau">
      Source des mesures de puissance non définie<br>
      À définir au bas de la page Paramètres<br><br>
      Données simulées
    </div><br>
  </div>

  <!-- U/I 20ms -->
  <div id="infoUxI">
    <div class="sec-title">Tension et Courant sur 20ms</div>
    <div class="ce">
      <h3 style="position:absolute;top:20px;right:40px">
        <span id="Ueff">.</span>
        <span id="Ieff"></span><br>
        <span id="cosphi"></span>
      </h3>
      <p id="SVG"></p>
    </div>
  </div>

  <!-- JSY-MK-194T -->
  <div id="infoUxIx2">
    <div class="sec-title">Données brutes capteur JSY-MK-194T</div>
    <div id="tableau" class="tableau"></div>
  </div>

  <!-- JSY-MK-333 -->
  <div id="infoUxIx3">
    <div class="sec-title">Données brutes capteur JSY-MK-333</div>
    <div id="dataUxIx3" class="tableau dataIn"></div>
  </div>

  <!-- Enphase -->
  <div id="infoEnphase">
    <div class="sec-title">Données Enphase Envoy-S Metered</div>
    <div id="tableauEnphase" class="tableau"></div>
  </div>

  <!-- SmartGateways -->
  <div id="infoSmartG">
    <div class="sec-title">Données SmartGateways</div>
    <div id="dataSmartG" class="tableau dataIn"></div>
  </div>

  <!-- HomeWizard -->
  <div id="infoHomeW">
    <div class="sec-title">Données HomeWizard</div>
    <div id="dataHomeW" class="tableau dataIn"></div>
  </div>

  <!-- Shelly EM -->
  <div id="infoShellyEm">
    <div class="sec-title">Données Shelly EM</div>
    <div id="dataShellyEm" class="tableau dataIn"></div>
  </div>

  <!-- MQTT -->
  <div id="infoPmqtt">
    <div class="sec-title">Données puissances reçues par MQTT</div>
    <div id="dataPmqtt" class="tableau dataIn"></div>
  </div>

  <!-- Linky -->
  <div id="infoLinky">
    <div id="dateLinky"></div>
    <div id="tableauLinky" class="tableau"></div>
    <br>
    <div class="sec-title">Données brutes Linky (mode standard)</div>
    <div id="DataLinky" class="tableau dataIn"></div>
  </div>

  <div id="donneeDistante">Données distantes</div>

  <!-- Données ESP32 -->
  <div class="sec-title">Données ESP32</div>
  <div id="DataESP32" class="tableau bloc"></div>
  <br>
</div>

<!-- Scripts -->
<script src="/ParaCommunJS"></script>
<script src="BruteJS1"></script>
<script src="BruteJS2"></script>
<script src="/CommunCouleurJS"></script>
<script>
// Bascule de thème : /theme.js (partagé)
</script>
</body>
</html>
)====";
