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
<link rel="stylesheet" href="/commun.css">
<script src="/theme.js"></script>
<style>
/* Shell Soleo : voir /commun.css — surcharges propres a la page */
.app,header.top,.botnav{max-width:760px}
h2,h3{margin:0;font-weight:700}
a{color:var(--accent);text-decoration:none}
a:hover{text-decoration:underline}
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
.btn-pri{display:inline-block;background:var(--accent);color:#08221a;border:none;border-radius:11px;padding:11px 22px;font-size:14.5px;font-weight:800;cursor:pointer;white-space:nowrap;text-decoration:none}
.btn-pri:active{transform:scale(.96)}
.pbar-wrap{height:10px;border-radius:999px;background:var(--card-2);overflow:hidden;margin-top:10px}
.pbar{display:block;height:100%;border-radius:999px;background:linear-gradient(90deg,var(--accent),var(--grid-exp));width:0%;transition:width .3s}
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
<p style="font-size:14px;margin:0 0 12px">Les binaires de ce firmware sont publiés sur GitHub.</p>
<a class="btn-pri" href="https://github.com/jeremyper/Routeur_f1atb/releases/latest" target="_blank" rel="noopener">⬇ Télécharger la dernière version</a>
<p style="font-size:12.5px;color:var(--txt-dim);margin:12px 0 0">
Ce routeur utilise un fork du firmware F1ATB : n'y envoyez pas un binaire officiel
<code>Solar_Router_Vxx.xx.ino.bin</code> pris sur f1atb.fr, les fonctions et le découpage
des partitions diffèrent. Pour revenir au firmware d'origine, passez par l'USB.
</p>
</div>

<div class="card">
<h3>📋 Procédure</h3>
<ol>
<li><a href="/Export">Sauvegardez vos paramètres</a> sur le PC avant toute mise à jour.</li>
<li>Téléchargez le binaire <code style="color:var(--accent)">.bin</code> de la dernière version depuis la page GitHub ci-dessus (ou compilez-le vous-même).</li>
<li>Cliquez sur "Choisir un fichier" et sélectionnez ce binaire.</li>
<li>Cliquez sur "Mettre à jour", puis patientez ~20 s et rechargez avec Ctrl+F5.</li>
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
// Bascule de thème : /theme.js (partagé)

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
    success:function(rep){
      //Le serveur répond "OK" ou "FAIL" selon le résultat de Update.end()
      if((""+rep).indexOf("FAIL")>=0){
        GH("prg","&#10007; Échec de l'écriture — le routeur redémarre sur l'ancienne version");
      }else{
        GH("prg","&#10003; Mise à jour reçue — redémarrage en cours, patientez ~20 s puis rechargez");
      }
    },
    error:function(xhr){
      //Sans ce retour, un refus passait inaperçu : la barre atteignait 100 % en silence
      var m="&#10007; Échec de l'envoi";
      if(xhr&&xhr.status==401) m="&#10007; Clé d'accès refusée — rouvrez la page et ressaisissez-la";
      else if(xhr&&xhr.status) m+=" (erreur HTTP "+xhr.status+")";
      GH("prg",m);
      GID("pgbar").style.width="0%";
    }
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
