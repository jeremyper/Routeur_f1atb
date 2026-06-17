const char * ActionsJS1 = R"====(
// Structure de données 
var mouseClick = false;
var blockEvent = false;
var draggedHandle = null; 
var ListeActions = [];
var SelectActions = "";
var PlotIdx = -1;
var TabIdx = 0;
var MesuresPwAct = [];
var Ecart = [];
var Ouvert = [];
var Retard = [];
var Prop = [];
var Integ = [];
var Deriv = [];
var Pause = false;
var IS = "|"; // Input Separator
var BordsInverse = [".Bactions"];

/**
 * Fonction d'initialisation appelée au chargement.
 */
function Init() {
    SetHautBas();
    LoadParaFixe();   
}

/**
 * Crée un objet représentant une action.
 */

function CreerAction(NumAction, Titre) {
    let type=3; //Pw : routage proportionnel
    const S = {
        Action: NumAction,
        Actif: 0,
        Titre: Titre,
        Host: "",
        Port: 80,
        OrdreOn: "",
        OrdreOff: "",
        Repet: 240,
        Tempo: 0,
        Kp: 10,
        Ki: 10,
        Kd: 10,
        PID: 0,
        ForceOuvre:100,
        Periodes: [{
                    Hfin: 2400,
                    Type: type,
                    Vmin: 0,
                    Vmax: 100,
                    ONouvre :100,
                    Tinf: 1600,
                    Tsup: 1600,
                    Hmin: 0,
                    Hmax: 0,
                    CanalTemp: -1,
                    SelAct: 255,
                    Ooff: 0,
                    O_on: 0,
                    Tarif: 31,
                    MeteoCond: 0,
                    MeteoSeuil: 0
                }]
    };
    return S;
}

/**
 * Trace le panneau de configuration pour une action donnée.
 * @param {number} iAct - Index de l'action.
 */
function TracePlanning(iAct) {
    // Modes d'activation
    let Radio0 = "<div><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-0' onclick='checkDisabled();'>Inactif</div>";
    let Radio1 = "<div><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-1' onclick='checkDisabled();' title='Tout ou rien : relais ou commande externe'>On/Off</div>";
    Radio1 += "<div><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-2' onclick='checkDisabled();' title='SSR zéro-crossing : sinusoïdes réparties dans la trame, régulation fine et sans harmoniques (recommandé)'>Multi-sinus</div>";
    Radio1 += "<div><input type='radio' name='modeactif" + iAct + "' id='radio" + iAct + "-3' onclick='checkDisabled();' title='SSR zéro-crossing : salves de sinusoïdes complètes, sans harmoniques'>Train de sinus</div>";
    
    // Sélecteur de pin GPIO
    let SelectPin = "<div id='SelectPin" + iAct + "'>Gpio <select id='selectPin" + iAct + "' onchange='checkDisabled();' title='Choix broche (GPIO) de commande'>";
    for (let i = 0; i < Pins.length; i++) {
        let v = "gpio:" + Pins[i];
        if (Pins[i] == 0) v = "pas choisi ou supprimer";
        if (Pins[i] == -1) v = "Externe";
        SelectPin += "<option value=" + Pins[i]  + ">" + v + "</option>";
    }
    SelectPin += "</select></div>";
    
    // Sélecteur de sortie (niveau logique)
    let SelectOut = "<div id='SelectOut" + iAct + "'>Sortie 'On' <select id='selectOut" + iAct + "' title='Dépend du relais. En général +3.3V'><option value=0>0V</option><option value=1 selected>3.3V</option></select></div>";

    let S = "<div class='titre'><span id ='titre" + iAct + "' onclick='editTitre(" + iAct + ")' title='Donnez un nom ou effacez pour supprimer l'action'>Titre</span></div>";
    S += "<div class='visu' onclick='Plot(" + iAct + ")' id='visu" + iAct + "' title='Zoom sur la régulation en temps réel. Les réglages peuvent être modifiés. Ne pas oublier de sauvegarder'>&#128200;</div>";
    S += "<div class='mode'><div class='TitZone' title='Choix du mode de découpe du secteur'>Mode</div>" + Radio0 + Radio1 + "</div>";
    S += "<div id='blocPlanning" + iAct + "'>";
    S += "<div class='les_select' id='sortie" + iAct + "'>";
    S += "<div class='TitZone' title='Définir la broche (GPIO) ou commande'>Sortie</div>" + SelectPin + SelectOut;
    S += "<div><span id='Tempo" + iAct + "'>Temporisation(s) <input type='number' class='tm' id='tempo" + iAct + "' title='Temporisation entre chaque changement d&apos;état pour éviter des oscillations quand un appareil dans la maison consomme en dents de scie (Ex: un four).'></span></div>";
    S += "<div><span id='forceOuvre" + iAct + "'>Ouverture si forcée<input type='number' class='tm' id='ForceOuvre" + iAct + "' min='0' max='100' step='1' title='Ouverture prioritaire maximum en % si action forcée'>%</span> </div>";
    S += "</div><div class='les_select' id='ligne_bas" + iAct + "'><div class='TitZone'>Externe</div>";
    S += "<div><span id='Host" + iAct + "'>Host<br><input type='text' id='host" + iAct + "' onchange='checkDisabled();' title='Adresse IP machine sur réseau LAN, nom de domaine ou rien.'></span></div>";
    S += "<div><span id='Port" + iAct + "'>Port<br><input type='number' class='tm' id='port" + iAct + "' title='Port d&apos;acc&egrave;s via le protocole http , uniquement pour machine distante. En g&eacute;n&eacute;ral <b>80</b>.'></span></div>";
    S += "<div><span id='ordreon" + iAct + "'>Ordre On<br><input type='text' id='ordreOn" + iAct + "' title='Ordre à passer à la machine distante.'></span></div>";
    S += "<div><span id='ordreoff" + iAct + "'>Ordre Off<br><input type='text' id='ordreOff" + iAct + "' ></span></div>";
    S += "<div><span id='Repet" + iAct + "'>Répétition(s)<br><input type='number' id='repet" + iAct + "' class='tm' title='P&eacute;riode en s de r&eacute;p&eacute;tition/rafra&icirc;chissement de la commande. Uniquement pour les commandes vers l&apos;extérieur. 0= pas de répétition.'></span></div>";
    S += "</div>";

    // Curseurs PID
    S += "<div class='bouton_curseur'><div class='boutons'><input  type='button' value='-' class='tbut' onclick='AddSub(-1," + iAct + ")' title='Retrait d&apos;une p&eacute;riode horaire.'>";
    S += "<input  type='button' value='+' class='tbut' onclick='AddSub(1," + iAct + ")' title='Ajout d&apos;une p&eacute;riode horaire.'></div>";
    S += "<div class='slideTriac' id='fen_slide" + iAct + "'>";
    S += "<div class='slideTriacIn' id='Propor" + iAct + "'>";
    S += "<div class='Tcell1'>Coef. Proportionnel</div>";
    S += "<div class='Tcell2'><input type='range' min='0' max='100' value='50' title='Correction proportionnelle à l&apos;écart en W' id='sliderKp" + iAct + "' style='width:100%;max-width:none;' oninput=\"GH('sensiKp" + iAct + "',Math.floor(this.value));UpdateK(" + iAct + ");\"></div>";
    S += "<div class='Tcell3'><strong id='sensiKp" + iAct + "'></strong></div>";
    S += "</div>";
    S += "<div class='slideTriacIn'>";
    S += "<div class='Tcell1'>Coef. Intégral ou R&eacute;activit&eacute;</div>";
    S += "<div class='Tcell2'><input type='range' min='0' max='100' value='50' title='Correction suivant l&apos;intégrale de l&apos;écart en W' id='sliderKi" + iAct + "' style='width:100%;max-width:none;' oninput=\"GH('sensiKi" + iAct + "',Math.floor(this.value));UpdateK(" + iAct + ");\"></div>";
    S += "<div class='Tcell3'><strong id='sensiKi" + iAct + "'></strong></div>";
    S += "</div>";
    S += "<div class='slideTriacIn' title='Correction suivant la derivée de l&apos;écart en W' id='Derive" + iAct + "'>";
    S += "<div class='Tcell1'>Coef. Dérivé</div>";
    S += "<div class='Tcell2'><input type='range' min='0' max='100' value='50' id='sliderKd" + iAct + "' style='width:100%;max-width:none;' oninput=\"GH('sensiKd" + iAct + "',Math.floor(this.value));UpdateK(" + iAct + ");\"></div>";
    S += "<div class='Tcell3'><strong id='sensiKd" + iAct + "'></strong></div>";
    S += "</div>";
    S += "</div>";
    S += "<div id='PIDbox" + iAct + "'><label for='PID" + iAct + "'> PID On</label><input type='checkbox' id='PID" + iAct + "' onclick='checkDisabled();'></div>";
    S += "</div>";
    
   
    S += "<div id='graphAction" + iAct + "' class='graphAction'><div id='graphSVG" + iAct + "' class='graphSVG'></div><div class='GraphSVG' onclick='Pause=!Pause;'>&#x23EF;</div></div>";
    
    // Conteneur des curseurs de période (avec correction onmouseup)
    S += "<div style='margin:4px;'>";
    S += "<div id='infoAction" + iAct + "' class='infoAction'></div>";
    S += "<div id='curseurs" + iAct + "' class='curseur' onmousemove='dragHandle(event," + iAct + ");' onmouseup='stopDrag();' ontouchmove='dragHandle(event," + iAct + ");' ontouchend='stopDrag();'></div>";
    S += "</div>";
    S += "</div>"; // Ferme le bloc principal

    // Remplissage des valeurs depuis F.Actions
    GH("planning" + iAct, S);
    const action = F.Actions[iAct];
    
    if (action) {
        // Mode Actif
        const radioActive = GID("radio" + iAct + "-" + action.Actif);
        if (radioActive) radioActive.checked = true;
        action.Action=iAct ;
        GH("titre" + iAct, action.Titre);
        GV("host" + iAct, action.Host);
        GV("port" + iAct, action.Port);
        GV("ordreOn" + iAct, action.OrdreOn);
        GV("ordreOff" + iAct, action.OrdreOff);
        GV("repet" + iAct, action.Repet);
        GV("tempo" + iAct, action.Tempo);
        GV("ForceOuvre" + iAct, action.ForceOuvre);
        
        // PID (avec mise à jour des valeurs affichées)
        GV("sliderKp" + iAct, action.Kp);
        GH("sensiKp" + iAct, action.Kp);
        GV("sliderKi" + iAct, action.Ki);
        GH("sensiKi" + iAct, action.Ki);
        GV("sliderKd" + iAct, action.Kd);
        GH("sensiKd" + iAct, action.Kd);
        
        GID("PID" + iAct).checked = action.PID == 1;   

        // Configuration Pin/Sortie (OrdreOn)
        const selectPin = GID("selectPin" + iAct);
        const selectOut = GID("selectOut" + iAct);

        if (action.OrdreOn.indexOf(IS) > 0) {
            const vals = action.OrdreOn.split(IS);
            if (selectPin) selectPin.value = vals[0];
            if (selectOut) selectOut.value = vals[1];
        } else {
            // Configuration externe ou par défaut
            if (selectPin) selectPin.value = (action.OrdreOn === "") ? 0 : -1;
            if (selectOut) selectOut.value = 1;
        }

        TracePeriodes(iAct);
    }
}

