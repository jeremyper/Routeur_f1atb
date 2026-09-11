// ***************
// *  WEB SERVER *
// ***************
#include <Arduino.h>
bool opened = false;
String ConfImport;
//Autorisation du téléversement en cours (/update firmware et /import fichier).
//Évaluée à l'ouverture du flux, relue à chaque bloc reçu et à la réponse finale.
bool UploadAutorise = false;
void Init_Server() {
  // Init Web Server on port 80
  server.on("/", handleRoot);          //Tableau de bord Soleo
  server.on("/DashJS", handleDashJS);
  server.on("/mesures", handleMesures);  //Ancienne page d'accueil : mesures détaillées
  server.on("/MainJS1", handleMainJS1);
  server.on("/MainJS2", handleMainJS2);
  server.on("/MainJS3", handleMainJS3);
  server.on("/Para", handlePara);
  server.on("/ParaJS1", handleParaJS1);
  server.on("/ParaJS2", handleParaJS2);
  server.on("/ParaCommunJS", handleParaCommunJS);
  server.on("/ParaFixe", handleParaFixe);  //Paramètres fixes en fichier
  server.on("/ParaVar", handleParaVar);    //Paramètres variables
  server.on("/ParaNew", HTTP_POST, handleParaNew);
  server.on("/CleUpdate", handleCleUpdate);
  server.on("/Actions", handleActions);
  server.on("/ActionsJS1", handleActionsJS1);
  server.on("/ActionsJS2", handleActionsJS2);
  server.on("/ActionsJS3", handleActionsJS3);
  server.on("/ActionsJS4", handleActionsJS4);
  server.on("/ActionsJS5", handleActionsJS5);
  server.on("/PinsActionsJS", handlePinsActionsJS);
  server.on("/ShowAction", handleShowAction);
  server.on("/UpdateK", handleUpdateK);
  server.on("/Brute", handleBrute);
  server.on("/BruteJS1", handleBruteJS1);
  server.on("/BruteJS2", handleBruteJS2);
  server.on("/ajax_histo48h", handleAjaxHisto48h);
  server.on("/ajax_histo1an", handleAjaxHisto1an);
  server.on("/ajax_histmeteo", handleAjaxHistMeteo);  //Historique prévision météo / production
  server.on("/ajax_journal", handleAjaxJournal);      //Journal d'événements en langage naturel
  server.on("/ajax_dataRMS", handleAjaxRMS);
  server.on("/ajax_dataESP32", handleAjaxESP32);
  server.on("/ajax_data", handleAjaxData);
  server.on("/ajax_data10mn", handleAjaxData10mn);
  server.on("/ajax_etatActions", handleAjax_etatActions);
  server.on("/ajax_etatActionX", handleAjax_etatActionX);
  server.on("/ForceAction", handleForceAction);
  server.on("/ajax_absence", handleAbsence);
  server.on("/ajax_Temperature", handleAjaxTemperature);
  server.on("/ajax_Noms", handleAjaxNoms);
  server.on("/ajaxRAZhisto", handleajaxRAZhisto);
  server.on("/SetGPIO", handleSetGpio);
  server.on("/Export", handleExport);
  server.on("/export_file", handleExport_file);
  server.on("/ListeFile", handleListeFile);
  server.on("/restart", handleRestart);
  server.on("/Wifi", handleWifi);
  server.on("/AP_ScanWifi", handleAP_ScanWifi);
  server.on("/AP_SetWifi", handleAP_SetWifi);
  server.on("/Heure", handleHeure);
  server.on("/HourUpdate", handleHourUpdate);
  server.on("/Couleurs", handleCouleurs);
  server.on("/CommunCouleurJS", handleCommunCouleurJS);
  server.on("/CouleursAjax", handleCouleursAjax);
  server.on("/CouleurUpdate", handleCouleurUpdate);
  server.on("/commun.css", handleCommunCSS);
  server.on("/theme.js", handleThemeJS);  //Bascule clair/sombre partagée par toutes les pages
  server.on("/ajax_testNotif", handleTestNotif);  //Envoi de test depuis la page Réglages
  server.on("/favicon.ico", handleFavicon);
  server.on("/favicon192.ico", handleFavicon192);
  server.on("/manifest.json", handleManifest);
  server.onNotFound(handleNotFound);

  // SERVER OTA
  server.on("/OTA", HTTP_GET, []() {
    lectureCookie(OtaHtml);
  });
  /*handling uploading firmware file */
  server.on(
    "/update", HTTP_POST, []() {
      //Double contrôle : le drapeau (posé à l'ouverture du flux) ET la clé au moment de
      //la réponse, pour qu'un POST sans fichier ne réutilise pas un drapeau resté à true.
      if (!UploadAutorise || !AccesUploadAutorise()) {
        UploadAutorise = false;
        server.send(401, "text/plain", "Acces refuse : cle d'acces requise");
        return;
      }
      UploadAutorise = false;
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ReseT((Update.hasError()) ? "Update FAIL" : "Update OK");
    },
    []() {
      HTTPUpload &upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        //Le contrôle se fait à l'ouverture du flux : sans clé valide on n'écrit pas
        //une seule ligne dans la partition OTA.
        UploadAutorise = AccesUploadAutorise();
        if (!UploadAutorise) {
          TelnetPrintln("Update refusé : clé d'accès invalide");
          return;
        }
        TelnetPrintln("Update: " + String(upload.filename));
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {  // start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (!UploadAutorise) return;
        /* flashing firmware to ESP*/
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (!UploadAutorise) return;
        if (Update.end(true)) {  // true to set the size to the current progress
          TelnetPrintln("Update Success: Rebooting..." + String(upload.totalSize));
        } else {
          Update.printError(Serial);
        }
      }
    });

  /*handling uploading file */
  server.on(
    "/import", HTTP_POST, []() {
      if (!UploadAutorise || !AccesUploadAutorise()) {
        UploadAutorise = false;
        server.send(401, "text/plain", "Acces refuse : cle d'acces requise");
        return;
      }
      UploadAutorise = false;
      server.send(200, "text/plain", "OK");
    },
    []() {
      HTTPUpload &upload = server.upload();

      // La variable pour stocker le nom du fichier (si elle est globale ou statique)
      static String fileName;

      if (upload.status == UPLOAD_FILE_START)  // État de début du téléversement
      {
        //Le nom de fichier vient du client : contrôle de la clé ET du nom avant
        //d'écrire quoi que ce soit dans LittleFS.
        UploadAutorise = AccesUploadAutorise();
        if (!UploadAutorise) {
          TelnetPrintln("Import refusé : clé d'accès invalide");
          return;
        }
        // --- C'EST ICI QUE VOUS RÉCUPÉREZ LE NOM DU FICHIER ---
        fileName = upload.filename;
        if (!NomFichierSur(fileName)) {
          TelnetPrintln("Import refusé : nom de fichier invalide (" + fileName + ")");
          UploadAutorise = false;
          return;
        }

        TelnetPrintln("Début Upload du fichier: " + fileName);
        ConfImport = "";

        // Votre logique existante (ici, pour `opened`)
        if (opened == false) {
          opened = true;
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (!UploadAutorise) return;
        for (int i = 0; i < upload.currentSize; i++) {
          ConfImport += String(char(upload.buf[i]));
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (!UploadAutorise) return;
        TelnetPrintln("Fin Upload du fichier: " + fileName);
        if (fileName.indexOf(".json") > 0) {
          int nbOK = 0;
          if (ConfImport.indexOf("Version") != -1)
            nbOK += 1;
          if (ConfImport.indexOf("Couleurs") != -1)
            nbOK += 1;
          if (ConfImport.indexOf("Source") != -1)
            nbOK += 1;
          if (ConfImport.indexOf("NbActions") != -1)
            nbOK += 1;

          if (nbOK == 4) ImportParametres(ConfImport);  //C'est un fichier de paramètres de configuration
        } else {                                        //Autre fichier
          StockFichier(fileName, ConfImport);
        }
      }
    });

  // here the list of headers to be recorded
  const char *headerkeys[] = { "User-Agent", "Cookie" };
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
  // ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);

  server.begin();
}

void handleRoot() {  // Page d'accueil : tableau de bord Soleo

  server.send(200, "text/html", DashHtml);
}
void handleDashJS() {  // Code Javascript du tableau de bord
  CacheEtClose(300);
  server.send(200, "text/javascript", DashJS);
}
void handleMesures() {  // Ancienne page d'accueil : mesures détaillées et graphiques

  server.send(200, "text/html", MainHtml);
}
void handleWifi() {
  lectureCookie(ConnectAP_Html);
}
void handleMainJS1() {                  // Code Javascript
  String S = "var biSonde=false;\r\n";  // Pour tracer immediatement tableau Mesures
  if (nomSondeFixe != "" && (Source_data == "UxIx2" || ((Source_data == "ShellyEm" || Source_data == "ShellyPro") && EnphaseSerial.toInt() != 3))) {
    S = "var biSonde=true;\r\n";
  }

  server.send(200, "text/javascript", S + MainJS1);  // Javascript code
}
void handleMainJS2() {  // Code Javascript
  CacheEtClose(300);
  server.send(200, "text/javascript", MainJS2);  // Javascript code
}
void handleMainJS3() {  // Code Javascript
  CacheEtClose(300);
  server.send(200, "text/javascript", MainJS3);  // Javascript code
}

void handleBrute() {  // Page données brutes
  CacheEtClose(300);
  server.send(200, "text/html", PageBrute);
}
void handleBruteJS1() {  // Code Javascript
  CacheEtClose(300);
  server.send(200, "text/javascript", PageBruteJS1);  // Javascript code
}
void handleBruteJS2() {  // Code Javascript
  CacheEtClose(300);
  server.send(200, "text/javascript", PageBruteJS2);  // Javascript code
}

void handleAjaxRMS() {  // Envoi des dernières données  brutes reçues du RMS
  String S = "";
  String RMSExtDataB = "";
  int LastIdx = server.arg(0).toInt();
  if (Source == "Ext") {
    // Use WiFiClient class to create TCP connections
    WiFiClient clientESP_RMS;
    String host = IP2String(RMSextIP);
    if (!clientESP_RMS.connect(host.c_str(), 80, 3000)) {
      delay(500);
      if (!clientESP_RMS.connect(host.c_str(), 80, 3000)) {
        StockMessage("connection to ESP_RMS external failed (call from  handleAjaxRMS)");
        clientESP_RMS.stop();
        delay(100);
        return;
      }
    }
    String url = "/ajax_dataRMS?idx=" + String(LastIdx);
    clientESP_RMS.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (clientESP_RMS.available() == 0) {
      if (millis() - timeout > 5000) {
        StockMessage(">>> clientESP_RMS Timeout !");
        clientESP_RMS.stop();
        return;
      }
    }
    // Lecture des données brutes distantes
    while (clientESP_RMS.available()) {
      RMSExtDataB += clientESP_RMS.readStringUntil('\r');
    }
    S = RMSExtDataB.substring(RMSExtDataB.indexOf("\n\n") + 2);
    clientESP_RMS.stop();
  } else {
    S = DATE + RS + Source_data;
    if (Source_data == "NotDef") {
      S += GS + " ";
    }
    if (Source_data == "UxI") {
      S += RS + String(Tension_M) + RS + String(Intensite_M) + RS + String(PowerFactor_M) + GS;
      int i0 = 0;
      int i1 = 0;
      for (int i = 0; i < 100; i++) {
        i1 = (i + 1) % 100;
        if (voltM[i] <= 0 && voltM[i1] > 0) {
          i0 = i1;  // Point de départ tableau . Phase positive
          i = 100;
        }
      }
      for (int i = 0; i < 100; i++) {
        i1 = (i + i0) % 100;
        S += String(int(10 * voltM[i1])) + RS;  // Voltages*10. Increase dynamic
      }
      S += "0" + GS;
      for (int i = 0; i < 100; i++) {
        i1 = (i + i0) % 100;
        S += String(int(10 * ampM[i1])) + RS;  // Currents*10
      }
      S += "0";
    }
    if (Source_data == "UxIx2") {

      S += GS + String(Tension_M) + RS + String(Intensite_M) + RS + String(PuissanceS_M - PuissanceI_M) + RS + String(PowerFactor_M) + RS + String(Energie_M_Soutiree) + RS + String(Energie_M_Injectee);
      S += RS + String(Tension_T) + RS + String(Intensite_T) + RS + String(PuissanceS_T - PuissanceI_T) + RS + String(PowerFactor_T) + RS + String(Energie_T_Soutiree) + RS + String(Energie_T_Injectee);
      S += RS + String(Frequence);
    }
    if (Source_data == "Enphase") {
      S += GS + String(Tension_M) + RS + String(Intensite_M) + RS + String(PuissanceS_M - PuissanceI_M) + RS + String(PowerFactor_M) + RS + String(Energie_M_Soutiree) + RS + String(Energie_M_Injectee);
      S += RS + String(PactProd) + RS + String(PactConso_M);
      String SessionId = "Not Received from Enphase";
      if (Session_id != "") {
        SessionId = "Ok Received from Enphase";
      }
      String Token_Enphase = "Not Received from Enphase";
      if (TokenEnphase.length() > 50) {
        Token_Enphase = "Ok Received from Enphase";
      }
      if (EnphaseUser == "") {
        SessionId = "Not Requested";
        Token_Enphase = "Not Requested";
      }
      S += RS + SessionId;

      S += RS + Token_Enphase;
    }
    if (Source_data == "SmartG") {
      S += GS + SG_dataBrute;
    }
    if (Source_data == "HomeW") {
      S += GS + HW_dataBrute;
    }
    if (Source_data == "ShellyEm" || Source_data == "ShellyPro") {
      S += GS + ShEm_dataBrute;
    }
    if (Source_data == "UxIx3") {
      S += GS + MK333_dataBrute;
    }
    if (Source_data == "Pmqtt") {
      S += GS + P_MQTT_Brute;
    }
  }

  server.send(200, "text/html", S);
}
void handleAjaxHisto48h() {  // Envoi Historique de 50h (600points) toutes les 5mn
  String S = "";
  String T = "";
  String U = "";
  String Ouverture = "";
  String Pmaxi = String(PuisMaxS_M) + RS + String(PuisMaxI_M) + RS + String(PuisMaxS_T) + RS + String(PuisMaxI_T);
  int iS = IdxStockPW;
  for (int i = 0; i < 600; i++) {
    S += String(tabPw_Maison_5mn[iS]) + ",";
    T += String(tabPw_Triac_5mn[iS]) + ",";
    iS = (1 + iS) % 600;
  }

  for (int canal = 0; canal < 4; canal++) {
    iS = IdxStockPW;
    for (int i = 0; i < 600; i++) {
      U += String(float(tabTemperature_5mn[canal][iS]) * 0.1) + ",";
      iS = (1 + iS) % 600;
    }
    U += String(temperature[canal]) + "|";
  }
  for (int i = 0; i < NbActions; i++) {
    if (LesActions[i].Actif > 0) {  //Action 0 incluse : la condition triac/zéro-cross (ITmode>0||i>0) n'a plus lieu d'être
      iS = IdxStockPW;
      Ouverture += GS;
      for (int j = 0; j < 600; j++) {
        Ouverture += String(tab_histo_ouverture[i][iS]) + RS;
        iS = (1 + iS) % 600;
      }
      Ouverture += LesActions[i].Titre;
    }
  }


  server.send(200, "text/html", Pmaxi + GS + S + GS + T + GS + U + Ouverture);
}
void handleAjaxESP32() {  // Envoi des dernières infos sur l'ESP32
  IT10ms = 0;
  IT10ms_in = 0;
  String S = "";
  float H = float(T_On_seconde) / 3600.0;
  String coeur0 = String(int(previousTimeRMSMin)) + ", " + String(int(previousTimeRMSMoy)) + ", " + String(int(previousTimeRMSMax));
  String coeur1 = String(int(previousLoopMin)) + ", " + String(int(previousLoopMoy)) + ", " + String(int(previousLoopMax));
  String acces = WiFi.RSSI() + RS + WiFi.BSSIDstr() + RS + WiFi.channel();
  String Mac = WiFi.macAddress();
  String adr = WiFi.localIP().toString() + US + hostname + US + WiFi.globalIPv6().toString() + RS + WiFi.gatewayIP().toString() + RS + WiFi.subnetMask().toString();
  S += String(H) + RS + String(ESP32_Type) + RS + acces + RS + Mac + RS + ssid + RS + adr;
  S += RS + coeur0 + RS + coeur1 + RS + "inutil" + RS;
  S += String(esp_get_free_internal_heap_size()) + RS + String(esp_get_minimum_free_heap_size()) + RS;
  delay(15);  // Comptage interruptions
  if (IT10ms_in > 0) {
    S += String(IT10ms_in) + "/" + String(IT10ms);
  } else {
    S += "Pas de Triac";
  }
  if (ITmode > 0) {
    S += RS + "Secteur";
  } else {
    S += RS + "Horloge ESP";
  }
  S += RS + String(Nbr_DS18B20);
  S += RS + AllTemp + GS;
  int j = idxMessage;
  for (int i = 0; i < 10; i++) {
    S += RS + MessageH[j];
    j = (j + 1) % 10;
  }
  S += GS;
  for (int i = 1; i < LES_ROUTEURS_MAX; i++) {
    if (RMS_IP[i] > 0) {
      String nom = "", after;
      SplitS(RMS_NomEtat[i], nom, US, after);
      S += nom + " (" + IP2String(RMS_IP[i]) + ") " + ES + String(RMS_Note[i]) + "/" + String(RMS_NbCx[i]) + RS;
    }
  }

  server.send(200, "text/html", S);
}
void handleAjaxHisto1an() {  // Envoi Historique Energie quotiiienne sur 1 an 370 points
  envoyerHistoriqueEnergie(server);  // envoi direct depuis la fonction qui produit les données
}
void handleAjaxData() {  // Données page d'accueil
  String DateLast = "Attente d'une mise à l'heure par internet";
  if (Horloge == 1)
    DateLast = "Attente d'une mise à l'heure par le Linky";
  if (ModeReseau == 0 && WiFi.getMode() != WIFI_STA)
    DateLast = "Sélectionnez un réseau <a href='/Wifi'>Wifi</a>";
  if (Horloge > 1 && Horloge < 5)
    DateLast = "Attente d'une mise à l'heure  <a href='/Heure' >manuellement</a> ";
  if (Horloge == 5)
    DateLast = "Attente d'une mise à l'heure un ESP externe (maître)";
  if (HeureValide) {
    DateLast = DATE;
  }
  String S = LesTemperatures();
  S = "Deb" + RS + DateLast + RS + Source_data + RS + LTARF + RS + STGEt + RS + S + RS + String(Pva_valide);
  S += GS + String(PuissanceS_M) + RS + String(PuissanceI_M) + RS + String(PVAS_M) + RS + String(PVAI_M);
  S += RS + String(EnergieJour_M_Soutiree) + RS + String(EnergieJour_M_Injectee) + RS + String(Energie_M_Soutiree) + RS + String(Energie_M_Injectee);
  if (Source_data == "UxIx2" || ((Source_data == "ShellyEm" || Source_data == "ShellyPro") && EnphaseSerial.toInt() != 3)) {  // UxIx2 ou Shelly monophasé avec 2 sondes
    S += GS + String(PuissanceS_T) + RS + String(PuissanceI_T) + RS + String(PVAS_T) + RS + String(PVAI_T);
    S += RS + String(EnergieJour_T_Soutiree) + RS + String(EnergieJour_T_Injectee) + RS + String(Energie_T_Soutiree) + RS + String(Energie_T_Injectee);
  }
  S += GS + "Fin\r";

  server.send(200, "text/html", S);
}
void handleAjax_etatActions() {
  int Force = server.arg("Force").toInt();
  int NumAction = server.arg("NumAction").toInt();
  ExtraitCookie();
  if (Force != 0 && NumAction >= 0 && NumAction < NbActions && CleAccesRef == CleAcces) {
    if (Force > 0) {
      if (LesActions[NumAction].tOnOff < 0) {
        LesActions[NumAction].tOnOff = 0;
      } else {
        LesActions[NumAction].tOnOff += 30;
      }
    } else {
      if (LesActions[NumAction].tOnOff > 0) {
        LesActions[NumAction].tOnOff = 0;
      } else {
        LesActions[NumAction].tOnOff -= 30;
      }
    }
    LesActions[NumAction].Prioritaire();
  }
  int NbActifs = 0;
  String S = "";
  String On_;
  for (int i = 0; i < NbActions; i++) {
    if (LesActions[i].Actif > 0) {  //Action 0 incluse : la condition triac/zéro-cross (ITmode>0||i>0) n'a plus lieu d'être
      S += String(i) + RS + LesActions[i].Titre + RS;
      if (LesActions[i].Actif == 1) {  //Relais On/Off (action 0 comprise)
        if (LesActions[i].On) {
          S += "On" + RS;
        } else {
          S += "Off" + RS;
        }
      } else {
        S += String(100 - Retard[i]) + RS;
      }
      S += String(LesActions[i].tOnOff) + RS;
      S += String(int(LesActions[i].H_Ouvre * 100.0)) + RS;
      S += GS;
      NbActifs++;
    }
  }
  String LesTemp = LesTemperatures();
  S = LesTemp + GS + String(Source_data) + GS + String(RMSextIP) + GS + NbActifs + GS + S;

  server.send(200, "text/html", S);
}
void handleAjax_etatActionX() {
  int NumAction = server.arg("NumAction").toInt();
  byte Actif = 0;
  int Ouvre = 0;
  int Hequiv = 0;
  if (NumAction >= 0 && NumAction < NbActions) {
    Actif = LesActions[NumAction].Actif;
    Ouvre = 100 - Retard[NumAction];
    Hequiv = int(100 * LesActions[NumAction].H_Ouvre);
  }
  String S = String(Actif) + GS + String(Ouvre) + GS + String(Hequiv);

  server.send(200, "text/html", S);
}
void handleForceAction() {
  if (!AccesAutorise()) return;
  int Force = server.arg("Force").toInt();
  int NumAction = server.arg("NumAction").toInt();
  if (NumAction >= 0 && NumAction < NbActions) {
    if (Force > 0 && LesActions[NumAction].tOnOff <= 0) {
      String titre = (LesActions[NumAction].Titre != "") ? LesActions[NumAction].Titre : "Action " + String(NumAction);
      JournalAjoute(titre + " : marche forcée démarrée (" + String(Force) + " mn)");
    }
    LesActions[NumAction].tOnOff = Force;
  }

  server.send(200, "text/html", "Force");
}
//Bascule manuelle du mode absence depuis l'accueil. ?set=1 active, ?set=0 désactive
void handleAbsence() {
  if (server.hasArg("set")) {  //Écriture : protégée. La simple lecture d'état reste libre.
    if (!AccesAutorise()) return;
    byte nouveau = (server.arg("set").toInt() == 1) ? 1 : 0;
    if (nouveau != AbsenceManuel) {  //Anti-rebond : pas d'écriture flash si l'état ne change pas
      AbsenceManuel = nouveau;
      GestionAbsence();           //Réévalue l'état immédiatement
      RecordFichierParametres();  //Persiste le choix
    }
  }
  server.send(200, "text/html", ModeAbsenceActif ? "1" : "0");
}
void handleShowAction() {
  int NumAction = server.arg("NumAction").toInt();
  if (NumAction < 0 || NumAction >= NbActions) {
    server.send(400, "text/plain", "Bad NumAction");
    return;
  }
  server.send(200, "text/html", String(round(LastErrorPw[NumAction])) + RS + String(Propor[NumAction]) + RS + String(IntegrErrorPw[NumAction]) + RS + String(DeriveF[NumAction]));
  LastShowActionMillis = millis();
}
void handleUpdateK() {
  if (!AccesAutorise()) return;
  int iAct = server.arg("iAct").toInt();
  if (iAct < 0 || iAct >= NbActions) {
    server.send(400, "text/plain", "Bad iAct");
    return;
  }
  LesActions[iAct].Kp = server.arg("Kp").toInt();
  LesActions[iAct].Ki = server.arg("Ki").toInt();
  LesActions[iAct].Kd = server.arg("Kd").toInt();

  server.send(200, "text/html", "Ok UpdateK");
}
void handleAjaxTemperature() {
  String LesTemp = LesTemperatures();

  server.send(200, "text/html", GS + LesTemp + RS);
}
void handleRestart() {  // Eventuellement Reseter l'ESP32 à distance
  if (!AccesAutorise()) return;
  server.send(200, "text/plain", "OK Reset. Attendez.");
  delay(1000);
  ReseT("Reset Demandé par le Web");
}
void handleAjaxData10mn() {  // Envoi Historique de 10mn (300points)Energie Active Soutiré - Injecté
  String S = "";
  String T = "";
  String Ouverture = "";
  int iS = IdxStock2s;
  for (int i = 0; i < 300; i++) {
    S += String(tabPw_Maison_2s[iS]) + ",";
    S += String(tabPva_Maison_2s[iS]) + ",";
    T += String(tabPw_Triac_2s[iS]) + ",";
    T += String(tabPva_Triac_2s[iS]) + ",";
    iS = (1 + iS) % 300;
  }
  for (int i = 0; i < NbActions; i++) {
    if (LesActions[i].Actif > 0) {
      iS = IdxStock2s;
      Ouverture += GS + String(i) + ES;
      for (int j = 0; j < 300; j++) {
        Ouverture += String(tab_histo_2s_ouverture[i][iS]) + RS;
        iS = (1 + iS) % 300;
      }
    }
  }

  server.send(200, "text/html", Source_data + GS + S + GS + T + Ouverture);
}
void handleAjaxNoms() {
  Liste_NomsEtats(0);  // Les noms de ce routeur
  String S = GS + RMS_NomEtat[0] + "\r";

  server.send(200, "text/html", S);
}

void handleActions() {
  lectureCookie(ActionsHtml);
}
void handleActionsJS1() {
  CacheEtClose(300);
  server.send(200, "text/javascript", ActionsJS1);
}
void handleActionsJS2() {
  CacheEtClose(300);
  server.send(200, "text/javascript", ActionsJS2);
}
void handleActionsJS3() {
  CacheEtClose(300);
  server.send(200, "text/javascript", ActionsJS3);
}
void handleActionsJS4() {
  CacheEtClose(300);
  server.send(200, "text/javascript", ActionsJS4);
}
void handleActionsJS5() {
  CacheEtClose(300);
  server.send(200, "text/javascript", ActionsJS5);
}



void handlePinsActionsJS() {  // Pins disponibles
  String S = "var Pins=[0,-1];";
  if (ESP32_Type == 1)
    S = "var Pins=[0,4,5,13,14,16,17,21,22,23,25,26,27,-1];";
  if (ESP32_Type == 2 || ESP32_Type == 3)
    S = "var Pins=[0,4,5,13,14,16,17,18,19,21,22,25,26,27,32,33,-1];";
  if (ESP32_Type == 4 || ESP32_Type == 5)
    S = "var Pins=[0,5,18,19,22,23,27,-1];";  //Ecran 2.8
  if (ESP32_Type >= 6 && ESP32_Type <= 8)
    S = "var Pins=[0,5,18,19,21,22,23,-1];";  //Ecran 2.4
  if (ESP32_Type == 9 || ESP32_Type == 101)  
    S= "var Pins=[0,4,5,16,17,18,19,21,22,23,-1];"; //Ecran 2.8 capacitif
  if (ESP32_Type == 10)
    S = "var Pins=[0,5,12,14,17,32,33,-1];";

  server.send(200, "text/javascript", S);
}


void handlePara() {
  lectureCookie(ParaHtml);
  Serial.print("Clé accès reçue :" + CleAcces);
  Serial.println("  Attendue :" + CleAccesRef);
  previousTempMillis = millis() - 120000;
}
void handleParaNew() {
  if (!AccesAutorise()) return;
  DeserializeConfiguration(server.arg("plain"));
  server.send(200, "application/json", "{\"new_config\":\"ok\"}");
  int j = 1;
  for (int i = 1; i < LES_ROUTEURS_MAX; i++) {
    unsigned long ip = RMS_IP[i];
    RMS_IP[i] = 0;
    if (ip > 0 && ModeReseau < 2) {
      RMS_IP[j] = ip;
      j++;
    }
  }
  previousTempMillis = millis() - 60000;
  EcritureEnROM();
  if (Source != "Ext") {
    Source_data = Source;
  }

  LastHeureRTE = -1;
  //La désérialisation ne met pas à jour les membres Gpio/OutOn/OutOff (seul InitGpio() le fait),
  //donc l'ancienne comparaison ne détectait jamais un changement de GPIO. On réinitialise toujours.
  InitGPIOs();
  // Recherche des Noms (routeurs, températures,actions) des RMS partenaires
  Liste_des_Noms();
}

void handleCleUpdate() {
  lectureCookie("");

  server.send(200, "text/plain", "OKcle");
}
void handleParaJS1() {
  CacheEtClose(300);
  server.send(200, "text/javascript", ParaJS1);
}
void handleParaJS2() {
  CacheEtClose(300);
  server.send(200, "text/javascript", ParaJS2);
}
void handleParaCommunJS() {
  CacheEtClose(300);
  server.send(200, "text/javascript", ParaCommunJS);
}
void handleParaFixe() {  //Paramètres stockés en fichier
  File file = LittleFS.open("/parametres.json", "r");
  if (!file) {
    server.send(200, "application/json", "{}");
    return;
  }
  String conf = file.readString();
  file.close();
  //Les pages Mesures, Actions, Brute et Accueil ont besoin de ce document, mais elles ne
  //sont pas protégées par la clé : on n'expose les secrets qu'à un appelant authentifié.
  //Sans cela /ParaFixe livrait le mot de passe WiFi, les identifiants MQTT/Enphase et la
  //clé d'accès elle-même à n'importe qui sur le réseau local.
  if (!AccesUploadAutorise()) {
    const char *secrets[] = { "password", "MQTTPwd", "MQTTUser", "EnphasePwd", "EnphaseUser", "CleAccesRef" };
    for (unsigned int s = 0; s < sizeof(secrets) / sizeof(secrets[0]); s++) {
      String cle = "\"" + String(secrets[s]) + "\":";
      int p = conf.indexOf(cle);
      if (p < 0) continue;
      int deb = conf.indexOf('"', p + cle.length());  //Ouvrante de la valeur
      if (deb < 0) continue;
      int fin = conf.indexOf('"', deb + 1);           //Fermante (ces champs n'ont pas d'échappement)
      if (fin < 0) continue;
      conf = conf.substring(0, deb + 1) + conf.substring(fin);  //Valeur vidée
    }
  }
  server.send(200, "application/json", conf);
}
void handleajaxRAZhisto() {
  if (!AccesAutorise()) return;
  RAZ_Histo_Conso();
  for (int i = 0; i < 600; i++) {
    tabPw_Maison_5mn[i] = 0;  // Puissance Active:Soutiré-Injecté toutes les 5mn
    tabPw_Triac_5mn[i] = 0;
    for (int j = 0; j < 4; j++) {
      tabTemperature_5mn[j][i] = 0;
    }
    for (int j = 0; j < LES_ACTIONS_LENGTH; j++) {
      tab_histo_ouverture[j][i] = 0;
    }
  }
  for (int i = 0; i < 300; i++) {
    tabPw_Maison_2s[i] = 0;   // Puissance Active: toutes les 2s
    tabPw_Triac_2s[i] = 0;    // Puissance Triac: toutes les 2s
    tabPva_Maison_2s[i] = 0;  // Puissance Active: toutes les 2s
    tabPva_Triac_2s[i] = 0;
    for (int j = 0; j < LES_ACTIONS_LENGTH; j++) {
      tab_histo_2s_ouverture[j][i] = 0;
    }
  }
  RAZ_JSY = true;

  server.send(200, "text/html", "OK");
}
void handleParaVar() {
  String localIP = WiFi.localIP().toString();
  JsonDocument conf;
  conf["Source_data"] = Source_data;
  conf["localIP"] = localIP;    //Ip explicit x.x.x.x
  conf["IP_Fixe"] = RMS_IP[0];  //Derniere IP connu si DHCP
  conf["LTARF"] = LTARF;        //Tarif
  conf["LTARFbin"] = LTARFbin;
  conf["MeteoOn"] = MeteoOn;
  conf["PrevisionJour"] = serialized(String(Meteo_PrevisionJour, 1));    //Production solaire estimée kWh
  conf["PrevisionDemain"] = serialized(String(Meteo_PrevisionDemain, 1));
  conf["SmaOn"] = SmaOn;
  conf["PuissancePV"] = serialized(String(PuissancePV, 0));              //Production onduleur en W
  conf["EnergieJourPV"] = EnergieJourPV;                                 //Production du jour en Wh
  conf["BallonCanal"] = BallonCanal;
  conf["BallonBesoin"] = serialized(String(Ballon_Besoin, 1));           //kWh pour remonter à la cible
  conf["BallonSurplus"] = serialized(String(Ballon_SurplusPrevu, 1));    //kWh de surplus attendu
  conf["BallonCoefAuto"] = BallonCoefAuto;                               //Coefficient routable appris en %
  conf["BallonModeIntel"] = BallonModeIntel;                            //Mode prédictif actif
  conf["BallonReserve"] = serialized(String(Ballon_Reserve, 1));         //kWh d'eau chaude utile disponible
  conf["BallonUsageMoyen"] = serialized(String(Ballon_UsageMoyen, 1));   //kWh/jour consommés en moyenne
  MajEconomieJour();
  conf["PrixHP"] = serialized(String(PrixHP, 3));                        //€/kWh Heure Pleine (ou tarif unique)
  conf["PrixHC"] = serialized(String(PrixHC, 3));                        //€/kWh Heure Creuse
  conf["PrixActuel"] = serialized(String(PrixKwhActuel(), 3));           //€/kWh du tarif en cours
  conf["EconomieJour"] = serialized(String(EconomieJour, 2));            //€ économisés aujourd'hui
  conf["EconomieMois"] = serialized(String(EconomieMois, 2));            //€ économisés ce mois
  conf["EconomieTotal"] = serialized(String(EconomieTotal, 2));          //€ économisés au total
  conf["ModeAbsenceActif"] = ModeAbsenceActif ? 1 : 0;                   //Absence active en ce moment
  conf["AntiLegioEnCours"] = AntiLegioEnCours ? 1 : 0;                   //Chauffe anti-légionelle en cours
  conf["DelestageActif"] = DelestageActif ? 1 : 0;                       //Délestage en cours
  conf["DelestagePlafond"] = int(DelestagePlafond + 0.5);                //Ouverture max autorisée (%)
  for (int c = 0; c < 4; c++) {
    conf["temperature"][c] = temperature[c];
  }
  int j = 0;
  for (int i = 0; i < LES_ROUTEURS_MAX; i++) {
    if (RMS_IP[i] > 0 && RMS_NomEtat[i] != "") {
      conf["IP_RMS"][j] = IP2String(RMS_IP[i]);
      conf["RMS_NomEtat"][j] = urlEncode(RMS_NomEtat[i]);  //Les caractères US etc ne passent pas dans le JSON
      j++;
    }
  }
  String Json;
  serializeJson(conf, Json);
  server.send(200, "application/json", Json);
}
void handleAjaxJournal() {  //Journal d'événements : "HH:MM;message", le plus récent en premier (30 lignes max)
  String S = "";
  if (LittleFS.exists("/journal.txt")) {
    File f = LittleFS.open("/journal.txt", "r");
    if (f) {
      S = f.readString();
      f.close();
    }
  }
  server.send(200, "text/plain", S);
}
void handleAjaxHistMeteo() {  //Historique quotidien : date;prevision_kWh;production_kWh;routee_kWh;coef
  String S = "";
  if (LittleFS.exists("/histmeteo.csv")) {
    File f = LittleFS.open("/histmeteo.csv", "r");
    if (f) {
      S = f.readString();
      f.close();
    }
  }
  server.send(200, "text/plain", S);
}
void handleSetGpio() {
  if (!AccesAutorise()) return;
  int gpio = server.arg("gpio").toInt();
  int out = server.arg("out").toInt();
  String S = "Refut : gpio =" + String(gpio) + " out =" + String(out);
  // GPIO 6 à 11 = flash SPI interne : ne jamais piloter sous peine de planter l'ESP32.
  // GPIO 34 à 39 = entrées seules (impossible en sortie). On reste donc dans [0,33] hors flash.
  bool gpioOk = (gpio >= 0 && gpio <= 33 && (gpio < 6 || gpio > 11));
  if (gpioOk && out >= 0 && out <= 1) {
    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, out);
    S = "OK : gpio =" + String(gpio) + " out =" + String(out);
  }

  server.send(200, "text/html", S);
}
void handleExport() {
  lectureCookie(ExportHtml);
}
void handleExport_file() {
  //parametres.json contient les mots de passe WiFi/MQTT/Enphase et la clé d'accès
  //elle-même : cet endpoint doit impérativement exiger la clé.
  if (!AccesAutorise()) return;
  String Fichier = server.arg("Fichier");
  if (!NomFichierSur(Fichier)) {
    server.send(400, "text/plain", "Nom de fichier invalide");
    return;
  }
  if (server.arg("Delete") == "OK") {
    LittleFS.remove("/" + Fichier);
    server.send(200, "application/json", "");
    return;
  }
  File file = LittleFS.open("/" + Fichier, "r");
  if (!file) {
    server.send(404, "text/plain", "Fichier introuvable");
    return;
  }
  //Envoi en flux : évite de reconstruire tout le fichier en RAM caractère par caractère
  server.sendHeader("Content-Disposition", "attachment; filename=\"" + server.arg("download") + "\"");
  server.streamFile(file, "application/octet-stream");
  file.close();
}
void handleListeFile() {
  if (!AccesAutorise()) return;
  String S = "";
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    S += file.name() + RS + String(file.size()) + GS;
    file = root.openNextFile();
  }
  S += FS + String(LittleFS.totalBytes());  // Espace total
  S += FS + String(LittleFS.usedBytes());   // Espace utilisé

  server.send(200, "text/javascript", S);
}
void handleAP_ScanWifi() {
  server.send(200, "text/html", Liste_AP);
}
bool Liste_WIFI() {  // Doit être fait avant toute connection WIFI depuis biblio ESP32 3.0.1
  int bestNetworkDb = -1000;
  bool bestFound = false;
  WIFIbug = 0;
  esp_task_wdt_reset();
  delay(1);
  int n = 0;
  WiFi.disconnect();
  delay(100);
  TelnetPrintln("Scan start");
  // WiFi.scanNetworks will return the number of networks found.
  n = WiFi.scanNetworks();
  TelnetPrintln("Scan done");
  Liste_AP = "";
  if (n == 0) {
    TelnetPrintln("Pas de réseau Wifi trouvé");
  } else {
    TelnetPrint(String(n));
    TelnetPrintln(" réseaux trouvés");
    TelnetPrintln("Nr | SSID         | RSSI | MAC | Channel");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      TelnetPrint(String(i + 1));
      TelnetPrint(" | ");
      TelnetPrint(String(WiFi.SSID(i)));
      TelnetPrint(" | ");
      TelnetPrint(String(WiFi.RSSI(i)));
      TelnetPrint(" | ");
      TelnetPrint(WiFi.BSSIDstr(i));
      TelnetPrint(" | ");
      TelnetPrintln(String(WiFi.channel(i)));
      Liste_AP += WiFi.SSID(i).c_str() + RS + String(WiFi.RSSI(i)) + RS + WiFi.BSSIDstr(i) + RS + String(WiFi.channel(i)) + GS;
      if (WiFi.SSID(i) == ssid) {
        if (WiFi.RSSI(i) > bestNetworkDb) {
          bestNetworkDb = WiFi.RSSI(i);
          memcpy(bestBSSID, WiFi.BSSID(i), 6);
          bestFound = true;
        }
      }
    }
  }
  WiFi.scanDelete();
  return bestFound;
}

