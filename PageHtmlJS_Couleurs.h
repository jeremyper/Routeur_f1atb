//****************************************************
// Page Couleurs — design Soleo (CommunCouleurJS inchangé)
//****************************************************
const char *CouleursHtml = R"====(
<!doctype html>
<html lang="fr" data-theme="dark">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="icon" href="/favicon.ico">
<title>Couleurs</title>
<link rel="stylesheet" href="/commun.css">
<script src="/theme.js"></script>
<style>
/* Shell Soleo : voir /commun.css — surcharges propres a la page */
.app,header.top,.botnav{max-width:760px}
h2{margin:0;font-weight:700}
section{margin:20px 0}
.card{background:var(--card);border:1px solid var(--line);border-radius:var(--radius-s);padding:16px;margin:12px 0;box-shadow:var(--shadow);overflow-x:auto}
.col-tbl{width:100%;border-collapse:collapse}
.col-tbl th{text-align:left;padding:8px 6px;font-size:12px;color:var(--txt-dim);font-weight:600;border-bottom:2px solid var(--line)}
.col-tbl td{padding:8px 6px;border-bottom:1px solid var(--line);font-size:13.5px;vertical-align:middle}
.col-tbl tr:last-child td{border-bottom:none}
.col-tbl input[type=color]{width:36px;height:28px;border:1px solid var(--line);border-radius:6px;background:transparent;cursor:pointer;padding:1px}
.btnrow{display:flex;gap:10px;margin-top:4px;flex-wrap:wrap}
.btn-pri{background:var(--accent);color:#08221a;border:none;border-radius:11px;padding:11px 22px;font-size:14.5px;font-weight:800;cursor:pointer}
.btn-pri:active,.btn-ghost:active{transform:scale(.96)}
.btn-ghost{border:1px solid var(--line);background:var(--card-2);color:var(--txt);border-radius:11px;padding:11px 22px;font-size:14.5px;font-weight:700;cursor:pointer}
.info-note{font-size:12.5px;color:var(--txt-dim);margin-top:10px}
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
<h2 style="margin-bottom:4px">🎨 Couleurs</h2>
<p style="font-size:13px;color:var(--txt-dim);margin:4px 0 12px">Personnalisez les couleurs des tableaux et graphiques des pages Mesures et Données brutes.</p>

<div class="card">
<div id="colors"></div>
</div>

<div class="btnrow">
<button class="btn-ghost" onclick="SendValues(false);">↩ Couleurs par défaut</button>
<button class="btn-pri" onclick="SendValues(true);">💾 Sauvegarder</button>
</div>
<p class="info-note">Les couleurs s'appliquent aux pages Mesures et Données brutes. Le thème sombre/clair est indépendant.</p>
</section>
</main>

<script src="/ParaCommunJS"></script>
<script src="/CommunCouleurJS"></script>
<script>
// Bascule de thème : /theme.js (partagé)

var BordsInverse=[];

function Init(){SetCurseurs();LoadParaFixe();}

function SetCurseurs(){
  var S="<table class='col-tbl'><thead><tr><th>Champ</th><th>Texte</th><th>Fond / Courbe</th><th>Bordure</th></tr></thead><tbody>";
  for(var i=0;i<Koul.length;i++){
    S+="<tr><td>"+Koul[i][0]+"</td>";
    S+="<td><input type='color' id='text_color"+i+"' value='#000000' onchange='readCouleur();'></td>";
    S+="<td><input type='color' id='bg_color"+i+"' value='#000000' onchange='readCouleur();'></td>";
    S+="<td><input type='color' id='bord_color"+i+"' value='#000000' onchange='readCouleur();'></td></tr>";
  }
  S+="</tbody></table>";
  GH("colors",S);
}

function readCouleur(){
  for(var i=0;i<Koul.length;i++){
    if(Koul[i][1]) Koul[i][1]=GID("text_color"+i).value.substring(1);
    if(Koul[i][3]) Koul[i][3]=GID("bg_color"+i).value.substring(1);
    if(Koul[i][5]) Koul[i][5]=GID("bord_color"+i).value.substring(1);
  }
  setCouleur();
}

function SendValues(update){
  var S="?couleurs=";
  if(update){
    for(var i=0;i<Koul.length;i++){
      if(Koul[i][1]) S+=Koul[i][1];
      if(Koul[i][3]) S+=Koul[i][3];
      if(Koul[i][5]) S+=Koul[i][5];
    }
  }
  var x=new XMLHttpRequest();
  x.onreadystatechange=function(){if(this.readyState===4&&this.status===200)location.reload();};
  x.open("GET","/CouleurUpdate"+S,true);x.send();
}

function SetParaFixe(){Set_Couleurs();}
function AdaptationSource(){}
function SetParaVar(){}
</script>
</body>
</html>
 )====";


