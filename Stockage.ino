

#include <Arduino.h>
String Record_Conf = "";




void EcritureEnROM() {
  if (ModePara == 0) {
    //La souscription aux commandes reste interdite hors mode Expert : en mode Standard,
    //MQTT ne fait que publier.
    subMQTT = 0;
    //La publication, elle, reste permise si un broker distant est configuré : c'est le
    //cas d'usage « supervision depuis l'extérieur » pour qui n'a que le routeur, et
    //l'imposer en mode Expert le rendrait inaccessible à ceux qu'il vise.
    if (MQTTHost.length() == 0) MQTTRepet = 0;
  }

  Calibration();
  RecordFichierParametres();  //Nouveau stockage depuis V17
}
void Calibration() {
  kV = KV * float(CalibU) / 1000.0;  //Calibration coefficient to be applied
  kI = KI * float(CalibI) / 1000.0;
}

void init_puissance() {
  PuissanceS_T = 0;
  PuissanceS_M = 0;
  PuissanceI_T = 0;
  PuissanceI_M = 0;  //Puissance Watt affichée en entiers Maison et Triac
  PVAS_T = 0;
  PVAS_M = 0;
  PVAI_T = 0;
  PVAI_M = 0;  //Puissance VA affichée en entiers Maison et Triac
  PuissanceS_T_inst = 0.0;
  PuissanceS_M_inst = 0.0;
  PuissanceI_T_inst = 0.0;
  PuissanceI_M_inst = 0.0;
  PVAS_T_inst = 0.0;
  PVAS_M_inst = 0.0;
  PVAI_T_inst = 0.0;
  PVAI_M_inst = 0.0;
  Puissance_T_moy = 0.0;
  Puissance_M_moy = 0.0;
  PVA_T_moy = 0.0;
  PVA_M_moy = 0.0;
}
void filtre_puissance() {  //Filtre RC
  LastMesureMillis = millis();  //Horodatage : toutes les sources de puissance passent ici (sécurité fermeture SSR)

  float A = 0.3;  //Coef pour un lissage en multi-sinus et train de sinus sur les mesures de puissance courte
  float B = 0.7;
  if (!LissageLong) {
    A = 1;
    B = 0;
  }

  Puissance_T_moy = A * (PuissanceS_T_inst - PuissanceI_T_inst) + B * Puissance_T_moy;
  if (Puissance_T_moy < 0) {
    PuissanceI_T = -int(Puissance_T_moy);  //Puissance Watt affichée en entier  Triac
    PuissanceS_T = 0;
  } else {
    PuissanceS_T = int(Puissance_T_moy);
    PuissanceI_T = 0;
  }



  Puissance_M_moy = A * (PuissanceS_M_inst - PuissanceI_M_inst) + B * Puissance_M_moy;
  if (Puissance_M_moy < 0) {
    PuissanceI_M = -int(Puissance_M_moy);  //Puissance Watt affichée en entier Maison
    PuissanceS_M = 0;
  } else {
    PuissanceS_M = int(Puissance_M_moy);
    PuissanceI_M = 0;
  }


  PVA_T_moy = A * (PVAS_T_inst - PVAI_T_inst) + B * PVA_T_moy;  //Puissance VA affichée en entiers
  if (PVA_T_moy < 0) {
    PVAI_T = -int(PVA_T_moy);
    PVAS_T = 0;
  } else {
    PVAS_T = int(PVA_T_moy);
    PVAI_T = 0;
  }

  PVA_M_moy = A * (PVAS_M_inst - PVAI_M_inst) + B * PVA_M_moy;
  if (PVA_M_moy < 0) {
    PVAI_M = -int(PVA_M_moy);
    PVAS_M = 0;
  } else {
    PVAS_M = int(PVA_M_moy);
    PVAI_M = 0;
  }
}

void StockMessage(String m) {
  if (DATE != "") m = DATE + " : " + m;
  MessageH[idxMessage] = m;
  idxMessage = (idxMessage + 1) % 10;
  PrintScroll(m);
}

// *************************************************************
// Stockage parametres en zone SPIFFS (methode 2026 depuis V17)
// *************************************************************
void RecordFichierParametres() {
  File file = LittleFS.open("/parametres.json", FILE_WRITE);
  file.print(SerializeConfiguration());  //Fichier au format JSON
  file.close();
  Serial.println("Ecriture fichier parametres");
}
void ReadFichierParametres() {
  if (!LittleFS.exists("/parametres.json")) {  //Fichier pas encore crée
    RecordFichierParametres();
  }

  File file = LittleFS.open("/parametres.json", "r");
  Serial.println("Lecture du fichier paramètres");
  String content = file.readString();  // lit tout le fichier
  file.close();
  DeserializeConfiguration(content);
  MigrePaletteTableaux();  //apres chargement complet : peut donc etre persistee
}