void handleAP_SetWifi() {
  if (!AccesAutorise()) return;
  esp_task_wdt_reset();
  delay(1);
  TelnetPrintln("Set Wifi");
  String NewSsid = server.arg("ssid");
  NewSsid.trim();
  String NewPassword = server.arg("passe");
  NewPassword.trim();
  TelnetPrintln(NewSsid);
  TelnetPrintln(NewPassword);
  ssid = NewSsid;
  password = NewPassword;
  ModeReseau = 0;
  EcritureEnROM();
  StockMessage("Wifi Begin : " + ssid);
  WiFi.begin(ssid.c_str(), password.c_str());
  unsigned long newstartMillis = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - newstartMillis < 20000)) {  // Attente connexion au Wifi
    Serial.write('!');
    Gestion_LEDs();
    TelnetPrint(String(WiFi.status()));
    delay(300);
  }
  TelnetPrintln("");
  String S = "";
  if (WiFi.status() == WL_CONNECTED) {
    TelnetPrint("IP address: ");
    String IP = WiFi.localIP().toString();
    TelnetPrintln(IP);
    S = "Ok" + RS;
    S += "ESP 32 connecté avec succès au wifi : " + ssid + " avec l'adresse IP : " + IP;
    S += "<br><br> Connectez vous au wifi : " + ssid;
    S += "<br><br> Cliquez sur l'adresse : <a href='http://" + IP + "' >http://" + IP + "</a>";
    dhcpOn = 1;
    ModeReseau = 0;  // A priori
    EcritureEnROM();
  } else {
    S = "No" + RS + "ESP32 non connecté à :" + ssid + "<br>";
  }

  server.send(200, "text/html", S);
  delay(1000);
  ReseT("Reset suite au changement de WiFi");
}

