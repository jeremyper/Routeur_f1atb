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
  <style>
  /* === Shell Soleo === */
  :root{--bg:#0d1117;--bg-soft:#161b22;--card:#1c2230;--card-2:#232b3b;--line:#2c3444;--txt:#e8edf6;--txt-soft:#9aa6b8;--txt-dim:#6b7585;--sun:#ffb547;--sun-soft:#ffd27d;--home:#4ab3f4;--water:#ff6f91;--grid-imp:#e5604d;--grid-exp:#4cd28b;--accent:#6ee7c7;--shadow:0 10px 30px rgba(0,0,0,.35);--radius:20px;--radius-s:14px}
  html[data-theme=light]{--bg:#eef2f8;--bg-soft:#fff;--card:#fff;--card-2:#f3f6fb;--line:#e0e6f0;--txt:#1a2230;--txt-soft:#586273;--txt-dim:#8a93a4}
  *{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
  body{margin:0;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;background:var(--bg);color:var(--txt);line-height:1.45;transition:background .4s,color .4s;padding-bottom:88px}
  a{text-decoration:none;color:inherit}
  .app{max-width:900px;margin:0 auto;padding:0 12px}
  header.top{position:sticky;top:0;z-index:40;background:color-mix(in srgb,var(--bg) 88%,transparent);backdrop-filter:blur(14px);display:flex;align-items:center;justify-content:space-between;padding:12px 16px;max-width:900px;margin:0 auto;gap:8px}
  .brand{display:flex;align-items:center;gap:10px;font-weight:800;font-size:18px;min-width:0}
  #nom_R{white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
  .logo{flex-shrink:0;width:30px;height:30px;border-radius:9px;background:radial-gradient(circle at 35% 30%,var(--sun-soft),var(--sun) 65%,#e98a1a);box-shadow:0 0 18px rgba(255,181,71,.45)}
  .top-actions{display:flex;gap:8px;align-items:center;flex-shrink:0}
  .badge{font-size:11.5px;font-weight:700;padding:4px 10px;border-radius:999px;background:var(--card-2);color:var(--txt-soft);border:1px solid var(--line);white-space:nowrap}
  .iconbtn{width:38px;height:38px;border-radius:12px;border:1px solid var(--line);background:var(--card);color:var(--txt);font-size:17px;cursor:pointer;display:grid;place-items:center}
  .iconbtn:active{transform:scale(.92)}
  .botnav{position:fixed;bottom:0;left:0;right:0;z-index:50;background:color-mix(in srgb,var(--bg-soft) 92%,transparent);backdrop-filter:blur(16px);border-top:1px solid var(--line);display:flex;justify-content:space-around;padding:8px 4px env(safe-area-inset-bottom)}
  .botnav a{color:var(--txt-dim);display:flex;flex-direction:column;align-items:center;gap:2px;font-size:10.5px;font-weight:600;padding:6px 14px;border-radius:12px}
  .botnav a .ni{font-size:20px}
  .botnav a.active{color:var(--accent)}
  @keyframes blink{50%{opacity:.3}}
  @media(min-width:700px){body{padding-bottom:20px}.botnav{position:static;border-top:none;border-bottom:1px solid var(--line);max-width:900px;margin:0 auto;justify-content:center;gap:20px;background:transparent;backdrop-filter:none}.botnav a{flex-direction:row;font-size:13px;gap:7px}.botnav a .ni{font-size:17px}}

  /* === Contenu données brutes — reskin Soleo === */
  .ri { text-align: right; }
  /* fallback CSS (CommunCouleurJS applique les couleurs en inline) */
  .Wh { background-color: #fdd; }
  .A { background-color: #ddf; }
  .W { background-color: #f88; }
  .phi { background-color: #ffd; }
  .V { background-color: #ee8; }
  .VA { background-color: #dfd; }
  .Hz, .Enph { background-color: #eeb; }
  /* tableaux : Soleo card */
  .tableau { background: var(--card); border: 1px solid var(--line) !important; border-radius: var(--radius-s) !important; box-shadow: var(--shadow); overflow: hidden; margin-bottom: 14px; }
  .titre { background: rgba(74,179,244,.22) !important; color: var(--txt) !important; text-align: center; font-weight: 700; }
  td { text-align: left; padding: 6px 8px; color: var(--txt); }
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
  #DataLinky{font-size:13px;tab-size:10;color:var(--txt)}
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
function setTheme(t){document.documentElement.setAttribute("data-theme",t);GID("btnTheme").textContent=(t==="dark")?"🌙":"☀️";try{localStorage.setItem("soleoTheme",t);}catch(e){}}
GID("btnTheme").onclick=function(){setTheme(document.documentElement.getAttribute("data-theme")==="dark"?"light":"dark");};
(function(){var t="dark";try{t=localStorage.getItem("soleoTheme")||"dark";}catch(e){}setTheme(t);})();
</script>
</body>
</html>
)====";
