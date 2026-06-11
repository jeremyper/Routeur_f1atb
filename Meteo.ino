// ***********************************************************************
// * Prévision météo solaire via Open-Meteo (gratuit, sans clé API)      *
// * Sert à conditionner le forçage des actions (ex: chauffe-eau la nuit *
// * en heures creuses uniquement si la production de demain sera faible)*
// ***********************************************************************

// L'API Open-Meteo retourne le rayonnement global quotidien (shortwave_radiation_sum)
// en MJ/m². Conversion en production estimée :
//   kWh estimés = MJ/m² / 3.6 (=> kWh/m²) x Puissance crête (kWc) x rendement 0.80

void Call_Meteo_data() {
  if (MeteoOn != 1 || ModeReseau != 0) {
    Meteo_PrevisionJour = -1;
    Meteo_PrevisionDemain = -1;
    return;
  }
  if (!HeureValide) return;
  unsigned long tps = millis();
  if (LastMeteoMillis != 0 && (tps - LastMeteoMillis) < 7200000UL) return;  //Rafraichissement toutes les 2h
  LastMeteoMillis = tps;

  const char* adr_Meteo_Host = "api.open-meteo.com";
  String Host = String(adr_Meteo_Host);
  String url = "/v1/forecast?latitude=" + String(MeteoLat, 4) + "&longitude=" + String(MeteoLon, 4) + "&daily=shortwave_radiation_sum&timezone=auto&forecast_days=2";
  String MeteoData = "";
  String line = "";

  clientSecuMeteo.setInsecure();  //skip verification
  clientSecuMeteo.setTimeout(6000);
  if (!clientSecuMeteo.connect(adr_Meteo_Host, 443, 5000)) {
    StockMessage("Connection failed to Open-Meteo : " + Host);
    return;
  }
  clientSecuMeteo.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + Host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (clientSecuMeteo.available() == 0) {
    if (millis() - timeout > 6000) {
      StockMessage(">>> Open-Meteo Timeout !");
      clientSecuMeteo.stop();
      return;
    }
    yield();
  }
  timeout = millis();
  while (clientSecuMeteo.connected() && (millis() - timeout < 5000)) {
    line = clientSecuMeteo.readStringUntil('\n');
    MeteoData += line;
    if (line.indexOf("shortwave_radiation_sum") >= 0 && line.indexOf("]") > 0) break;
    yield();
  }
  clientSecuMeteo.stop();

  // Extraction du tableau "shortwave_radiation_sum":[jour,demain]
  int p = MeteoData.lastIndexOf("\"shortwave_radiation_sum\":[");
  if (p < 0) {
    StockMessage("Open-Meteo : données invalides");
    return;
  }
  String S = MeteoData.substring(p + 27);
  int q = S.indexOf("]");
  if (q < 0) {
    StockMessage("Open-Meteo : données invalides");
    return;
  }
  S = S.substring(0, q);  // "25.5,12.3"
  q = S.indexOf(",");
  if (q < 0) {
    StockMessage("Open-Meteo : données incomplètes");
    return;
  }
  float radJour = S.substring(0, q).toFloat();    //MJ/m² aujourd'hui
  float radDemain = S.substring(q + 1).toFloat();  //MJ/m² demain

  //Production estimée en kWh (rendement global 0.80)
  Meteo_PrevisionJour = radJour / 3.6 * MeteoPVcrete * 0.80;
  Meteo_PrevisionDemain = radDemain / 3.6 * MeteoPVcrete * 0.80;
  StockMessage("Météo : prévision solaire jour " + String(Meteo_PrevisionJour, 1) + " kWh, demain " + String(Meteo_PrevisionDemain, 1) + " kWh");
}