void handleHeure() {
  lectureCookie(HeureHtml);
}
void handleHourUpdate() {
  if (!AccesAutorise()) return;
  String New_H = server.arg("New_H");
  String New_J = server.arg("New_J");
  Horloge = server.arg("Horloge").toInt();
  if (Horloge == 0) {
    idxFuseau = server.arg("idxFuseau").toInt();
    ntpServer = server.arg("ntpServer");
  }
  MiseAheure(New_H, New_J);
  EcritureEnROM();
  server.send(200, "text/plain", "OKheure");
}

void handleCouleurs() {
  lectureCookie(CouleursHtml);
}
void handleCommunCouleurJS() {  // Code Javascript
  CacheEtClose(300);
  server.send(200, "text/javascript", CommunCouleurJS);  // Javascript code
}
void handleCouleursAjax() {

  server.send(200, "text/javascript", Couleurs);  // tableau des couleurs
}
void handleCouleurUpdate() {
  if (!AccesAutorise()) return;
  Couleurs = server.arg("couleurs");
  if (Couleurs.length()==0) Couleurs=String(CouleurDefaut);
  EcritureEnROM();

  server.send(200, "text/plain", "OK couleurs");
}
void handleTestNotif() {
  if (!AccesAutorise()) return;
  NotifTest();
  server.send(200, "text/plain", NotifOn == 1 ? "OK" : "Notifications desactivees");
}
void handleThemeJS() {
  CacheEtClose(300);
  //Chargé en <head> (bloquant) pour que le thème soit posé avant le premier rendu :
  //sinon les utilisateurs en mode clair voient un flash sombre à chaque navigation.
  server.send(200, "text/javascript", CommunThemeJS);
}
void handleCommunCSS() {
  CacheEtClose(60);
  //L'ancien dégradé bleu et son body{font-size:150%;text-align:center;max-width:1000px;
  //padding:10px} s'appliquaient encore à toutes les pages Soleo, dont les <style> inline
  //ne redéfinissaient pas ces propriétés : typographie surdimensionnée et entête bridée.
  //Le fond et le texte de page relèvent désormais du thème Soleo (clair/sombre) ;
  //la page Couleurs continue de piloter les couleurs des mesures (.W/.VA/.Wh...).
  server.send(200, "text/css", CommunCSS);
}

