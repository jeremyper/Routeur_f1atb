// Écran de saisie de la clé d'accès — premier écran vu quand une clé est définie.
const char *ParaCleHtml = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="icon" href="/favicon.ico">
  <link rel="stylesheet" href="/commun.css">
  <script src="/theme.js"></script>
  <style>
    body{padding-bottom:20px;display:grid;place-items:center;min-height:100vh}
    .carte{width:100%;max-width:400px;margin:0 auto;padding:28px 24px;background:var(--card);border:1px solid var(--line);border-radius:var(--radius);box-shadow:var(--shadow);text-align:center}
    .cadenas{font-size:40px;line-height:1;margin-bottom:12px}
    h1{font-size:19px;margin:0 0 6px}
    .sous{font-size:13.5px;color:var(--txt-soft);margin:0 0 22px}
    label{display:block;text-align:left;font-size:12px;font-weight:700;color:var(--txt-soft);text-transform:uppercase;letter-spacing:.06em;margin-bottom:6px}
    input[type=password],input[type=text]{width:100%;padding:12px 14px;font-size:16px;background:var(--card-2);color:var(--txt);border:1px solid var(--line);border-radius:var(--rad);outline:none}
    input:focus{border-color:var(--accent)}
    .valider{width:100%;margin:18px 0 0;height:46px;font-size:15px;font-weight:700;border:none;border-radius:var(--rad);cursor:pointer;background:var(--accent);color:#0d1117}
    .valider:active{transform:translateY(1px)}
    .err{margin-top:14px;font-size:13px;color:var(--grid-imp);min-height:18px}
    .lds-dual-ring{margin-top:10px}
  </style>
  <title>Accès protégé</title>
</head>

<body>
  <div class="carte">
    <div class="cadenas">🔒</div>
    <h1>Accès protégé</h1>
    <p class="sous">Ce routeur demande une clé d'accès pour consulter et modifier ses réglages.</p>
    <label for="CleAcces">Clé d'accès</label>
    <input type="password" name="CleAcces" id="CleAcces" autocomplete="current-password" autofocus>
    <button class="valider" onclick="SendCle();">Déverrouiller</button>
    <div class="err" id="err"></div>
    <div class="lds-dual-ring" id="attente"></div>
  </div>

  <script>
    function SendCle() {
      var v = document.getElementById("CleAcces").value.trim();
      document.cookie = "CleAcces=" + encodeURIComponent(v) + ";path=/";
      document.getElementById("err").textContent = "";
      document.getElementById("attente").style.visibility = "visible";
      fetch("/CleUpdate")
        .then(function () { location.reload(); })
        .catch(function () {
          document.getElementById("attente").style.visibility = "hidden";
          document.getElementById("err").textContent = "Routeur injoignable, réessayez.";
        });
    }
    //Validation à la touche Entrée
    document.getElementById("CleAcces").addEventListener("keydown", function (e) {
      if (e.key === "Enter") SendCle();
    });
  </script>
</body>

</html>
)====";