//Migration v1.21 : l'ancienne palette peignait les tableaux en gris clair, ce qui
//donne du texte pale sur fond pale avec le theme sombre. On ne compare que le
//triplet « Tableaux » (4e entree, 3 couleurs de 6 caracteres a partir de l'offset
//54) : comparer la chaine entiere ne migrait personne, une seule autre couleur
//modifiee suffisait a faire echouer l'egalite. Qui a choisi la couleur de ses
//tableaux garde la sienne.
void MigrePaletteTableaux() {
  const int OFFSET = 54;
  if (Couleurs.length() < OFFSET + 18) return;
  if (Couleurs.substring(OFFSET, OFFSET + 18) != "000000cccccc888888") return;
  Couleurs = Couleurs.substring(0, OFFSET) + "e8edf61c22302c3444" + Couleurs.substring(OFFSET + 18);
  //Indispensable : handleParaFixe() ressert le fichier, pas la variable en RAM.
  //Sans cette ecriture la page continuerait d'afficher l'ancienne palette.
  RecordFichierParametres();
  StockMessage("Palette des tableaux migree vers le theme sombre");
}
void StockFichier(String filename, String Contenu) {  //Fichier de données
  File file = LittleFS.open("/" + filename, FILE_WRITE);
  file.print(Contenu);
  file.close();
  Serial.println("Ecriture fichier : " + filename);
}
// Sauvegarde des compteurs juste avant un redémarrage volontaire (reset ou OTA).
// Reprise du correctif amont V17.19.
//
// Les sources UxI, NotDef et UxIx3 intègrent l'énergie en RAM : un redémarrage
// remettait leurs totaux à zéro, et l'énergie du jour repartait de la dernière
// photo de minuit. Avec la mise à jour par OTA devenue le chemin normal, la perte
// se produisait à chaque nouvelle version. UxIx2 n'est pas concerné : le module
// JSY conserve ses propres compteurs et les relit au démarrage.
//
// On conserve les références de minuit telles quelles, pour que l'énergie du jour
// reste juste, et on ajoute les accumulateurs RAM propres à la source.
void RecordEnergieEncours(String date) {
  JsonDocument conf;

  conf["Date"] = date;
  conf["Energie_M_Soutiree"] = EAS_M_J0;  //Références de minuit, pas les totaux courants
  conf["Energie_M_Injectee"] = EAI_M_J0;
  conf["Energie_T_Soutiree"] = EAS_T_J0;
  conf["Energie_T_Injectee"] = EAI_T_J0;

  if (Source == "UxIx3") {
    conf["Energie_jour_Soutiree"] = Energie_jour_Soutiree;
    conf["Energie_jour_Injectee"] = Energie_jour_Injectee;
  } else if (Source == "UxI" || Source == "NotDef") {
    conf["EASfloat"] = EASfloat;
    conf["EAIfloat"] = EAIfloat;
  }

  String Json;
  serializeJson(conf, Json);
  File file = LittleFS.open("/EnergieMinuit.eng", FILE_WRITE);
  file.print(Json);
  file.close();
  Serial.println("Ecriture compteurs avant redemarrage");
}