void handleFavicon() {
  server.send(200, "image/svg+xml", Favicon);
}
void handleFavicon192() {
  server.send(200, "image/svg+xml", Favicon192);
}
void handleManifest() {
  server.send(200, "application/json", Manifest);
}
void handleNotFound() {  // Page Web pas trouvé
  String message = "Fichier non trouvé\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}
void CacheEtClose(int16_t seconde) {
  server.sendHeader("Cache-Control", "max-age=" + String(seconde));
}
// Les pages sont des const char* en flash. Recevoir un String en construisait une copie
// intégrale en tas à chaque requête : 34 Ko pour la page Réglages, la plus lourde, avant
// même que le serveur ne bâtisse sa réponse. Sur un tas fragmenté — d'autant plus depuis
// qu'une connexion TLS permanente peut être ouverte pour MQTT — l'allocation échouait et
// la page arrivait vide. On envoie désormais directement depuis la flash.
void lectureCookie(const char *S) {
  ExtraitCookie();
  if (S != NULL && S[0] != '\0') {

    if (CleAccesRef == CleAcces) {
      server.send(200, "text/html", S);
    } else {
      server.send(200, "text/html", ParaCleHtml);  // Demande clé d'acces / mot de passe
    }
  }
}
void ExtraitCookie() {
  CleAcces = "";
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
    cookie.trim();
    int p = cookie.indexOf("CleAcces=");
    if (p >= 0) {
      CleAcces = cookie.substring(p + 9);
      CleAcces = urlDecode(CleAcces);
    }
    CleAcces.trim();
  }
}

