//*********************************************
// Page Mesures — design Soleo (shell upgrade)
//*********************************************
const char *MainHtml = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="icon" href="/favicon.ico">
  <link rel="stylesheet" href="/commun.css">
  <title>Mesures</title>

  <style>
  /* === Shell Soleo === */
  :root{--bg:#0d1117;--bg-soft:#161b22;--card:#1c2230;--card-2:#232b3b;--line:#2c3444;--txt:#e8edf6;--txt-soft:#9aa6b8;--txt-dim:#6b7585;--sun:#ffb547;--sun-soft:#ffd27d;--home:#4ab3f4;--water:#ff6f91;--grid-imp:#e5604d;--grid-exp:#4cd28b;--accent:#6ee7c7;--shadow:0 10px 30px rgba(0,0,0,.35);--radius:20px;--radius-s:14px}
  html[data-theme=light]{--bg:#eef2f8;--bg-soft:#fff;--card:#fff;--card-2:#f3f6fb;--line:#e0e6f0;--txt:#1a2230;--txt-soft:#586273;--txt-dim:#8a93a4}
  *{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
  body{margin:0;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;background:var(--bg);color:var(--txt);line-height:1.45;transition:background .4s,color .4s;padding-bottom:88px}
  a{text-decoration:none;color:inherit}
  .top-wrap{max-width:1100px;margin:0 auto}
  header.top{position:sticky;top:0;z-index:40;background:color-mix(in srgb,var(--bg) 88%,transparent);backdrop-filter:blur(14px);display:flex;align-items:center;justify-content:space-between;padding:12px 16px;gap:8px}
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
  @media(min-width:700px){body{padding-bottom:20px}.botnav{position:static;border-top:none;border-bottom:1px solid var(--line);max-width:1100px;margin:0 auto;justify-content:center;gap:20px;background:transparent;backdrop-filter:none}.botnav a{flex-direction:row;font-size:13px;gap:7px}.botnav a .ni{font-size:17px}}

  /* === Contenu page Mesures — reskin Soleo === */
    .grid-container2,
    .grid-container2M {
      display: grid;
      grid-template-columns: repeat(6, auto);
      gap: 2px;
      background: var(--card);
      padding: 8px;
      border: 1px solid var(--line);
      border-radius: var(--radius-s);
      box-shadow: var(--shadow);
      text-align: right;
      margin-bottom: 14px;
    }
    .grid-container1 {
      display: grid;
      grid-template-columns: repeat(4, auto);
      gap: 2px;
      background: var(--card);
      padding: 8px;
      border: 1px solid var(--line);
      border-radius: var(--radius-s);
      box-shadow: var(--shadow);
      text-align: right;
      margin-bottom: 14px;
    }
    .grid-container1 > div,
    .grid-container2 > div,
    .grid-container2M > div {
      padding: 8px 10px;
    }
    /* cellules d'en-tête : fond Soleo, texte forcé lisible dans les 2 thèmes */
    .item1 { grid-column: 2 / span 2; background: rgba(74,179,244,.22); color: var(--txt) !important; font-weight: 700; border-radius: 6px; }
    .item2 { grid-column: 4 / span 2; background: rgba(74,179,244,.22); color: var(--txt) !important; font-weight: 700; border-radius: 6px; }
    .item3 { grid-column-start: 6; background: rgba(74,179,244,.22); color: var(--txt) !important; border-radius: 6px; }
    .item31 { grid-column-start: 4; background: rgba(74,179,244,.22); color: var(--txt) !important; border-radius: 6px; }
    .item4 { grid-column-start: 2; background: rgba(74,179,244,.22); color: var(--txt) !important; font-weight: 700; border-radius: 6px; }
    .item5,.item6,.item7,.item14 { background: rgba(74,179,244,.22); color: var(--txt) !important; font-weight: 700; border-radius: 6px; }
    .item8 { grid-column-start: 1; text-align: left; color: var(--txt) !important; }
    .item_Act { grid-column: 1 / span 2; background: rgba(74,179,244,.22); color: var(--txt) !important; font-weight: 700; border-radius: 6px; }
    .item_H { background: rgba(74,179,244,.22); color: var(--txt) !important; font-weight: 700; border-radius: 6px; }
    .item_val { background: rgba(255,255,255,.06); color: var(--txt) !important; border-radius: 4px; }
    .item_F { background: transparent; }
    .item_Force { grid-column: 4 / span 3; background: rgba(74,179,244,.22); color: var(--txt) !important; font-weight: 700; border-radius: 6px; }
    /* températures : vert Soleo */
    .item_temp_nom { grid-column: 1 / span 2; background: rgba(76,210,139,.22); color: var(--grid-exp) !important; border-radius: 6px; }
    .item_temp_val { grid-column: 3 / span 4; background: rgba(76,210,139,.22); color: var(--grid-exp) !important; border-radius: 6px; }
    .le { text-align: left; }
    .ce { text-align: center; }
    /* fallback CSS (CommunCouleurJS applique les couleurs en inline) */
    .Wh { background-color: #ff8; }
    .W { background-color: #f88; }
    .VA { background-color: #0ff; }
    .deg { background-color: #fdf; }
    #foot { font-size: 16px; display: none; }
    svg { border: 1px solid var(--line); border-radius: var(--radius-s); }
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
    .Baccueil { border: 2px solid var(--accent); border-radius: var(--radius-s); }
    .jauge { background-color: var(--sun); height: 28px; position: absolute; top: 4px; left: 4px; text-align: left; overflow: visible; }
    .jaugeBack { background: rgba(74,179,244,.3); width: 208px; height: 36px; position: relative; padding: 4px; border-radius: 6px; }
    .w100 { width: 100%; position: absolute; top: 4px; left: 4px; }
    #TabMesures,#SVG_PW48hT,#SVG_PW2sT,.SVG_Temp48h,#SVG_Ouvertures,#SVG_Ouvertures_2s { display: none; }
    #donneeDistante { font-size: 50%; color: var(--txt-soft); text-align: center; display: none; margin-bottom: 10px; }
    #info { position: absolute; border-left: 1px solid var(--line); display: none; }
    #info_txt { position: absolute; background: var(--card-2); padding: 4px 8px; right: 10px; border: 1px solid var(--line); border-radius: 8px; text-align: right; color: var(--txt); }
    #couleurTarif_jour,#couleurTarif_J1 { font-size: 10px; }
    #etatActions { display: none; }
    .choixG { text-align: left; width: 100%; height: 0; position: relative; }
    .choix { position: absolute; left: 120px; top: -80px; font-size: 16px; display: none; }
    iframe { width: 100%; min-height: 900px; }
    .autreRMS { width: 100%; height: 36px; background: var(--card-2); color: var(--txt-soft); font-size: 24px; display: flex; justify-content: space-between; margin-top: 4px; border: 1px solid var(--line); border-radius: 8px; padding: 0 8px; }
    .autreRif { width: 100%; }
    #B_graph { display: none; justify-content: space-between; position: relative; margin-top: 10px; margin-bottom: -20px; }
    #B_Graph { border: 1px solid var(--accent); border-radius: var(--radius-s); font-size: 16px; padding: 8px; position: absolute; top: -30px; right: 30px; display: none; background: var(--card); color: var(--txt); }
    #B_Graph_in { display: grid; text-align: left; grid-template-columns: auto auto auto auto; }
    #date { display: none; }
    #meteoPrev { display: none; text-align: center; font-size: 14px; padding: 6px 14px; margin: 8px auto; width: fit-content; border: 1px solid rgba(74,179,244,.4); border-radius: 10px; background: rgba(74,179,244,.1); color: var(--txt-soft); }
    @media(max-width:700px){
      .grid-container2,.grid-container2M{grid-template-columns:repeat(3,auto)}
      .grid-container1{grid-template-columns:repeat(2,auto)}
      .item1{grid-column:1/span 2}
      .item2{grid-column:3/span 1}
      .item_Force{grid-column:2/span 2}
    }
  </style>
</head>

<body onload="Init();">

  <div id="LED" title="Réception des données"></div>

  <div class="top-wrap">
    <header class="top">
      <div class="brand"><span class="logo"></span><span id="nom_R">Routeur solaire</span></div>
      <div class="top-actions">
        <span class="badge" id="tarifBadge">—</span>
        <button class="iconbtn" id="btnTheme" title="Thème">🌙</button>
      </div>
    </header>
    <nav class="botnav">
      <a href="/"><span class="ni">⚡</span>Accueil</a>
      <a class="active" href="/mesures"><span class="ni">📊</span>Mesures</a>
      <a href="/Actions"><span class="ni">✨</span>Actions</a>
      <a href="/Para"><span class="ni">🛠️</span>Réglages</a>
    </nav>
  </div>

  <div style="padding:0 8px;max-width:1100px;margin:0 auto">
    <div id="date">DATE</div>
    <div id="meteoPrev"></div>
    <div id="TabMesures"></div>
    <br>
    <div id="donneeDistante">Données distantes</div>
    <div id="etatActions" class="grid-container2"></div>
    <div id="B_graph"><div>&nbsp;</div><div>Graphiques</div><div style="cursor:pointer;"><span onclick="ListeGraph();">📈</span><div id="B_Graph"></div></div></div>
    <div id="LesGraphes"></div>
    <div id="info"><div id="info_txt"></div></div>
    <br><br>
    <div id="foot">Données RMS<div id="source"></div></div>
    <div id="autresRMS"></div>
  </div>

  <!-- Scripts -->
  <script src="/ParaCommunJS"></script>
  <script src="MainJS1"></script>
  <script src="MainJS2"></script>
  <script src="MainJS3"></script>
  <script src="/CommunCouleurJS"></script>
  <script>
  function setTheme(t){document.documentElement.setAttribute("data-theme",t);GID("btnTheme").textContent=(t==="dark")?"🌙":"☀️";try{localStorage.setItem("soleoTheme",t);}catch(e){}}
  GID("btnTheme").onclick=function(){setTheme(document.documentElement.getAttribute("data-theme")==="dark"?"light":"dark");};
  (function(){var t="dark";try{t=localStorage.getItem("soleoTheme")||"dark";}catch(e){}setTheme(t);})();
  </script>
  <br>

</body>
</html>

)====";
