//********************************************************
// Page Import / Export — design Soleo
//********************************************************
const char *ExportHtml = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="icon" href="/favicon.ico">
<title>Import / Export</title>
<link rel="stylesheet" href="/commun.css">
<script src="/theme.js"></script>
<style>
/* Shell Soleo : voir /commun.css — surcharges propres a la page */
.app,header.top,.botnav{max-width:760px}
h2,h3{margin:0;font-weight:700}
section{margin:20px 0}
.card{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);padding:16px;margin:12px 0;box-shadow:var(--shadow)}
.card h3{font-size:15px;margin-bottom:8px}
.card .sub{font-size:12.5px;color:var(--txt-dim);margin-bottom:12px}
.file-row{display:flex;align-items:center;gap:10px;padding:10px 0;border-bottom:1px solid var(--line);flex-wrap:wrap}
.file-row:last-child{border-bottom:none}
.file-name{flex:1;font-size:13.5px;font-weight:600;min-width:0;word-break:break-all}
.file-size{font-size:12px;color:var(--txt-dim);flex-shrink:0}
.btn-sm{border:1px solid var(--line);background:var(--card-2);color:var(--txt);border-radius:8px;padding:6px 12px;font-size:12.5px;font-weight:600;cursor:pointer;flex-shrink:0;white-space:nowrap}
.btn-sm:active{transform:scale(.96)}
.btn-del{border-color:rgba(229,96,77,.45);color:var(--grid-imp)}
.mem-strip{display:flex;gap:20px;font-size:13px;padding:10px 0 2px;border-top:1px solid var(--line);margin-top:8px}
.mem-strip strong{color:var(--accent)}
input[type=file]{width:100%;background:var(--card-2);border:1px solid var(--line);border-radius:10px;color:var(--txt);font-size:14px;padding:9px 12px;outline:none}
.btn-pri{background:var(--accent);color:#08221a;border:none;border-radius:11px;padding:11px 22px;font-size:14.5px;font-weight:800;cursor:pointer;white-space:nowrap}
.btn-pri:active,.btn-ghost:active{transform:scale(.96)}
.btn-ghost{border:1px solid var(--line);background:var(--card-2);color:var(--txt);border-radius:11px;padding:11px 22px;font-size:14.5px;font-weight:700;cursor:pointer}
.lds-dual-ring{display:inline-block;width:22px;height:22px;border:3px solid var(--line);border-top-color:var(--accent);border-radius:50%;animation:spin .8s linear infinite;visibility:hidden}
@keyframes spin{to{transform:rotate(360deg)}}
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
<h2 style="margin-bottom:4px">📦 Import / Export</h2>

<div class="card">
<h3>📤 Fichiers disponibles</h3>
<p class="sub">Sauvegardez vos paramètres et données sur votre PC</p>
<div id="liste_file"></div>
<div id="memoire" class="mem-strip"></div>
</div>

<div class="card">
<h3>📥 Import</h3>
<p class="sub">Importer des paramètres (.json) ou des données mensuelles (.csv)</p>
<form method="POST" onsubmit="submit_para(event);" action="#" enctype="multipart/form-data" id="upload_form">
<div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin-bottom:10px">
<input type="file" name="fichier_para_" id="fichier_para_" accept=".json,.csv" style="flex:1;min-width:180px">
<button type="submit" class="btn-pri">⬆ Importer</button>
</div>
<div class="lds-dual-ring" id="attente"></div>
</form>
<p style="font-size:12px;color:var(--txt-dim);margin:8px 0 0">Après un import de paramètres, faites un Restart pour redémarrer avec les nouveaux réglages.</p>
</div>

<div style="margin-top:8px" id="BoutonsBas">
<button class="btn-ghost" onclick="Reset();" style="border-color:rgba(229,96,77,.4);color:var(--grid-imp)">🔄 ESP32 Restart</button>
</div>
</section>
</main>

<script src="/ParaCommunJS"></script>
<script>
// Bascule de thème : /theme.js (partagé)

function Init(){LoadParaFixe();}
function AdaptationSource(){}
function SetParaFixe(){LoadParaVar();}
function SetParaVar(){LoadListeFile();}

function LoadListeFile(){
  var x=new XMLHttpRequest();
  x.onreadystatechange=function(){
    if(this.readyState==4&&this.status==200){
      var data=this.responseText.split(FS);
      var files=data[0].split(GS);
      var S="";
      var MyIP=int2ip(V.IP_Fixe);
      for(var i=0;i<files.length-1;i++){
        var Fi=files[i].split(RS);
        var F_Long=Fi[0];
        if(F_Long=="parametres.json") F_Long="parametres_V"+F.VersionStocke+"_IP"+MyIP.substring(MyIP.lastIndexOf(".")+1)+".json";
        S+="<div class='file-row'><span class='file-name'>"+F_Long+"</span><span class='file-size'>"+Fi[1]+" o</span>";
        S+="<a href='/export_file?Fichier="+Fi[0]+"&download="+F_Long+"'><button class='btn-sm'>⬇ Télécharger</button></a>";
        S+="<button class='btn-sm btn-del' onclick='valideEfface(\""+Fi[0]+"\");'>✕ Effacer</button></div>";
      }
      GH("liste_file",S||"<p style='color:var(--txt-dim);font-size:13px'>Aucun fichier.</p>");
      var used=parseInt(data[2]),total=parseInt(data[1]);
      GH("memoire","<div>Total : <strong>"+total+"</strong> o</div><div>Utilisé : <strong>"+used+"</strong> o (<strong>"+Math.floor(used*100/total)+"%</strong>)</div>");
    }
  };
  x.open("GET","ListeFile",true);x.send();
}

function valideEfface(Fn){
  if(!confirm("Confirmer l'effacement de "+Fn+" ?")) return;
  var x=new XMLHttpRequest();
  x.onreadystatechange=function(){if(this.readyState==4&&this.status==200)location.reload();};
  x.open("GET","/export_file?Fichier="+Fn+"&Delete=OK",true);x.send();
}

function submit_para(event){
  GID("attente").style.visibility="visible";
  event.preventDefault();
  var x=new XMLHttpRequest();
  x.onreadystatechange=function(){
    if(this.readyState==4&&this.status==200){location.reload();GID("attente").style.visibility="hidden";}
  };
  var file=GID("fichier_para_").files[0];
  var data=new FormData();
  data.append("file",file);
  x.open("POST","/import",true);
  x.send(data);
}
</script>
</body>
</html>
)====";
