//*********************************************
// Page principale HTML
//*********************************************
const char *MainHtml = R"====(
<!doctype html>
<html lang="fr">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="stylesheet" href="/commun.css">
  <title>RMS F1ATB</title>

  <style>
    .grid-container2,
    .grid-container2M {
      display: grid;
      grid-template-columns: repeat(6, auto);
      gap: 2px;
      background-color: rgba(30, 30, 30, 0.3);
      padding: 10px;
      border: 2px inset grey;
      border-radius: 10px;
      text-align: right;
      color: black;
    }

    .grid-container1 {
      display: grid;
      grid-template-columns: repeat(4, auto);
      gap: 2px;
      background-color: rgba(30, 30, 30, 0.3);
      padding: 10px;
      border: 2px inset grey;
      border-radius: 10px;
      text-align: right;
      color: black;
    }

    .grid-container1 > div,
    .grid-container2 > div,
    .grid-container2M > div {
      padding: 10px;
    }

    .item1 {
      grid-column: 2 / span 2;
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item2 {
      grid-column: 4 / span 2;
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item3 {
      grid-column-start: 6;
      background-color: rgba(62, 194, 247, 0.5);
    }

    .item31 {
      grid-column-start: 4;
      background-color: rgba(62, 194, 247, 0.5);
    }

    .item4 {
      grid-column-start: 2;
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item5,
    .item6,
    .item7,
    .item14 {
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item8 {
      grid-column-start: 1;
      text-align: left;
    }

    .item_Act {
      grid-column: 1 / span 2;
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item_H {
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item_val {
      background-color: rgba(200, 200, 200, 0.5);
    }

    .item_F {
      background-color: rgba(250, 250, 200, 0.5);
    }

    .item_Force {
      grid-column: 4 / span 3;
      background-color: rgba(62, 194, 247, 0.5);
      font-weight: bold;
    }

    .item_temp_nom {
      grid-column: 1 / span 2;
      background-color: #8f8;
    }

    .item_temp_val {
      grid-column: 3 / span 4;
      background-color: #8f8;
    }

    .le { text-align: left; }
    .ce { text-align: center; }

    .Wh { background-color: #ff8; }
    .W { background-color: #f88; }
    .VA { background-color: #0ff; }
    .deg { background-color: #fdf; }

    #foot {
      font-size: 16px;
      display:none;
    }

    svg {
      border: 2px inset grey;
      border-radius: 10px;
    }

    #LED {
      position: absolute;
      top: 4px;
      left: 4px;
      width: 0;
      height: 0;
      border: 5px solid red;
      border-radius: 5px;
    }

    .Baccueil {
      border: inset 8px azure;
    }

    .jauge {
      background-color: #ff8;
      height: 28px;
      position: absolute;
      top: 4px;
      left: 4px;
      text-align: left;
      overflow: visible;
    }

    .jaugeBack {
      background-color: rgb(0, 195, 255);
      width: 208px;
      height: 36px;
      position: relative;
      padding: 4px;
    }

    .w100 {
      width: 100%;
      position: absolute;
      top: 4px;
      left: 4px;
    }

    #TabMesures,
    #SVG_PW48hT,
    #SVG_PW2sT,
    .SVG_Temp48h,
    #SVG_Ouvertures,
    #SVG_Ouvertures_2s {
      display: none;
    }

    #donneeDistante {
      font-size: 50%;
      color: white;
      text-align: center;
      display: none;
      margin-bottom: 10px;
    }

    #info {
      position: absolute;
      border-left: 1px solid black;
      display: none;
    }

    #info_txt {
      position: absolute;
      background: rgba(120,120,120,0.7);
      padding: 4px;
      right: 10px;
      border: 1px solid black;
      text-align: right;
    }

    #couleurTarif_jour,
    #couleurTarif_J1 {
      font-size: 10px;
    }

    #etatActions {
      display: none;
    }

    .choixG {
      text-align: left;
      width: 100%;
      height: 0;
      position: relative;
    }

    .choix {
      position: absolute;
      left: 120px;
      top: -80px;
      font-size: 16px;
      display: none;
    }

    iframe {
      width: 100%;
      min-height: 900px;
    }

    .autreRMS {
      width: 100%;
      height: 36px;
      background-color: grey;
      color: white;
      font-size: 30px;
      display: flex;
      justify-content: space-between;
      margin-top: 4px;
    }

    .autreRif {
      width: 100%;
    }
    #B_graph{ 
      display:none;
      justify-content:space-between;
      position:relative;
      margin-top:10px;
      margin-bottom:-20px;
    }
    #B_Graph{
      border: 6px inset azure;
      border-radius: 10px;
      font-size:20px;
      padding:8px;
      position:absolute;
      top:-30px;
      right:30px;
      display:none;
    }
    #B_Graph_in{
      display:grid;text-align:left;
      grid-template-columns: auto auto auto auto;
      
    }
    #date{
      display:none;
    }
    #meteoPrev{
      display:none;
      text-align:center;
      font-size:15px;
      padding:4px 10px;
      margin:4px auto;
      width:fit-content;
      border:1px solid rgba(74,179,244,.5);
      border-radius:8px;
      background:rgba(30,30,60,.4);
    }
    @media(max-width:700px){
      .grid-container2,.grid-container2M{grid-template-columns:repeat(3,auto)}
      .grid-container1{grid-template-columns:repeat(2,auto)}
      .item1{grid-column:1/span 2}
      .item2{grid-column:3/span 1}
      .item_Force{grid-column:2/span 2}
      h2{font-size:1.2rem}
    }
  </style>
</head>

<body onload="Init();">

  <div id="LED" title="Réception des données"></div>
  <div id="lesOnglets"></div>
  <div id="date">DATE</div>
  <div id="meteoPrev"></div>
  <h2 id="routeur">Routeur - RMS - F1ATB</h2>
  <div id="TabMesures"></div>

  <br>

  <div id="donneeDistante">Données distantes</div>

  <div id="etatActions" class="grid-container2"></div>

  <div id="B_graph"><div>&nbsp;</div><div>Graphiques</div><div style="cursor:pointer;"><span onclick="ListeGraph();">📈</span><div id="B_Graph"></div></div></div>
  <div id="LesGraphes"></div>

  <div id="info">
    <div id="info_txt"></div>
  </div>

  <br><br>

  <div id="foot">
    Données RMS
    <div id="source"></div>
  </div>

  <div id="autresRMS"></div>
  <div id="pied"></div>

  <!-- Scripts -->
  <script src="/ParaCommunJS"></script>
  <script src="MainJS1"></script>
  <script src="MainJS2"></script>
  <script src="MainJS3"></script>
  <script src="/CommunCouleurJS"></script>

  <br>

</body>
</html>

)====";