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

  //Le tas total ne dit pas tout : mbedTLS reclame un bloc contigu d'une
  //trentaine de Ko. Avec un tas fragmente, le total reste confortable alors
  //que le plus gros bloc disponible est deja trop petit. On journalise donc
  //les deux, et on separe l'echec DNS de l'echec TLS, que connect() confond.
  IPAddress ipMeteo;
  if (!WiFi.hostByName(adr_Meteo_Host, ipMeteo)) {
    StockMessage("Open-Meteo : resolution DNS impossible pour " + Host);
    return;
  }
  LibereTLSpourAppelSortant();  //place au tas : voir EnvoiMQTT.ino
  clientSecuMeteo.setInsecure();  //skip verification
  clientSecuMeteo.setTimeout(6000);
  if (!clientSecuMeteo.connect(adr_Meteo_Host, 443, 5000)) {
    //Mesure avant stop() : apres liberation le tas parait confortable et
    //masque justement la penurie qui a fait echouer la poignee de main.
    uint32_t tasEchec = esp_get_free_internal_heap_size();
    uint32_t blocEchec = ESP.getMaxAllocHeap();
    char errTLS[100] = "";
    int codeTLS = clientSecuMeteo.lastError(errTLS, sizeof(errTLS));
    clientSecuMeteo.stop();  //libere le contexte TLS, voir Tempo_RTE.ino
    StockMessage("Connection failed to Open-Meteo : " + Host + " (TLS " + String(codeTLS)
                 + " " + String(errTLS) + ", DNS ok " + ipMeteo.toString()
                 + ", tas a l'echec " + String(tasEchec)
                 + " o, plus gros bloc " + String(blocEchec) + " o)");
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
  Meteo_PrevisionJourMemo = Meteo_PrevisionJour;  //Mémo de la prévision annoncée pour l'historique quotidien
  StockMessage("Météo : prévision solaire jour " + String(Meteo_PrevisionJour, 1) + " kWh, demain " + String(Meteo_PrevisionDemain, 1) + " kWh");
}