// Feuille de style commune = source unique du shell Soleo.
// Chargée par <link rel="stylesheet" href="/commun.css"> en tête de chaque page ;
// les <style> inline des pages ne portent plus que leurs règles spécifiques.
const char *CommunCSS = R"====(
:root{--bg:#0d1117;--bg-soft:#161b22;--card:#1c2230;--card-2:#232b3b;--line:#2c3444;--txt:#e8edf6;--txt-soft:#9aa6b8;--txt-dim:#6b7585;--sun:#ffb547;--sun-soft:#ffd27d;--home:#4ab3f4;--water:#ff6f91;--grid-imp:#e5604d;--grid-exp:#4cd28b;--accent:#6ee7c7;--shadow:0 10px 30px rgba(0,0,0,.35);--radius:20px;--radius-s:14px;--acc:var(--home);--acc2:var(--accent);--rad:8px}
html[data-theme=light]{--bg:#eef2f8;--bg-soft:#fff;--card:#fff;--card-2:#f3f6fb;--line:#e0e6f0;--txt:#1a2230;--txt-soft:#586273;--txt-dim:#8a93a4;--shadow:0 8px 24px rgba(20,30,50,.10)}
*,*::before,*::after{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
body{margin:0;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,Helvetica,Arial,sans-serif;background:var(--bg);color:var(--txt);line-height:1.45;transition:background .4s,color .4s;padding-bottom:88px}
a{text-decoration:none;color:inherit}
a:link,a:visited{color:var(--acc)}
a:hover{color:var(--acc2)}
.app,.top-wrap{max-width:1100px;margin:0 auto;padding:0 16px}
/* --- Entête --- */
header.top{position:sticky;top:0;z-index:40;background:color-mix(in srgb,var(--bg) 88%,transparent);backdrop-filter:blur(14px);display:flex;align-items:center;justify-content:space-between;padding:12px 16px;gap:8px}
.brand{display:flex;align-items:center;gap:10px;font-weight:800;font-size:18px;min-width:0}
#nom_R{white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.logo{flex-shrink:0;width:30px;height:30px;border-radius:9px;background:radial-gradient(circle at 35% 30%,var(--sun-soft),var(--sun) 65%,#e98a1a);box-shadow:0 0 18px rgba(255,181,71,.45)}
.top-actions{display:flex;gap:8px;align-items:center;flex-shrink:0}
.badge{font-size:11.5px;font-weight:700;padding:4px 10px;border-radius:999px;background:var(--card-2);color:var(--txt-soft);border:1px solid var(--line);white-space:nowrap}
.iconbtn{width:38px;height:38px;border-radius:12px;border:1px solid var(--line);background:var(--card);color:var(--txt);font-size:17px;cursor:pointer;display:grid;place-items:center}
.iconbtn:active{transform:scale(.92)}
/* --- Navigation basse --- */
.botnav{position:fixed;bottom:0;left:0;right:0;z-index:50;background:color-mix(in srgb,var(--bg-soft) 92%,transparent);backdrop-filter:blur(16px);border-top:1px solid var(--line);display:flex;justify-content:space-around;padding:8px 4px env(safe-area-inset-bottom)}
.botnav a{color:var(--txt-dim);display:flex;flex-direction:column;align-items:center;gap:2px;font-size:10.5px;font-weight:600;padding:6px 14px;border-radius:12px}
.botnav a .ni{font-size:20px}
.botnav a.active{color:var(--accent)}
@media(min-width:700px){body{padding-bottom:20px}.botnav{position:static;border-top:none;border-bottom:1px solid var(--line);max-width:1100px;margin:0 auto;justify-content:center;gap:20px;background:transparent;backdrop-filter:none}.botnav a{flex-direction:row;font-size:13px;gap:7px}.botnav a .ni{font-size:17px}}
@keyframes blink{50%{opacity:.3}}
/* --- Éléments hérités, retintés sur la palette Soleo --- */
.onglets{margin-top:4px;display:flex;flex-wrap:wrap;gap:4px;align-items:center}
#onglets2{margin-top:6px;display:none;flex-wrap:wrap;gap:4px;font-size:85%}
.Bonglet{border:1px solid var(--line);background:var(--card);border-radius:var(--rad);padding:5px 14px;display:inline-block;transition:background .15s,border-color .15s}
.Bonglet:hover{background:var(--card-2);border-color:var(--acc)}
.Bonglet2{border:1px solid var(--line);background:var(--card);border-radius:6px;padding:3px 10px;display:inline-block;transition:background .15s}
.Bonglet2:hover{background:var(--card-2)}
.Bheure{display:inline-block}
#pied{display:flex;justify-content:space-between;font-size:13px;color:var(--txt-dim);padding:6px 2px;border-top:1px solid var(--line);margin-top:8px}
.fsize12{font-size:12px;height:16px}
.fsize10{font-size:10px;height:14px}
.fsize8{font-size:8px}
.tableau{background:var(--card);display:inline-block;margin:auto;padding:8px;color:var(--txt);border:1px solid var(--line);border-radius:var(--radius-s);box-shadow:var(--shadow)}
table{border-collapse:collapse}
.lds-dual-ring{color:var(--sun);visibility:hidden;display:inline-block;width:32px;height:32px;vertical-align:middle}
.lds-dual-ring,.lds-dual-ring::after{box-sizing:border-box}
.lds-dual-ring::after{content:" ";display:block;width:24px;height:24px;margin:4px;border-radius:50%;border:4px solid currentColor;border-color:currentColor transparent currentColor transparent;animation:spin 1s linear infinite}
@keyframes spin{to{transform:rotate(360deg)}}
.bouton,input[type=file]::file-selector-button{margin:4px;font-size:15px;height:36px;padding:0 16px;border:1px solid var(--line);border-radius:var(--rad);cursor:pointer;background:var(--card);color:var(--txt);font-weight:600;transition:background .15s,border-color .15s,transform .1s}
.bouton:hover,input[type=file]::file-selector-button:hover{background:var(--card-2);border-color:var(--acc)}
.bouton:active{transform:translateY(1px)}
@media(max-width:600px){.onglets{font-size:90%}.Bonglet{padding:4px 8px}}
)====";

// Bascule de thème partagée par toutes les pages (clé localStorage "soleoTheme").
// Servie par /CommunCouleurJS, donc appliquée sans dupliquer le script dans chaque page.
const char *CommunThemeJS = R"====(
function setTheme(t){
  document.documentElement.setAttribute("data-theme",t);
  var b=document.getElementById("btnTheme");
  if(b)b.textContent=(t==="dark")?"🌙":"☀️";
  try{localStorage.setItem("soleoTheme",t);}catch(e){}
}
(function(){
  var t="dark";try{t=localStorage.getItem("soleoTheme")||"dark";}catch(e){}
  setTheme(t);
  document.addEventListener("DOMContentLoaded",function(){
    var b=document.getElementById("btnTheme");
    if(b)b.onclick=function(){
      setTheme(document.documentElement.getAttribute("data-theme")==="dark"?"light":"dark");
    };
    setTheme(document.documentElement.getAttribute("data-theme"));
  });
})();
)====";

const char *CouleurDefaut = R"====(ffffff77b5fe000033ccccff333333808080000000ffffff808080000000cccccc888888ff444400ffffffff66ffaa88aaffaaaaffeeeeeebbffffff66666633333300ff00aaff0000ffaaaaffaaff883333ffaa6688ffaaff11)====";
