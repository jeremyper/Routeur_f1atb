// ***********************************************************************
// * Onduleur SMA Sunny Boy via Modbus TCP (port 502, unit ID 3)         *
// * + Forçage adaptatif du ballon :                                     *
// *   Besoin (kWh) = Volume x 1.163/1000 x (Tcible - Tballon)           *
// *   Surplus prévu = prévision météo x coefficient routable            *
// *   Le coefficient s'auto-ajuste en comparant chaque soir la          *
// *   production réelle SMA à l'énergie réellement routée (H_Ouvre).    *
// ***********************************************************************

// Le serveur Modbus doit être activé sur l'onduleur :
// interface web SMA > Paramètres de l'appareil > Communication externe > Modbus > TCP on

WiFiClient clientSma;

//Lecture d'un registre 32 bits (2 registres Modbus) en FC3. ok passe à false en cas d'échec.
long SMA_Read32(uint16_t reg, bool &ok) {
  while (clientSma.available()) clientSma.read();  //Vider le buffer avant la requête (résidus d'un échange précédent)
  uint8_t trame[12] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x03, 0x03,
                        (uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF), 0x00, 0x02 };
  clientSma.write(trame, 12);
  unsigned long timeout = millis();
  while (clientSma.available() < 9) {  //9 octets = taille minimale d'une réponse valide (y compris exception Modbus)
    if (millis() - timeout > 3000 || !clientSma.connected()) {
      ok = false;
      return 0;
    }
    yield();
  }
  uint8_t rep[13] = { 0 };
  int n = min(clientSma.available(), 13);
  clientSma.read(rep, n);
  if (n < 9 || rep[7] == 0x83 || rep[8] != 4) {  //Exception Modbus (FC|0x80) ou réponse inattendue
    ok = false;
    return 0;
  }
  return ((long)rep[9] << 24) | ((long)rep[10] << 16) | ((long)rep[11] << 8) | (long)rep[12];
}

void Call_SMA_data() {
  if (SmaOn != 1 || SmaIP == "" || ModeReseau != 0) {
    PuissancePV = 0;
    EnergieTotalePV = -1;
    return;
  }
  static unsigned long delaiSma = 20000UL;  //Lecture toutes les 20s, 5mn si injoignable
  unsigned long tps = millis();
  if (LastSmaMillis != 0 && (tps - LastSmaMillis) < delaiSma) return;
  LastSmaMillis = tps;

  char hostbuf[20];
  SmaIP.toCharArray(hostbuf, sizeof(hostbuf));
  if (!clientSma.connect(hostbuf, 502, 3000)) {
    clientSma.stop();
    PuissancePV = 0;          //Onduleur injoignable (nuit : certains modèles coupent le réseau)
    delaiSma = 300000UL;      //On réessaie dans 5 minutes pour ne pas ralentir la boucle
    return;
  }
  delaiSma = 20000UL;
  bool ok = true;
  long pw = SMA_Read32(30775, ok);  //Puissance AC en W (S32)
  if (ok) {
    if (pw == (long)0x80000000 || pw < 0) pw = 0;  //NaN SMA = onduleur en veille
    PuissancePV = float(pw);
  }
  ok = true;
  long total = SMA_Read32(30529, ok);  //Energie totale en Wh (U32)
  if (ok && total > 0) {
    EnergieTotalePV = total;
    if (EnergieTotalePV < EnergiePV_J0 || EnergiePV_J0 == 0) EnergiePV_J0 = EnergieTotalePV;
    EnergieJourPV = EnergieTotalePV - EnergiePV_J0;
  }
  clientSma.stop();
}

//Calcul du besoin de chauffe du ballon et du surplus solaire attendu.
//Appelé régulièrement depuis le loop. Ballon_Deficit>0 autorise le forçage adaptatif
//(condition météo "Adaptatif" des périodes d'action).
void CalculBallon() {
  if (BallonCanal < 0 || MeteoOn != 1) {
    Ballon_Besoin = -1;
    Ballon_SurplusPrevu = -1;
    Ballon_Deficit = 0;
    return;
  }
  if (TemperatureValide[BallonCanal] <= 0 || temperature[BallonCanal] < -50) {
    Ballon_Deficit = 0;  //Sonde invalide : pas de forçage adaptatif
    return;
  }
  float Tb = temperature[BallonCanal];
  float dT = float(BallonTcible) - Tb;
  if (dT < 0) dT = 0;
  Ballon_Besoin = float(BallonVolume) * 1.163 / 1000.0 * dT;  //kWh (1.163 Wh par litre par °C)

  //La production "qui vient" : avant midi c'est celle du jour, après midi celle du lendemain
  float prevision = (HeureCouranteDeci < 1200) ? Meteo_PrevisionJour : Meteo_PrevisionDemain;
  if (prevision < 0) prevision = 0;  //Pas de météo : on considère 0 => on autorise la chauffe (fail-safe confort)
  Ballon_SurplusPrevu = prevision * float(BallonCoefAuto) / 100.0;
  Ballon_Deficit = Ballon_Besoin - Ballon_SurplusPrevu;

  //Journal : on signale une seule fois le passage en "besoin détecté"
  static bool besoinSignale = false;
  if (Ballon_Deficit > 0.05) {
    if (!besoinSignale) {
      JournalAjoute("Ballon : besoin détecté (" + String(Ballon_Besoin, 1) + " kWh), chauffe autorisée");
      besoinSignale = true;
    }
  } else {
    besoinSignale = false;
  }
}