/**
 * Trace les périodes horaires pour une action donnée.
 * @param {number} iAct - Index de l'action.
 */
function TracePeriodes(iAct) {
    let S = "";
    let Sinfo = "";
    let SinfoClick = "";
    let H0 = 0;
    const colors = ["#666", "#66f", "#f66", "#6f6", "#cc4"]; // NO, OFF, ON, PW, Triac
    blockEvent = false;
    
    const action = F.Actions[iAct];
    if (!action || !action.Periodes) return;

    for (let i = 0; i < action.Periodes.length; i++) {
        const periode = action.Periodes[i];
        
        // Calcul des dimensions de la période
        const w = (periode.Hfin - H0) / 24 ; // Largeur en %
        const left = H0 / 24 ; // Position en %
        H0 = periode.Hfin;
        
        const Type = periode.Type;
        const color = colors[Type];
        
        let temperature = "";
        
        // Conditions de température
        if (periode.CanalTemp >= 0) {
            if (V.temperature[periode.CanalTemp] > -100) { // La sonde de température fonctionne
                const Tsup = periode.Tsup;
                if (Tsup >= 0 && Tsup <= 1000) temperature += `<div>T &ge;${Tsup / 10}°</div>`;
                const Tinf = periode.Tinf;
                if (Tinf >= 0 && Tinf <= 1000) temperature += `<div>T &le;${Tinf / 10}°</div>`;
            }
        }
        
        // Conditions horaires d'ouverture (pour action 255/autre action)
        let H_Ouvert = "";
        if (periode.SelAct != 255) {
            if (periode.Hmin > 0) H_Ouvert += `<div>H<span class='fsize8'>ouverture</span> &ge;${Hdeci2Hmn(periode.Hmin)}</div>`;
            if (periode.Hmax > 0) H_Ouvert += `<div>H<span class='fsize8'>ouverture</span> &le;${Hdeci2Hmn(periode.Hmax)}</div>`;
            if (periode.Ooff > 0) H_Ouvert += `<div>On à Off si &le;${periode.Ooff}%</div>`;
            if (periode.O_on > 0) H_Ouvert += `<div>Off à On si &ge;${periode.O_on}%</div>`;
        }
        
        // Conditions tarifaires (Tempo, HP/HC)
        let TxtTarif = "";
        if (V.LTARFbin > 0) {
            let Tarif_ = periode.Tarif;
            let tarifText = "Tarif : ";
            if (V.LTARFbin <= 3) {
                tarifText += (Tarif_ & 1) ? "<span style='color:red;'>H. Pleine</span>" : "";
                tarifText += (Tarif_ & 2) ? "<span style='color:green;'> H. Creuse</span>" : "";
            } else {
                tarifText += (Tarif_ & 4) ? "Tempo<span style='color:blue;'>Bleu</span>" : "";
                tarifText += (Tarif_ & 8) ? "<span style='color:white;'> Blanc</span>" : "";
                tarifText += (Tarif_ & 16) ? "<span style='color:red;'> Rouge</span>" : "";
            }
            TxtTarif = `<div>${tarifText}</div>`;
        }

        // Condition météo (prévision solaire)
        let TxtMeteo = "";
        if (V.MeteoOn == 1 && periode.MeteoCond > 0) {
            if (periode.MeteoCond == 5) {
                TxtMeteo = "<div>&#9728; Adaptatif ballon</div>";
            } else {
                const lib = ["", "Prev. demain &lt;", "Prev. demain &ge;", "Prev. jour &lt;", "Prev. jour &ge;"];
                TxtMeteo = `<div>&#9728; ${lib[periode.MeteoCond]} ${(periode.MeteoSeuil || 0) / 10} kWh</div>`;
            }
        }

        const condition = (temperature !== "" || H_Ouvert !== "" || TxtTarif !== "" || TxtMeteo !== "") ? "<div>Condition(s) :</div>" + temperature + H_Ouvert + TxtTarif + TxtMeteo : "";
        let TexteMinMax = "";

        // Détermination du contenu de la zone d'info en fonction du mode de l'action
        if (action.Actif <= 1) {
            // Action On/Off
            periode.Vmax = Math.max(periode.Vmin, periode.Vmax);
            TexteMinMax = `<div>Off si Pw&gt;${periode.Vmax}W</div><div>On si Pw&lt;${periode.Vmin}W</div>${condition}`;
        } else {
            // Routage proportionnel (Multi/Train de sinus)
            periode.Vmax = Math.max(0, periode.Vmax);
            periode.Vmax = Math.min(100, periode.Vmax);
            TexteMinMax = `<div>Seuil Pw : ${periode.Vmin}W</div><div>Ouvre Max : ${periode.Vmax}%</div>${condition}`;
        }

        const paras = [`Pas de contr&ocirc;le`, `OFF`, `<div>ON</div><div>Ouverture : ${periode.ONouvre}%</div>` + condition, TexteMinMax, TexteMinMax];
        const para = paras[Type];
        
        // Remplissage du div de la période (curseur visuel)
          // Créer la période
        S += "<div id='zone" + iAct + "_" + i + "' class='periode' data-idx='" + i + "' style='width:" + w + "%;left:" + left + "%;background-color:" + color + ";'>";
       
        // Ajouter une poignée de déplacement uniquement si ce n'est pas la dernière période
        if (i < action.Periodes.length - 1) {
            
           
            S += "<div class='handleStyle' ' data-action='" + iAct + "' data-periode='" + i + "' ";
            S += "onmousedown='startDrag(this,event," + iAct + "," + i + ");' ";
            S += "ontouchstart='startDrag(this,event," + iAct + "," + i + ");'>|||</div>";
        }
       
        S += "</div>";
        
        // Remplissage du div d'information
        const Hmn = Hdeci2Hmn(H0);
        const fs = Math.max(8, Math.min(16, w / 2)) + "px";
        Sinfo += `<div class='infoZone' style='width:${w}%;border-color:${color};font-size:${fs}' onclick='infoZclicK(${i},${iAct})'>`;
        Sinfo += `<div class='Hfin'>${Hmn}</div>${para}</div>`;
        
        SinfoClick += `<div id='info${iAct}Z${i}' class='infoZ'></div>`;
    }
    
    GH("curseurs" + iAct, S);
    // Combine les zones d'info cliquables (SinfoClick) et le contenu (Sinfo)
    GH("infoAction" + iAct, SinfoClick + Sinfo);
}

)====";

const char * ActionsJS2 = R"====(
    function startDrag(handleElement, ev, iAct, iPeriode) {
   if (ev.preventDefault) ev.preventDefault();
    if (ev.stopPropagation) ev.stopPropagation();
     draggedHandle = {
        action: iAct,
        periode: iPeriode,
        element: handleElement,
        originalStyle: handleElement.style.cssText
    };
    var dragStyle = "position:absolute;right:-15px;top:50%;transform:translateY(-50%) scale(1.2);width:30px;height:40px;";
    dragStyle += "background-color:#ffeb3b;border:2px solid #f57c00;border-radius:8px;";
    dragStyle += "cursor:ew-resize;display:flex;align-items:center;justify-content:center;";
    dragStyle += "font-size:14px;font-weight:bold;color:#333;z-index:10;";
    dragStyle += "box-shadow:0 4px 8px rgba(0,0,0,0.5);touch-action:none;user-select:none;";
    handleElement.style.cssText = dragStyle;
}

function dragHandle(ev, iAct) {
    if (!draggedHandle || draggedHandle.action !== iAct) return;
   
    if (ev.preventDefault) ev.preventDefault();
   
    var curseur = GID('curseurs' + iAct);
    var leftPos;
   
      if (ev.touches && ev.touches.length > 0) {
        leftPos = ev.touches[0].clientX - curseur.getBoundingClientRect().left;
    } else {
        leftPos = ev.clientX - curseur.getBoundingClientRect().left;
    }
   var width = curseur.getBoundingClientRect().width;
    var HeureMouse = leftPos * 2420 / width;
    var iPeriode = draggedHandle.periode;
   
    var NewHfin = Math.max(0, Math.min(HeureMouse, 2400));
   const action = F.Actions[iAct];
  if (iPeriode < action.Periodes.length - 1) {
        NewHfin = Math.min(NewHfin, action.Periodes[iPeriode + 1].Hfin);
    }
    if (iPeriode > 0) {
        NewHfin = Math.max(NewHfin, action.Periodes[iPeriode - 1].Hfin);
    }
   
   action.Periodes[iPeriode].Hfin = Math.floor(NewHfin);
   draggedHandle.element.textContent = Hdeci2Hmn(Math.floor(NewHfin));
   var H0 = (iPeriode > 0) ? action.Periodes[iPeriode - 1].Hfin : 0;
    var left = H0 / 24;
    var w = (NewHfin - H0) / 24;
   
   var zone = GID('zone' + iAct + '_' + iPeriode);
    if (zone) {
        zone.style.width = w + '%';
    }
   
     if (iPeriode < action.Periodes.length - 1) {
        var nextZone = GID('zone' + iAct + '_' + (iPeriode + 1));
        var nextLeft = NewHfin / 24;
        var nextW = (action.Periodes[iPeriode + 1].Hfin - NewHfin) / 24;
        if (nextZone) {
            nextZone.style.left = nextLeft + '%';
            nextZone.style.width = nextW + '%';
        }
    }
}