const char *CommunCouleurJS = R"====(
 let Koul = []; // Couleurs courantes
let Coul_Page, Coul_Bout, Coul_W, Coul_VA, Coul_Wh, Coul_Tab, Coul_Graphe, Coul_Temp, Coul_Ouvre;

//------------------------------------------------------
// Etabli les couleurs 
//------------------------------------------------------
function Set_Couleurs() {
 
      let Retour = F.Couleurs; 

      if (Retour !== "") {
        let L = Retour.length;
        let j = 0;

        for (let i = 0; i < Koul.length; i++) {
          if (Koul[i][1] && j + 6 <= L) { Koul[i][1] = Retour.substring(j, j + 6); j += 6; }
          if (Koul[i][3] && j + 6 <= L) { Koul[i][3] = Retour.substring(j, j + 6); j += 6; }
          if (Koul[i][5] && j + 6 <= L) { Koul[i][5] = Retour.substring(j, j + 6); j += 6; }
        }
      }
      setCouleur();
  
}

//------------------------------------------------------
// Applique les couleurs partout dans la page
//------------------------------------------------------
function setCouleur() {

  for (let i = 0; i < Koul.length; i++) {

    // --- TEXTE ---
    if (Koul[i][1]) {
      let tc = GID("text_color" + i);
      if (tc) tc.value = "#" + Koul[i][1];

      if (Koul[i][2]) {
        for (let j = 0; j < Koul[i][2].length; j++) {
          setColorQuery(Koul[i][2][j], "#" + Koul[i][1]);
        }
      }
    } else {
      if (GID("text_color" + i))
        GID("text_color" + i).style.display = "none";
    }

    // --- FOND ---
    if (Koul[i][3]) {
      let bg = GID("bg_color" + i);
      if (bg) bg.value = "#" + Koul[i][3];

      if (Koul[i][4]) {
        for (let j = 0; j < Koul[i][4].length; j++) {
          setBgColorQuery(Koul[i][4][j], "#" + Koul[i][3]);
          // Pas de couleur de texte propre a cette entree : on la deduit du fond
          if (!Koul[i][2]) setColorQuery(Koul[i][4][j], contraste("#" + Koul[i][3]));
        }
      }
    } else {
      if (GID("bg_color" + i))
        GID("bg_color" + i).style.display = "none";
    }

    // --- BORDURE ---
    if (Koul[i][5]) {
      let bd = GID("bord_color" + i);
      if (bd) bd.value = "#" + Koul[i][5];

      if (Koul[i][6]) {
        for (let j = 0; j < Koul[i][6].length; j++) {
          setBoColorQuery(Koul[i][6][j], "#" + Koul[i][5]);
        }
      }
    } else {
      if (GID("bord_color" + i))
        GID("bord_color" + i).style.display = "none";
    }
  }

  //------------------------------------------------------
  // Inversion bordures (éclaircir)
  //------------------------------------------------------
  for (let i = 0; i < BordsInverse.length; i++) {
    let liste = document.querySelectorAll(BordsInverse[i]);

    for (let j = 0; j < liste.length; j++) {
      let rgb = liste[j].style.borderColor;

      if (!rgb || rgb.indexOf("(") < 0) continue;

      rgb = rgb.substring(rgb.indexOf("(") + 1, rgb.indexOf(")"));
      let tmp = rgb.split(",");

      if (tmp.length < 3) continue;

      let hexColor = "#";
      for (let k = 0; k < 3; k++) {
        let c = Math.min(255, Math.floor(tmp[k] * 1.8));
        let H = ("00" + c.toString(16)).slice(-2);
        hexColor += H;
      }
      liste[j].style.borderColor = hexColor;
    }
  }
}