void RecordEnergieMinuit(String date) {
  JsonDocument conf;

  conf["Date"] = date;
  conf["Energie_M_Soutiree"] = Energie_M_Soutiree;
  conf["Energie_M_Injectee"] = Energie_M_Injectee;
  conf["Energie_T_Soutiree"] = Energie_T_Soutiree;
  conf["Energie_T_Injectee"] = Energie_T_Injectee;
  String Json;
  serializeJson(conf, Json);
  File file = LittleFS.open("/EnergieMinuit.eng", FILE_WRITE);
  file.print(Json);  //Fichier au format JSON
  file.close();
  Serial.println("Ecriture fichier EnergieMinuit.eng");
}
void LectureConsoMatinJour(void) {
  if (!LittleFS.exists("/EnergieMinuit.eng")) {  //Fichier pas encore crée
    RecordEnergieMinuit(DateAMJ);
  }

  File file = LittleFS.open("/EnergieMinuit.eng", "r");  //Fichier energie de la veille
  Serial.println("Lecture du fichier /EnergieMinuit.eng");
  String json = file.readString();  // lit tout le fichier
  file.close();
  Serial.print("Json Energie reçu:");
  Serial.println(json);
  JsonDocument conf;
  DeserializationError error = deserializeJson(conf, json);

  if (error) {
    Serial.print("Erreur de parsing des paramètres: ");
    Serial.println(error.c_str());
    return;
  }

  EAS_T_J0 = conf["Energie_T_Soutiree"];  //Triac
  EAI_T_J0 = conf["Energie_T_Injectee"];
  EAS_M_J0 = conf["Energie_M_Soutiree"];  //Maison
  EAI_M_J0 = conf["Energie_M_Injectee"];

  //Restauration des accumulateurs RAM sauvés par RecordEnergieEncours() avant un
  //redémarrage volontaire. Les champs sont absents d'un fichier écrit à minuit ou
  //par une version antérieure : .isNull() laisse alors les valeurs à zéro, ce qui
  //correspond à l'ancien comportement. Source est déjà connue ici, ReadFichierParametres()
  //étant appelée juste avant dans setup().
  //Les accumulateurs sont bornés aux références de minuit : sans cela, le calage
  //plus bas relèverait Energie_M_* sans toucher au flottant, que la prochaine
  //intégration ferait aussitôt retomber.
  if (Source == "UxIx3") {
    if (!conf["Energie_jour_Soutiree"].isNull()) Energie_jour_Soutiree = conf["Energie_jour_Soutiree"].as<float>();
    if (!conf["Energie_jour_Injectee"].isNull()) Energie_jour_Injectee = conf["Energie_jour_Injectee"].as<float>();
    Energie_jour_Soutiree = max(Energie_jour_Soutiree, float(EAS_M_J0));
    Energie_jour_Injectee = max(Energie_jour_Injectee, float(EAI_M_J0));
    Energie_M_Soutiree = int(Energie_jour_Soutiree);
    Energie_M_Injectee = int(Energie_jour_Injectee);
  } else if (Source == "UxI" || Source == "NotDef") {
    if (!conf["EASfloat"].isNull()) EASfloat = conf["EASfloat"].as<float>();
    if (!conf["EAIfloat"].isNull()) EAIfloat = conf["EAIfloat"].as<float>();
    EASfloat = max(EASfloat, float(EAS_M_J0));
    EAIfloat = max(EAIfloat, float(EAI_M_J0));
    Energie_M_Soutiree = int(EASfloat);
    Energie_M_Injectee = int(EAIfloat);
  }

  if (Energie_T_Soutiree < EAS_T_J0) {
    Energie_T_Soutiree = EAS_T_J0;
  }
  if (Energie_T_Injectee < EAI_T_J0) {
    Energie_T_Injectee = EAI_T_J0;
  }
  if (Energie_M_Soutiree < EAS_M_J0) {
    Energie_M_Soutiree = EAS_M_J0;
  }
  if (Energie_M_Injectee < EAI_M_J0) {
    Energie_M_Injectee = EAI_M_J0;
  }
}
void RAZ_Histo_Conso() {
  Energie_T_Soutiree = 0;
  Energie_T_Injectee = 0;
  Energie_M_Soutiree = 0;
  Energie_M_Injectee = 0;
  RecordEnergieMinuit("");
}
//Importation des paramètres
//***************************
void ImportParametres(String Conf) {
  DeserializeConfiguration(Conf);
  EcritureEnROM();
}
void DeserializeConfiguration(String json) {

  int16_t Hdeb;
  Serial.print("Json reçu:");
  Serial.println(json);
  JsonDocument conf;
  DeserializationError error = deserializeJson(conf, json);

  if (error) {
    Serial.print("Erreur de parsing des paramètres: ");
    Serial.println(error.c_str());
    return;
  }

  String V = Version;

  int Versioncompile = round(100 * V.toFloat());
  int VersionStocke = conf["VersionStocke"];
  ssid = conf["ssid"].as<String>();
  password = conf["password"].as<String>();
  dhcpOn = conf["dhcpOn"];
  Gateway = conf["Gateway"];
  masque = conf["masque"];
  dns = conf["dns"];
  hostname = conf["hostname"] | hostname;
  RMS_IP[0] = conf["IP_Fixe"];
  CleAccesRef = conf["CleAccesRef"] | CleAccesRef;
  hostname = conf["hostname"] | hostname;
  Couleurs = conf["Couleurs"] | Couleurs;
  if (Couleurs.length() == 0) Couleurs = String(CouleurDefaut);
  ModePara = conf["ModePara"];
  ModeReseau = conf["ModeReseau"];
  Horloge = conf["Horloge"];
  idxFuseau = conf["idxFuseau"].isNull() ? idxFuseau : conf["idxFuseau"];
  ntpServer = conf["ntpServer"] | ntpServer;
  ESP32_Type = conf["ESP32_Type"];
  LEDgroupe = conf["LEDgroupe"];
  rotation = conf["rotation"];
  DurEcran = conf["DurEcran"];
  clickPresence = conf["clickPresence"].isNull() ? clickPresence : conf["clickPresence"];
  NumPageBoot = conf["NumPageBoot"].isNull() ? NumPageBoot : conf["NumPageBoot"];
  for (int i = 0; i < 8; i++) {
    Calibre[i] = conf["Calibre" + String(i)];
  }
  pUxI = conf["pUxI"];
  pTemp = conf["pTemp"];
  Source = conf["Source"].as<String>();
  RMSextIP = conf["RMSextIP"];
  RMSextIPauto = conf["RMSextIPauto"].isNull() ? RMSextIPauto : conf["RMSextIPauto"];
  EnphaseUser = conf["EnphaseUser"].as<String>();
  EnphasePwd = conf["EnphasePwd"].as<String>();
  EnphaseSerial = conf["EnphaseSerial"].as<String>();
  MQTTRepet = conf["MQTTRepet"];
  MQTTIP = conf["MQTTIP"];
  MQTTHost = conf["MQTTHost"].isNull() ? MQTTHost : conf["MQTTHost"].as<String>();
  MQTTSecure = conf["MQTTSecure"] | 0;
  MQTTPort = conf["MQTTPort"];
  MQTTUser = conf["MQTTUser"].as<String>();
  MQTTPwd = conf["MQTTPwd"].as<String>();
  MQTTPrefix = conf["MQTTPrefix"].as<String>();
  MQTTPrefixEtat = conf["MQTTPrefixEtat"].as<String>();
  MQTTdeviceName = conf["MQTTdeviceName"].as<String>();
  TopicP = conf["TopicP"].as<String>();
  subMQTT = conf["subMQTT"];
  nomRouteur = conf["nomRouteur"].as<String>();
  nomSondeFixe = conf["nomSondeFixe"].as<String>();
  nomSfixePpos = conf["nomSfixePpos"] | nomSfixePpos;
  nomSfixePneg = conf["nomSfixePneg"] | nomSfixePneg;
  nomSondeMobile = conf["nomSondeMobile"].as<String>();
  for (int i = 1; i < LES_ROUTEURS_MAX; i++) {
    RMS_IP[i] = conf["RMS_IP" + String(i)];
  }
  for (int c = 0; c < 4; c++) {
    nomTemperature[c] = conf["nomTemperature" + String(c)].as<String>();
    Source_Temp[c] = conf["Source_Temp" + String(c)].as<String>();
    refTempIP[c] = conf["refTempIP" + String(c)];
    TopicT[c] = conf["TopicT" + String(c)].as<String>();
    canalTempExterne[c] = conf["canalTempExterne" + String(c)];
    offsetTemp[c] = conf["offsetTemp" + String(c)];
  }
  CalibU = conf["CalibU"];
  CalibI = conf["CalibI"];
  Calibration();  //pour UxI
  TempoRTEon = conf["TempoRTEon"];
  MeteoOn = conf["MeteoOn"] | 0;
  MeteoLat = conf["MeteoLat"].isNull() ? MeteoLat : conf["MeteoLat"].as<float>();
  MeteoLon = conf["MeteoLon"].isNull() ? MeteoLon : conf["MeteoLon"].as<float>();
  MeteoPVcrete = conf["MeteoPVcrete"].isNull() ? MeteoPVcrete : conf["MeteoPVcrete"].as<float>();
  LastMeteoMillis = 0;  //Forcer une relecture météo après changement de paramètres
  SmaOn = conf["SmaOn"] | 0;
  SmaIP = conf["SmaIP"].isNull() ? SmaIP : conf["SmaIP"].as<String>();
  BallonVolume = conf["BallonVolume"].isNull() ? BallonVolume : conf["BallonVolume"].as<int16_t>();
  BallonTcible = conf["BallonTcible"].isNull() ? BallonTcible : conf["BallonTcible"].as<int16_t>();
  BallonPuissance = conf["BallonPuissance"].isNull() ? BallonPuissance : conf["BallonPuissance"].as<int16_t>();
  BallonCanal = conf["BallonCanal"].isNull() ? BallonCanal : conf["BallonCanal"].as<int8_t>();
  BallonModeIntel = conf["BallonModeIntel"] | 0;
  BallonTmin = conf["BallonTmin"].isNull() ? BallonTmin : conf["BallonTmin"].as<int16_t>();
  LastSmaMillis = 0;  //Forcer une relecture SMA après changement de paramètres
  AbsenceManuel = conf["AbsenceManuel"] | 0;
  AbsenceDebut = conf["AbsenceDebut"].isNull() ? AbsenceDebut : conf["AbsenceDebut"].as<String>();
  AbsenceFin = conf["AbsenceFin"].isNull() ? AbsenceFin : conf["AbsenceFin"].as<String>();
  AbsenceAntiLegio = conf["AbsenceAntiLegio"].isNull() ? AbsenceAntiLegio : conf["AbsenceAntiLegio"].as<byte>();
  AbsenceJoursSansChauffe = conf["AbsenceJoursSansChauffe"] | 0;
  FanGpio = conf["FanGpio"].isNull() ? FanGpio : conf["FanGpio"].as<int8_t>();
  FanCanalTemp = conf["FanCanalTemp"].isNull() ? FanCanalTemp : conf["FanCanalTemp"].as<int8_t>();
  FanTdemarrage = conf["FanTdemarrage"].isNull() ? FanTdemarrage : conf["FanTdemarrage"].as<int16_t>();
  FanTmax = conf["FanTmax"].isNull() ? FanTmax : conf["FanTmax"].as<int16_t>();
  FanVitesseMin = conf["FanVitesseMin"].isNull() ? FanVitesseMin : conf["FanVitesseMin"].as<uint8_t>();
  NotifOn = conf["NotifOn"] | 0;
  NotifUrl = conf["NotifUrl"].isNull() ? NotifUrl : conf["NotifUrl"].as<String>();
  DelestageOn = conf["DelestageOn"] | 0;
  DelestagePuissance = conf["DelestagePuissance"].isNull() ? DelestagePuissance : conf["DelestagePuissance"].as<int16_t>();
  DelestageMarge = conf["DelestageMarge"].isNull() ? DelestageMarge : conf["DelestageMarge"].as<byte>();
  PrixHP = conf["PrixHP"].isNull() ? PrixHP : conf["PrixHP"].as<float>();
  PrixHC = conf["PrixHC"].isNull() ? PrixHC : conf["PrixHC"].as<float>();
  PrixBleuHP = conf["PrixBleuHP"].isNull() ? PrixBleuHP : conf["PrixBleuHP"].as<float>();
  PrixBleuHC = conf["PrixBleuHC"].isNull() ? PrixBleuHC : conf["PrixBleuHC"].as<float>();
  PrixBlancHP = conf["PrixBlancHP"].isNull() ? PrixBlancHP : conf["PrixBlancHP"].as<float>();
  PrixBlancHC = conf["PrixBlancHC"].isNull() ? PrixBlancHC : conf["PrixBlancHC"].as<float>();
  PrixRougeHP = conf["PrixRougeHP"].isNull() ? PrixRougeHP : conf["PrixRougeHP"].as<float>();
  PrixRougeHC = conf["PrixRougeHC"].isNull() ? PrixRougeHC : conf["PrixRougeHC"].as<float>();
  EconomieMois = conf["EconomieMois"].isNull() ? EconomieMois : conf["EconomieMois"].as<float>();
  EconomieTotal = conf["EconomieTotal"].isNull() ? EconomieTotal : conf["EconomieTotal"].as<float>();
  WifiSleep = conf["WifiSleep"];
  ComSurv = conf["ComSurv"];
  pSerial = conf["pSerial"];
  Serial2V = conf["Serial2V"].isNull() ? Serial2V : conf["Serial2V"];
  pTriac = conf["pTriac"];
  // Zone des actions
  ReacCACSI = conf["ReacCACSI"];
  Fpwm = conf["Fpwm"];
  NbActions = conf["NbActions"];
  int iAct = 0;
  JsonArray arr = conf["Actions"];
  for (JsonObject obj : arr) {
    if (iAct >= NbActions) break;  // éviter de dépasser le tableau
    LesActions[iAct].Actif = obj["Actif"];
    if (LesActions[iAct].Actif > 3) LesActions[iAct].Actif = 0;  //Modes PWM/Demi-sinus supprimés
    LesActions[iAct].Titre = obj["Titre"].as<String>();
    LesActions[iAct].Host = obj["Host"].as<String>();
    LesActions[iAct].Port = obj["Port"];
    LesActions[iAct].OrdreOn = obj["OrdreOn"].as<String>();
    LesActions[iAct].OrdreOff = obj["OrdreOff"].as<String>();
    LesActions[iAct].Repet = obj["Repet"];
    LesActions[iAct].Tempo = obj["Tempo"];

    if (!obj["Reactivite"].isNull()) {  //Ancien nom de variable en V15
      LesActions[iAct].Ki = obj["Reactivite"];
    } else {
      LesActions[iAct].Ki = obj["Ki"];
      LesActions[iAct].Kp = obj["Kp"];
      LesActions[iAct].Kd = obj["Kd"];
      LesActions[iAct].PID = obj["PID"];
    }
    LesActions[iAct].ForceOuvre = !obj["ForceOuvre"].isNull() ? obj["ForceOuvre"] : 100;
    LesActions[iAct].NbPeriode = obj["NbPeriode"];

    Hdeb = 0;
    JsonArray arrP = !obj["Périodes"].isNull() ? obj["Périodes"] : obj["Periodes"];  //Anciens fichiers avec accent à éviter
    byte i = 0;
    for (JsonObject objP : arrP) {
      if (i >= LesActions[iAct].NbPeriode) break;  // éviter de dépasser le tableau
      LesActions[iAct].Type[i] = objP["Type"];
      LesActions[iAct].Hfin[i] = objP["Hfin"];
      LesActions[iAct].Hdeb[i] = Hdeb;
      Hdeb = LesActions[iAct].Hfin[i];
      LesActions[iAct].Vmin[i] = objP["Vmin"];
      LesActions[iAct].Vmax[i] = objP["Vmax"];
      LesActions[iAct].ONouvre[i] = !objP["ONouvre"].isNull() ? objP["ONouvre"] : 100;
      LesActions[iAct].Tinf[i] = objP["Tinf"];
      LesActions[iAct].Tsup[i] = objP["Tsup"];
      LesActions[iAct].Hmin[i] = objP["Hmin"];
      LesActions[iAct].Hmax[i] = objP["Hmax"];
      LesActions[iAct].CanalTemp[i] = objP["CanalTemp"] | 0;
      LesActions[iAct].SelAct[i] = objP["SelAct"] | 0;
      LesActions[iAct].Ooff[i] = objP["Ooff"] | 0;
      LesActions[iAct].O_on[i] = objP["O_on"] | 0;
      LesActions[iAct].Tarif[i] = objP["Tarif"] | 0;
      LesActions[iAct].MeteoCond[i] = objP["MeteoCond"] | 0;
      LesActions[iAct].MeteoSeuil[i] = objP["MeteoSeuil"] | 0;
      i++;
    }
    iAct++;
  }
  if (Versioncompile != VersionStocke) RecordFichierParametres();  //Mise à jour num version
}

