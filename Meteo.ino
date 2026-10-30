// ***********************************************************************
// * Prévision météo solaire via Open-Meteo (gratuit, sans clé API)      *
// * Sert à conditionner le forçage des actions (ex: chauffe-eau la nuit *
// * en heures creuses uniquement si la production de demain sera faible)*
// ***********************************************************************

// L'API Open-Meteo retourne, heure par heure, le rayonnement reçu par un plan incliné
// comme les panneaux (global_tilted_irradiance, W/m², moyenne de l'heure écoulée).
// La somme des 24 heures d'un jour donne des kWh/m². Conversion en production estimée :
//   kWh estimés = somme W/m² / 1000 x Puissance crête (kWc) x rendement 0.80
// Le rayonnement horizontal (shortwave_radiation_sum) utilisé jusqu'en v1.29 sous-estimait
// la production d'environ 35 % en automne : le soleil bas frappe bien mieux un panneau
// incliné que le sol. L'écart s'inverse presque en été, aucun coefficient fixe ne convenait.

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
  //timeformat=unixtime raccourcit le tableau des heures : la réponse tient en ~1 Ko
  String url = "/v1/forecast?latitude=" + String(MeteoLat, 4) + "&longitude=" + String(MeteoLon, 4)
               + "&hourly=global_tilted_irradiance&tilt=" + String(MeteoInclinaison) + "&azimuth=" + String(MeteoAzimut)
               + "&timezone=auto&forecast_days=2&timeformat=unixtime";
  String MeteoData = "";
  MeteoData.reserve(2048);  //voir Tempo_RTE.ino : evite les reallocations en boucle
  uint32_t minAvantMeteo = esp_get_minimum_free_heap_size();
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
  uint32_t minAvantTLS = esp_get_minimum_free_heap_size();
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
  SuiviPlancher("Poignee de main meteo", minAvantTLS);
  minAvantMeteo = esp_get_minimum_free_heap_size();  //voir Tempo_RTE.ino
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
    int deb = line.indexOf("\"global_tilted_irradiance\":[");
    if (deb >= 0 && line.indexOf("]", deb) > 0) break;
    yield();
  }
  clientSecuMeteo.stop();
  SuiviPlancher("Lecture reponse meteo", minAvantMeteo);

  // Extraction du tableau "global_tilted_irradiance":[48 valeurs horaires]. L'unité figure
  // aussi dans "hourly_units" ("global_tilted_irradiance":"W/m²"), sans crochet : le "[" lève
  // l'ambiguïté.
  int p = MeteoData.indexOf("\"global_tilted_irradiance\":[");
  if (p < 0) {
    StockMessage("Open-Meteo : données invalides");
    return;
  }
  int fin = MeteoData.indexOf("]", p);
  if (fin < 0) {
    StockMessage("Open-Meteo : données invalides");
    return;
  }
  //Les 24 premières heures sont celles d'aujourd'hui (timezone=auto), les 24 suivantes demain.
  //Un jour de changement d'heure décale d'une heure la frontière de minuit : la nuit, le
  //rayonnement est nul, l'erreur aussi.
  float radJour = 0, radDemain = 0;  //Wh/m²
  int n = 0;
  int pos = p + 28;  //longueur de "global_tilted_irradiance":[ guillemets compris
  while (pos < fin && n < 48) {
    int virgule = MeteoData.indexOf(",", pos);
    if (virgule < 0 || virgule > fin) virgule = fin;
    float v = MeteoData.substring(pos, virgule).toFloat();  //"null" donne 0
    if (n < 24) radJour += v;
    else radDemain += v;
    n++;
    pos = virgule + 1;
  }
  if (n < 48) {
    StockMessage("Open-Meteo : données incomplètes (" + String(n) + " heures)");
    return;
  }

  //Production estimée en kWh (rendement global 0.80)
  Meteo_PrevisionJour = radJour / 1000.0 * MeteoPVcrete * 0.80;
  Meteo_PrevisionDemain = radDemain / 1000.0 * MeteoPVcrete * 0.80;
  Meteo_PrevisionJourMemo = Meteo_PrevisionJour;  //Mémo de la prévision annoncée pour l'historique quotidien
  StockMessage("Météo : prévision solaire jour " + String(Meteo_PrevisionJour, 1) + " kWh, demain " + String(Meteo_PrevisionDemain, 1) + " kWh");
}