// Contrôle d'accès des endpoints qui MODIFIENT l'état (configuration, GPIO, fichiers,
// redémarrage, mise à jour du firmware).
// lectureCookie() ne protégeait que l'affichage des pages HTML : les API sous-jacentes
// restaient appelables directement. AccesAutorise() ferme ce contournement.
// Si aucune clé n'est configurée, l'accès reste libre (comportement historique, pas de
// régression pour les installations existantes).
bool AccesAutorise() {
  if (CleAccesRef.length() == 0) return true;  //Pas de clé définie : accès libre
  ExtraitCookie();
  if (CleAccesRef == CleAcces) return true;
  server.send(401, "text/plain", "Acces refuse : cle d'acces requise");
  return false;
}

// Variante silencieuse pour les téléversements : les callbacks d'upload sont appelés
// pendant la réception du corps de la requête, on ne peut pas y émettre de réponse HTTP.
// Le refus est renvoyé par le handler final via UploadAutorise.
bool AccesUploadAutorise() {
  if (CleAccesRef.length() == 0) return true;
  ExtraitCookie();
  return (CleAccesRef == CleAcces);
}

// Un nom de fichier LittleFS doit rester à la racine : ni séparateur, ni remontée de
// répertoire, ni nom vide. Empêche /export_file et /import de sortir du dossier.
bool NomFichierSur(const String &nom) {
  if (nom.length() == 0 || nom.length() > 64) return false;
  if (nom.indexOf('/') >= 0 || nom.indexOf('\\') >= 0) return false;
  if (nom.indexOf("..") >= 0) return false;
  return true;
}