String SerializeConfiguration() {
  JsonDocument conf;

  conf["Routeur"] = "Soleo";
  String V = Version;
  int VersionStocke = round(100 * V.toFloat());
  conf["VersionStocke"] = VersionStocke;
  conf["ssid"] = ssid;
  conf["password"] = password;
  conf["dhcpOn"] = dhcpOn;
  conf["IP_Fixe"] = RMS_IP[0];
  conf["Gateway"] = Gateway;
  conf["masque"] = masque;
  conf["dns"] = dns;
  conf["hostname"] = hostname;
  conf["CleAccesRef"] = CleAccesRef;
  conf["hostname"] = hostname;
  conf["Couleurs"] = Couleurs;
  conf["ModePara"] = ModePara;
  conf["ModeReseau"] = ModeReseau;
  conf["Horloge"] = Horloge;
  conf["idxFuseau"] = idxFuseau;
  conf["ntpServer"] = ntpServer;
  conf["ESP32_Type"] = ESP32_Type;
  conf["LEDgroupe"] = LEDgroupe;
  conf["rotation"] = rotation;
  conf["DurEcran"] = DurEcran;
  conf["clickPresence"] = clickPresence;
  conf["NumPageBoot"] = NumPageBoot;
  for (int i = 0; i < 8; i++) {
    conf["Calibre" + String(i)] = Calibre[i];
  }
  conf["pUxI"] = pUxI;
  conf["pTemp"] = pTemp;
  conf["Source"] = Source;
  conf["RMSextIP"] = RMSextIP;
  conf["RMSextIPauto"] = RMSextIPauto;
  conf["EnphaseUser"] = EnphaseUser;
  conf["EnphasePwd"] = EnphasePwd;
  conf["EnphaseSerial"] = EnphaseSerial;
  if (ModePara == 0) {
    MQTTRepet = 0;
    subMQTT = 0;
  }
  conf["MQTTRepet"] = MQTTRepet;
  conf["MQTTIP"] = MQTTIP;
  conf["MQTTHost"] = MQTTHost;
  conf["MQTTSecure"] = MQTTSecure;
  conf["MQTTPort"] = MQTTPort;
  conf["MQTTUser"] = MQTTUser;
  conf["MQTTPwd"] = MQTTPwd;
  conf["MQTTPrefix"] = MQTTPrefix;
  conf["MQTTPrefixEtat"] = MQTTPrefixEtat;
  conf["MQTTdeviceName"] = MQTTdeviceName;
  conf["TopicP"] = TopicP;
  conf["subMQTT"] = subMQTT;
  conf["nomRouteur"] = nomRouteur;
  conf["nomSondeFixe"] = nomSondeFixe;
  conf["nomSfixePpos"] = nomSfixePpos;
  conf["nomSfixePneg"] = nomSfixePneg;
  conf["nomSondeMobile"] = nomSondeMobile;
  for (int i = 0; i < LES_ROUTEURS_MAX; i++) {
    conf["RMS_IP" + String(i)] = RMS_IP[i];
  }

  for (int c = 0; c < 4; c++) {
    conf["nomTemperature" + String(c)] = nomTemperature[c];
    conf["Source_Temp" + String(c)] = Source_Temp[c];
    conf["refTempIP" + String(c)] = refTempIP[c];
    conf["TopicT" + String(c)] = TopicT[c];
    conf["canalTempExterne" + String(c)] = canalTempExterne[c];
    conf["offsetTemp" + String(c)] = offsetTemp[c];
  }
  conf["CalibU"] = CalibU;
  conf["CalibI"] = CalibI;
  conf["TempoRTEon"] = TempoRTEon;
  conf["WifiSleep"] = WifiSleep;
  conf["ComSurv"] = ComSurv;
  conf["pSerial"] = pSerial;
  conf["Serial2V"] = Serial2V;
  conf["pTriac"] = pTriac;
  conf["MeteoOn"] = MeteoOn;
  conf["MeteoLat"] = MeteoLat;
  conf["MeteoLon"] = MeteoLon;
  conf["MeteoPVcrete"] = MeteoPVcrete;
  conf["SmaOn"] = SmaOn;
  conf["SmaIP"] = SmaIP;
  conf["BallonVolume"] = BallonVolume;
  conf["BallonTcible"] = BallonTcible;
  conf["BallonPuissance"] = BallonPuissance;
  conf["BallonCanal"] = BallonCanal;
  conf["BallonModeIntel"] = BallonModeIntel;
  conf["BallonTmin"] = BallonTmin;
  conf["AbsenceManuel"] = AbsenceManuel;
  conf["AbsenceDebut"] = AbsenceDebut;
  conf["AbsenceFin"] = AbsenceFin;
  conf["AbsenceAntiLegio"] = AbsenceAntiLegio;
  conf["AbsenceJoursSansChauffe"] = AbsenceJoursSansChauffe;
  conf["FanGpio"] = FanGpio;
  conf["FanCanalTemp"] = FanCanalTemp;
  conf["FanTdemarrage"] = FanTdemarrage;
  conf["FanTmax"] = FanTmax;
  conf["FanVitesseMin"] = FanVitesseMin;
  conf["NotifOn"] = NotifOn;
  conf["NotifUrl"] = NotifUrl;
  conf["DelestageOn"] = DelestageOn;
  conf["DelestagePuissance"] = DelestagePuissance;
  conf["DelestageMarge"] = DelestageMarge;
  conf["PrixHP"] = serialized(String(PrixHP, 3));
  conf["PrixHC"] = serialized(String(PrixHC, 3));
  conf["PrixBleuHP"] = serialized(String(PrixBleuHP, 4));
  conf["PrixBleuHC"] = serialized(String(PrixBleuHC, 4));
  conf["PrixBlancHP"] = serialized(String(PrixBlancHP, 4));
  conf["PrixBlancHC"] = serialized(String(PrixBlancHC, 4));
  conf["PrixRougeHP"] = serialized(String(PrixRougeHP, 4));
  conf["PrixRougeHC"] = serialized(String(PrixRougeHC, 4));
  conf["EconomieMois"] = serialized(String(EconomieMois, 2));
  conf["EconomieTotal"] = serialized(String(EconomieTotal, 2));
  // Enregistrement des Actions
  if (ReacCACSI < 1)
    ReacCACSI = 1;
  if (Fpwm < 5)
    Fpwm = 500;
  conf["ReacCACSI"] = ReacCACSI;
  conf["Fpwm"] = Fpwm;
  conf["NbActions"] = NbActions;
  JsonArray arr = conf["Actions"].to<JsonArray>();
  for (int iAct = 0; iAct < NbActions; iAct++) {

    JsonObject obj = arr.add<JsonObject>();
    obj["Action"] = iAct;
    obj["Actif"] = LesActions[iAct].Actif;
    obj["Titre"] = LesActions[iAct].Titre;
    obj["Host"] = LesActions[iAct].Host;
    obj["Port"] = LesActions[iAct].Port;
    obj["OrdreOn"] = LesActions[iAct].OrdreOn;
    obj["OrdreOff"] = LesActions[iAct].OrdreOff;
    obj["ForceOuvre"] = LesActions[iAct].ForceOuvre;
    obj["Repet"] = LesActions[iAct].Repet;
    obj["Tempo"] = LesActions[iAct].Tempo;
    obj["Kp"] = LesActions[iAct].Kp;
    obj["Ki"] = LesActions[iAct].Ki;
    obj["Kd"] = LesActions[iAct].Kd;
    obj["PID"] = LesActions[iAct].PID;
    obj["NbPeriode"] = LesActions[iAct].NbPeriode;
    //JsonArray arrP = obj["Périodes"].to<JsonArray>();
    JsonArray arrP = obj["Periodes"].to<JsonArray>();
    for (byte i = 0; i < LesActions[iAct].NbPeriode; i++) {
      JsonObject objP = arrP.add<JsonObject>();
      if (ModePara == 0) {  // standard
        LesActions[iAct].CanalTemp[i] = -1;
        LesActions[iAct].SelAct[i] = 255;
      }
      objP["Periode"] = i;
      objP["Type"] = LesActions[iAct].Type[i];
      objP["Hfin"] = LesActions[iAct].Hfin[i];
      objP["Vmin"] = LesActions[iAct].Vmin[i];
      objP["Vmax"] = LesActions[iAct].Vmax[i];
      objP["ONouvre"] = LesActions[iAct].ONouvre[i];
      objP["Tinf"] = LesActions[iAct].Tinf[i];
      objP["Tsup"] = LesActions[iAct].Tsup[i];
      objP["Hmin"] = LesActions[iAct].Hmin[i];
      objP["Hmax"] = LesActions[iAct].Hmax[i];
      objP["CanalTemp"] = LesActions[iAct].CanalTemp[i];
      objP["SelAct"] = LesActions[iAct].SelAct[i];
      objP["Ooff"] = LesActions[iAct].Ooff[i];
      objP["O_on"] = LesActions[iAct].O_on[i];
      objP["Tarif"] = LesActions[iAct].Tarif[i];
      objP["MeteoCond"] = LesActions[iAct].MeteoCond[i];
      objP["MeteoSeuil"] = LesActions[iAct].MeteoSeuil[i];
    }
  }
  String Json;
  serializeJson(conf, Json);
  return Json;
}

