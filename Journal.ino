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

// Économie du jour : accumulée en continu depuis l'énergie réellement détournée.
//
// L'ancien calcul créditait toute la production de l'onduleur SMA, ce qui exigeait un
// onduleur SMA — donc rien pour la plupart des installations — et surestimait le gain en
// comptant aussi ce qui repartait vers le réseau.
//
// On ne retient que la part de la puissance envoyée au ballon que la maison n'achète pas :
// à chaque mesure, puissance du ballon moins puissance soutirée au réseau. Jusqu'en v1.29,
// le calcul était en tout ou rien (rien dès que la maison soutirait plus de 20 W) : comme la
// régulation vise 0 W, la maison soutire un peu une mesure sur deux, et la moitié du soleil
// envoyé au ballon n'était pas comptée (1,7 kWh annoncés pour ~4 kWh le 30/10/2026). Ce qui
// est chauffé en marche forcée est acheté au réseau et reste exclu de fait. Le tarif est
// appliqué à l'instant où l'énergie est détournée, ce qui respecte les changements heure
// pleine / heure creuse et les couleurs Tempo au fil de la journée.
//
// La réinjection est cumulée à côté : soleil + réinjection = surplus réellement disponible,
// qui sert à apprendre le coefficient routable du ballon (voir ApprentissageBallon()).
//
// Appelée depuis la boucle 2 s ; dt est la durée réelle écoulée en millisecondes.
void SuiviEconomieRoutee(unsigned long dt) {
  float h = float(dt) / 3600000.0;
  EnergieInjecteeJour += float(PuissanceI_M) * h;
  float pSolaire = PuissanceBallon() - float(PuissanceS_M);
  if (pSolaire <= 0) return;
  float wh = pSolaire * h;
  EnergieRouteeJour += wh;
  EconomieJour += wh / 1000.0 * PrixKwhActuel();
}

// Puissance reçue par le ballon (W).
// La voie T alimente une charge : elle ne produit jamais. Un tore posé à l'envers range la
// puissance en "injectée" ; on additionne les deux sens pour rester juste dans les deux cas.
// Sans voie T (source à une seule mesure), on l'estime par l'ouverture de la sortie x la
// puissance de la résistance. Une sortie grande ouverte alors que la maison réinjecte veut
// dire, le plus souvent, que le thermostat du ballon a coupé : on ne compte alors rien. Dans
// le doute, le surplus appris est sous-estimé, ce qui penche vers le confort (plus de forçage).
float PuissanceBallon() {
  float pT = float(PuissanceS_T + PuissanceI_T);
  if (pT > 100) VoieTMesureBallon = true;
  if (VoieTMesureBallon) return pT;
  int i = IndexActionChauffeEau();
  if (i < 0 || i >= NbActions || Actif[i] == MODE_INACTIF) return 0;
  float ouverture = (100.0 - RetardF[i]) / 100.0;
  if (ouverture >= 0.95 && PuissanceI_M > 150) return 0;
  return ouverture * float(BallonPuissance);
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

//Le minimum de tas est monotone : comparer sa valeur avant et apres une
//operation dit si c'est bien elle qui a creuse le plancher. La sonde ne parle
//que dans ce cas — le journal ne retient que 10 messages, des jalons bavards
//chassent justement ceux du demarrage qu'on cherche a lire.
void SuiviPlancher(const String &etape, uint32_t minAvant) {
  uint32_t minApres = esp_get_minimum_free_heap_size();
  if (minApres < minAvant)
    StockMessage(etape + " : plancher du tas abaisse a " + String(minApres)
                 + " o (etait " + String(minAvant) + " o)");
}