function stopDrag() {
    if (draggedHandle && draggedHandle.element) {
      draggedHandle.element.style.cssText = draggedHandle.originalStyle;
       
      var iAct = draggedHandle.action;
        TracePeriodes(iAct);
    }
    draggedHandle = null;
}

/**
 * Gère le mouvement du doigt (touch) pour ajuster les périodes.
 * @param {HTMLElement} t - L'élément curseur cliqué.
 * @param {Event} ev - L'objet événement touch.
 * @param {number} iAct - Index de l'action.
 */
function touchMove(t, ev, iAct) {
    // Calcul de la position X relative à l'élément (t)
    const leftPos = ev.touches[0].clientX - GID(t.id).getBoundingClientRect().left;
    NewPosition(t, leftPos, iAct);
}

/**
 * Gère le mouvement de la souris pour ajuster les périodes.
 * @param {HTMLElement} t - L'élément curseur cliqué.
 * @param {Event} ev - L'objet événement souris.
 * @param {number} iAct - Index de l'action.
 */
function mouseMove(t, ev, iAct) {
    if (mouseClick) {
        // Calcul de la position X relative à l'élément (t)
        const leftPos = ev.clientX - GID(t.id).getBoundingClientRect().left;
        NewPosition(t, leftPos, iAct);
    }
}


/**
 * Calcule et applique la nouvelle position d'un curseur horaire.
 * @param {HTMLElement} t - L'élément curseur.
 * @param {number} leftPos - La position X du pointeur/doigt relative à l'élément.
 * @param {number} iAct - Index de l'action.
 */
function NewPosition(t, leftPos, iAct) {
    
    // Récupère la largeur totale de la zone de glissement
    const width = GID(t.id).getBoundingClientRect().width;
    
    // Convertit la position en centièmes d'heure (0 à 2400)
    // Utilisation de 2400 comme référence pour 24 heures (le 2420 est ajusté au standard 2400)
    const HeureMouse = leftPos * 2400 / width;
    
    let idxClick = 0;
    let deltaX = 999999;
    
    // Cherche le curseur le plus proche (sauf le dernier qui doit rester à 2400)
    for (let i = 0; i < F.Actions[iAct].Periodes.length - 1; i++) {
        const dist = Math.abs(HeureMouse - F.Actions[iAct].Periodes[i].Hfin);
        if (dist < deltaX) {
            idxClick = i;
            deltaX = dist;
        }
    }
    
    // Borne la nouvelle position entre 0 et 2400
    let NewHfin = Math.max(0, Math.min(HeureMouse, 2400));
    
    // La dernière période (qui représente 24h) ne doit pas être déplacée
    if (idxClick === F.Actions[iAct].Periodes.length - 1) {
        NewHfin = 2400;
    }
    
    // S'assure que le curseur ne dépasse pas le curseur suivant (à droite)
    if (idxClick < F.Actions[iAct].Periodes.length - 1) {
        NewHfin = Math.min(NewHfin, F.Actions[iAct].Periodes[idxClick + 1].Hfin);
    }
    
    // S'assure que le curseur ne recule pas au-delà du curseur précédent (à gauche)
    // Note: Pour le premier curseur (idxClick = 0), cette condition est ignorée.
    if (idxClick > 0) {
        // Le curseur précédent est celui qui se termine là où celui-ci commence.
        // On vérifie la fin du curseur précédent (idxClick - 1)
        NewHfin = Math.max(NewHfin, F.Actions[iAct].Periodes[idxClick - 1].Hfin);
    }
    
    // Affecte la nouvelle fin de période (arrondie à l'entier pour les centièmes d'heure)
    F.Actions[iAct].Periodes[idxClick].Hfin = Math.floor(NewHfin);
    
    // Redessine l'affichage des périodes
    TracePeriodes(iAct);
}

/**
 * Ajoute ou supprime une période horaire pour une action.
 * @param {number} v - 1 pour ajouter, -1 pour supprimer.
 * @param {number} iAct - Index de l'action.
 */
function AddSub(v, iAct) {
    if (v === 1) {
        // Ajout
        let type=3; //Pw : routage proportionnel
        if (F.Actions[iAct].Periodes.length < 8) {
            // Crée la nouvelle période avec des valeurs par défaut
            const newPeriod = {
                Hfin: 2400,
                Type: type,
                Vmin: 0,
                Vmax: 100,
                ONouvre : 100,
                Tinf: 1600,
                Tsup: 1600,
                Hmin: 0,
                Hmax: 0,
                CanalTemp: -1,
                SelAct: 255,
                Ooff: 0,
                O_on: 0,
                Tarif: 31, // Tarif codé en bits (toutes les options actives par défaut)
                MeteoCond: 0,
                MeteoSeuil: 0
            };
            F.Actions[iAct].Periodes.push(newPeriod);
            
            // Recalcule et ajuste Hfin de l'avant-dernière période pour centrer la nouvelle
            let Hbas = 0;
            const len = F.Actions[iAct].Periodes.length;
            if (len > 2) {
                // Hbas est l'Hfin de la période précédant l'avant-dernière
                Hbas = parseInt(F.Actions[iAct].Periodes[len - 3].Hfin, 10);
            }
            if (len > 1) {
                // Ajuste Hfin de l'avant-dernière période (celle qui vient d'être créée devient la dernière)
                F.Actions[iAct].Periodes[len - 2].Hfin = Math.floor((Hbas + 2400) / 2);
            }
        }
    } else {
        // Suppression
        if (F.Actions[iAct].Periodes.length > 1) {
            F.Actions[iAct].Periodes.pop();
            // S'assure que la nouvelle dernière période se termine à 2400
            if (F.Actions[iAct].Periodes.length > 0) {
                F.Actions[iAct].Periodes[F.Actions[iAct].Periodes.length - 1].Hfin = 2400;
            }
        }
    }
    F.Actions[iAct].NbPeriode= F.Actions[iAct].Periodes.length;
    TracePeriodes(iAct);
}

/**
 * Ouvre le panneau de configuration pour une période donnée.
 * @param {number} i - Index de la période.
 * @param {number} iAct - Index de l'action.
 */
