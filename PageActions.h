// PageActions.h
//************************************************
// Page HTML de gestion des Actions - refonte Soleo
// Vue cartes (langage naturel) + editeur plein ecran
//************************************************

const char *ActionsHtml = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="icon" href="/favicon.ico">
  <title>Automatismes</title>
  <script src="/PinsActionsJS"></script>
  <script src="/CommunCouleurJS"></script>
  <link rel="stylesheet" href="/commun.css">
  <style>
/* ── Soleo base ─────────────────────────────────────── */
:root{--bg:#0d1117;--bg-soft:#161b22;--card:#1c2230;--card-2:#232b3b;--line:#2c3444;--txt:#e8edf6;--txt-soft:#9aa6b8;--txt-dim:#6b7585;--sun:#ffb547;--sun-soft:#ffd27d;--home:#4ab3f4;--water:#ff6f91;--grid-imp:#e5604d;--grid-exp:#4cd28b;--accent:#6ee7c7;--shadow:0 10px 30px rgba(0,0,0,.35);--radius:20px;--radius-s:14px;--acc:#4ab3f4;--acc2:#7dcfff;}
html[data-theme=light]{--bg:#eef2f8;--bg-soft:#fff;--card:#fff;--card-2:#f3f6fb;--line:#e0e6f0;--txt:#1a2230;--txt-soft:#586273;--txt-dim:#8a93a4;--shadow:0 10px 30px rgba(40,60,90,.1);}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
body{margin:0;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;background:var(--bg);color:var(--txt);line-height:1.45;padding-bottom:100px}
h1,h2,h3{margin:0;font-weight:700;letter-spacing:-.01em}
a{text-decoration:none;color:inherit}
.app{max-width:760px;margin:0 auto;padding:0 16px}
header.top{position:sticky;top:0;z-index:40;background:color-mix(in srgb,var(--bg) 88%,transparent);backdrop-filter:blur(14px);display:flex;align-items:center;justify-content:space-between;padding:12px 16px;max-width:760px;margin:0 auto;gap:8px}
.brand{display:flex;align-items:center;gap:10px;font-weight:800;font-size:18px;min-width:0}
#nom_R{white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.logo{flex-shrink:0;width:30px;height:30px;border-radius:9px;background:radial-gradient(circle at 35% 30%,var(--sun-soft),var(--sun) 65%,#e98a1a);box-shadow:0 0 18px rgba(255,181,71,.45)}
.top-actions{display:flex;gap:8px;align-items:center}
.iconbtn{width:38px;height:38px;border-radius:12px;border:1px solid var(--line);background:var(--card);color:var(--txt);font-size:17px;cursor:pointer;display:grid;place-items:center}
.iconbtn:active{transform:scale(.92)}
.nav{display:flex;gap:6px;padding:0 16px 8px;max-width:760px;margin:0 auto;overflow-x:auto}
.nav a{padding:7px 14px;border-radius:999px;font-size:13.5px;font-weight:600;color:var(--txt-soft);white-space:nowrap;border:1px solid transparent;text-decoration:none}
.nav a.act{background:var(--card);border-color:var(--line);color:var(--txt)}
section{margin:26px 0}
.sec-head{display:flex;align-items:baseline;justify-content:space-between;margin-bottom:12px}
.sec-head h2{font-size:20px}
.sec-head .sub{font-size:13px;color:var(--txt-soft)}
/* action cards */
.act-card{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);padding:16px;margin-bottom:14px;transition:border-color .2s,box-shadow .2s}
.act-card.active-card{border-color:var(--accent);box-shadow:0 0 0 1px var(--accent)}
.act-card-head{display:flex;align-items:center;gap:10px;margin-bottom:10px}
.act-icon{width:40px;height:40px;border-radius:10px;display:grid;place-items:center;font-size:20px;flex-shrink:0}
.act-icon.routing{background:rgba(110,231,199,.15)}
.act-icon.relay{background:rgba(74,179,244,.15)}
.act-icon.inactive{background:rgba(107,117,133,.15)}
.act-name{font-weight:700;font-size:16px;color:var(--txt)}
.act-sub{font-size:12px;color:var(--txt-dim)}
.act-status{margin-left:auto;font-size:12px;font-weight:700;padding:4px 10px;border-radius:999px;white-space:nowrap;flex-shrink:0}
.status-routing{background:rgba(110,231,199,.2);color:var(--accent)}
.status-forced{background:rgba(229,96,77,.2);color:var(--grid-imp)}
.status-idle{background:rgba(107,117,133,.15);color:var(--txt-dim)}
.mini-tl{height:8px;border-radius:4px;overflow:hidden;background:rgba(107,117,133,.2);display:flex;margin:0 0 12px;position:relative}
.tl-seg{height:100%}
.tl-seg.tl-off{background:rgba(107,117,133,.25)}
.tl-seg.tl-route{background:linear-gradient(90deg,#4cd28b,#6ee7c7)}
.tl-seg.tl-on{background:linear-gradient(90deg,#e5604d,#ff6f91)}
.tl-now{position:absolute;top:0;width:2px;height:100%;background:#fff;opacity:.85}
.act-phrase{font-size:13.5px;color:var(--txt-soft);margin-bottom:10px;line-height:1.6}
.act-btns{display:flex;gap:8px;flex-wrap:wrap;align-items:center}
.btn-s{padding:7px 14px;border-radius:10px;border:1px solid var(--line);background:var(--card-2);color:var(--txt-soft);font-size:13px;font-weight:600;cursor:pointer;transition:background .15s,color .15s}
.btn-s:hover{background:var(--card);color:var(--txt)}
.btn-force{border-color:rgba(110,231,199,.4);color:var(--accent)}
.btn-stop{border-color:rgba(229,96,77,.4);color:var(--grid-imp)}
.empty-state{text-align:center;padding:48px 20px;color:var(--txt-dim)}
.empty-state .emo{font-size:48px;margin-bottom:16px}
/* editor section */
.editor-top{display:flex;align-items:center;gap:14px;margin-bottom:20px;padding:12px 0;border-bottom:1px solid var(--line)}
.btn-back{background:var(--card);border:1px solid var(--line);color:var(--txt-soft);padding:8px 16px;border-radius:10px;font-size:14px;font-weight:600;cursor:pointer}
.btn-back:hover{color:var(--txt);border-color:var(--accent)}
/* save bar */
.save-bar{position:fixed;bottom:88px;left:50%;transform:translateX(-50%);z-index:25;background:var(--card-2);border:1px solid var(--accent);border-radius:16px;padding:10px 20px;display:flex;align-items:center;gap:12px;box-shadow:0 4px 24px rgba(110,231,199,.2);white-space:nowrap}
.btn-save{background:var(--accent);color:#0d1117;font-weight:800;font-size:14px;border:none;border-radius:10px;padding:9px 20px;cursor:pointer}
/* FAB */
.fab{position:fixed;bottom:24px;right:20px;z-index:30;padding:14px 22px;border-radius:16px;background:linear-gradient(135deg,var(--accent),#4cd28b);color:#0d1117;font-weight:800;font-size:15px;border:none;cursor:pointer;box-shadow:0 8px 24px rgba(110,231,199,.35)}
.fab:active{transform:scale(.95)}
/* spinner */
.lds-dual-ring{display:inline-block;width:24px;height:24px;vertical-align:middle}
.lds-dual-ring:after{content:" ";display:block;width:18px;height:18px;margin:3px;border-radius:50%;border:2px solid var(--acc,#4ab3f4);border-color:var(--acc,#4ab3f4) transparent;animation:ldsR .8s linear infinite}
@keyframes ldsR{to{transform:rotate(360deg)}}
@media(max-width:480px){.fab{bottom:16px;right:12px;padding:12px 18px;font-size:14px}.save-bar{bottom:76px}}

/* ── Legacy planning CSS (editeur technique) ─────────── */
.Bactions { border: inset 8px azure; }
.cadre { width:100%; max-width:1200px; margin:auto; padding:0 6px; }
.form { width:100%; text-align:left; }
.form2 { margin:auto; padding:5px; display:table; text-align:left; width:100%; }
h4 { padding:6px 4px; margin:8px 0 2px; color:#cce; border-left:4px solid var(--acc,#4ab3f4); padding-left:10px; }
h5 { padding:4px 4px; margin:6px 0 2px; color:#aac; border-left:3px solid #557; padding-left:8px; }
label { text-align:right; }
tr { margin:2px; text-align:left; font-size:20px; }
.planning {
    width:100%; position:relative; margin:4px 0 12px;
    padding:6px; border:1px solid #3a3a5a;
    background:rgba(28,28,44,0.55); color:#222;
    border-radius:10px; box-shadow:0 3px 12px rgba(0,0,0,.4);
}
.planning.active-edit { border-color:#6ee7c7; box-shadow:0 0 0 2px #6ee7c7; }
.titre { display:flex; justify-content:center; cursor:pointer; color:#e8e8ff; font-weight:700; font-size:115%; padding:6px 0; letter-spacing:.5px; }
.titre:hover { color:#7dcfff; }
.visu { position:absolute; top:4px; right:6px; cursor:pointer; font-size:120%; opacity:.7; }
.visu:hover { opacity:1; }
.mode { display:flex; flex-wrap:wrap; gap:4px; font-size:14px; text-align:center; background:rgba(20,20,36,.6); border-radius:8px; margin:4px 0; padding:6px 4px; border:1px solid #333; }
.mode>div { display:flex; align-items:center; gap:4px; padding:3px 8px; border:1px solid #444; border-radius:20px; background:rgba(50,50,70,.5); cursor:pointer; transition:background .15s,border-color .15s; color:#ccc; white-space:nowrap; }
.mode>div:has(input:checked) { background:rgba(74,179,244,.2); border-color:#4ab3f4; color:#fff; }
.mode>div>input[type=radio] { width:14px; cursor:pointer; accent-color:#4ab3f4; }
.TitZone { font-size:11px; font-style:italic; font-weight:700; color:#aaa; white-space:nowrap; }
.les_select { display:flex; flex-wrap:wrap; gap:8px; font-size:14px; text-align:left; margin:4px 0; padding:8px; border:1px solid #333; background:rgba(20,20,36,.5); color:#ccc; border-radius:8px; align-items:flex-end; }
.les_select>div { display:flex; flex-direction:column; gap:2px; }
.les_select input,.les_select select { margin:2px 0; font-size:14px; max-width:150px; }
.boutons { display:inline-flex; gap:4px; }
.tbut { width:36px; height:36px; padding:0; text-align:center; font-weight:700; font-size:22px; cursor:pointer; display:inline-flex; align-items:center; justify-content:center; border:2px solid #555; border-radius:8px; background:linear-gradient(135deg,#3a3a5e,#252540); color:#dde; transition:background .15s,border-color .15s,transform .1s; }
.tbut:hover { background:linear-gradient(135deg,#4a4a7e,#353560); border-color:#4ab3f4; }
.tbut:active { transform:scale(.95); }
.bouton_curseur { display:flex; flex-wrap:wrap; gap:8px; font-size:14px; text-align:center; margin:4px 0; padding:8px; border:1px solid #333; background:rgba(20,20,36,.5); color:#ccc; border-radius:8px; align-items:center; }
.slideTriac { width:100%; position:relative; display:table; }
.slideTriacIn { display:table-row; width:100%; }
.Tcell1 { display:table-cell; width:36%; text-align:right; padding-right:8px; color:#bbb; font-size:13px; vertical-align:middle; }
.Tcell2 { display:table-cell; width:56%; vertical-align:middle; }
.Tcell3 { display:table-cell; width:8%; text-align:left; padding-left:8px; font-weight:700; color:#4ab3f4; vertical-align:middle; font-size:15px; }
input[type=range] { pointer-events:none; accent-color:#4ab3f4; }
input[type=range]::-webkit-slider-thumb { pointer-events:all; cursor:ew-resize; }
input[type=range]::-moz-range-thumb { pointer-events:all; cursor:ew-resize; }
.curseur { position:relative; width:100%; height:34px; }
.periode { position:absolute; height:100%; border:2px solid rgba(255,255,255,.3); border-radius:5px; display:flex; justify-content:space-between; align-items:center; color:#fff; font-size:12px; box-shadow:inset 0 1px 0 rgba(255,255,255,.15); }
.Hfin { position:absolute; bottom:2px; right:3px; font-size:11px; opacity:.85; }
.handleStyle { position:absolute; right:-14px; top:50%; transform:translateY(-50%); width:28px; height:36px; background:rgba(255,255,255,.9); border:2px solid #333; border-radius:7px; cursor:ew-resize; display:flex; align-items:center; justify-content:center; font-size:13px; font-weight:700; color:#333; z-index:10; box-shadow:0 2px 6px rgba(0,0,0,.4); touch-action:none; user-select:none; transition:background .1s; }
.handleStyle:hover { background:#ffe680; border-color:#f57c00; }
.infoAction { position:relative; display:flex; width:100%; min-height:44px; font-size:13px; flex-wrap:wrap; }
.infoZone { position:relative; display:flex; flex-direction:column; justify-content:center; align-items:center; border:1px solid #999; border-radius:5px; text-align:center; background:rgba(220,220,220,.88); cursor:pointer; padding:2px 4px; transition:filter .15s; font-size:12px; }
.infoZone:hover { filter:brightness(1.1); }
.infoZone div { display:inline-block; }
.infoZ { position:fixed; top:50%; left:50%; transform:translate(-50%,-50%); display:none; border:2px solid #4a4a6a; border-radius:12px; background:#1e2030; color:#e0e0f0; z-index:2000; font-size:15px; min-width:280px; max-width:420px; width:90vw; box-shadow:0 8px 32px rgba(0,0,0,.7); overflow:hidden; }
.infoZ input { width:72px; margin:3px; font-size:14px; border-radius:5px; border:1px solid #555; background:#252538; color:#dde; padding:2px 4px; }
.infoZ select { font-size:14px; border-radius:5px; border:1px solid #555; background:#252538; color:#dde; padding:2px; }
.selectZ { background:linear-gradient(135deg,#3a3a5e,#252550); padding:8px 10px; font-size:15px; font-weight:700; color:#cce; position:relative; border-bottom:1px solid #4a4a6a; display:flex; align-items:center; justify-content:space-between; }
.closeZ { background:rgba(255,60,60,.7); border:none; color:#fff; cursor:pointer; height:24px; width:24px; border-radius:50%; font-size:14px; font-weight:700; display:flex; align-items:center; justify-content:center; flex-shrink:0; transition:background .15s; }
.closeZ:hover { background:rgba(255,60,60,1); }
.fcontainer { display:flex; flex-wrap:wrap; gap:6px; background:rgba(20,20,36,.6); padding:8px; border-top:1px solid #333; }
.fcontleft { display:flex; flex-direction:column; gap:6px; }
.radioC { border:2px solid #444; border-radius:8px; display:inline-flex; align-items:center; gap:6px; padding:5px 10px; text-align:left; background:rgba(50,50,70,.5); transition:border-color .15s,background .15s; cursor:pointer; }
.radioC:has(input:checked) { border-color:#4ab3f4; background:rgba(74,179,244,.15); }
.radioC input[type=radio] { width:16px; cursor:pointer; accent-color:#4ab3f4; }
.zOff { background:rgba(80,80,200,.25); border:1px solid #66f; margin:4px; padding:6px 8px; border-radius:8px; }
.zOn { background:rgba(200,60,60,.25); border:1px solid #f77; margin:4px; padding:6px 8px; border-radius:8px; }
.zPw { background:rgba(60,180,60,.25); border:1px solid #6f6; margin:4px; padding:6px 8px; border-radius:8px; }
.zTriac { background:rgba(200,180,40,.25); border:1px solid #dd6; margin:4px; padding:6px 8px; border-radius:8px; }
.bord1px { border:1px solid #445; margin:5px 4px; padding:6px; border-radius:6px; background:rgba(30,30,50,.5); }
.minmax { display:flex; flex-wrap:wrap; gap:6px; align-items:center; padding:4px 0; }
.minmax>div { margin:0; }
.tm { width:62px; text-align:left; }
.ligne { display:table-row; padding:10px; }
.graphAction { position:relative; font-size:14px; display:none; margin:4px; }
.graphSVG { width:100%; height:100%; }
.GraphSVG { position:absolute; bottom:8px; right:70px; cursor:pointer; border:2px solid #666; background:rgba(255,255,255,.9); width:28px; height:28px; text-align:center; border-radius:5px; display:flex; align-items:center; justify-content:center; }
svg { border:1px solid #445; border-radius:6px; width:100%; }
#assistant { position:fixed; inset:0; z-index:3000; background:rgba(0,0,0,.6); backdrop-filter:blur(3px); display:none; align-items:center; justify-content:center; }
#assistantBox { background:#1e2030; border:2px solid #4a4a6a; border-radius:12px; color:#e0e0f0; width:92vw; max-width:480px; max-height:90vh; overflow-y:auto; box-shadow:0 8px 32px rgba(0,0,0,.7); font-size:15px; }
.wizBody { padding:4px; }
.wizLigne { padding:4px 2px; display:flex; align-items:center; gap:6px; flex-wrap:wrap; }
.wizLigne input[type=time],.wizLigne input[type=number],.wizLigne input[type=text],.wizLigne select { font-size:14px; border-radius:5px; border:1px solid #555; background:#252538; color:#dde; padding:3px 4px; }
.wizLigne input[type=checkbox] { width:17px; height:17px; accent-color:#4ab3f4; }
.tbutWide { width:auto; padding:0 14px; font-size:15px; margin-left:8px; }
#message { position:fixed; border:2px solid #555; border-radius:8px; top:8px; right:8px; background:rgba(20,20,40,.95); backdrop-filter:blur(4px); color:#dde; font-size:15px; display:none; text-align:left; padding:8px 12px; box-shadow:0 4px 16px rgba(0,0,0,.5); max-width:300px; }
@media(max-width:600px){
    .mode>div { padding:3px 6px; font-size:13px; }
    .les_select { gap:6px; }
    .infoZ { font-size:14px; }
    .slideTriac { display:block; }
    .slideTriacIn { display:flex; flex-direction:column; gap:2px; margin-bottom:4px; }
    .Tcell1,.Tcell2,.Tcell3 { display:block; width:100%; text-align:left; padding:0; }
    .Tcell2 input[type=range] { width:100% !important; }
}
  </style>
</head>
<body onload="Init();" onmouseup="mouseClick=false;">
  <header class="top">
    <div class="brand"><div class="logo"></div><span id="nom_R">Soleo</span></div>
    <div class="top-actions">
      <button class="iconbtn" onclick="toggleTheme()" title="Theme">&#9728;</button>
    </div>
  </header>
  <nav class="nav">
    <a href="/">Tableau</a>
    <a href="/mesures">Mesures</a>
    <a href="/Actions" class="act">Automatismes</a>
    <a href="/para">Reglages</a>
    <a href="/accueil">Accueil</a>
  </nav>
  <div class="app">
    <!-- CARD VIEW (default visible) -->
    <section id="cards-section">
      <div class="sec-head"><h2>Automatismes</h2><span class="sub" id="act-count"></span></div>
      <div id="action-cards"></div>
    </section>
    <!-- EDITOR VIEW (hidden by default) -->
    <section id="editor-section" style="display:none">
      <div class="editor-top">
        <button class="btn-back" onclick="FermerDetail()">&#8592; Retour</button>
        <h2>Modifier les automatismes</h2>
      </div>
      <div id="plannings"></div>
      <div id="BoutonsBas">
        <input class="bouton" type="button" onclick="Send_Values();" value="Sauvegarder">
        <div class="lds-dual-ring" id="attente"></div>
        <input class="bouton" type="button" onclick="Reset();" value="ESP32 Reset">
      </div>
    </section>
  </div>
  <!-- Floating save bar (after wizard creates action) -->
  <div class="save-bar" id="save-bar" style="display:none">
    <span>V&eacute;rifiez puis sauvegardez</span>
    <button class="btn-save" onclick="OuvrirDetail(F.Actions.length-1)">Voir &amp; Sauvegarder</button>
  </div>
  <!-- FAB -->
  <button class="fab" id="fab" onclick="OuvrirAssistant()">+ Automatisme</button>
  <!-- compat hidden divs -->
  <div id="lesOnglets" style="display:none"></div>
  <div id="pied" style="display:none"></div>
  <!-- existing wizard + message -->
  <div id="assistant"><div id="assistantBox"></div></div>
  <div id="message"></div>
  <!-- scripts -->
  <script src="/ParaCommunJS"></script>
  <script src="/ActionsJS1"></script>
  <script src="/ActionsJS2"></script>
  <script src="/ActionsJS3"></script>
  <script src="/ActionsJS4"></script>
  <script src="/ActionsJS5"></script>
</body>
</html>
)====";
