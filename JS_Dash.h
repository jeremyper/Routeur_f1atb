//*****************************************************
// Tableau de bord Soleo - Javascript
// Utilise F, V, GS, RS, GID, GH et LoadParaFixe fournis par /ParaCommunJS
//*****************************************************
const char *DashJS = R"====(
var pwImport=0,pwExport=0,kwhJourSout=0,ssrOuvre=-1,forceBallon=0,lastOk=0;

function fmt1(n){return (Math.round(n*10)/10).toFixed(1).replace(".",",");}
function eur(n){return n.toFixed(2).replace(".",",");}
function kW(w){return fmt1(Math.abs(w)/1000)+" kW";}

//Hooks attendus par ParaCommunJS
function SetParaFixe(){MajFlux();MajTuiles();}
function AdaptationSource(){}
function SetParaVar(){}

//---------- Thème clair / sombre ----------
// Bascule de thème : /theme.js (partagé par toutes les pages)

//---------- Données temps réel /ajax_data (2 s) ----------
async function PollData(){
  let retry=2000;
  try{
    const r=await fetch("/ajax_data");
    if(!r.ok)throw 0;
    const g=(await r.text()).split(GS);
    const G1=g[1].split(RS);
    pwImport=parseFloat(G1[0])||0;       //Puissance soutirée W
    pwExport=parseFloat(G1[1])||0;       //Puissance injectée W
    kwhJourSout=(parseFloat(G1[4])||0)/1000;
    lastOk=Date.now();
    MajFlux();MajTuiles();
  }catch(e){retry=8000;}
  setTimeout(PollData,retry);
}

//---------- Paramètres variables /ParaVar (10 s) ----------
async function PollVar(){
  try{
    const r=await fetch("/ParaVar");
    if(r.ok){V=await r.json();MajVar();}
  }catch(e){}
  setTimeout(PollVar,10000);
}
function MajVar(){
  if(!V)return;
  const b=parseInt(V.LTARFbin)||0;
  const tempo=(b&4)?"Tempo Bleu":(b&8)?"Tempo Blanc":(b&16)?"Tempo Rouge":"";
  const h=(b&2)?"HC":(b&1)?"HP":"";
  const el=GID("tarifBadge");
  el.textContent=tempo?(tempo+(h?" · "+h:"")):(h||"Tarif —");
  const c=(b&16)?"var(--grid-imp)":(b&8)?"var(--txt)":(b&4)?"var(--home)":(b&2)?"var(--grid-exp)":(b&1)?"var(--sun)":"var(--txt-dim)";
  el.style.color=c;el.style.borderColor=c;
  MajAbsence();
  MajFlux();MajTuiles();
}

//---------- Mode absence ----------
function MajAbsence(){
  if(!V)return;
  var actif=(parseInt(V.ModeAbsenceActif)||0)==1;
  var legio=(parseInt(V.AntiLegioEnCours)||0)==1;
  var ban=GID("absenceBanner");
  if(ban){
    ban.style.display=actif?"block":"none";
    ban.innerHTML=actif?(legio
      ?"🏨 Absence — chauffe anti-légionelle en cours · appuyez pour désactiver"
      :"🏨 Mode absence actif — actions coupées · appuyez pour désactiver"):"";
  }
  var btn=GID("btnAbsence");
  if(btn)btn.style.background=actif?"rgba(255,181,71,.25)":"";
}
async function ToggleAbsence(){
  var actif=V&&(parseInt(V.ModeAbsenceActif)||0)==1;
  try{
    var r=await fetch("/ajax_absence?set="+(actif?0:1));
    var t=await r.text();
    if(V)V.ModeAbsenceActif=(t.trim()=="1")?1:0;
    MajAbsence();
  }catch(e){}
}

