//***************************************************
// Page OTA mise à jour firmware — design Soleo
//***************************************************
const char *OtaHtml = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="icon" href="/favicon.ico">
<title>Mise à jour</title>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
<style>
:root{--bg:#0d1117;--bg-soft:#161b22;--card:#1c2230;--card-2:#232b3b;--line:#2c3444;--txt:#e8edf6;--txt-soft:#9aa6b8;--txt-dim:#6b7585;--sun:#ffb547;--sun-soft:#ffd27d;--home:#4ab3f4;--water:#ff6f91;--grid-imp:#e5604d;--grid-exp:#4cd28b;--accent:#6ee7c7;--shadow:0 10px 30px rgba(0,0,0,.35);--radius:20px;--radius-s:14px}
html[data-theme=light]{--bg:#eef2f8;--bg-soft:#fff;--card:#fff;--card-2:#f3f6fb;--line:#e0e6f0;--txt:#1a2230;--txt-soft:#586273;--txt-dim:#8a93a4;--shadow:0 10px 30px rgba(40,60,90,.1)}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
body{margin:0;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;background:var(--bg);color:var(--txt);line-height:1.45;transition:background .4s,color .4s;padding-bottom:88px}
h2,h3{margin:0;font-weight:700}
a{color:var(--accent);text-decoration:none}
a:hover{text-decoration:underline}
.app{max-width:760px;margin:0 auto;padding:0 16px}
header.top{position:sticky;top:0;z-index:40;background:color-mix(in srgb,var(--bg) 88%,transparent);backdrop-filter:blur(14px);display:flex;align-items:center;justify-content:space-between;padding:12px 16px;max-width:760px;margin:0 auto;gap:8px}
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
section{margin:20px 0}
.card{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);padding:16px;margin:12px 0;box-shadow:var(--shadow)}
.card h3{font-size:15px;margin-bottom:10px}
.ver-hero{display:flex;align-items:center;gap:14px}
.ver-hero .ico{font-size:30px;flex-shrink:0}
.ver-hero .lbl{font-size:13px;color:var(--txt-soft)}
.ver-hero .val{font-size:22px;font-weight:800;color:var(--accent)}
ol{padding-left:20px;margin:0;display:flex;flex-direction:column;gap:8px}
ol li{font-size:14px;line-height:1.5}
input[type=file]{width:100%;background:var(--card-2);border:1px solid var(--line);border-radius:10px;color:var(--txt);font-size:14px;padding:9px 12px;outline:none;margin-bottom:10px}
.btn-pri{background:var(--accent);color:#08221a;border:none;border-radius:11px;padding:11px 22px;font-size:14.5px;font-weight:800;cursor:pointer;white-space:nowrap}
.btn-pri:active{transform:scale(.96)}
.pbar-wrap{height:10px;border-radius:999px;background:var(--card-2);overflow:hidden;margin-top:10px}
.pbar{display:block;height:100%;border-radius:999px;background:linear-gradient(90deg,var(--accent),var(--grid-exp));width:0%;transition:width .3s}
iframe{width:100%;height:160px;border:1px solid var(--line);border-radius:10px;background:var(--card-2)}
@media(min-width:700px){body{padding-bottom:20px}.botnav{position:static;border-top:none;border-bottom:1px solid var(--line);max-width:760px;margin:0 auto;justify-content:center;gap:20px;background:transparent;backdrop-filter:none}.botnav a{flex-direction:row;font-size:13px;gap:7px}.botnav a .ni{font-size:17px}}
</style>
</head>
<body onload="Init();">
<header class="top">
<div class="brand"><span class="logo"></span><span id="nom_R">Routeur solaire</span></div>
<div class="top-actions"><span class="badge" id="tarifBadge">—</span><button class="iconbtn" id="btnTheme" title="Thème">🌙</button></div>
</header>
<nav class="botnav">
<a href="/"><span class="ni">⚡</span>Accueil</a>
<a href="/mesures"><span class="ni">📊</span>Mesures</a>
<a href="/Actions"><span class="ni">✨</span>Actions</a>
<a class="active" href="/Para"><span class="ni">🛠️</span>Réglages</a>
</nav>
<main class="app">
<section>
<h2 style="margin-bottom:4px">🔄 Mise à jour firmware (OTA)</h2>

<div class="card">
<div class="ver-hero">
<span class="ico">📦</span>
<div><div class="lbl">Version actuelle du routeur</div><div class="val" id="Version_actu">—</div></div>
</div>
</div>

<div class="card">
<h3>🌐 Versions disponibles</h3>
<iframe src="https://f1atb.fr/web_tool/scan_dir_bin.php"></iframe>
</div>

<div class="card">
<h3>📋 Procédure</h3>
<ol>
<li><a href="/Export">Sauvegardez vos paramètres</a> sur le PC si la partie entière de la version change.</li>
<li>Téléchargez le binaire souhaité (<code style="color:var(--accent)">Solar_Router_Vxx.xx.ino.bin</code>) en cliquant dessus.</li>
<li>Cliquez sur "Choisir un fichier" et sélectionnez ce binaire.</li>
<li>Cliquez sur "Mettre à jour".</li>
<li><a href="/Export">Importez si besoin vos anciens paramètres</a> depuis le PC.</li>
</ol>
<form method="POST" action="#" enctype="multipart/form-data" id="upload_form" style="margin-top:16px">
<input type="file" name="update" id="fichier" accept=".bin">
<div style="display:flex;gap:10px;align-items:center">
<button type="submit" class="btn-pri">⬆ Mettre à jour</button>
<span id="prg" style="font-size:13.5px;color:var(--txt-soft)">0%</span>
</div>
<div class="pbar-wrap"><span class="pbar" id="pgbar"></span></div>
</form>
</div>

<p style="font-size:12px;color:var(--txt-dim);text-align:center;margin-top:8px">Après mise à jour : Restart ESP32 + Ctrl+F5 (vider le cache navigateur).</p>
</section>
</main>

<script src="/ParaCommunJS"></script>
<script>
function setTheme(t){document.documentElement.setAttribute("data-theme",t);GID("btnTheme").textContent=(t==="dark")?"🌙":"☀️";try{localStorage.setItem("soleoTheme",t);}catch(e){}}
GID("btnTheme").onclick=function(){setTheme(document.documentElement.getAttribute("data-theme")==="dark"?"light":"dark");};
(function(){var t="dark";try{t=localStorage.getItem("soleoTheme")||"dark";}catch(e){}setTheme(t);})();

$("form").submit(function(e){
  e.preventDefault();
  $.ajax({
    url:"/update",type:"POST",data:new FormData($("#upload_form")[0]),contentType:false,processData:false,
    xhr:function(){
      var xhr=new window.XMLHttpRequest();
      xhr.upload.addEventListener("progress",function(evt){
        if(evt.lengthComputable){
          var pct=Math.round(evt.loaded/evt.total*100);
          $("#prg").html(pct+"%");
          $("#pgbar").css("width",pct+"%");
        }
      },false);
      return xhr;
    },
    success:function(){console.log("OTA OK");},error:function(){}
  });
});

function Init(){LoadParaFixe();}
function AdaptationSource(){}
function SetParaVar(){}
function SetParaFixe(){GH("Version_actu",F.VersionStocke);}
</script>
</body>
</html>
)====";
