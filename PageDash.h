//*****************************************************
// Page d'accueil "Soleo" - tableau de bord temps réel
// Flux d'énergie SVG, économies en €, tuiles, journal
//*****************************************************
const char *DashHtml = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="icon" href="/favicon.ico">
<title>Accueil</title>
<link rel="stylesheet" href="/commun.css">
<script src="/theme.js"></script>
<style>
/* Shell Soleo : voir /commun.css — surcharges propres a la page */
.app,header.top,.botnav{max-width:760px}
h1,h2{margin:0;font-weight:700;letter-spacing:-.01em}
.online{display:flex;align-items:center;gap:5px;font-size:11.5px;font-weight:700;color:var(--grid-exp)}
.online.off{color:var(--txt-dim)}
.odot{width:9px;height:9px;border-radius:50%;background:currentColor;box-shadow:0 0 8px currentColor;animation:blink 2.4s infinite}
section{margin:26px 0}
.sec-head{display:flex;align-items:baseline;justify-content:space-between;margin-bottom:12px}
.sec-head h2{font-size:20px}
.sec-head .sub{font-size:13px;color:var(--txt-soft)}
.more{font-size:13.5px;font-weight:700;color:var(--accent)}
.hero{background:linear-gradient(155deg,var(--card),var(--card-2));border:1px solid var(--line);border-radius:var(--radius);padding:16px 18px;box-shadow:var(--shadow);display:flex;align-items:center;gap:14px;margin-top:10px}
.pulse{flex-shrink:0;width:48px;height:48px;border-radius:50%;background:radial-gradient(circle,#7c8aa3,#4a5568);display:grid;place-items:center;font-size:24px;animation:breathe 3.5s ease-in-out infinite}
@keyframes breathe{0%,100%{box-shadow:0 0 0 0 rgba(76,210,139,.45)}50%{box-shadow:0 0 0 12px rgba(76,210,139,0)}}
.hero h1{font-size:18px;line-height:1.3}
.hero p{margin:3px 0 0;font-size:13.5px;color:var(--txt-soft)}
.flow-wrap{position:relative;display:grid;place-items:center;padding:4px 0 0}
.flow-svg{width:100%;max-width:340px;height:auto}
.flow-center{position:absolute;text-align:center;pointer-events:none}
.flow-center .big{font-size:30px;font-weight:800;line-height:1}
.flow-center .unit{font-size:13px;color:var(--txt-soft)}
.flow-center .euros{margin-top:5px;font-size:13.5px;color:var(--accent);font-weight:700}
.flow-dot{animation:dash 1.1s linear infinite;transition:opacity .6s}
@keyframes dash{to{stroke-dashoffset:-22}}
.flow-legend{display:grid;grid-template-columns:repeat(3,1fr);gap:10px;margin-top:12px}
.leg{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);padding:10px;text-align:center}
.leg .lbl{font-size:12px;color:var(--txt-soft);display:flex;align-items:center;justify-content:center;gap:6px}
.leg .val{font-size:17px;font-weight:800;margin-top:2px}
.swatch{width:10px;height:10px;border-radius:3px;display:inline-block}
.tiles{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.tile{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);padding:14px;display:flex;flex-direction:column}
.tile .ttl{display:flex;align-items:center;gap:7px;font-weight:700;font-size:14.5px}
.tile .emo{font-size:19px}
.tile .reading{font-size:24px;font-weight:800;margin:7px 0 2px}
.tile .state{font-size:12.5px;color:var(--txt-soft)}
.chip-status{display:inline-flex;align-items:center;gap:6px;font-size:11.5px;font-weight:700;padding:3px 9px;border-radius:999px;margin-top:8px;align-self:flex-start}
.chip-status.on{background:rgba(76,210,139,.16);color:var(--grid-exp)}
.chip-status.idle{background:rgba(154,166,184,.16);color:var(--txt-soft)}
.chip-status.force{background:rgba(255,181,71,.18);color:var(--sun)}
.chip-status.off{background:rgba(229,96,77,.16);color:var(--grid-imp)}
.dotpulse{width:8px;height:8px;border-radius:50%;background:currentColor;animation:blink 1.4s infinite}
.bar{height:8px;border-radius:999px;background:var(--card-2);overflow:hidden;margin-top:9px}
.bar>span{display:block;height:100%;border-radius:999px;background:linear-gradient(90deg,var(--water),var(--sun));transition:width 1s ease}
.btnrow{display:flex;gap:8px;margin-top:10px}
.mini{flex:1;border:1px solid var(--line);background:var(--card-2);color:var(--txt);border-radius:10px;padding:7px 0;font-size:12.5px;font-weight:700;cursor:pointer}
.mini:active{transform:scale(.96)}
.autocard{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);padding:12px 14px;margin-bottom:10px;display:flex;align-items:center;gap:12px}
.autocard .nm{font-weight:700;font-size:14.5px;flex:1;min-width:0;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.autocard .chip-status{margin-top:0;flex-shrink:0}
.feed{list-style:none;margin:0;padding:0}
.feed li{display:flex;gap:12px;padding:11px 4px;border-bottom:1px solid var(--line)}
.feed li:last-child{border-bottom:none}
.feed .ic{flex-shrink:0;width:34px;height:34px;border-radius:11px;display:grid;place-items:center;font-size:17px;background:var(--card-2)}
.feed .txt{font-size:13.5px}
.feed .when{font-size:11.5px;color:var(--txt-dim)}
@media(min-width:700px){
body{padding-bottom:20px}
.tiles{grid-template-columns:repeat(4,1fr)}
.botnav{position:static;border-top:none;border-bottom:1px solid var(--line);max-width:760px;margin:0 auto;justify-content:center;gap:20px;background:transparent;backdrop-filter:none}
.botnav a{flex-direction:row;font-size:13px;gap:7px}
.botnav a .ni{font-size:17px}
}
</style>
</head>
<body>
<header class="top">
<div class="brand"><span class="logo"></span><span id="nom_R">Routeur solaire</span></div>
<div class="top-actions"><span class="badge" id="tarifBadge">—</span><span class="online off" id="online"><span class="odot"></span><span id="onlineTxt">…</span></span><button class="iconbtn" id="btnAbsence" title="Mode absence" onclick="ToggleAbsence();">🏨</button><button class="iconbtn" id="btnTheme" title="Thème">🌙</button></div>
</header>
<nav class="botnav">
<a class="active" href="/"><span class="ni">⚡</span>Accueil</a>
<a href="/mesures"><span class="ni">📊</span>Mesures</a>
<a href="/Actions"><span class="ni">✨</span>Actions</a>
<a href="/Para"><span class="ni">🛠️</span>Réglages</a>
</nav>
<div id="absenceBanner" onclick="ToggleAbsence();" style="display:none;cursor:pointer;max-width:760px;margin:8px auto 0;padding:11px 16px;border-radius:14px;background:rgba(255,181,71,.14);border:1px solid var(--sun);color:var(--sun-soft);font-weight:700;font-size:13.5px;text-align:center">🏨 Mode absence actif — actions coupées · appuyez pour désactiver</div>
<div id="delestageBanner" style="display:none;max-width:760px;margin:8px auto 0;padding:11px 16px;border-radius:14px;background:rgba(229,96,77,.14);border:1px solid var(--grid-imp);color:var(--grid-imp);font-weight:700;font-size:13.5px;text-align:center"></div>
<main class="app">
<section>
<div class="sec-head"><h2>En direct</h2><span class="sub" id="liveClock">—</span></div>
<div class="flow-wrap">
<svg class="flow-svg" viewBox="0 0 320 320" aria-label="Flux d'énergie">
<circle cx="160" cy="160" r="120" fill="none" stroke="var(--line)" stroke-width="14"/>
<circle id="arcSun" cx="160" cy="160" r="120" fill="none" stroke="var(--sun)" stroke-width="14" stroke-linecap="round" transform="rotate(-90 160 160)" stroke-dasharray="754" stroke-dashoffset="754" style="transition:stroke-dashoffset 1s"/>
<path d="M160 60 L72 232" stroke="var(--home)" stroke-width="3" fill="none" opacity=".3"/>
<line id="fHome" class="flow-dot" x1="160" y1="60" x2="72" y2="232" stroke="var(--sun-soft)" stroke-width="3.5" stroke-dasharray="2 9" stroke-linecap="round" opacity="0"/>
<path d="M160 60 L248 232" stroke="var(--water)" stroke-width="3" fill="none" opacity=".3"/>
<line id="fWater" class="flow-dot" x1="160" y1="60" x2="248" y2="232" stroke="var(--water)" stroke-width="3.5" stroke-dasharray="2 9" stroke-linecap="round" opacity="0"/>
<g transform="translate(160 36)"><circle r="22" fill="var(--sun)"/><text y="7" text-anchor="middle" font-size="22">☀️</text></g>
<g transform="translate(58 250)"><circle r="22" fill="var(--home)"/><text y="7" text-anchor="middle" font-size="20">🏠</text></g>
<g transform="translate(262 250)"><circle r="22" fill="var(--water)"/><text y="7" text-anchor="middle" font-size="20">🚿</text></g>
</svg>
<div class="flow-center"><div class="big" id="flowKw">—</div><div class="unit" id="flowUnit">kW produits</div><div class="euros" id="flowEur"></div></div>
</div>
<div class="hero"><div class="pulse" id="heroIcon">⏳</div><div><h1 id="heroTitle">Connexion…</h1><p id="heroMsg">Recherche des données du routeur.</p></div></div>
<div class="flow-legend">
<div class="leg"><div class="lbl"><span class="swatch" style="background:var(--home)"></span>Maison</div><div class="val" id="legHome">—</div></div>
<div class="leg"><div class="lbl"><span class="swatch" style="background:var(--water)"></span>Eau chaude</div><div class="val" id="legWater">—</div></div>
<div class="leg"><div class="lbl"><span class="swatch" style="background:var(--grid-exp)"></span>Réseau</div><div class="val" id="legGrid">—</div></div>
</div>
</section>
<section>
<div class="sec-head"><h2>Aujourd'hui</h2><a class="more" href="/mesures">Mesures détaillées →</a></div>
<div class="tiles">
<div class="tile"><div class="ttl"><span class="emo">🌤️</span>Production</div><div class="reading" id="tPv">—</div><div class="state" id="tPvSub">en attente</div><span class="chip-status on" id="tPvChip" style="display:none"></span></div>
<div class="tile"><div class="ttl"><span class="emo">🏠</span>Maison</div><div class="reading" id="tMaison">—</div><div class="state" id="tMaisonSub">—</div><span class="chip-status on" id="tMaisonChip" style="display:none"><span class="dotpulse"></span>100% solaire</span></div>
<div class="tile"><div class="ttl"><span class="emo">🚿</span>Eau chaude</div><div class="reading" id="tBallon">—</div><div class="state" id="tBallonSub">—</div><div class="bar"><span id="tBallonBar" style="width:0%"></span></div><div class="btnrow"><button class="mini" onclick="Forcer(1)">Forcer</button><button class="mini" onclick="Forcer(-1)">Arrêter</button></div></div>
<div class="tile"><div class="ttl"><span class="emo">💶</span>Ce mois</div><div class="reading" id="tMois">—</div><div class="state" id="tMoisSub">—</div></div>
</div>
</section>
<section>
<div class="sec-head"><h2>Automatismes</h2><a class="more" href="/Actions">Gérer →</a></div>
<div id="autoList"><div class="autocard"><span class="nm" style="color:var(--txt-soft)">Chargement…</span></div></div>
</section>
<section>
<div class="sec-head"><h2>Journal</h2><span class="sub">ce qui se passe chez vous</span></div>
<ul class="feed" id="feed"></ul>
</section>
</main>
<script src="/ParaCommunJS"></script>
<script src="/DashJS"></script>
</body>
</html>
)====";
