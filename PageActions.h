//************************************************
// Page HTML de gestion des Actions
//************************************************

const char *ActionsHtml = R"====(
<!doctype html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Actions</title>

    <script src="/PinsActionsJS"></script>
    <script src="/CommunCouleurJS"></script>
    <link rel="stylesheet" href="/commun.css">

    <style>
        /* ── Variables ─────────────────────────────────────── */
        :root{
            --acc:#4ab3f4; --acc2:#7dcfff;
            --card:rgba(28,28,44,0.55);
            --card-border:#3a3a5a;
            --rad:10px;
            --popup-bg:#1e2030;
            --popup-border:#4a4a6a;
        }

        /* ── Layout ─────────────────────────────────────────── */
        .Bactions { border: inset 8px azure; }
        .cadre { width:100%; max-width:1200px; margin:auto; padding:0 6px; }
        .form { width:100%; text-align:left; }
        .form2 { margin:auto; padding:5px; display:table; text-align:left; width:100%; }

        /* ── Titres de section ───────────────────────────────── */
        h4 { padding:6px 4px; margin:8px 0 2px; color:#cce; border-left:4px solid var(--acc); padding-left:10px; }
        h5 { padding:4px 4px; margin:6px 0 2px; color:#aac; border-left:3px solid #557; padding-left:8px; }
        label { text-align:right; }
        tr { margin:2px; text-align:left; font-size:20px; }

        /* ── Carte planning ─────────────────────────────────── */
        .planning {
            width:100%; position:relative; margin:4px 0 12px;
            padding:6px; border:1px solid var(--card-border);
            background:var(--card); color:#222;
            border-radius:var(--rad);
            box-shadow:0 3px 12px rgba(0,0,0,.4);
        }

        /* ── Titre cliquable de l'action ────────────────────── */
        .titre {
            display:flex; justify-content:center;
            cursor:pointer; color:#e8e8ff;
            font-weight:700; font-size:115%;
            padding:6px 0; letter-spacing:.5px;
        }
        .titre:hover { color:var(--acc2); }
        .visu { position:absolute; top:4px; right:6px; cursor:pointer; font-size:120%; opacity:.7; }
        .visu:hover { opacity:1; }

        /* ── Zone mode (radio buttons) ──────────────────────── */
        .mode {
            display:flex; flex-wrap:wrap; gap:4px;
            font-size:14px; text-align:center;
            background:rgba(20,20,36,.6);
            border-radius:8px; margin:4px 0; padding:6px 4px;
            border:1px solid #333;
        }
        .mode>div { display:flex; align-items:center; gap:4px; padding:3px 8px;
            border:1px solid #444; border-radius:20px; background:rgba(50,50,70,.5);
            cursor:pointer; transition:background .15s,border-color .15s;
            color:#ccc; white-space:nowrap; }
        .mode>div:has(input:checked) { background:rgba(74,179,244,.2); border-color:var(--acc); color:#fff; }
        .mode>div>input[type=radio] { width:14px; cursor:pointer; accent-color:var(--acc); }
        .TitZone { font-size:11px; font-style:italic; font-weight:700; color:#aaa; white-space:nowrap; }

        /* ── Sortie / Externe ───────────────────────────────── */
        .les_select {
            display:flex; flex-wrap:wrap; gap:8px;
            font-size:14px; text-align:left;
            margin:4px 0; padding:8px;
            border:1px solid #333; background:rgba(20,20,36,.5);
            color:#ccc; border-radius:8px;
            align-items:flex-end;
        }
        .les_select>div { display:flex; flex-direction:column; gap:2px; }
        .les_select input,
        .les_select select { margin:2px 0; font-size:14px; max-width:150px; }

        /* ── Bouton +/- périodes ────────────────────────────── */
        .boutons { display:inline-flex; gap:4px; }
        .tbut {
            width:36px; height:36px; padding:0;
            text-align:center; font-weight:700;
            font-size:22px; cursor:pointer;
            display:inline-flex; align-items:center; justify-content:center;
            border:2px solid #555; border-radius:8px;
            background:linear-gradient(135deg,#3a3a5e,#252540);
            color:#dde; transition:background .15s,border-color .15s,transform .1s;
        }
        .tbut:hover { background:linear-gradient(135deg,#4a4a7e,#353560); border-color:var(--acc); }
        .tbut:active { transform:scale(.95); }

        /* ── PID sliders ────────────────────────────────────── */
        .bouton_curseur {
            display:flex; flex-wrap:wrap; gap:8px;
            font-size:14px; text-align:center;
            margin:4px 0; padding:8px;
            border:1px solid #333; background:rgba(20,20,36,.5);
            color:#ccc; border-radius:8px; align-items:center;
        }
        .slideTriac { width:100%; position:relative; display:table; }
        .slideTriacIn { display:table-row; width:100%; }
        .Tcell1 { display:table-cell; width:36%; text-align:right; padding-right:8px; color:#bbb; font-size:13px; vertical-align:middle; }
        .Tcell2 { display:table-cell; width:56%; vertical-align:middle; }
        .Tcell3 { display:table-cell; width:8%; text-align:left; padding-left:8px; font-weight:700; color:var(--acc); vertical-align:middle; font-size:15px; }
        input[type=range] { pointer-events:none; accent-color:var(--acc); }
        input[type=range]::-webkit-slider-thumb { pointer-events:all; cursor:ew-resize; }
        input[type=range]::-moz-range-thumb { pointer-events:all; cursor:ew-resize; }

        /* ── Barre de timeline (curseurs) ───────────────────── */
        .curseur { position:relative; width:100%; height:34px; }
        .periode {
            position:absolute; height:100%;
            border:2px solid rgba(255,255,255,.3); border-radius:5px;
            display:flex; justify-content:space-between; align-items:center;
            color:#fff; font-size:12px;
            box-shadow:inset 0 1px 0 rgba(255,255,255,.15);
        }
        .Hfin { position:absolute; bottom:2px; right:3px; font-size:11px; opacity:.85; }
        .handleStyle {
            position:absolute; right:-14px; top:50%; transform:translateY(-50%);
            width:28px; height:36px;
            background:rgba(255,255,255,.9); border:2px solid #333; border-radius:7px;
            cursor:ew-resize; display:flex; align-items:center; justify-content:center;
            font-size:13px; font-weight:700; color:#333; z-index:10;
            box-shadow:0 2px 6px rgba(0,0,0,.4); touch-action:none; user-select:none;
            transition:background .1s;
        }
        .handleStyle:hover { background:#ffe680; border-color:#f57c00; }

        /* ── Zone info cliquable ─────────────────────────────── */
        .infoAction {
            position:relative; display:flex; width:100%;
            min-height:44px; font-size:13px; flex-wrap:wrap;
        }
        .infoZone {
            position:relative; display:flex; flex-direction:column;
            justify-content:center; align-items:center;
            border:1px solid #999; border-radius:5px;
            text-align:center;
            background:rgba(220,220,220,.88); cursor:pointer;
            padding:2px 4px; transition:filter .15s;
            font-size:12px;
        }
        .infoZone:hover { filter:brightness(1.1); }
        .infoZone div { display:inline-block; }

        /* ── Modal popup de configuration de période ─────────── */
        .infoZ {
            position:fixed; top:50%; left:50%;
            transform:translate(-50%,-50%);
            display:none;
            border:2px solid var(--popup-border);
            border-radius:12px;
            background:var(--popup-bg);
            color:#e0e0f0;
            z-index:2000;
            font-size:15px;
            min-width:280px; max-width:420px; width:90vw;
            box-shadow:0 8px 32px rgba(0,0,0,.7);
            overflow:hidden;
        }
        .infoZ input { width:72px; margin:3px; font-size:14px; border-radius:5px;
            border:1px solid #555; background:#252538; color:#dde; padding:2px 4px; }
        .infoZ select { font-size:14px; border-radius:5px;
            border:1px solid #555; background:#252538; color:#dde; padding:2px; }

        /* ── En-tête du popup ───────────────────────────────── */
        .selectZ {
            background:linear-gradient(135deg,#3a3a5e,#252550);
            padding:8px 10px; font-size:15px; font-weight:700;
            color:#cce; position:relative; border-bottom:1px solid var(--popup-border);
            display:flex; align-items:center; justify-content:space-between;
        }
        .closeZ {
            background:rgba(255,60,60,.7); border:none;
            color:#fff; cursor:pointer;
            height:24px; width:24px; border-radius:50%;
            font-size:14px; font-weight:700; display:flex; align-items:center; justify-content:center;
            flex-shrink:0; transition:background .15s;
        }
        .closeZ:hover { background:rgba(255,60,60,1); }

        /* ── Choix de type (OFF/ON/Routage/Triac) ───────────── */
        .fcontainer {
            display:flex; flex-wrap:wrap; gap:6px;
            background:rgba(20,20,36,.6);
            padding:8px; border-top:1px solid #333;
        }
        .fcontleft { display:flex; flex-direction:column; gap:6px; }
        .radioC {
            border:2px solid #444; border-radius:8px;
            display:inline-flex; align-items:center; gap:6px;
            padding:5px 10px; text-align:left;
            background:rgba(50,50,70,.5);
            transition:border-color .15s,background .15s;
            cursor:pointer;
        }
        .radioC:has(input:checked) { border-color:var(--acc); background:rgba(74,179,244,.15); }
        .radioC input[type=radio] { width:16px; cursor:pointer; accent-color:var(--acc); }

        /* ── Couleurs des blocs de type ─────────────────────── */
        .zOff  { background:rgba(80,80,200,.25); border:1px solid #66f; margin:4px; padding:6px 8px; border-radius:8px; }
        .zOn   { background:rgba(200,60,60,.25); border:1px solid #f77; margin:4px; padding:6px 8px; border-radius:8px; }
        .zPw   { background:rgba(60,180,60,.25);  border:1px solid #6f6; margin:4px; padding:6px 8px; border-radius:8px; }
        .zTriac{ background:rgba(200,180,40,.25); border:1px solid #dd6; margin:4px; padding:6px 8px; border-radius:8px; }
        .zOff small,.zOn small,.zPw small,.zTriac small { opacity:.8; }

        /* ── Conditions optionnelles ─────────────────────────── */
        .bord1px {
            border:1px solid #445; margin:5px 4px;
            padding:6px; border-radius:6px;
            background:rgba(30,30,50,.5);
        }
        .minmax { display:flex; flex-wrap:wrap; gap:6px; align-items:center; padding:4px 0; }
        .minmax>div { margin:0; }
        .tm { width:62px; text-align:left; }
        .ligne { display:table-row; padding:10px; }

        /* ── Graphique temps réel ────────────────────────────── */
        .graphAction { position:relative; font-size:14px; display:none; margin:4px; }
        .graphSVG { width:100%; height:100%; }
        .GraphSVG {
            position:absolute; bottom:8px; right:70px;
            cursor:pointer; border:2px solid #666;
            background:rgba(255,255,255,.9); width:28px; height:28px;
            text-align:center; border-radius:5px;
            display:flex; align-items:center; justify-content:center;
        }

        /* ── Message flottant ───────────────────────────────── */
        #message {
            position:fixed; border:2px solid #555; border-radius:8px;
            top:8px; right:8px;
            background:rgba(20,20,40,.95); backdrop-filter:blur(4px);
            color:#dde; font-size:15px;
            display:none; text-align:left; padding:8px 12px;
            box-shadow:0 4px 16px rgba(0,0,0,.5);
            max-width:300px;
        }

        /* ── SVG ────────────────────────────────────────────── */
        svg { border:1px solid #445; border-radius:6px; width:100%; }

        /* ── Responsive ─────────────────────────────────────── */
        @media(max-width:600px){
            .mode>div { padding:3px 6px; font-size:13px; }
            .les_select { gap:6px; }
            .infoZ { font-size:14px; }
            .slideTriac { display:block; }
            .slideTriacIn { display:flex; flex-direction:column; gap:2px; margin-bottom:4px; }
            .Tcell1,.Tcell2,.Tcell3 { display:block; width:100%; text-align:left; padding:0; }
            .Tcell2 input[type=range] { width:100% !important; }
        }
    </style>
</head>

<body onload="Init();" onmouseup="mouseClick=false;">
    <div class="cadre">
        <div id="lesOnglets"></div>

        <h4>Planning des Routages <small>(suivant <span id="nomSondeMobile">sonde Maison</span>)</small></h4>

        <h5>Actions (SSR, relais, commandes externes)</h5>
        <div class="form">
            <div id="plannings"></div>
        </div>

        <div id="BoutonsBas">
            <br>
            <input class="bouton" type="button" onclick="Send_Values();" value="Sauvegarder">
            <div class="lds-dual-ring" id="attente"></div>
            <input class="bouton" type="button" onclick="Reset();" value="ESP32 Reset">
        </div>
    </div>

    <div id="message"></div><br>
    <div id="pied"></div>

    <script src="/ParaCommunJS"></script>
    <script src="/ActionsJS1"></script>
    <script src="/ActionsJS2"></script>
    <script src="/ActionsJS3"></script>
    <script src="/ActionsJS4"></script>
</body>
</html>

)====";