//Apprentissage du coefficient routable, appelé une fois par jour à minuit.
//Compare la production SMA du jour à l'énergie réellement routée vers le ballon
//(H_Ouvre de la première action SSR x puissance résistance).
void ApprentissageBallon() {
  float prodJour = (SmaOn == 1 && EnergieJourPV > 0) ? float(EnergieJourPV) / 1000.0 : -1;  //kWh (-1 = pas de donnée)
  float routee = 0;
  for (int i = 0; i < NbActions; i++) {
    if (LesActions[i].Actif == MODE_MULTISINUS || LesActions[i].Actif == MODE_TRAINSINUS) {
      routee = LesActions[i].H_Ouvre * float(BallonPuissance) / 1000.0;  //kWh routés depuis 6h
      break;                                                             //Première action SSR = ballon
    }
  }
  if (SmaOn == 1 && BallonCanal >= 0 && prodJour > 1.0) {
    if (Ballon_Besoin > 0.5) {
      //Le ballon n'a pas atteint sa cible : le ratio mesuré reflète le surplus réellement disponible
      float ratio = routee / prodJour * 100.0;
      ratio = constrain(ratio, 20.0, 90.0);
      BallonCoefAuto = round(0.7 * float(BallonCoefAuto) + 0.3 * ratio);
    } else {
      //Cible atteinte par le solaire : le coefficient peut monter doucement
      BallonCoefAuto = min(90, BallonCoefAuto + 2);
    }
    SauveCoefAuto();
    StockMessage("Ballon : prod " + String(prodJour, 1) + " kWh, routé " + String(routee, 1) + " kWh, coef " + String(BallonCoefAuto) + "%");
    JournalAjoute("Bilan du jour : " + String(routee, 1) + " kWh envoyés au ballon sur " + String(prodJour, 1) + " kWh produits");
  }
  //Historique prévision/production : une ligne par jour si au moins une donnée valide
  if (prodJour >= 0 || Meteo_PrevisionJourMemo >= 0) HistMeteoAjoute(prodJour, routee);
  EnergiePV_J0 = EnergieTotalePV;  //Calage du compteur jour à minuit
}

//Ajoute la ligne du jour écoulé dans /histmeteo.csv : date;prevision_kWh;production_kWh;routee_kWh;coef
//Le fichier est limité aux 60 derniers jours (1 écriture par jour, usure LittleFS négligeable).
void HistMeteoAjoute(float prodJour, float routee) {
  if (oldDateAMJ.length() < 8) return;  //Pas de date valide
  String dateJM = oldDateAMJ.substring(6, 8) + "/" + oldDateAMJ.substring(4, 6) + "/" + oldDateAMJ.substring(0, 4);
  String ligne = dateJM + ";" + String(Meteo_PrevisionJourMemo, 1) + ";" + String(prodJour, 1) + ";" + String(routee, 1) + ";" + String(BallonCoefAuto);
  String contenu = "";
  if (LittleFS.exists("/histmeteo.csv")) {
    File f = LittleFS.open("/histmeteo.csv", "r");
    if (f) {
      contenu = f.readString();
      f.close();
    }
  }
  contenu += ligne + "\n";
  int nb = 0;
  for (unsigned int i = 0; i < contenu.length(); i++) {
    if (contenu[i] == '\n') nb++;
  }
  while (nb > 60) {  //On ne garde que les 60 dernières lignes
    int p = contenu.indexOf('\n');
    if (p < 0) break;
    contenu = contenu.substring(p + 1);
    nb--;
  }
  File f = LittleFS.open("/histmeteo.csv", "w");
  if (f) {
    f.print(contenu);
    f.close();
  }
}

//Persistance du coefficient appris (petit fichier dédié, 1 écriture/jour max)
void SauveCoefAuto() {
  File f = LittleFS.open("/coefauto.txt", "w");
  if (f) {
    f.print(String(BallonCoefAuto));
    f.close();
  }
}
void LitCoefAuto() {
  if (LittleFS.exists("/coefauto.txt")) {
    File f = LittleFS.open("/coefauto.txt", "r");
    if (f) {
      int v = f.readString().toInt();
      if (v >= 20 && v <= 90) BallonCoefAuto = v;
      f.close();
    }
  }
}