// class pour découper au format chunked attendu par navigateur
class ChunkedWriter : public Print {
  public:
    ChunkedWriter(WiFiClient& client) : _client(client), _pos(0) {}

    size_t write(uint8_t c) override {  // receptionne les octets un par un
      _buffer[_pos++] = c;
      if (_pos == 512) flushChunk(); // Envoie un chunk dès que le buffer est plein
      return 1;
    }

    size_t write(const uint8_t* buffer, size_t size) override {
      for (size_t i = 0; i < size; i++) write(buffer[i]);
      return size;
    }

    void finalise() {
      flushChunk(); // Envoie le reste du buffer
      _client.print("0\r\n\r\n"); // Marqueur de fin HTTP
    }

  private:
    void flushChunk() {
      if (_pos == 0) return;
      _client.print(_pos, HEX);
      _client.print("\r\n");
      _client.write(_buffer, _pos);
      _client.print("\r\n");
      _pos = 0;
    }

    WiFiClient& _client;
    uint8_t _buffer[512]; // Buffer de 512 octets (bon compromis RAM/Vitesse)
    size_t _pos;
};

void envoyerHistoriqueEnergie(WebServer &serverRef) {
  JsonDocument doc;

  // //Vue par jour/mois Soutiré et Injecté (LittleFS)
  int M0 = DateAMJ.substring(4, 6).toInt();
  int an0 = DateAMJ.substring(0, 4).toInt();
  String ligne;
  ligne.reserve(64);

  for (int M = -2; M <= 0; M++) {
    int M1 = M0 + M;
    int an1 = an0;
    if (M1 < 1) { M1 += 12; an1--; }
    
    char fileName[32];
    snprintf(fileName, sizeof(fileName), "/Mois_Wh_%04d%02d.csv", an1, M1);

    if (LittleFS.exists(fileName)) {
      File file = LittleFS.open(fileName, "r");
      while (file.available()) {
        ligne = file.readStringUntil('\n');
        ligne.trim();
        if (ligne.length() > 10 && ligne.indexOf("Date,") == -1) {
          doc["EnergieJour"].add(ligne);
        }
      }
      file.close();
    }
  }

  if (doc["EnergieJour"].isNull()) doc["EnergieJour"] = "";

  serverRef.setContentLength(CONTENT_LENGTH_UNKNOWN);
  serverRef.sendHeader("Transfer-Encoding", "chunked");
  serverRef.send(200, "application/json", ""); 

  NetworkClient client = serverRef.client(); 
  ChunkedWriter writer(client);
  
  // Sérialisation directe
  serializeJson(doc, writer);

  //envoie le marqueur "0"
  writer.finalise();
}

