// ***********************************************************************
// * Notifications sortantes — ntfy.sh ou webhook générique              *
// *                                                                     *
// * Les événements du journal partent vers un service de notification   *
// * pour que le routeur puisse alerter sans qu'on ouvre sa page :        *
// * chauffe anti-légionelle, délestage, marche forcée, bilan du jour.    *
// *                                                                     *
// * JournalAjoute() est appelé depuis la boucle de régulation et depuis  *
// * les handlers web : un envoi HTTP y serait bloquant. Les messages     *
// * sont donc empilés dans une file, drainée par Task_Reseau sur le      *
// * cœur 0, là où les appels réseau lents sont déjà isolés.              *
// ***********************************************************************

#define NOTIF_FILE_TAILLE 6  //Au-delà, les plus anciens sont abandonnés

String NotifFile[NOTIF_FILE_TAILLE];
volatile int NotifTete = 0;   //prochaine écriture
volatile int NotifQueue = 0;  //prochaine lecture
unsigned long LastNotifMillis = 0;

// Empile un message. Appelable depuis les deux cœurs : protégé par MutexJournal,
// déjà pris pour l'écriture du journal, donc aucun verrou supplémentaire ici.
void NotifEmpile(const String &msg) {
  if (NotifOn != 1 || NotifUrl.length() < 8) return;
  int suivant = (NotifTete + 1) % NOTIF_FILE_TAILLE;
  if (suivant == NotifQueue) return;  //File pleine : on abandonne plutôt que d'écraser
  NotifFile[NotifTete] = msg;
  NotifTete = suivant;
}

// Découpe "https://ntfy.sh/mon-topic" en hôte, chemin et port.
static bool NotifDecoupeUrl(const String &url, String &hote, String &chemin, uint16_t &port, bool &tls) {
  String u = url;
  tls = u.startsWith("https://");
  if (tls) u = u.substring(8);
  else if (u.startsWith("http://")) u = u.substring(7);
  else return false;
  int slash = u.indexOf('/');
  String hp = (slash < 0) ? u : u.substring(0, slash);
  chemin = (slash < 0) ? "/" : u.substring(slash);
  int deuxPoints = hp.indexOf(':');
  if (deuxPoints >= 0) {
    hote = hp.substring(0, deuxPoints);
    port = hp.substring(deuxPoints + 1).toInt();
  } else {
    hote = hp;
    port = tls ? 443 : 80;
  }
  return hote.length() > 0;
}

// Envoie un message. Appelée uniquement depuis Task_Reseau (cœur 0).
static void NotifEnvoie(const String &msg) {
  String hote, chemin;
  uint16_t port;
  bool tls;
  if (!NotifDecoupeUrl(NotifUrl, hote, chemin, port, tls)) {
    StockMessage("Notification : URL invalide");
    return;
  }

  String corps = msg;
  String entetes = "POST " + chemin + " HTTP/1.1\r\n";
  entetes += "Host: " + hote + "\r\n";
  entetes += "User-Agent: Soleo\r\n";
  entetes += "Title: " + nomRouteur + "\r\n";  //ntfy affiche ce titre ; ignoré ailleurs
  entetes += "Content-Type: text/plain; charset=utf-8\r\n";
  entetes += "Content-Length: " + String(corps.length()) + "\r\n";
  entetes += "Connection: close\r\n\r\n";

  if (tls) {
    WiFiClientSecure c;
    c.setInsecure();
    c.setTimeout(5000);
    if (!c.connect(hote.c_str(), port, 5000)) {
      StockMessage("Notification : connexion à " + hote + " échouée");
      return;
    }
    c.print(entetes + corps);
    unsigned long t0 = millis();
    while (c.connected() && millis() - t0 < 4000) {
      if (c.available()) { c.readStringUntil('\n'); t0 = millis(); }
      delay(1);
    }
    c.stop();
  } else {
    WiFiClient c;
    c.setTimeout(5000);
    if (!c.connect(hote.c_str(), port, 5000)) {
      StockMessage("Notification : connexion à " + hote + " échouée");
      return;
    }
    c.print(entetes + corps);
    unsigned long t0 = millis();
    while (c.connected() && millis() - t0 < 4000) {
      if (c.available()) { c.readStringUntil('\n'); t0 = millis(); }
      delay(1);
    }
    c.stop();
  }
}

// Vide la file. Appelée depuis Task_Reseau : un seul message par passage, pour
// qu'une rafale d'événements n'immobilise pas la tâche réseau.
void NotifTraiteFile() {
  if (NotifOn != 1 || ModeReseau != 0) return;
  if (NotifTete == NotifQueue) return;  //Rien à envoyer
  if (millis() - LastNotifMillis < 3000UL) return;  //Espacement minimal entre deux envois
  LastNotifMillis = millis();
  String msg = NotifFile[NotifQueue];
  NotifQueue = (NotifQueue + 1) % NOTIF_FILE_TAILLE;
  NotifEnvoie(msg);
}

// Envoi de test déclenché depuis la page Réglages.
void NotifTest() {
  NotifEmpile("Test de notification depuis " + nomRouteur + " — si vous lisez ceci, tout fonctionne.");
}
