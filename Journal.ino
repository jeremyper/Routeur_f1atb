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
// Économie du jour : accumulée en continu depuis l'énergie réellement détournée.
//
// L'ancien calcul créditait toute la production de l'onduleur SMA, ce qui exigeait un
// onduleur SMA — donc rien pour la plupart des installations — et surestimait le gain en
// comptant aussi ce qui repartait vers le réseau.
//
// On ne retient désormais que la puissance envoyée aux charges pendant que la maison
// n'achète rien : c'est de l'énergie obtenue sans la payer. Ce qui est chauffé en marche
// forcée est acheté au réseau, donc explicitement exclu. Le tarif est appliqué à l'instant
// où l'énergie est détournée, ce qui respecte les changements heure pleine / heure creuse
// et les couleurs Tempo au fil de la journée.
//
// Appelée depuis la boucle 2 s ; dt est la durée réelle écoulée en millisecondes.
void SuiviEconomieRoutee(unsigned long dt) {
  if (PuissanceS_M > 20) return;  //La maison soutire : ce n'est pas du surplus
  int pRoutee = PuissanceS_T - PuissanceI_T;
  if (pRoutee <= 0) return;
  float wh = float(pRoutee) * float(dt) / 3600000.0;
  EnergieRouteeJour += wh;
  EconomieJour += wh / 1000.0 * PrixKwhActuel();
}

//Conservée pour les appelants existants : le cumul se fait désormais en continu.
void MajEconomieJour() {
}

//Ajoute un événement en tête du journal et tronque à 30 lignes.
//Appelé depuis les deux cœurs (boucle 2s et handlers web sur le cœur 1, Task_Reseau sur
//le cœur 0). L'ajout étant un cycle lecture/troncature/réécriture du fichier entier, il
//doit être sérialisé : sans verrou, deux ajouts simultanés tronquaient le journal.
void JournalAjoute(const String &msg) {
  if (!HeureValide) return;  //Pas d'horodatage possible
  if (MutexJournal == NULL) return;
  //Attente bornée : un ajout au journal ne doit jamais retarder la régulation
  if (xSemaphoreTake(MutexJournal, pdMS_TO_TICKS(200)) != pdTRUE) return;

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
  //Empilé sous le même verrou ; l'envoi HTTP se fera depuis le cœur 0
  NotifEmpile(msg);
  xSemaphoreGive(MutexJournal);
}
