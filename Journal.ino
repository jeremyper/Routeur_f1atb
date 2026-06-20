// ***********************************************************************
// * Fondations UI Soleo                                                 *
// * - Tarif électricité courant (HP/HC) et économies du jour            *
// * - Journal d'événements en langage naturel (/journal.txt)            *
// *   30 lignes max, la plus récente en premier, format "HH:MM;message" *
// ***********************************************************************

//Prix du kWh en cours, selon la couleur Tempo du jour (si activé) et l'heure pleine/creuse
float PrixKwhActuel() {
  bool estHC = (LTARF.indexOf("CREUSE") >= 0 || LTARF.indexOf("HC") >= 0);
  if (TempoRTEon == 1) {  //Tarif Tempo : 6 prix selon couleur
    if (LTARF.indexOf("ROUGE") >= 0) return estHC ? PrixRougeHC : PrixRougeHP;
    if (LTARF.indexOf("BLANC") >= 0) return estHC ? PrixBlancHC : PrixBlancHP;
    if (LTARF.indexOf("BLEU") >= 0) return estHC ? PrixBleuHC : PrixBleuHP;
  }
  return estHC ? PrixHC : PrixHP;  //Tarif Base / HP-HC classique
}

//Economie estimée du jour en € : toute la production PV est considérée autoconsommée (routeur)
//Utilise le tarif courant (couleur Tempo prise en compte) pour rester cohérent
void MajEconomieJour() {
  if (SmaOn == 1 && EnergieJourPV > 0) EconomieJour = float(EnergieJourPV) / 1000.0 * PrixKwhActuel();
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