function infoZclicK(i, iAct) {
    let capteurT = false;
    
    // Empêche l'ouverture multiple du panneau
    if (!blockEvent) {
        blockEvent = true;
        
        const periode = F.Actions[iAct].Periodes[i];
        const Type = periode.Type;
        const idZ = "info" + iAct + "Z" + i;
        const ONouvre = periode.ONouvre;
        
        let S = "<div class='selectZ'> S&eacute;lection Action<div class='closeZ' onclick='infoZclose(\"" + idZ + "\")'>X</div></div>";
        
        // --- Choix du Mode ---
        
        // OFF
        let check = (Type === 1) ? "checked" : "";
        S += "<div class='zOff'><div class='radioC'><input type='radio' name='R" + idZ + "' onclick='selectZ(1," + i + "," + iAct + ");' " + check + " title='Off forcé'>OFF</div></div>";
        
        S += "<div class='fcontainer'><div class='fcontleft'>";
        
        // ON (100%)
        check = (Type === 2) ? "checked" : "";
        S += "<div class='zOn'><div class='radioC'><input type='radio' name='R" + idZ + "' onclick='selectZ(2," + i + "," + iAct + ");' " + check + " title='On forcé (si conditions optionnelles valides)'>ON </div>";
        S += "<div><small>Ouverture: &nbsp;</small><input id='ONouvre_" + idZ + "' type='number' value='" + ONouvre + "' onchange='NewVal(this)' title='Ouverture en position ON.' >%</div></div>";
        
        // Routage/PWM/Triac
        check = (Type > 2) ? "checked" : "";
        
        const Vmin = periode.Vmin;
        const Vmax = periode.Vmax;
        const Tinf = periode.Tinf;
        const Tsup = periode.Tsup;
        
        let TinfC = Tinf / 10;
        let TsupC = Tsup / 10;
        
        const Hmin = (periode.Hmin > 0) ? Hdeci2Hmn(periode.Hmin) : "";
        const Hmax = (periode.Hmax > 0) ? Hdeci2Hmn(periode.Hmax) : "";
        const Ooff = (periode.Ooff > 0) ? periode.Ooff : "";
        const O_on = (periode.O_on > 0) ? periode.O_on : "";
        
        // Vider si hors de la plage -50.0 à 150.0
        if (Tinf > 1500 || Tinf < -500) TinfC = ""; 
        if (Tsup > 1500 || Tsup < -500) TsupC = ""; 

        {
            // Mode Routage ON/OFF ou Multi/Train de sinus
            const Routage = ["", "Routage ON/Off", "Routage Multi-sinus", "Routage Train de Sinus"];
            S += "<div class='zPw'><div class='radioC'><input type='radio' name='R" + idZ + "' onclick='selectZ(3," + i + "," + iAct + ");' " + check + ">" + (Routage[F.Actions[iAct].Actif] || "Routage") + "</div>";

            if (F.Actions[iAct].Actif <= 1) {
                // Routage ON/OFF
                S += "<div><small>On : &nbsp;</small>Pw &lt;<input id='Pw_min_" + idZ + "' type='number' value='" + Vmin + "' onchange='NewVal(this)' title='Seuil de puissance pour activer ou désactiver le routage. Attention, en cas de mode On/Off la diff&eacute;rence, seuil sup&eacute;rieur moins seuil inf&eacute;rieur doit &ecirc;tre sup&eacute;rieure &agrave; la consommation du dipositif pour &eacute;viter l&apos;oscillation du relais de commande.'>W</div>";
                S += "<div><small>Off : </small>Pw &gt;<input id='Pw_max_" + idZ + "' type='number' value='" + Vmax + "' onchange='NewVal(this)'>W</div>";
                S += "<div><small>Puissance active en entrée de maison</small></div></div>";
            } else {
                // Routage Multi/Train de sinus
                S += "<div><small>Seuil Pw : &nbsp;</small><input id='Pw_min_" + idZ + "' type='number' value='" + Vmin + "' onchange='NewVal(this)' title='Seuil de puissance pour activer ou désactiver le routage.' >W</div>";
                S += "<div><small>Puissance active en entrée de maison</small></div>";
                S += "<div><small>Ouvre Max : </small><input id='Pw_max_" + idZ + "' type='number' value='" + Vmax + "' onchange='NewVal(this)' title='Ouverture maximum du SSR. Valeur typique : 100%'>%</div></div>";
            }
        }
        
        S += "</div>"; // Ferme fcontleft
        
        // --- Conditions optionnelles (Température, Action, Tarif) ---
        
        // Sélecteur de canal de température
        let SelectT = "<div>Canal de Température <select id='CanalTemp" + idZ + "' onchange='NewVal(this)'><option value=-1 selected>Non exploité</option>";
        for (let c = 0; c < 4; c++) {
            if (V.temperature[c] > -100) {
                const Temper = parseFloat(V.temperature[c]).toFixed(1);
                SelectT += "<option value=" + c + " >" + F["nomTemperature" +c] + " (" + Temper + "°)" + "</option>";
                capteurT = true;
            }
        }
        SelectT += "</select></div>";
        
        // Affichage des conditions optionnelles
        const styleDisplay = (F.ModePara === 0) ? "none" : "block";
        const style = `style='display:${styleDisplay}';`;
        S += "<div>";
        S += "<div class='TitZone' " + style + ">&nbsp;&nbsp;&nbsp;Conditions optionnelles pour activer</div>";
        
        // Conditions de Température
        if (capteurT) {
            S += "<div class='bord1px' " + style + ">";
            S += SelectT;
            S += "<div class='minmax'><div>T &ge;<input id='T_sup_" + idZ + "' type='number' value='" + TsupC + "' onchange='NewVal(this)' title='Définir la ou les températures qui permettent l&apos;activation de la fonction On ou Routage.'>°</div>";
            S += "<div>T &le;<input id='T_inf_" + idZ + "' type='number' value='" + TinfC + "' onchange='NewVal(this)'>°</div></div>";
            S += "<div><small>T en degré (-50.0 à 150.0) ou laisser vide</small></div>";
            S += "</div>";
        }
        
        // Conditions d'Action
        S += "<div class='bord1px' " + style + " >";
        S += "<div>Etat d'une Action <select id='SelAct" + idZ + "' onchange='NewVal(this)'>" + SelectActions + "</select></div>";
        S += "<div class='minmax'><div>Durée : </div><div>H &ge;<input id='H_min_" + idZ + "' type='text' value='" + Hmin + "' onchange='NewVal(this)'>h:mn</div>";
        S += "<div>H &le;<input id='H_max_" + idZ + "' type='text' value='" + Hmax + "' onchange='NewVal(this)'>h:mn</div></div>";
        S += "<div class='minmax'><div>Seuil : </div><div>On à Off si &le;<input id='O_min_" + idZ + "' type='number' value='" + Ooff + "' onchange='NewVal(this)'>%</div>";
        S += "<div>Off à On si &ge;<input id='O_max_" + idZ + "' type='number' value='" + O_on + "' onchange='NewVal(this)'>%</div></div>";
        S += "<div><small>h:mn ou % ou laisser vide</small></div>";
        S += "</div>";


        // Conditions Tarifaires
        if (V.LTARFbin > 0) {
            // Récupère l'état actuel du tarif
            const Tarif_ = F.Actions[iAct].Periodes[i].Tarif;
            const isHPle = (Tarif_ & 1);
            const isHCre = (Tarif_ & 2);
            const isBe = (Tarif_ & 4);
            const isBa = (Tarif_ & 8);
            const isRo = (Tarif_ & 16);

            S += "<div class='bord1px'>";
            S += "<div title='Condition d&apos;activation suivant la tarification. Sinon ordre Off envoyé ou Triac/SSR se ferme.'>Actif si tarif :</div>";
            if (V.LTARFbin <= 3) {
                // HP/HC
                S += "<div id='PleineCreuse'><span style='color:red;'>Heure Pleine</span><input type='checkbox' " + (isHPle ? "checked" : "") + " id='TarifPl_" + idZ + "' onchange='NewVal(this)'> <span style='color:green;'>Heure Creuse</span><input type='checkbox' " + (isHCre ? "checked" : "") + " id='TarifCr_" + idZ + "' onchange='NewVal(this)'></div>";
            } else {
                // Tempo
                S += "<div id='Tempo'>Tempo <span style='color:blue;'>Bleu</span><input type='checkbox' " + (isBe ? "checked" : "") + " id='TarifBe_" + idZ + "' onchange='NewVal(this)'><span style='color:white;'> Blanc</span><input type='checkbox' " + (isBa ? "checked" : "") + " id='TarifBa_" + idZ + "' onchange='NewVal(this)'><span style='color:red;'> Rouge</span><input type='checkbox' " + (isRo ? "checked" : "") + " id='TarifRo_" + idZ + "' onchange='NewVal(this)'></div>";
            }
            S += "</div>";
        }

        // Condition Météo (prévision solaire Open-Meteo)
        if (V.MeteoOn == 1) {
            const MeteoSeuilkWh = (periode.MeteoSeuil || 0) / 10;
            S += "<div class='bord1px'>";
            S += "<div title='Condition d&apos;activation suivant la production solaire pr&eacute;vue. Exemple : forcer le chauffe-eau la nuit uniquement si demain sera couvert.'>&#9728; Prévision solaire :</div>";
            S += "<div>Actif si <select id='MeteoC_" + idZ + "' onchange='NewVal(this)'>";
            S += "<option value=0>Toujours (pas de condition)</option>";
            S += "<option value=1>Prévision demain &lt; seuil</option>";
            S += "<option value=2>Prévision demain &ge; seuil</option>";
            S += "<option value=3>Prévision aujourd'hui &lt; seuil</option>";
            S += "<option value=4>Prévision aujourd'hui &ge; seuil</option>";
            if (V.BallonCanal >= 0) S += "<option value=5>Adaptatif ballon (besoin &gt; surplus prévu)</option>";
            S += "</select></div>";
            S += "<div>Seuil <input id='MeteoS_" + idZ + "' type='number' step='0.1' min='0' value='" + MeteoSeuilkWh + "' onchange='NewVal(this)'> kWh</div>";
            if (V.PrevisionJour >= 0) {
                S += "<div><small>Prévision : aujourd'hui " + V.PrevisionJour + " kWh, demain " + V.PrevisionDemain + " kWh</small></div>";
            } else {
                S += "<div><small>En attente de données Open-Meteo...</small></div>";
            }
            if (V.BallonCanal >= 0 && V.BallonBesoin >= 0) {
                S += "<div><small>Ballon : besoin " + V.BallonBesoin + " kWh, surplus prévu " + V.BallonSurplus + " kWh</small></div>";
            }
            S += "</div>";
        }
        S += "</div>"; // Ferme le bloc conditions

        S += "</div>"; // Ferme la div container

        GH(idZ, S);

        // --- Affectation des valeurs aux sélecteurs (après construction du HTML) ---
        if (capteurT) GID("CanalTemp" + idZ).value = periode.CanalTemp;
        GID("SelAct" + idZ).value = periode.SelAct;
        if (V.MeteoOn == 1) GID("MeteoC_" + idZ).value = periode.MeteoCond || 0;
        GID(idZ).style.display = "block";
    }
}

)====";

const char * ActionsJS3 = R"====(
/**
 * Ferme le panneau d'information de la zone cliquée.
 * @param {string} idx - L'ID de l'élément à fermer (e.g., "info0Z0").
 */
function infoZclose(idx) {
    const champs = idx.split("info");
    const idx_parts = champs[1].split("Z");
    const iAct = parseInt(idx_parts[0], 10);
    
    const element = GID(idx);
    if (element) {
        element.style.display = "none";
    }

    // Utilisation d'une fonction anonyme pour un appel sécurisé et déverrouillage
    setTimeout(() => {
        blockEvent = false; // Réinitialise le verrou
        TracePeriodes(iAct);
    }, 100);
}

/**
 * Gère la sélection du type de fonctionnement (OFF, ON, Routage...) pour une période.
 * @param {number} T - Nouveau type de période (1=OFF, 2=ON, 3+=Routage/PID...).
 * @param {number} i - Index de la période.
 * @param {number} iAct - Index de l'action.
 */
function selectZ(T, i, iAct) {
    if (F.Actions[iAct] && F.Actions[iAct].Periodes[i]) {
        if (F.Actions[iAct].Periodes[i].Type != T) {
            F.Actions[iAct].Periodes[i].Type = T;
            const idZ = "info" + iAct + "Z" + i;
            
            // Si le mode est OFF (1), on ferme et on rafraîchit.
            if (T === 1) { 
                infoZclose(idZ);
            } else {
                // Pour les modes actifs (ON, Routage), on rafraîchit l'affichage
                // car le contenu de la zone peut changer (changement de couleur/texte)
                TracePeriodes(iAct);
            }
        }
    }
}

/**
 * Met à jour la valeur d'un paramètre spécifique d'une période après modification dans un champ input.
 * @param {HTMLElement} t - L'élément input/select qui a déclenché l'événement (this).
 */
