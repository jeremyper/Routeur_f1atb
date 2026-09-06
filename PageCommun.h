const char *ParaCleHtml = R"====(
<!doctype html>
<html>

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="stylesheet" href="/commun.css">
  <style>
    body {
      color: white;
    }

    .form {
      margin: auto;
      padding: 10px;
      display: table;
      text-align: left;
      width: 100%;
    }

    .ligne {
      display: table-row;
      padding: 10px;
    }

    .cadre {
      border-top: 1px solid azure;
    }

    label,
    .nomR {
      display: table-cell;
      margin: 5px;
      text-align: left;
      font-size: 20px;
      height: 25px;
      width: 60%;
    }

    input {
      display: table-cell;
      margin: 5px;
      text-align: left;
      font-size: 20px;
      height: 25px;
    }

    .boldT {
      text-align: left;
      font-weight: bold;
      padding: 10px;
    }

    .Bparametres {
      border: inset 10px azure;
    }

    .Bgeneraux {
      border: inset 4px azure;
    }

    #BoutonsBas {
      text-align: center;
    }

    .Zone {
      width: 100%;
      border: 1px solid grey;
      border-radius: 10px;
      margin-top: 10px;
      background-color: rgba(30, 30, 30, 0.3);
    }

    #onglets2 {
      display: none;
    }
  </style>
  <script>
    let BordsInverse = [".Bparametres"];
    function SendCle() {
      document.cookie = "CleAcces=" + encodeURIComponent(GID("CleAcces").value.trim()) + ";path=/";

      GID("attente").style = "visibility: visible;";
      let xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
          let retour = this.responseText;
          location.reload();
        }
      };
      xhttp.open('GET', '/CleUpdate', true);
      xhttp.send();
    }
    function init() {
      SetHautBas();
      LoadParaVar();
      Set_Couleurs();
      document.cookie = "CleAcces=" + encodeURIComponent(GID("CleAcces").value.trim()) + ";path=/";
    }
    function AdaptationSource() { };
    function SetParaVar() { };
  </script>
  <title>Passe Accès Routeur</title>
</head>

<body onload="init();">
  <div id="lesOnglets"></div>
  <h4>Mot de passe d'accès</h4>
  <div class="Zone">
    <div class="boldT">Sécurité d'accès aux paramètres et Actions</div>
    <div class="form">
      <div class="ligne">
        <label for="CleAcces">Entrez le mot de passe d'accès : </label>
        <input type="text" name="CleAcces" id="CleAcces">
      </div>
    </div>
  </div>
  <div id="BoutonsBas">
    <br><input class="bouton" type="button" onclick="SendCle();" value="Envoyer">
    <div class="lds-dual-ring" id="attente"></div>
  </div>
  <br>
  <div id="pied"></div>
  <script src="/ParaCommunJS"></script>
  <script src="/CommunCouleurJS"></script>
</body>

</html>
)====";

const char *CommunCSS = R"====(
:root{--acc:#4ab3f4;--acc2:#7dcfff;--btn-bg:linear-gradient(135deg,#3a3a5e,#252540);--btn-bg2:linear-gradient(135deg,#4a4a7e,#353560);--rad:8px}
*,*::before,*::after{box-sizing:border-box}
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif}
a:link{color:var(--acc);text-decoration:none}
a:visited{color:#a0c8f0;text-decoration:none}
a:hover{color:var(--acc2);text-decoration:underline}
.onglets{margin-top:4px;display:flex;flex-wrap:wrap;gap:4px;font-size:105%;align-items:center}
#onglets2{margin-top:6px;display:none;flex-wrap:wrap;gap:4px;font-size:85%}
.Bonglet{border:2px solid #444;background:linear-gradient(135deg,#2a2a3e,#1e1e32);border-radius:6px;padding:4px 14px;display:inline-block;transition:background .15s,border-color .15s}
.Bonglet:hover{background:linear-gradient(135deg,#3a3a5e,#2a2a48);border-color:var(--acc)}
.Bonglet2{border:1px solid #444;background:#1e1e30;border-radius:5px;padding:3px 10px;display:inline-block;transition:background .15s}
.Bonglet2:hover{background:#2a2a3e}
.Bheure{display:inline-block}
#pied{display:flex;justify-content:space-between;font-size:13px;color:#888;padding:6px 2px;border-top:1px solid #333;margin-top:8px}
.fsize12{font-size:12px;height:16px}
.fsize10{font-size:10px;height:14px}
.fsize8{font-size:8px}
.tableau{background:#fff;display:inline-block;margin:auto;padding:6px;color:#000;border:2px solid #888;border-radius:10px;box-shadow:0 2px 8px rgba(0,0,0,.35)}
table{border-collapse:collapse}
.lds-dual-ring{color:#e6e655;visibility:hidden;display:inline-block;width:32px;height:32px;vertical-align:middle}
.lds-dual-ring,.lds-dual-ring::after{box-sizing:border-box}
.lds-dual-ring::after{content:" ";display:block;width:24px;height:24px;margin:4px;border-radius:50%;border:4px solid currentColor;border-color:currentColor transparent currentColor transparent;animation:spin 1s linear infinite}
@keyframes spin{to{transform:rotate(360deg)}}
.bouton,input[type=file]::file-selector-button{margin:4px;font-size:15px;height:36px;padding:0 16px;border:2px solid #555;border-radius:var(--rad);cursor:pointer;background:var(--btn-bg);color:#dde;font-weight:500;transition:background .15s,border-color .15s,transform .1s}
.bouton:hover,input[type=file]::file-selector-button:hover{background:var(--btn-bg2);border-color:var(--acc)}
.bouton:active{transform:translateY(1px)}
@media(max-width:600px){.onglets{font-size:90%}.Bonglet{padding:4px 8px}}
)====";

const char *CouleurDefaut = R"====(ffffff77b5fe000033ccccff333333808080000000ffffff808080000000cccccc888888ff444400ffffffff66ffaa88aaffaaaaffeeeeeebbffffff66666633333300ff00aaff0000ffaaaaffaaff883333ffaa6688ffaaff11)====";