//------------------------------------------------------
function setColorQuery(S, C) {
  document.querySelectorAll(S).forEach(e => e.style.color = C);
}

function setBgColorQuery(S, C) {
  document.querySelectorAll(S).forEach(e => e.style.background = C);
}

// Les cellules d'unite (.W, .Wh, .V ...) recoivent un fond choisi par
// l'utilisateur mais aucune couleur de texte : elles heritent de celle du
// tableau, qui peut etre claire. On ecrit donc en noir ou en blanc selon la
// luminance du fond, pour que la valeur reste lisible quel que soit le choix.
function contraste(hex) {
  const r = parseInt(hex.substr(1, 2), 16),
        v = parseInt(hex.substr(3, 2), 16),
        b = parseInt(hex.substr(5, 2), 16);
  return (0.299 * r + 0.587 * v + 0.114 * b) > 150 ? "#101418" : "#f2f5fa";
}

function setBoColorQuery(S, C) {
  document.querySelectorAll(S).forEach(e => e.style.borderColor = C);
}

//------------------------------------------------------
// Valeurs par défaut 
//------------------------------------------------------
function CouleurDefaut(){  
    let Coul=[];
    //Format: nom,CoulTexte,QueryTexte,CoulFond,QueryFond,CoulBorder,QueryBorder
    Coul_Page = Coul.length;
    Coul.push(["Page","*",,"*",,"*",]);
    Coul_Bout = Coul.length;
    Coul.push(["Boutons haut des pages","*",["a:visited","a:link"],"*",[".Bonglet",".Bonglet2"],"*",[".Bonglet",".Bonglet2"]]);
    Coul.push(["Champs de saisie","*",["input","select"],"*",["input","select"],"*",["input","select"]]);
    Coul_Tab = Coul.length;
    Coul.push(["Tableaux","*",[".tableau",".grid-container1",".grid-container2",".grid-container2M"],"*",[".tableau"],"*",[".tableau"]]);
    Coul_W = Coul.length;
    Coul.push(["Puissance Active en W",,,"*",[".W"],,]);
    Coul_VA = Coul.length;
    Coul.push(["Puissance Apparente en VA",,,"*",[".VA"],,]);
    Coul_Wh = Coul.length;
    Coul.push(["Energie Active en Wh",,,"*",[".Wh"],,]);
    Coul.push(["Volt V",,,"*",[".V"],,]);
    Coul.push(["Ampère A",,,"*",[".A"],,]);
    Coul.push(["Cosinus Phi",,,"*",[".phi"],,]);
    Coul.push(["Hertz et divers",,,"*",[".Hz",".Enph"],,]);
    Coul_Graphe = Coul.length;
    Coul.push(["Graphes","*",,"*",,"*",]);
    Coul_Temp = Coul.length;
    Coul.push(["Temperature Canal 0",,,"*",,,]);
    Coul.push(["Temperature Canal 1",,,"*",,,]);
    Coul.push(["Temperature Canal 2",,,"*",,,]);
    Coul.push(["Temperature Canal 3",,,"*",,,]);
    Coul_Ouvre = Coul.length; 
    Coul.push(["Ouverture SSR/Triac 0",,,"*",,,]);
    Coul.push(["Ouverture SSR/Triac 1",,,"*",,,]);
    Coul.push(["Ouverture SSR/Triac 2",,,"*",,,]);
    Coul.push(["Ouverture SSR/Triac 3",,,"*",,,]);
    Koul=Coul.slice(); //copie de travail
  }
CouleurDefaut();  // initialisation couleurs courantes

 )====";