function NewVal(t) {
    const champs = t.id.split("info");
    const idx = champs[1].split("Z"); // Num Action, Num période
    const iAct = parseInt(idx[0], 10);
    const i = parseInt(idx[1], 10);
    
    if (!F.Actions[iAct] || !F.Actions[iAct].Periodes[i]) return;

    const periode = F.Actions[iAct].Periodes[i];
    const value = GID(t.id).value;

    if (champs[0].indexOf("Pw_min") >= 0) {
        periode.Vmin = Math.floor(value);
    } else if (champs[0].indexOf("Pw_max") >= 0) {
        periode.Vmax = Math.floor(value);
    } else if (champs[0].indexOf("ouvre") > 0) { // Ouverture ON
        periode.ONouvre = Math.floor(value);
    } else if (champs[0].indexOf("inf") > 0) { // Température inférieure (Tinf)
        let V = value;
        if (V === "") V = 158; 
        periode.Tinf = Math.floor(parseFloat(V) * 10);
    } else if (champs[0].indexOf("sup") > 0) { // Température supérieure (Tsup)
        let V = value;
        if (V === "") V = 158; 
        periode.Tsup = Math.floor(parseFloat(V) * 10);
    } else if (champs[0].indexOf("H_min") >= 0) {
        periode.Hmin = Hmn2Hdeci(value);
    } else if (champs[0].indexOf("H_max") >= 0) {
        periode.Hmax = Hmn2Hdeci(value);
    } else if (champs[0].indexOf("O_min") >= 0) {
        periode.Ooff = Math.max(0, Math.min(100, Math.floor(value)));
    } else if (champs[0].indexOf("O_max") >= 0) {
        periode.O_on = Math.max(0, Math.min(100, Math.floor(value)));
    } else if (champs[0].indexOf("Tarif") >= 0) {
        const idZ = "info" + champs[1];
        let Tarif_ = 0;
        
        // Récupère l'état des checkboxes et construit la valeur binaire du tarif
        if (V.LTARFbin <= 3) { // HP/HC
            Tarif_ += GID("TarifPl_" + idZ).checked ? 1 : 0; // H pleine (bit 0)
            Tarif_ += GID("TarifCr_" + idZ).checked ? 2 : 0; // H creuse (bit 1)
        } else { // Tempo
            Tarif_ += GID("TarifBe_" + idZ).checked ? 4 : 0;  // Bleu (bit 2)
            Tarif_ += GID("TarifBa_" + idZ).checked ? 8 : 0;  // Blanc (bit 3)
            Tarif_ += GID("TarifRo_" + idZ).checked ? 16 : 0; // Rouge (bit 4)
        }
        periode.Tarif = Tarif_;
    } else if (champs[0].indexOf("CanalTemp") >= 0) {
        periode.CanalTemp = parseInt(value, 10);
    } else if (champs[0].indexOf("SelAct") >= 0) {
        periode.SelAct = parseInt(value, 10);
    } else if (champs[0].indexOf("MeteoC") >= 0) {
        periode.MeteoCond = parseInt(value, 10);
    } else if (champs[0].indexOf("MeteoS") >= 0) {
        periode.MeteoSeuil = Math.max(0, Math.round(parseFloat(value) * 10) || 0);
    }
}

/**
 * Active le mode édition pour le titre d'une action.
 * @param {number} iAct - Index de l'action.
 */
function editTitre(iAct) {
    const titreElement = GID("titre" + iAct);
    if (titreElement && titreElement.innerHTML.indexOf("<input") === -1) {
        GH("titre" + iAct, "<input type='text' value='" + titreElement.innerHTML + "' id='Etitre" + iAct + "' onblur='TitreValid(" + iAct + ")'>");
        const input = GID("Etitre" + iAct);
        if (input) input.focus();
    }
}

/**
 * Valide le nouveau titre et réaffiche le label.
 * @param {number} iAct - Index de l'action.
 */
function TitreValid(iAct) {
    const input = GID("Etitre" + iAct);
    if (input) {
        F.Actions[iAct].Titre = input.value.trim();
        GH("titre" + iAct, F.Actions[iAct].Titre);
    }
}

/**
 * Vérifie et applique l'état "disabled" ou "visible" aux éléments
 * en fonction du mode d'action (`Actif`) et du choix de sortie (GPIO ou Externe).
 */
function checkDisabled() {

    const show = (id, disp) => GID(id).style.display = disp;
    const val  = id => GID(id).value;
    const chk  = id => GID(id).checked;

    for (let iAct = 0; iAct < F.Actions.length; iAct++) {

        // --- Détermination du mode actif ---
        for (let m = 0; m <= 3; m++) {
            if (chk(`radio${iAct}-${m}`)) {
                F.Actions[iAct].Actif = m;
            }
        }

        const pinVal = parseInt(val(`selectPin${iAct}`), 10);

        // Forçage si pas de pin : seul le On/Off (commande externe) est possible
        if (pinVal === -1 && F.Actions[iAct].Actif > 1) {
            F.Actions[iAct].Actif = 1;
            GID(`radio${iAct}-1`).checked = true;
        }

        // Mise à jour planning
        TracePeriodes(iAct);

        const actif = F.Actions[iAct].Actif;
        const actifVisible = (actif > 0) ? "block" : "none";

        show(`blocPlanning${iAct}`, actifVisible);
        show(`visu${iAct}`, actifVisible);
        show(`forceOuvre${iAct}`, actifVisible);

        // Mode On/Off simple → pas de visu/graph
        if (actif === 1) {
            show(`graphAction${iAct}`, "none");
            show(`visu${iAct}`, "none");
            show(`forceOuvre${iAct}`, "none");
        }

        // Zones Host/Port/Repet
        let hostDisp = (actif === 1) ? "block" : "none";
        show(`Tempo${iAct}`, hostDisp);

        let disable = (pinVal < 0);
        let disp = disable ? "block" : "none";

        if (!disable) {
            hostDisp = "none";
        }

        show(`SelectOut${iAct}`, (pinVal <= 0) ? "none" : "inline-block");
        show(`Host${iAct}`, hostDisp);
        show(`Port${iAct}`, hostDisp);
        show(`Repet${iAct}`, hostDisp);

        // Désactivation des modes proportionnels si pas de pin
        for (const mode of [2, 3]) {
            const r = GID(`radio${iAct}-${mode}`);
            if (r) r.disabled = disable;
        }

        show(`ordreoff${iAct}`, disp);
        show(`ordreon${iAct}`, disp);

        // Correction d'ordreOn si IS présent alors qu'aucune pin
        const ordreOn = GID(`ordreOn${iAct}`);
        if (ordreOn && pinVal === -1 && ordreOn.value.indexOf(IS) > 0) {
            ordreOn.value = "";
        }

        // Ligne basse
        show(
            `ligne_bas${iAct}`,
            (actif === 1 && pinVal <= 0) ? "flex" : "none"
        );

        // Sliders PID
        show(
            `fen_slide${iAct}`,
            (actif === 1) ? "none" : "table"
        );

        // PID
        const pid = GID(`PID${iAct}`);
        if (F.ModePara === 0 && pid) pid.checked = false;

        show(
            `PIDbox${iAct}`,
            (F.ModePara === 0 || pinVal <= 0 || actif === 1)
                ? "none" : "block"
        );

        const pidActive = pid && pid.checked;
        show(`Propor${iAct}`, pidActive ? "table-row" : "none");
        show(`Derive${iAct}`, pidActive ? "table-row" : "none");
    }
}


/**
 * Etabli les configurations d'actions 
 */
function TraceActions(rajout) {

            if (F.Actions.length == 0) {  //Au moins une action
                F.Actions[0] = CreerAction(0, "Chauffe-eau (SSR)");
            }
            let Imax=F.Actions.length;
            if (rajout) F.Actions[Imax] = CreerAction(Imax, "Action " + Imax);

            var S = "";
            for (var i = 0; i < F.Actions.length; i++) {
                S += "<div id='planning" + i + "' class='planning' ></div>";
            }


            if (F.Actions.length<10) {
                S += "<input id='butR' type='button'  class='tbut' value='+' onclick='TraceActions(true);' title='Rajouter une action (mode expert).'>";
                S += "<input id='butA' type='button' class='tbut tbutWide' value='&#129668; Assistant' onclick='OuvrirAssistant();' title='Création guidée d&apos;une automatisation.'>";
            }
            GH("plannings", S);
            for (var iAct = 0; iAct < F.Actions.length; iAct++) {
                TracePlanning(iAct);
            }
            checkDisabled();
 
}

/**
 * Envoie les coefficients PID au serveur en temps réel (uniquement si PlotIdx >= 0).
 * Ces coefficients ne sont pas enregistrés.
 * @param {number} iAct - Index de l'action.
 */
function UpdateK(iAct) {
    if (PlotIdx >= 0) {
        const Kp = Math.floor(GID("sliderKp" + iAct).value);
        const Ki = Math.floor(GID("sliderKi" + iAct).value);
        const Kd = Math.floor(GID("sliderKd" + iAct).value);

        const xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                // var retour = this.responseText; // Le retour n'est pas utilisé
            }
        };

        xhttp.open('GET', `/UpdateK?iAct=${iAct}&Kp=${Kp}&Ki=${Ki}&Kd=${Kd}`, true);
        xhttp.send();
    }
}

/**
 * Assistant de création guidée d'une automatisation.
 */
