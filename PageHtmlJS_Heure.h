//********************************************************
// Page Horloge ESP32 — design Soleo
//********************************************************
const char *HeureHtml = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="icon" href="/favicon.ico">
<title>Horloge</title>
<link rel="stylesheet" href="/commun.css">
<script src="/theme.js"></script>
<style>
/* Shell Soleo : voir /commun.css — surcharges propres a la page */
.app,header.top,.botnav{max-width:760px}
h2{margin:0;font-weight:700}
section{margin:20px 0}
.card{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);padding:16px;margin:12px 0;box-shadow:var(--shadow)}
.row{padding:11px 0;border-bottom:1px solid var(--line);display:flex;flex-direction:column;gap:6px}
.row:last-child{border-bottom:none}
label{font-size:14px;font-weight:600}
.hint{display:block;font-size:12px;font-weight:400;color:var(--txt-dim);margin-top:2px}
input[type=text],select{width:100%;background:var(--card-2);border:1px solid var(--line);border-radius:10px;color:var(--txt);font-size:16px;padding:10px 12px;outline:none}
input:focus,select:focus{border-color:var(--accent)}
.rad-group{display:flex;flex-wrap:wrap;gap:8px;margin-top:10px}
.rad-opt{display:flex;align-items:center;gap:8px;padding:9px 14px;border-radius:10px;background:var(--card-2);border:1px solid var(--line);cursor:pointer;font-size:13.5px;font-weight:600;transition:background .15s,border-color .15s}
.rad-opt input[type=radio]{accent-color:var(--accent)}
.btnrow{display:flex;gap:10px;margin-top:16px;flex-wrap:wrap;align-items:center}
.btn-pri{background:var(--accent);color:#08221a;border:none;border-radius:11px;padding:11px 22px;font-size:14.5px;font-weight:800;cursor:pointer}
.btn-pri:active,.btn-ghost:active{transform:scale(.96)}
.btn-ghost{border:1px solid var(--line);background:var(--card-2);color:var(--txt);border-radius:11px;padding:11px 22px;font-size:14.5px;font-weight:700;cursor:pointer}
.spin{display:inline-block;width:22px;height:22px;border:3px solid var(--line);border-top-color:var(--accent);border-radius:50%;animation:spin .8s linear infinite;visibility:hidden}
.lds-dual-ring{display:inline-block;width:22px;height:22px;border:3px solid var(--line);border-top-color:var(--accent);border-radius:50%;animation:spin .8s linear infinite;visibility:hidden}
@keyframes spin{to{transform:rotate(360deg)}}
@media(min-width:700px){body{padding-bottom:20px}.botnav{position:static;border-top:none;border-bottom:1px solid var(--line);max-width:760px;margin:0 auto;justify-content:center;gap:20px;background:transparent;backdrop-filter:none}.botnav a{flex-direction:row;font-size:13px;gap:7px}.botnav a .ni{font-size:17px}.row{flex-direction:row;align-items:center;justify-content:space-between}.row>label{flex:1;padding-right:14px}.row>input,.row>select{width:46%}}
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
<h2>🕐 Horloge — <span id="date" style="font-size:15px;font-weight:500;color:var(--txt-soft)">—</span></h2>

<div class="card">
<label>Source de l'horloge</label>
<div class="rad-group">
<label class="rad-opt"><input type="radio" name="Horlo" id="Hor0" value="0" onclick="checkDisabled();">🌐 Internet (NTP)</label>
<label class="rad-opt"><input type="radio" name="Horlo" id="Hor1" value="1" onclick="checkDisabled();">⚡ Linky</label>
<label class="rad-opt"><input type="radio" name="Horlo" id="Hor2" value="2" onclick="checkDisabled();">✏️ Interne (manuelle)</label>
<label class="rad-opt"><input type="radio" name="Horlo" id="Hor3" value="3" onclick="checkDisabled();">⏱ IT 10ms / 100 Hz (Triac)</label>
<label class="rad-opt"><input type="radio" name="Horlo" id="Hor4" value="4" onclick="checkDisabled();">⏱ IT 20ms / 50 Hz</label>
<label class="rad-opt"><input type="radio" name="Horlo" id="Hor5" value="5" onclick="checkDisabled();">📡 ESP Externe</label>
</div>
</div>

<div class="card" id="ZoneNew_H" style="display:none">
<div class="row"><label for="New_J">Date <span class="hint">Format JJ/MM/AAAA</span></label><input type="text" id="New_J" placeholder="25/12/2025"></div>
<div class="row"><label for="New_H">Heure <span class="hint">Format HH:MN</span></label><input type="text" id="New_H" placeholder="15:36"></div>
</div>

<div class="card" id="ZoneFuseau">
<div class="row"><label for="Fuseau">Fuseau horaire</label>
<select id="Fuseau">
<option value="0" selected>Europe centrale</option>
<option value="1">Guadeloupe / Martinique</option>
<option value="2">Guyane</option>
<option value="3">Réunion</option>
<option value="4">Mayotte</option>
<option value="5">Nouvelle Calédonie</option>
<option value="6">Wallis et Futuna</option>
</select></div>
<div class="row"><label for="ntpServer">Serveur NTP (optionnel) <span class="hint">Laissez vide pour le serveur par défaut · ex : mafreebox.free.fr</span></label><input type="text" id="ntpServer" placeholder="pool.ntp.org"></div>
<p class="hint" style="margin:8px 0 0">* Nécessite une sauvegarde puis un Restart pour prise en compte</p>
</div>

<div class="btnrow" id="BoutonsBas">
<button class="btn-pri" onclick="SendHour();">💾 Sauvegarder</button>
<div class="lds-dual-ring" id="attente"></div>
<button class="btn-ghost" onclick="Reset();">🔄 ESP32 Restart</button>
</div>
</section>
</main>

<script src="/ParaCommunJS"></script>
<script>
// Bascule de thème : /theme.js (partagé)

function Init(){LoadParaFixe();LoadData();}
function AdaptationSource(){}
function SetParaVar(){}

function SetParaFixe(){
  GID("Hor"+F.Horloge).checked=true;
  GID("Fuseau").value=F.idxFuseau;
  GID("ntpServer").value=F.ntpServer;
  checkDisabled();
}

function LoadData(){
  var x=new XMLHttpRequest();
  x.onreadystatechange=function(){
    if(this.readyState==4){
      if(this.status==200) GID("date").innerHTML=this.responseText.split(GS)[0].split(RS)[1];
      setTimeout(LoadData,2000);
    }
  };
  x.open("GET","/ajax_data",true);x.send();
}

function SendHour(){
  GID("attente").style.visibility="visible";
  var x=new XMLHttpRequest();
  x.onreadystatechange=function(){
    if(this.readyState==4&&this.status==200){GID("attente").style.visibility="hidden";location.reload();}
  };
  F.Horloge=document.querySelector("input[name='Horlo']:checked").value;
  F.idxFuseau=GID("Fuseau").value;
  F.ntpServer=GID("ntpServer").value.trim();
  x.open("GET","HourUpdate?New_H="+encodeURIComponent(GID("New_H").value.trim())+"&New_J="+GID("New_J").value.trim()+"&Horloge="+F.Horloge+"&idxFuseau="+F.idxFuseau+"&ntpServer="+F.ntpServer,true);
  x.send();
}

function checkDisabled(){
  if(F.ModeReseau>0&&F.Horloge==0){GID("Hor2").checked=true;F.Horloge=2;}
  F.Horloge=document.querySelector("input[name='Horlo']:checked").value;
  GID("ZoneNew_H").style.display=(F.Horloge<2||F.Horloge>4)?"none":"block";
  GID("ZoneFuseau").style.display=(F.Horloge==0)?"block":"none";
}
</script>
</body>
</html>
)====";
