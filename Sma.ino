// ***********************************************************************
// * Onduleur SMA Sunny Boy via Modbus TCP (port 502, unit ID 3)         *
// * + Forçage adaptatif du ballon :                                     *
// *   Besoin (kWh) = Volume x 1.163/1000 x (Tcible - Tballon)           *
// *   Surplus prévu = prévision météo x coefficient routable            *
// *   Le coefficient s'auto-ajuste chaque nuit : surplus mesuré du jour *
// *   (soleil reçu par le ballon + réinjection) / production SMA.       *
// ***********************************************************************

// Le serveur Modbus doit être activé sur l'onduleur :
// interface web SMA > Paramètres de l'appareil > Communication externe > Modbus > TCP on

WiFiClient clientSma;

//Attend n octets du serveur Modbus (3 s au plus)
bool SMA_Attend(int n) {
  unsigned long timeout = millis();
  while (clientSma.available() < n) {
    if (millis() - timeout > 3000 || !clientSma.connected()) return false;
    yield();
  }
  return true;
}

//Lecture d'un registre 32 bits (2 registres Modbus) en FC3. ok passe à false en cas d'échec.
//Chaque requête porte son propre identifiant de transaction, contrôlé dans la réponse : une
//réponse arrivée en retard (après le délai de 3 s de la requête précédente) était acceptée
//comme celle de la requête suivante. La puissance lue au registre 30775 devenait alors le
//compteur total, et la production du jour affichait 14 707 kWh (26/10/2026).
long SMA_Read32(uint16_t reg, bool &ok) {
  static uint16_t transaction = 0;
  transaction++;
  while (clientSma.available()) clientSma.read();  //Vider le buffer avant la requête (résidus d'un échange précédent)
  uint8_t trame[12] = { (uint8_t)(transaction >> 8), (uint8_t)(transaction & 0xFF), 0x00, 0x00, 0x00, 0x06, 0x03, 0x03,
                        (uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF), 0x00, 0x02 };
  clientSma.write(trame, 12);
  //En-tête MBAP (7 octets) + code fonction + nombre d'octets : 9 octets, exception Modbus comprise
  uint8_t rep[13] = { 0 };
  if (!SMA_Attend(9)) {
    ok = false;
    return 0;
  }
  clientSma.read(rep, 9);
  bool bonneReponse = rep[0] == (transaction >> 8) && rep[1] == (transaction & 0xFF) && rep[7] == 0x03 && rep[8] == 4;
  //Les 4 octets de données doivent être là en entier : une réponse lue à moitié donnait
  //une valeur tronquée, complétée de zéros.
  if (!bonneReponse || !SMA_Attend(4)) {
    ok = false;
    return 0;
  }
  clientSma.read(rep + 9, 4);
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
  //Un compteur total ne recule pas et ne bondit pas de 50 kWh entre deux lectures (20 s, ou
  //une nuit quand l'onduleur dort). Une valeur aberrante est écartée ; si elle se répète
  //cinq fois de suite, c'est le compteur qui a changé (onduleur remplacé) : on la prend.
  static int rejets = 0;
  if (ok && total > 0 && EnergieTotalePV > 0 && (total < EnergieTotalePV || total - EnergieTotalePV > 50000L)) {
    if (++rejets < 5) {
      ok = false;
      StockMessage("SMA : compteur total incohérent (" + String(total) + " Wh après " + String(EnergieTotalePV) + " Wh), lecture ignorée");
    } else {
      EnergiePV_J0 = 0;  //Recalage sur le compteur du jour de l'onduleur
    }
  }
  if (ok && total > 0) {
    rejets = 0;
    EnergieTotalePV = total;
    if (EnergiePV_J0 == 0) {
      //Premier relevé après un démarrage : on recale sur le compteur jour de l'onduleur (30535)
      //pour ne pas perdre ce qui a été produit avant le reboot
      ok = true;
      long jour = SMA_Read32(30535, ok);  //Energie du jour en Wh (U32)
      EnergiePV_J0 = (ok && jour >= 0 && jour <= EnergieTotalePV) ? EnergieTotalePV - jour : EnergieTotalePV;
    }
    if (EnergieTotalePV < EnergiePV_J0) EnergiePV_J0 = EnergieTotalePV;
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
  //Deux moitiés : bas à la sonde du bas, haut à la sonde du haut (ou du bas si absente).
  //Chaque moitié est comptée séparément : un bas froid n'efface pas l'eau chaude du haut.
  float Tb = temperature[BallonCanal];
  float Th = BallonHautValide() ? temperature[BallonCanalHaut] : Tb;
  float demi = float(BallonVolume) / 2.0;
  Ballon_Besoin = KwhEau(demi, float(BallonTcible) - Tb) + KwhEau(demi, float(BallonTcible) - Th);  //kWh pour atteindre la cible

  //Énergie utile actuellement stockée (eau au-dessus de la température minimale exploitable)
  Ballon_Reserve = KwhEau(demi, Tb - float(BallonTmin)) + KwhEau(demi, Th - float(BallonTmin));

  //La production "qui vient" : avant midi c'est celle du jour, après midi celle du lendemain
  float prevision = (HeureCouranteDeci < 1200) ? Meteo_PrevisionJour : Meteo_PrevisionDemain;
  if (prevision < 0) prevision = 0;  //Pas de météo : on considère 0 => on autorise la chauffe (fail-safe confort)
  Ballon_SurplusPrevu = prevision * float(BallonCoefAuto) / 100.0;

  //Besoin retenu pour le forçage adaptatif :
  // - mode classique : viser la température cible (Ballon_Besoin)
  // - mode intelligent : couvrir seulement l'usage habituel (marge 30%) compte tenu de la réserve déjà stockée
  float besoinDeficit = Ballon_Besoin;
  if (BallonModeIntel == 1 && Ballon_UsageMoyen > 0.1) {
    float manqueUtile = Ballon_UsageMoyen * 1.3 - Ballon_Reserve;  //kWh d'eau chaude utile qui manquent
    if (manqueUtile <= 0) {
      besoinDeficit = 0;
    } else {
      //Pour ajouter de l'eau utile, la résistance doit d'abord remonter jusqu'à Tmin l'eau qui
      //est en dessous. L'ancien calcul (manqueUtile seul) ne comptait pas cette montée : il ne
      //baissait pas pendant la chauffe de l'eau froide, et la marche forcée de nuit poussait
      //tout le ballon à Tmin + manque, même quand le soleil prévu aurait fait la fin.
      //Plafonné à Ballon_Besoin : on ne demande jamais plus que remplir le ballon à la cible,
      //le mode intelligent ne chauffe donc jamais davantage que le mode classique.
      float montee = KwhEau(demi, float(BallonTmin) - Tb) + KwhEau(demi, float(BallonTmin) - Th);
      besoinDeficit = min(Ballon_Besoin, montee + manqueUtile);
    }
  }
  Ballon_Deficit = besoinDeficit - Ballon_SurplusPrevu;

  //Journal : on signale une seule fois le passage en "besoin détecté". L'hystérésis évite
  //qu'un déficit qui oscille autour de zéro remplisse le journal (7 lignes en 2 h le 28/10/2026).
  static bool besoinSignale = false;
  if (Ballon_Deficit > 0.05) {
    if (!besoinSignale) {
      JournalAjoute("Ballon : besoin détecté (" + String(besoinDeficit, 1) + " kWh), chauffe autorisée");
      besoinSignale = true;
    }
  } else if (Ballon_Deficit < -0.3) {
    besoinSignale = false;
  }
}

//Apprentissage du coefficient routable, appelé une fois par jour à minuit.
//Compare la production SMA du jour au surplus réellement disponible : soleil reçu par le
//ballon (EnergieRouteeJour) + énergie réinjectée (EnergieInjecteeJour), toutes deux cumulées
//par SuiviEconomieRoutee(). Jusqu'en v1.29, on prenait le temps d'ouverture de la sortie x la
//puissance de la résistance : cela comptait la marche forcée au réseau et les heures où le
//thermostat du ballon avait coupé, et le coefficient grimpait sans raison (33 à 70 % en 3 jours).
void ApprentissageBallon() {
  //Relève du compteur du jour puis remise à zéro, sous verrou : SuiviUsageBallon()
  //l'accumule depuis le cœur 1. On ne garde le verrou que le temps de la copie, le
  //journal et l'écriture flash qui suivent se font sur la valeur relevée.
  float usageJour = 0;
  if (MutexBallon != NULL && xSemaphoreTake(MutexBallon, pdMS_TO_TICKS(500)) == pdTRUE) {
    usageJour = Ballon_UsageJour;
    Ballon_UsageJour = 0;  //Nouveau jour
    xSemaphoreGive(MutexBallon);
  }

  //Apprentissage de l'usage d'eau chaude : moyenne glissante sur ~5 jours (mode ballon intelligent)
  if (BallonCanal >= 0 && usageJour > 0.05) {
    if (Ballon_UsageMoyen < 0.05) {
      Ballon_UsageMoyen = usageJour;  //Première initialisation
    } else {
      Ballon_UsageMoyen = 0.8 * Ballon_UsageMoyen + 0.2 * usageJour;
    }
    JournalAjoute("Ballon : usage estimé du jour " + String(usageJour, 1) + " kWh (moyenne " + String(Ballon_UsageMoyen, 1) + " kWh)");
    SauveCoefAuto();  //Persiste coef + usage moyen
  }

  float prodJour = (SmaOn == 1 && EnergieJourPV > 0) ? float(EnergieJourPV) / 1000.0 : -1;  //kWh (-1 = pas de donnée)
  float routee = EnergieRouteeJour / 1000.0;     //kWh de soleil reçus par le ballon
  float reinjecte = EnergieInjecteeJour / 1000.0;  //kWh de surplus partis au réseau
  //Garde-fou : une production du jour plus de 2 fois supérieure à ce que la puissance crête
  //permet en 24 h est une lecture faussée, elle ne doit pas fausser le coefficient.
  bool prodPlausible = prodJour > 1.0 && prodJour < MeteoPVcrete * 24.0 * 2.0;
  if (SmaOn == 1 && BallonCanal >= 0 && prodPlausible) {
    //Le surplus mesuré est valable que le ballon ait été plein ou non : ce qu'il n'a pas pris
    //est parti au réseau et figure dans la réinjection.
    float ratio = (routee + reinjecte) / prodJour * 100.0;
    ratio = constrain(ratio, 20.0, 90.0);
    BallonCoefAuto = round(0.7 * float(BallonCoefAuto) + 0.3 * ratio);
    SauveCoefAuto();
    StockMessage("Ballon : prod " + String(prodJour, 1) + " kWh, soleil au ballon " + String(routee, 1) + " kWh, réinjecté " + String(reinjecte, 1) + " kWh, coef " + String(BallonCoefAuto) + "%");
    JournalAjoute("Bilan du jour : " + String(routee, 1) + " kWh de soleil dans le ballon, " + String(reinjecte, 1) + " kWh réinjectés, sur " + String(prodJour, 1) + " kWh produits");
  }
  if (prodJour > 1.0 && !prodPlausible) prodJour = -1;  //Lecture faussée : pas dans l'historique
  //Historique prévision/production : une ligne par jour si au moins une donnée valide
  if (prodJour >= 0 || Meteo_PrevisionJourMemo >= 0) HistMeteoAjoute(prodJour, routee);
  //Calage du compteur jour à minuit. Sans lecture valide depuis le démarrage (onduleur
  //endormi), EnergieTotalePV vaut -1 : le reprendre tel quel faisait compter le lendemain
  //tout le compteur total comme production du jour. 0 provoque un recalage sur le
  //compteur du jour de l'onduleur à la première lecture.
  EnergiePV_J0 = (EnergieTotalePV > 0) ? EnergieTotalePV : 0;
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
    f.print(String(BallonCoefAuto) + ";" + String(Ballon_UsageMoyen, 2));  //"coef;usageMoyen"
    f.close();
  }
}
void LitCoefAuto() {
  if (LittleFS.exists("/coefauto.txt")) {
    File f = LittleFS.open("/coefauto.txt", "r");
    if (f) {
      String s = f.readString();
      f.close();
      int sep = s.indexOf(';');
      int v = (sep >= 0 ? s.substring(0, sep) : s).toInt();
      if (v >= 20 && v <= 90) BallonCoefAuto = v;
      if (sep >= 0) {
        float u = s.substring(sep + 1).toFloat();
        if (u >= 0 && u < 100) Ballon_UsageMoyen = u;
      }
    }
  }
}