function OuvrirAssistant() {
    if (F.Actions.length >= 10) { alert("Maximum de 10 actions atteint."); return; }
    let S = "<div class='selectZ'>&#129668; Nouvelle automatisation<div class='closeZ' onclick='FermerAssistant()'>X</div></div>";
    S += "<div class='wizBody'>";

    // 1. Équipement
    S += "<div class='bord1px'><b>1. L'&eacute;quipement &agrave; piloter</b>";
    S += "<div class='wizLigne'>Nom : <input type='text' id='wizNom' value='Chauffe-eau' style='width:150px;'></div>";
    S += "<div class='wizLigne'>Sortie : <select id='wizType'>";
    S += "<option value='2'>SSR &mdash; puissance variable (chauffe-eau)</option>";
    S += "<option value='1'>Relais &mdash; tout ou rien</option></select></div>";
    S += "<div class='wizLigne'>Gpio : <select id='wizPin'>";
    for (let i = 0; i < Pins.length; i++) {
        if (Pins[i] > 0) S += "<option value='" + Pins[i] + "'>gpio:" + Pins[i] + "</option>";
    }
    S += "</select></div></div>";

    // 2. Routage du surplus
    S += "<div class='bord1px'><b>2. Router le surplus solaire</b>";
    S += "<div class='wizLigne'><input type='checkbox' id='wizRout' checked> Envoyer le surplus dans l'&eacute;quipement</div>";
    S += "<div class='wizLigne'>De <input type='time' id='wizRoutD' value='09:00'> &agrave; <input type='time' id='wizRoutF' value='17:00'></div></div>";

    // 3. Forçage conditionnel
    S += "<div class='bord1px'><b>3. Marche forc&eacute;e (optionnel)</b>";
    S += "<div class='wizLigne'><input type='checkbox' id='wizForce'> Forcer la marche sur une plage horaire</div>";
    S += "<div class='wizLigne'>De <input type='time' id='wizForceD' value='02:00'> &agrave; <input type='time' id='wizForceF' value='06:00'></div>";
    S += "<div class='wizLigne'>Seulement si temp&eacute;rature <select id='wizTcanal'><option value='-1'>(ignorer)</option>";
    if (V.temperature) {
        for (let c = 0; c < V.temperature.length; c++) {
            if (V.temperature[c] > -100) S += "<option value='" + c + "'>canal " + c + " (" + V.temperature[c] + "&deg;)</option>";
        }
    }
    S += "</select> &le; <input type='number' id='wizTseuil' value='50' style='width:60px;'> &deg;C</div>";
    if (V.MeteoOn == 1) {
        if (V.BallonCanal >= 0) {  //Ballon configuré : choix entre 3 modes météo
            S += "<div class='wizLigne'>M&eacute;t&eacute;o : <select id='wizMeteoMode' onchange='WizMeteoMode()'>";
            S += "<option value='0'>pas de condition m&eacute;t&eacute;o</option>";
            S += "<option value='1'>si pr&eacute;vision solaire demain &lt; seuil</option>";
            S += "<option value='5' selected>Adaptatif ballon (auto-apprentissage)</option>";
            S += "</select></div>";
            S += "<div class='wizLigne' id='wizMeteoSeuilZ' style='display:none;'>Seuil : <input type='number' id='wizMeteoS' value='8' step='0.5' style='width:60px;'> kWh</div>";
            let etatBallon = "";
            if (V.BallonBesoin >= 0) etatBallon = "<br>Actuellement : besoin " + V.BallonBesoin + " kWh, surplus pr&eacute;vu " + V.BallonSurplus + " kWh";
            S += "<div class='wizLigne' id='wizMeteoInfo'><small>Forcer uniquement si le besoin du ballon d&eacute;passe le surplus solaire pr&eacute;vu (auto-apprentissage)." + etatBallon + "</small></div>";
        } else {
            S += "<div class='wizLigne'><input type='checkbox' id='wizMeteo' checked> Seulement si pr&eacute;vision solaire demain &lt; <input type='number' id='wizMeteoS' value='8' step='0.5' style='width:60px;'> kWh</div>";
        }
    }
    if (V.LTARFbin > 0 && V.LTARFbin <= 3) {
        S += "<div class='wizLigne'><input type='checkbox' id='wizHC' checked> Seulement en Heures Creuses</div>";
    } else if (V.LTARFbin > 3) {
        S += "<div class='wizLigne'><input type='checkbox' id='wizPasRouge' checked> Pas les jours Tempo Rouge</div>";
    }
    S += "</div>";

    S += "<div style='text-align:center;padding:8px;'><input type='button' class='bouton' value='Cr&eacute;er l&apos;automatisation' onclick='CreerWizard()'></div>";
    S += "</div>";
    GH("assistantBox", S);
    GID("assistant").style.display = "flex";
}

function FermerAssistant() {
    GID("assistant").style.display = "none";
}

// Affichage du seuil / de l'état ballon selon le mode météo choisi dans l'assistant
function WizMeteoMode() {
    const mode = GID("wizMeteoMode").value;
    GID("wizMeteoSeuilZ").style.display = (mode == "1") ? "block" : "none";
    GID("wizMeteoInfo").style.display = (mode == "5") ? "block" : "none";
}

// "09:30" -> 950 (centièmes d'heure)
function WizH(id) {
    const v = GID(id).value.split(":");
    return Math.floor(parseInt(v[0], 10) * 100 + parseInt(v[1], 10) * 100 / 60);
}

function WizPeriode(Hfin, type) {
    return { Hfin: Hfin, Type: type, Vmin: 0, Vmax: 100, ONouvre: 100, Tinf: 1600, Tsup: 1600,
             Hmin: 0, Hmax: 0, CanalTemp: -1, SelAct: 255, Ooff: 0, O_on: 0, Tarif: 31,
             MeteoCond: 0, MeteoSeuil: 0 };
}

function CreerWizard() {
    const iAct = F.Actions.length;
    const act = CreerAction(iAct, GID("wizNom").value.trim() || ("Action " + iAct));
    act.Actif = parseInt(GID("wizType").value, 10);
    act.OrdreOn = GID("wizPin").value + IS + "1";

    // Construit la liste des plages demandées
    const segs = [];
    if (GID("wizRout").checked) {
        const h1 = WizH("wizRoutD"), h2 = WizH("wizRoutF");
        if (h2 > h1) segs.push({ h1: h1, h2: h2, t: 3, c: false });
    }
    if (GID("wizForce").checked) {
        const h1 = WizH("wizForceD"), h2 = WizH("wizForceF");
        if (h1 < h2) {
            segs.push({ h1: h1, h2: h2, t: 2, c: true });
        } else { // plage à cheval sur minuit : 2 segments
            if (h2 > 0) segs.push({ h1: 0, h2: h2, t: 2, c: true });
            if (h1 < 2400) segs.push({ h1: h1, h2: 2400, t: 2, c: true });
        }
    }
    segs.sort((a, b) => a.h1 - b.h1);
    for (let i = 1; i < segs.length; i++) {
        if (segs[i].h1 < segs[i - 1].h2) { alert("Les plages horaires se chevauchent. Corrigez les heures."); return; }
    }

    // Remplit la journée : OFF entre les plages
    const periodes = [];
    let H = 0;
    for (const sg of segs) {
        if (sg.h1 > H) periodes.push(WizPeriode(sg.h1, 1)); // OFF
        const p = WizPeriode(sg.h2, sg.t);
        if (sg.c) { // conditions du forçage
            const canal = parseInt(GID("wizTcanal").value, 10);
            if (canal >= 0) {
                p.CanalTemp = canal;
                p.Tinf = Math.round(parseFloat(GID("wizTseuil").value) * 10);
            }
            const wmm = GID("wizMeteoMode");
            if (wmm) {  //Ballon configuré : select à 3 modes (0=aucun, 1=demain<seuil, 5=adaptatif ballon)
                p.MeteoCond = parseInt(wmm.value, 10) || 0;
                if (p.MeteoCond === 1) p.MeteoSeuil = Math.round(parseFloat(GID("wizMeteoS").value) * 10);
            } else {  //Pas de ballon : checkbox historique
                const wm = GID("wizMeteo");
                if (wm && wm.checked) {
                    p.MeteoCond = 1; // prévision demain < seuil
                    p.MeteoSeuil = Math.round(parseFloat(GID("wizMeteoS").value) * 10);
                }
            }
            const hc = GID("wizHC");
            if (hc && hc.checked) p.Tarif = 2;       // HC uniquement
            const pr = GID("wizPasRouge");
            if (pr && pr.checked) p.Tarif = 12;      // Tempo Bleu + Blanc
        }
        periodes.push(p);
        H = sg.h2;
    }
    if (H < 2400) {
        periodes.push(WizPeriode(2400, 1)); // OFF jusqu'à minuit
    } else if (periodes.length > 0) {
        periodes[periodes.length - 1].Hfin = 2400;
    }
    if (periodes.length === 0) periodes.push(WizPeriode(2400, 1));
    if (periodes.length > 8) { alert("Trop de p&eacute;riodes (max 8). Simplifiez les plages."); return; }

    act.Periodes = periodes;
    act.NbPeriode = periodes.length;
    F.Actions[iAct] = act;
    FermerAssistant();
    TraceActions(false);
    GH("message", "Automatisation cr&eacute;&eacute;e.<br>V&eacute;rifiez puis cliquez <b>Sauvegarder</b>.");
    GID("message").style.display = "block";
    setTimeout(() => { GID("message").style.display = "none"; }, 8000);
    const el = GID("planning" + iAct);
    if (el) el.scrollIntoView({ behavior: "smooth" });
}
)====";

const char * ActionsJS4 = R"====(

