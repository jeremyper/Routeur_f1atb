// ***********************************************************************
// * Fondations UI Soleo                                                 *
// * - Tarif électricité courant (HP/HC) et économies du jour            *
// * - Journal d'événements en langage naturel (/journal.txt)            *
// *   30 lignes max, la plus récente en premier, format "HH:MM;message" *
// ***********************************************************************

//Prix du kWh en cours : Heure Creuse si le Linky / RTE l'indique, sinon Heure Pleine (ou tarif unique)
float PrixKwhActuel() {
  if (LTARF.indexOf("CREUSE") >= 0 || LTARF.indexOf("HC") >= 0) return PrixHC;
  return PrixHP;
}

//Economie estimée du jour en € : toute la production PV est considérée autoconsommée (routeur)
//Référence tarif HP, cohérent avec le bilan accumulé à minuit dans Heure.ino
void MajEconomieJour() {
  if (SmaOn == 1 && EnergieJourPV > 0) EconomieJour = float(EnergieJourPV) / 1000.0 * PrixHP;
}

//Ajoute un événement en tête du journal et tronque à 30 lignes
void JournalAjoute(const String &msg) {
  if (!HeureValide) return;  //Pas d'horodatage possible
  String hh = (Int_Heure < 10) ? "0" + String(Int_Heure) : String(Int_Heure);
  String mn = (Int_Minute < 10) ? "0" + String(Int_Minute) : String(Int_Minute);
  String contenu = hh + ":" + mn + ";" + msg + "\n";
  if (LittleFS.exists("/journal.txt")) {
    File f = LittleFS.open("/journal.txt", "r");
    if (f) {
      contenu += f.readString();  //La plus récente en premier
      f.close();
    }
  }
  int nb = 0;
  for (unsigned int i = 0; i < contenu.length(); i++) {
    if (contenu[i] == '\n') {
      nb++;
      if (nb == 30) {  //On ne garde que les 30 événements les plus récents
        contenu = contenu.substring(0, i + 1);
        break;
      }
    }
  }
  File f = LittleFS.open("/journal.txt", "w");
  if (f) {
    f.print(contenu);
    f.close();
  }
}
