// **********************************************************
//  Page connexion WiFi (Access Point) — design Soleo
// **********************************************************
const char *ConnectAP_Html = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Connexion WiFi</title>
<link rel="stylesheet" href="/commun.css">
<script src="/theme.js"></script>
<style>
/* Shell Soleo : voir /commun.css — surcharges propres a la page */
.app,header.top,.botnav{max-width:600px}
body{padding-bottom:40px}
h2{margin:0;font-weight:700}
section{margin:20px 0}
.card{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);padding:16px;margin:12px 0;box-shadow:var(--shadow)}
.card h3{margin:0 0 12px;font-size:15px;font-weight:700}
.wifi-row{display:flex;align-items:center;gap:10px;padding:11px 0;border-bottom:1px solid var(--line);cursor:pointer}
.wifi-row:last-child{border-bottom:none}
.wifi-row:hover{background:var(--card-2);margin:0 -16px;padding:11px 16px;border-radius:8px}
.wifi-ssid{flex:1;font-weight:600;font-size:14px}
.wifi-dbm{font-size:12px;color:var(--txt-dim);flex-shrink:0}
.wifi-mac{font-size:11px;color:var(--txt-dim);flex-shrink:0}
.wifi-row input[type=radio]{accent-color:var(--accent)}
.hint{font-size:12px;color:var(--txt-dim);margin-top:4px}
input[type=password]{width:100%;background:var(--card-2);border:1px solid var(--line);border-radius:10px;color:var(--txt);font-size:16px;padding:10px 12px;outline:none;margin-top:8px}
input:focus{border-color:var(--accent)}
.btn-pri{background:var(--accent);color:#08221a;border:none;border-radius:11px;padding:11px 22px;font-size:14.5px;font-weight:800;cursor:pointer;width:100%;margin-top:12px}
.btn-pri:active,.btn-scan:active{transform:scale(.96)}
.btn-scan{border:1px solid var(--line);background:var(--card-2);color:var(--txt);border-radius:11px;padding:10px 20px;font-size:14px;font-weight:700;cursor:pointer;width:100%}
#form-passe,#envoyer{display:none}
.status-msg{font-size:14px;color:var(--txt-soft);text-align:center;padding:12px;background:var(--card-2);border-radius:10px;margin-top:10px}
</style>
</head>
<body onload="init();">
<header class="top">
<div class="brand"><span class="logo"></span><span>Routeur Solaire</span></div>
</header>
<main class="app">
<section>
<h2>📶 Connexion réseau WiFi</h2>

<div class="card" id="scanCard">
<p class="hint" style="margin:0 0 12px">Sélectionnez votre réseau WiFi local. Une nouvelle adresse IP peut être attribuée par votre box.</p>
<button class="btn-scan" onclick="ScanWIFI();">🔍 Scanner les réseaux WiFi</button>
</div>

<div class="card" id="ListeWifi" style="display:none"></div>

<div class="card" id="form-passe">
<h3>🔑 Mot de passe</h3>
<div>Réseau sélectionné : <strong id="nom_reseau" style="color:var(--accent)"></strong></div>
<input type="password" name="passe" id="passe" placeholder="Mot de passe WiFi">
</div>

<div id="envoyer" style="margin-top:10px">
<button class="btn-pri" onclick="Envoyer();">✅ Connecter</button>
</div>

<div id="attente2" style="display:none" class="status-msg">⏳ Connexion en cours — attendez l'adresse IP attribuée à l'ESP32…</div>
</section>
</main>

<script src="/ParaCommunJS"></script>
<script>
function ScanWIFI(){
  GH("ListeWifi","<p style='color:var(--txt-dim);font-size:14px'>⏳ Scan en cours (2 s)…</p>");
  GID("ListeWifi").style.display="block";
  GID("form-passe").style.display="none";
  GID("envoyer").style.display="none";
  GID("attente2").style.display="none";
  var x=new XMLHttpRequest();
  x.onreadystatechange=function(){
    if(this.readyState==4&&this.status==200){
      var Wifi=this.responseText.split(GS);
      var S="<h3>Choisissez un réseau</h3><p class='hint' style='margin-bottom:12px'>Ne sont visibles que les réseaux scannés à la mise sous tension.</p>";
      for(var i=0;i<Wifi.length-1;i++){
        var w=Wifi[i].split(RS);
        S+="<div class='wifi-row' onclick='ChoixWifi(\""+w[0]+"\");'>";
        S+="<input type='radio' name='Wifi' value='"+w[0]+"'>";
        S+="<span class='wifi-ssid'>"+w[0]+"</span>";
        S+="<span class='wifi-dbm'>"+w[1]+" dBm</span>";
        S+="<span class='wifi-mac' style='display:none'>"+w[2]+"</span>";
        S+="</div>";
      }
      GH("ListeWifi",S);
    }
  };
  x.open("GET","/AP_ScanWifi",true);x.send();
}

function ChoixWifi(V){
  F.ssid=V;
  GH("nom_reseau",V);
  GID("form-passe").style.display="block";
  GID("envoyer").style.display="block";
  document.querySelector("input[name='Wifi'][value='"+V+"']").checked=true;
}

function Envoyer(){
  GID("form-passe").style.display="none";
  GID("envoyer").style.display="none";
  if(window.location.href.indexOf("192.168.4.1")==-1){
    GH("attente2","Adresse IP reconduite ou modifiée suivant votre box internet.");
  }
  GID("attente2").style.display="block";
  var x=new XMLHttpRequest();
  x.onreadystatechange=function(){
    if(this.readyState==4&&this.status==200){
      var Txt=this.responseText.split(RS);
      GH("ListeWifi","<p style='font-size:14px'>"+Txt[1]+"</p>");
      GID("attente2").style.display="none";
      if(Txt[0]=="Ok") GID("scanCard").style.display="none";
    }
  };
  x.open("GET","/AP_SetWifi?ssid="+encodeURIComponent(F.ssid)+"&passe="+encodeURIComponent(GID("passe").value),true);
  x.send();
}

function init(){LoadParaFixe();}
function AdaptationSource(){}
function SetParaVar(){}
function SetParaFixe(){}
</script>
</body>
</html>
)====";