function Send_Values(){
  
    GID("attente").style.visibility = "visible"; 

    // 1. Mise à jour du modèle  depuis le DOM
    for (let iAct = 0; iAct < F.Actions.length; iAct++) {
        const action = F.Actions[iAct];

        // Modes 0 à 3 (Inactif, On/Off, Multi-sinus, Train de sinus)
        for (let i = 0; i <= 3; i++) {
            const radio = GID(`radio${iAct}-${i}`);
            if (radio && radio.checked) {
                action.Actif = i;
            }
        }
        
        // Conversion explicite en entier pour les nombres
        action.Action=iAct; //Pour info numéro Action
        action.Titre = GID("titre" + iAct).innerHTML.trim();
        action.Host = GID("host" + iAct).value.trim();
        action.Port = parseInt(GID("port" + iAct).value, 10) || 0; // Sécurité: valeur par défaut 0
        action.OrdreOn = GID("ordreOn" + iAct).value.trim();
        action.OrdreOff = GID("ordreOff" + iAct).value.trim();
        action.Repet = parseInt(GID("repet" + iAct).value, 10) || 0;
        action.Tempo = parseInt(GID("tempo" + iAct).value, 10) || 0;
        action.ForceOuvre = parseInt(GID("ForceOuvre" + iAct).value, 10) || 100;
        
        // Coefficients PID
        action.Kp = parseInt(GID("sliderKp" + iAct).value, 10) || 0;
        action.Ki = parseInt(GID("sliderKi" + iAct).value, 10) || 0;
        action.Kd = parseInt(GID("sliderKd" + iAct).value, 10) || 0;
        action.PID = GID("PID" + iAct).checked ? 1 : 0;
        
        const selectPin = GID("selectPin" + iAct);
        if (selectPin && selectPin.value >= 0) {
            // Reconstruit OrdreOn pour la sortie GPIO/relais (Pin + Sortie)
            const selectOut = GID("selectOut" + iAct);
            action.OrdreOn = selectPin.value + IS + selectOut.value;
        }

         for (let i = 0; i < action.Periodes.length; i++) {
                const periode = action.Periodes[i];
                
                // Logique spécifique au mode Standard (non Routage)
                if (F.ModePara == 0) { 
                    periode.CanalTemp = -1;
                    periode.SelAct = 255;
                }
                
                
            }
        
        // Logique d'effacement de l'action
        if (selectPin && (selectPin.value == 0 || action.Titre === "")) {
            action.Actif = -1; // Marque l'action à effacer
        }
        action.NbPeriode= action.Periodes.length;
    }
    
    // 2. Effacement des Actions inutilisées
    let j=0 ;
    for (let iAct = 0; iAct < F.Actions.length; iAct++) {
        if (F.Actions[iAct].Actif >= 0) { // N'envoie que les actions valides ou actives
            if(j!=iAct) F.Actions[j]=F.Actions[iAct];
            F.Actions[j].Action=j;
            j++;
        }
    }
    
    F.Actions=F.Actions.slice(0,j);
   
    // 3. Récupération des paramètres globaux
    F.NbActions=F.Actions.length;
    // 4. Envoi POST
    fetch("/ParaNew", {
      method: "POST",                     // méthode HTTP
      headers: {
        "Content-Type": "application/json" // indique qu'on envoie du JSON
      },

      body: JSON.stringify(F)          // conversion de l'objet JS en texte JSON

    })
      .then(response => {
        if (!response.ok) throw new Error("Erreur HTTP " + response.status);
        return response.json();             // transforme la réponse JSON en objet JS
      })
      .then(resultat => {
        console.log("Réponse du serveur :", resultat);
        location.reload();
      })
      .catch(error => {
        console.error("Erreur d'envoi :", error);
      });
    
}
function Plot(iAct) {
    for (i = 0; i < F.Actions.length; i++) {
        GID('graphAction' + i).style.display = "none";
    }
    if (PlotIdx != iAct) {
        PlotIdx = iAct;
        GID('graphAction' + iAct).style.display = "block";
        PlotR(iAct);
    } else {
        PlotIdx = -1;
    }


}
function PlotR(iAct) { // Courbes détaillées PID
    if (PlotIdx >= 0) {
        if (!Pause) {
            TabIdx = (TabIdx + 1) % 150;
            Ecart[TabIdx] = parseInt(MesuresPwAct[0]);
            Prop[TabIdx] = parseFloat(MesuresPwAct[1]);
            Integ[TabIdx] = parseFloat(MesuresPwAct[2]);
            Deriv[TabIdx] = parseFloat(MesuresPwAct[3]);
            let retard = Prop[TabIdx] + Integ[TabIdx] + Deriv[TabIdx];
            Retard[TabIdx] = Math.floor(Math.max(Math.min(100, retard), 0));
            Ouvert[TabIdx] = 100 - Retard[TabIdx];
            var cW = "#" + Koul[Coul_W][3];
            var cOuvre = "#" + Koul[Coul_Ouvre][3];
            var cRetard = "#" + Koul[Coul_VA][3];
            var cT = "#" + Koul[Coul_Graphe][1];
            var cP = "#" + Koul[Coul_Temp][3];
            var cI = "#" + Koul[Coul_Temp + 1][3];
            var cD = "#" + Koul[Coul_Temp + 3][3];
            var style = 'background:linear-gradient(#' + Koul[Coul_Graphe][5] + ',#' + Koul[Coul_Graphe][3] + ',#' + Koul[Coul_Graphe][5] + ');border-color:#' + Koul[Coul_Tab][5] + ';';
            var S = "<svg viewbox='0 0 970 240' style='" + style + "' height='240'  >"; //   
            S += "<line x1='50' y1='20' x2='50' y2='220' style='stroke:" + cW + ";stroke-width:2' />";
            S += "<line x1='800' y1='20' x2='800' y2='220' style='stroke:" + cOuvre + ";stroke-width:2' />";
            S += "<line x1='50' y1='220' x2='800' y2='220' style='stroke:" + cT + ";stroke-width:2' />";
            for (var t = 0; t < 75; t = t + 5) {
                let x = 800 - t * 10;
                S += "<text x='" + x + "' y='237' style='font-size:16px;fill:" + cT + ";'>" + t + "</text>";
                S += "<line x1='" + x + "' y1='220' x2='" + x + "' y2='224' style='stroke:" + cT + ";stroke-width:2' />";
            }
            S += "<text x='790' y='218' style='font-size:12px;fill:" + cT + ";'>s</text>";
            S += "<text x='804' y='220' style='font-size:12px;fill:" + cOuvre + ";'>0%</text>";
            S += "<text x='804' y='20' style='font-size:12px;fill:" + cOuvre + ";'>100%</text>";
            S += "<line x1='50' y1='20' x2='800' y2='20' style='stroke:" + cOuvre + ";stroke-width:2' stroke-dasharray='1 4' />";
            S += "<line x1='50' y1='120' x2='800' y2='120' style='stroke:" + cW + ";stroke-width:2' stroke-dasharray='1 4' />";
            S += "<text x='20' y='120' style='font-size:12px;fill:" + cW + ";'>0 W</text>";
            S += "<text x='5' y='20' style='font-size:12px;fill:" + cW + ";'>100 W</text>";
            S += "<text x='1' y='220' style='font-size:12px;fill:" + cW + ";'>-100 W</text>";


            S += "<text x='830' y='40' style='font-size:16px;fill:" + cW + ";'>Ecart..........." + Ecart[TabIdx] + " W</text>";
            S += "<text x='830' y='60' style='font-size:16px;fill:" + cOuvre + ";'>Ouverture....." + Ouvert[TabIdx] + " %</text>";
            S += "<text x='860' y='76' style='font-size:12px;fill:" + cT + ";'>= 100 - Retard</text>";
            S += "<text x='830' y='96' style='font-size:16px;fill:" + cRetard + ";'>Retard........." + Retard[TabIdx] + " %</text>";            
            S += "<text x='830' y='168' style='font-size:16px;fill:" + cI + ";'>Intégral....." + Integ[TabIdx] + "</text>";
            
            if (F.ModePara == 1 && GID("PID" + iAct).checked ){
                S += "<text x='860' y='112' style='font-size:12px;fill:" + cT + ";'>=</text>";
                S += "<text x='830' y='132' style='font-size:16px;fill:" + cP + ";'>Proportion.." + Prop[TabIdx] + "</text>";
                S += "<text x='860' y='148' style='font-size:12px;fill:" + cT + ";'>+</text>";
                S += "<text x='860' y='184' style='font-size:12px;fill:" + cT + ";'>+</text>";
                S += "<text x='830' y='204' style='font-size:16px;fill:" + cD + ";'>Dérivée........" + Deriv[TabIdx] + "</text>";
            }

            let SE = "<polyline points='";
            let SO = "<polyline points='";
            let SR = "<polyline points='";
            let SP = "<polyline points='";
            let SI = "<polyline points='";
            let SD = "<polyline points='";
            for (let i = 0; i < 150; i++) { //toutes les 0.5s
                let j = (TabIdx - i + 150) % 150;
                let x = 800 - i * 5;
                let y = 120 - Ecart[j];
                y = Math.min(y, 220); y = Math.max(y, 20);
                SE += x + "," + y + " ";
                y = 220 - 2 * Ouvert[j];
                SO += x + "," + y + " ";
                y = 220 - 2 * Retard[j];
                SR += x + "," + y + " ";
                y = 120 - Prop[j];
                y = Math.min(y, 220); y = Math.max(y, 20);
                SP += x + "," + y + " ";
                y = 220 - 2 * Integ[j];
                SI += x + "," + y + " ";
                y = 120 - Deriv[j];
                y = Math.min(y, 220); y = Math.max(y, 20);
                SD += x + "," + y + " ";
            }
            SE += "' style='fill:none;stroke:" + cW + ";stroke-width:2' />";
            SO += "' style='fill:none;stroke:" + cOuvre + ";stroke-width:2' />";
            SR += "' style='fill:none;stroke:" + cRetard + ";stroke-width:2' />";
            SP += "' style='fill:none;stroke:" + cP + ";stroke-width:2' />";
            SI += "' style='fill:none;stroke:" + cI + ";stroke-width:2' />";
            SD += "' style='fill:none;stroke:" + cD + ";stroke-width:2' />";
             if (F.ModePara == 1 && GID("PID" + iAct).checked){
                    S += SD  + SP;
             }
            S +=  SI  + SR + SO + SE + "</svg>";
            GID('graphSVG' + iAct).innerHTML = S;

        }
        setTimeout(() => {
            PlotR(iAct);
        }, 500);
    }
}

function ShowAction() {
    
    let Dt = 200; //ms
    if (PlotIdx >= 0) {
        const xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                const retour = this.responseText;
                // Supposons que MesuresPwAct est un tableau global
                MesuresPwAct = retour.split(RS).map(val => isNaN(parseFloat(val)) ? 0 : parseFloat(val)); // Assure la conversion numérique et la sécurité
            }
        };
        xhttp.open('GET', '/ShowAction?NumAction=' + PlotIdx, true);
        xhttp.send();
    } else {
        // Initialisation propre des tableaux si PlotIdx < 0
        MesuresPwAct = [0, 0, 0, 0];
        
        // Initialisation de toutes les valeurs du graphe à zéro
        // (En supposant que Ecart, Ouvert, Retard, Prop, Integ, Deriv sont des tableaux globaux de taille 150)
        const zeros = new Array(150).fill(0);
        Ecart = [...zeros];
        Ouvert = [...zeros];
        Retard = [...zeros];
        Prop = [...zeros];
        Integ = [...zeros];
        Deriv = [...zeros];
        
        Dt = 2000;
    }
    
    setTimeout(() => {
        ShowAction();
    }, Dt);
}

//=============Fin Initialisation  ===============
function SetParaFixe(){
    LoadParaVar();
    Set_Couleurs();
    ShowAction();
}


function SetParaVar() {
    SelectActions = "<option value=255>Non exploité</option>";
    for ( let esp = 0; esp < nomRMS.length; esp++) { //Liste des actions par routeur
        for (var iAct = 0; iAct < nomActions[esp].length; iAct++) {
            var v = esp * 10 + parseInt(nomActions[esp][iAct][0]); //Nombre refletant la référence esp et action
            var T = (esp == 0) ? "" : nomRMS[esp] + " / ";
            SelectActions += "<option value=" + v + ">" + T + nomActions[esp][iAct][1] + "</option>";
            ListeActions[v] = T + nomActions[esp][iAct][1];
        }
    }
    TraceActions(false);
}



function AdaptationSource() {

}
)====";

// ActionsJS5 — append at end of JS_Actions.h
//************************************************
// Refonte Soleo : vue cartes en langage naturel,
// editeur plein ecran, badges live, theme.
//************************************************

const char *ActionsJS5 = R"====(

// ---- Heure courante en echelle Hdeci (0..2400) ----
function _NowHdeci(){
  var d = new Date();
  return d.getHours() * 100 + Math.round(d.getMinutes() / 0.6);
}