//---------- Flux d'énergie + phrase statut ----------
function MajFlux(){
  const sma=V&&V.SmaOn==1;
  const pv=sma?Math.max(0,parseFloat(V.PuissancePV)||0):null;
  const net=pwImport-pwExport; //>0 : import réseau
  let rout=0;
  if(pv!==null&&ssrOuvre>0)rout=Math.min(pv,pv*(parseFloat(V.BallonCoefAuto)||0)/100);
  const maison=(pv!==null)?Math.max(0,pv+net-rout):Math.max(0,net);
  GH("flowKw",pv!==null?fmt1(pv/1000):"—");
  GH("flowEur",(pv!==null&&pv>0&&V)?("+"+eur(pv/1000*(parseFloat(V.PrixActuel)||0))+" €/h économisés"):"");
  const crete=(F&&parseFloat(F.MeteoPVcrete)>0)?parseFloat(F.MeteoPVcrete)*1000:3000;
  const ratio=(pv!==null)?Math.min(1,pv/crete):0;
  GID("arcSun").setAttribute("stroke-dashoffset",String(Math.round(754*(1-ratio))));
  GH("legHome",kW(maison));
  GH("legWater",kW(rout));
  GH("legGrid",kW(net));
  SetFlux("fHome",(pv!==null)?Math.max(0,Math.min(pv-rout,maison)):0);
  SetFlux("fWater",rout);
  let ic,t,m,bg;
  if(pv!==null&&pv>50){
    if(net<=10&&rout>0){ic="☀️";t="Tout roule.";m="Le soleil couvre la maison et le surplus chauffe votre eau.";bg="radial-gradient(circle,var(--grid-exp),#2faa6c)";}
    else if(net<=10){ic="✨";t="Surplus disponible.";m="Vous produisez plus que vous ne consommez : "+kW(net)+" repartent vers le réseau.";bg="radial-gradient(circle,var(--sun),#e98a1a)";}
    else{ic="🌤️";t="Production limitée.";m="Le solaire aide, mais "+kW(net)+" sont encore achetés au réseau.";bg="radial-gradient(circle,#e8a14a,#cf7d1a)";}
  }else if(net>50){ic="🌙";t="Réseau seul.";m="Pas de production solaire : la maison tire "+kW(net)+" du réseau.";bg="radial-gradient(circle,#7c8aa3,#4a5568)";}
  else if(net<-50){ic="✨";t="Vous injectez.";m=kW(net)+" repartent vers le réseau.";bg="radial-gradient(circle,var(--sun),#e98a1a)";}
  else{ic="🌙";t="Tout est calme.";m="Très peu d'énergie circule en ce moment.";bg="radial-gradient(circle,#7c8aa3,#4a5568)";}
  GH("heroTitle",t);GH("heroMsg",m);
  const hi=GID("heroIcon");hi.textContent=ic;hi.style.background=bg;
}
function SetFlux(id,w){ //épaisseur du flux animé proportionnelle à la puissance (W)
  const el=GID(id);
  if(w>50){el.style.opacity=1;el.setAttribute("stroke-width",String(Math.min(6,1.5+w/800)));}
  else el.style.opacity=0;
}

//---------- Tuiles ----------
function MajTuiles(){
  if(!V)return;
  const sma=V.SmaOn==1;
  //Production
  if(sma){
    GH("tPv",fmt1((parseFloat(V.PuissancePV)||0)/1000)+" kW");
    GH("tPvSub",fmt1((parseFloat(V.EnergieJourPV)||0)/1000)+" kWh aujourd'hui");
    const ch=GID("tPvChip");ch.style.display="inline-flex";
    ch.textContent="≈ "+eur(parseFloat(V.EconomieJour)||0)+" € économisés";
  }else{
    GH("tPv","—");
    GH("tPvSub",(V.MeteoOn==1&&V.PrevisionJour>=0)?("Prévision : "+V.PrevisionJour+" kWh"):"production non mesurée");
  }
  //Maison
  const net=pwImport-pwExport;
  GH("tMaison",kW(net));
  GH("tMaisonSub",(net>=0?"tirés du réseau":"renvoyés au réseau")+" · "+fmt1(kwhJourSout)+" kWh achetés ce jour");
  GID("tMaisonChip").style.display=(pwExport>10)?"inline-flex":"none";
  //Eau chaude
  const c=parseInt(V.BallonCanal);
  let temp=null;
  if(c>=0&&V.temperature&&V.temperature[c]>-50)temp=parseFloat(V.temperature[c]);
  const cible=(F&&parseFloat(F.BallonTcible)>20)?parseFloat(F.BallonTcible):55;
  if(temp!==null){
    GH("tBallon",fmt1(temp)+"°");
    GID("tBallonBar").style.width=Math.max(0,Math.min(100,Math.round(100*(temp-20)/(cible-20))))+"%";
  }else{GH("tBallon","—");GID("tBallonBar").style.width="0%";}
  let sb;
  const besoin=parseFloat(V.BallonBesoin),surplus=parseFloat(V.BallonSurplus);
  if(forceBallon>0)sb="⚡ Marche forcée · "+forceBallon+" mn";
  else if(forceBallon<0)sb="✋ Arrêt forcé · "+(-forceBallon)+" mn";
  else if(ssrOuvre>0)sb="🔆 En chauffe ("+ssrOuvre+"%)";
  else if(besoin>=0&&surplus>=0)sb=(besoin-surplus>0.05)?"🌙 Chauffe prévue cette nuit":"☀️ Le soleil suffira";
  else sb="En attente de surplus";
  GH("tBallonSub",sb);
  //Ce mois
  const eco=parseFloat(V.EconomieMois)||0;
  GH("tMois",eur(eco)+" €");
  const php=parseFloat(V.PrixHP)||0;
  GH("tMoisSub",php>0?("≈ "+Math.round(eco/php)+" kWh solaires ce mois"):"économies ce mois");
}