void Record_Data(String dateAMJ, String MesSage, int16_t HeureCouranteDeci_) {
  if (dateAMJ == "") return;
  //Test s'il y a de la place
  int Occupation = 100 * LittleFS.usedBytes() / LittleFS.totalBytes();
  Serial.println("Occupation:" + String(Occupation));
  if (Occupation > 80) {
    int LePlusVieux = 333333;
    int p, dateJ;
    String S = "", FileName;
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
      FileName = String(file.name());
      p = FileName.indexOf(".csv");
      //On ne purge QUE les fichiers de données mensuels "Mois_Wh_AAAAMM.csv".
      //Sinon "histmeteo.csv" donnait dateJ=0 (chaîne non numérique) et était toujours supprimé en premier.
      if (p > 6 && FileName.indexOf("Mois_Wh_") >= 0) {
        dateJ = FileName.substring(p - 6, p).toInt();
        if (dateJ >= 200000 && dateJ < LePlusVieux) {  //AAAAMM valide, on exclut les valeurs aberrantes
          LePlusVieux = dateJ;
          S = FileName;
        }
      }
      file = root.openNextFile();
    }
    if (S != "") LittleFS.remove("/" + S);  //On retire le fichier du mois le plus vieux (s'il existe)
  }
  String AM_file = "/Mois_Wh_" + dateAMJ.substring(0, 6) + ".csv";
  bool biSonde = false;
  if (nomSondeFixe != "" && (Source_data == "UxIx2" || ((Source_data == "ShellyEm" || Source_data == "ShellyPro") && EnphaseSerial.toInt() != 3))) biSonde = true;

  String New_Record_Conf = "Date";
  String Data = dateAMJ + "," + String(EnergieJour_M_Soutiree) + "," + String(EnergieJour_M_Injectee);
  if (EnergieJour_M_Soutiree > 1000000 || EnergieJour_M_Injectee > 1000000) Data = dateAMJ + ", ,";  //Aberration à certains ReseT
  New_Record_Conf += "," + Filtre_Nom(nomSondeMobile) + " / Soutirée," + Filtre_Nom(nomSondeMobile) + " / Injectée";
  if (nomSondeFixe != "" && nomSfixePpos != "" && biSonde && EnergieJour_T_Soutiree < 1000000) {
    New_Record_Conf += "," + Filtre_Nom(nomSondeFixe) + " / " + Filtre_Nom(nomSfixePpos);
    Data += "," + String(EnergieJour_T_Soutiree);
  } else {
    New_Record_Conf += ",";  //Colonne vide
    Data += ",";
  }
  if (nomSondeFixe != "" && nomSfixePneg != "" && biSonde && EnergieJour_T_Injectee < 1000000) {
    New_Record_Conf += "," + Filtre_Nom(nomSondeFixe) + " / " + Filtre_Nom(nomSfixePneg);
    Data += "," + String(EnergieJour_T_Injectee);
  } else {
    New_Record_Conf += ",";  //Colonne vide
    Data += ",";
  }
  New_Record_Conf += ",Heure Déci. ,Message";
  float Duree = float(HeureCouranteDeci_) / 100.0;
  Data += "," + String(Duree) + "," + MesSage;
  if (!LittleFS.exists(AM_file)) {
    File file = LittleFS.open(AM_file, FILE_WRITE);
    file.print(New_Record_Conf + "\r\n");
    file.close();
  } else {

    if (New_Record_Conf != Record_Conf) {  // Nouvelle configuration
      Record_Conf = New_Record_Conf;
      File fileJ = LittleFS.open(AM_file, FILE_APPEND);
      fileJ.print(Record_Conf + "\r\n");
      fileJ.close();
    }
  }
  File fileJ = LittleFS.open(AM_file, FILE_APPEND);
  fileJ.print(Data + "\r\n");
  fileJ.close();
}
String Filtre_Nom(String Nom) {
  Nom.replace(",", ".");
  Nom.replace("#", "=");
  return Nom;
}
void LitLastRecord_Conf() {  // Lire la dernière configuration connue

  String ligne = "";
  LastRecordConf = true;
  String AM_file = "/Mois_Wh_" + DateAMJ.substring(0, 6) + ".csv";

  if (LittleFS.exists(AM_file)) {
    File file = LittleFS.open(AM_file);
    while (file.available()) {
      char c = file.read();
      if (c == '\n' || c == '\r') {
        if (ligne.length() > 10) {
          if (ligne.indexOf("Date,") == 0) {
            ligne.trim();
            Record_Conf = ligne;
          }
          ligne = "";
        }
      } else {
        ligne += String(c);
      }
    }
    file.close();
  } else {
    Record_Conf = "";
  }
}