// ---- Libelles ----
var _ModeNoms  = ["Inactif", "Tout ou rien", "Multi-sinus", "Train de sinus"];
var _MeteoNoms = ["", "si prevision demain < seuil", "si prevision demain suffisante",
                  "si prevision aujourd'hui < seuil", "si prevision aujourd'hui suffisante",
                  "si besoin ballon non couvert (adaptatif)"];

function _TarifPhrase(t){
  var p = [];
  if (t & 1)  p.push("Heures Pleines");
  if (t & 2)  p.push("Heures Creuses");
  if (t & 4)  p.push("jours Bleus");
  if (t & 8)  p.push("jours Blancs");
  if (t & 16) p.push("jours Rouges");
  return p.length ? "en " + p.join(", ") : "";
}

// ---- ActionToPhrase : description en francais courant ----
function ActionToPhrase(act){
  if (!act) return "";
  if (act.Actif == 0) return "Automatisme inactif";
  var morceaux = [];
  var H0 = 0;
  for (var i = 0; i < act.Periodes.length; i++){
    var p = act.Periodes[i];
    var hd = parseInt(p.Hfin);
    var deb = Hdeci2Hmn(H0);
    var fin = Hdeci2Hmn(hd);
    H0 = hd;
    if (p.Type == 0 || p.Type == 1) continue; // pas de controle / OFF : ignore
    var phrase = "";
    if (p.Type == 3){
      phrase = "Surplus solaire de " + deb + " a " + fin;
    } else if (p.Type == 2){
      phrase = "Marche forcee de " + deb + " a " + fin;
    } else {
      continue;
    }
    var cond = [];
    if (typeof p.CanalTemp != "undefined" && p.CanalTemp >= 0){
      var tinf = parseFloat(p.Tinf), tsup = parseFloat(p.Tsup);
      if (tinf < 1500 && tinf > -500) cond.push("T < " + (tinf/10).toFixed(0) + "deg");
      if (tsup < 1500 && tsup > -500) cond.push("T > " + (tsup/10).toFixed(0) + "deg");
    }
    if (p.MeteoCond > 0 && _MeteoNoms[p.MeteoCond]){
      cond.push(_MeteoNoms[p.MeteoCond]);
    }
    var tarif = parseInt(p.Tarif) || 0;
    if (tarif > 0 && tarif < 31){   // 31 = toutes les cases cochees = pas de restriction
      var tp = _TarifPhrase(tarif);
      if (tp) cond.push(tp);
    }
    if (cond.length) phrase += " — " + cond.join(", ");
    morceaux.push(phrase);
  }
  if (morceaux.length == 0){
    return "Mode " + (_ModeNoms[act.Actif] || "");
  }
  return morceaux.join(" · ");
}

// ---- RenderMiniTimeline : barre coloree 24h + aiguille ----
function RenderMiniTimeline(act){
  var s = "<div class='mini-tl'>";
  if (act && act.Actif != 0){
    var H0 = 0;
    for (var i = 0; i < act.Periodes.length; i++){
      var p = act.Periodes[i];
      var hd = parseInt(p.Hfin);
      var w = ((hd - H0) / 2400) * 100;
      if (w < 0) w = 0;
      var cls = "tl-off";
      if (p.Type == 3) cls = "tl-route";
      else if (p.Type == 2) cls = "tl-on";
      s += "<div class='tl-seg " + cls + "' style='width:" + w.toFixed(2) + "%'></div>";
      H0 = hd;
    }
  }
  var now = (_NowHdeci() / 2400) * 100;
  s += "<div class='tl-now' style='left:" + now.toFixed(2) + "%'></div>";
  s += "</div>";
  return s;
}

// ---- RenderCardView : toutes les actions en cartes ----
function RenderCardView(){
  var cont = GID("action-cards");
  if (!cont) return;
  if (!F || !F.Actions || F.Actions.length == 0){
    cont.innerHTML = "<div class='empty-state'><div class='emo'>☀️</div>" +
      "<div>Aucun automatisme pour l'instant.</div>" +
      "<div>Appuyez sur <b>+ Automatisme</b> pour commencer.</div></div>";
    GH("act-count", "");
    return;
  }
  var s = "";
  var nbActifs = 0;
  for (var i = 0; i < F.Actions.length; i++){
    var act = F.Actions[i];
    if (act.Actif > 0) nbActifs++;
    var icone = (act.Actif == 0) ? "⏸" : (act.Actif == 1 ? "🔌" : "⚡");
    var icls  = (act.Actif == 0) ? "inactive" : (act.Actif == 1 ? "relay" : "routing");
    var sub   = _ModeNoms[act.Actif] || "";
    var titre = act.Titre ? act.Titre : ("Action " + i);
    s += "<div class='act-card' id='actcard" + i + "'>";
    s += "<div class='act-card-head'>";
    s += "<div class='act-icon " + icls + "'>" + icone + "</div>";
    s += "<div><div class='act-name'>" + titre + "</div><div class='act-sub'>" + sub + "</div></div>";
    s += "<div class='act-status status-idle' id='actstatus" + i + "'>⏸ Veille</div>";
    s += "</div>";
    s += RenderMiniTimeline(act);
    s += "<div class='act-phrase'>" + ActionToPhrase(act) + "</div>";
    s += "<div class='act-btns'>";
    if (act.Actif > 0){
      s += "<button class='btn-s btn-force' onclick='ForceAction(" + i + ",1)'>Forcer +30mn</button>";
      s += "<button class='btn-s btn-stop' onclick='ForceAction(" + i + ",-1)'>Stopper 30mn</button>";
    }
    s += "<button class='btn-s' onclick='OuvrirDetail(" + i + ")'>Modifier</button>";
    s += "<span style='flex:1 1 auto'></span>";
    s += "<button class='btn-s' title='Modifier / supprimer dans l editeur' onclick='OuvrirDetail(" + i + ")'>🗑</button>";
    s += "</div>";
    s += "</div>";
  }
  cont.innerHTML = s;
  GH("act-count", nbActifs + " actif" + (nbActifs > 1 ? "s" : ""));
}

// ---- Wrap TraceActions pour rafraichir les cartes ----
var _TA = typeof TraceActions === 'function' ? TraceActions : null;
if (_TA) window.TraceActions = function(r){ _TA(r); RenderCardView(); };

// ---- Bascule vue cartes <-> editeur ----
function OuvrirDetail(iAct){
  GID("cards-section").style.display = "none";
  GID("editor-section").style.display = "block";
  var fab = GID("fab"); if (fab) fab.style.display = "none";
  var sb = GID("save-bar"); if (sb) sb.style.display = "none";
  var pls = document.querySelectorAll(".planning");
  for (var k = 0; k < pls.length; k++) pls[k].classList.remove("active-edit");
  if (iAct >= 0){
    var el0 = GID("planning" + iAct);
    if (el0) el0.classList.add("active-edit");
  }
  setTimeout(function(){
    var el = GID("planning" + iAct);
    if (el) el.scrollIntoView({behavior:"smooth", block:"start"});
  }, 100);
}

function FermerDetail(){
  GID("cards-section").style.display = "block";
  GID("editor-section").style.display = "none";
  var fab = GID("fab"); if (fab) fab.style.display = "block";
  RenderCardView();
}

// ---- Forcage marche / arret ----
function ForceAction(iAct, dir){
  fetch("/ajax_etatActions?Force=" + dir + "&NumAction=" + iAct)
    .then(function(r){ return r.text(); })
    .catch(function(){})
    .finally(function(){ setTimeout(PollStatuses, 500); });
}

// ---- PollStatuses : rafraichit les badges live ----
function PollStatuses(){
  fetch("/ajax_etatActions?Force=0&NumAction=0")
    .then(function(r){ return r.text(); })
    .then(function(txt){
      var champs = txt.split(GS);
      // champs[0..3] = temps, source, RMSextIP, NbActifs ; puis 1 par action
      for (var c = 4; c < champs.length; c++){
        var d = champs[c].split(RS);
        if (d.length < 3) continue;
        var idx = parseInt(d[0]);
        if (isNaN(idx)) continue;
        var state = d.length > 2 ? d[2] : "";   // d[0]=idx d[1]=titre d[2]=state d[3]=tOnOff
        var tOnOff = d.length > 3 ? parseFloat(d[3]) : 0;
        var badge = GID("actstatus" + idx);
        var card = GID("actcard" + idx);
        if (!badge) continue;
        var cls = "act-status status-idle";
        var lbl = "⏸ Veille";
        var actifCard = false;
        if (tOnOff > 0){
          cls = "act-status status-forced";
          lbl = "🔥 Force " + Math.round(tOnOff) + "mn";
          actifCard = true;
        } else if (tOnOff < 0){
          cls = "act-status status-idle";
          lbl = "⏹ Stop " + Math.round(-tOnOff) + "mn";
        } else if (state == "On"){
          cls = "act-status status-on status-routing";
          lbl = "⚡ Marche";
          actifCard = true;
        } else if (state == "Off"){
          cls = "act-status status-idle";
          lbl = "⏸ Arret";
        } else if (state !== "" && !isNaN(parseFloat(state))){
          cls = "act-status status-routing";
          lbl = "☀️ " + Math.round(parseFloat(state)) + "%";
          actifCard = parseFloat(state) > 0;
        }
        badge.className = cls;
        badge.innerHTML = lbl;
        if (card){
          if (actifCard) card.classList.add("active-card");
          else card.classList.remove("active-card");
        }
      }
    })
    .catch(function(){})
    .finally(function(){ setTimeout(PollStatuses, 4000); });
}

// ---- Theme ----
function toggleTheme(){
  var el = document.documentElement;
  var cur = el.getAttribute("data-theme") == "light" ? "dark" : "light";
  el.setAttribute("data-theme", cur);
  try { localStorage.setItem("soleo-theme", cur); } catch(e){}
}
(function(){
  try {
    var t = localStorage.getItem("soleo-theme");
    if (t) document.documentElement.setAttribute("data-theme", t);
  } catch(e){}
})();

// ---- Override Init : doit etre defini APRES l'original ----
window.Init = function(){
  LoadParaFixe();
  setTimeout(PollStatuses, 3000);
};

)====";