//---------- Actions /ajax_etatActions (4 s) ----------
async function FetchActions(force,num){
  try{
    const r=await fetch("/ajax_etatActions?Force="+force+"&NumAction="+num);
    if(!r.ok)throw 0;
    const m=(await r.text()).split(GS);
    const nb=parseInt(m[3])||0;
    let S="";
    ssrOuvre=-1;forceBallon=0;
    for(let i=0;i<nb;i++){
      const d=m[i+4].split(RS);
      if(d.length<5)continue;
      const idx=parseInt(d[0]),f=parseInt(d[3])||0;
      let pct=0;
      if(d[2]==="On")pct=100;else if(d[2]!=="Off")pct=parseInt(d[2])||0;
      if(idx===0){ssrOuvre=pct;forceBallon=f;}
      let cls,txt;
      if(f>0){cls="force";txt="Forcé On · "+f+" mn";}
      else if(f<0){cls="off";txt="Arrêté · "+(-f)+" mn";}
      else if(d[2]==="On"){cls="on";txt="En marche";}
      else if(pct>0){cls="on";txt="En chauffe ("+pct+"%)";}
      else{cls="idle";txt="En attente";}
      S+=`<div class="autocard"><span class="nm">${d[1]}</span><span class="chip-status ${cls}">`+(cls==="on"?`<span class="dotpulse"></span>`:"")+`${txt}</span></div>`;
    }
    if(S==="")S=`<div class="autocard"><span class="nm" style="color:var(--txt-soft)">Aucune action active</span></div>`;
    GH("autoList",S);
    MajFlux();MajTuiles();
  }catch(e){}
}
async function PollActions(){await FetchActions(0,0);setTimeout(PollActions,4000);}
function Forcer(s){FetchActions(s,0);} //Boutons Forcer / Arrêter de la tuile Eau chaude (action 0)

//---------- Journal /ajax_journal (30 s) ----------
function icoJ(msg){
  const s=msg.toLowerCase();
  if(s.indexOf("forc")>=0)return "⚡";
  if(s.indexOf("ballon")>=0||s.indexOf("chauffe")>=0||s.indexOf("eau")>=0)return "🚿";
  if(s.indexOf("météo")>=0||s.indexOf("prévision")>=0||s.indexOf("solaire")>=0)return "🌤️";
  if(s.indexOf("économ")>=0||s.indexOf("€")>=0)return "💶";
  if(s.indexOf("démarr")>=0||s.indexOf("reset")>=0)return "🔄";
  return "📍";
}
async function PollJournal(){
  try{
    const r=await fetch("/ajax_journal");
    if(r.ok){
      const lignes=(await r.text()).split("\n").filter(l=>l.indexOf(";")>0).slice(0,10);
      let S="";
      for(const l of lignes){
        const p=l.indexOf(";");
        S+=`<li><span class="ic">${icoJ(l.substring(p+1))}</span><div class="txt"><div class="when">${l.substring(0,p)}</div>${l.substring(p+1)}</div></li>`;
      }
      GH("feed",S||`<li><span class="ic">📜</span><div class="txt"><div class="when">—</div>Aucun événement pour le moment.</div></li>`);
    }
  }catch(e){}
  setTimeout(PollJournal,30000);
}

//---------- Horloge + indicateur en ligne ----------
function Tick(){
  GH("liveClock",new Date().toLocaleTimeString("fr-FR",{hour:"2-digit",minute:"2-digit",second:"2-digit"}));
  const ok=(Date.now()-lastOk)<6000;
  GID("online").className=ok?"online":"online off";
  GH("onlineTxt",ok?"En ligne":"Hors ligne");
}
setInterval(Tick,1000);

//---------- Démarrage ----------
LoadParaFixe(); //F + nom du routeur (id nom_R) + titre
PollVar();
PollData();
PollActions();
PollJournal();
Tick();
)====";
