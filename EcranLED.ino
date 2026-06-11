//****************************************
//* Gestion des LEDs d'état (ESP32 seul) *
//****************************************
void Init_LED_OLED(void) {
  //LEDgroupe = 0;   //0:pas de LED, 1 à 9 pour les LEDs
  if (LEDgroupe > 0 && LEDgroupe < 10) {  //Simples LEDs
    pinMode(LEDyellow[LEDgroupe], OUTPUT);
    pinMode(LEDgreen[LEDgroupe], OUTPUT);
  }
}

void Gestion_LEDs() {

  int retard_min = 100;
  int retardI;
  cptLEDyellow++;
  if (WiFi.status() != WL_CONNECTED) {  // Attente connexion au Wifi
    if (WiFi.getMode() == WIFI_STA) {   // en  Station mode
      cptLEDyellow = (cptLEDyellow + 6) % 10;
      cptLEDgreen = cptLEDyellow;
    } else {  //AP Mode
      cptLEDyellow = cptLEDyellow % 10;
      cptLEDgreen = (cptLEDyellow + 5) % 10;
    }
  } else {
    for (int i = 0; i < NbActions; i++) {
      retardI = Retard[i];
      retard_min = min(retard_min, retardI);
    }
    if (retard_min < 100) {
      cptLEDgreen = int((cptLEDgreen + 1 + 8 / (1 + retard_min / 10))) % 10;
    } else {
      cptLEDgreen = 10;
    }
  }


  if (LEDgroupe > 0 && LEDgroupe < 10) {
    int L = 0, H = 1;  //LED classique
    if (LEDgroupe == 2 || LEDgroupe == 4) { //Inversé pour LED sur carte graphique GPIOs 4,16,17
      L = 1;
      H = 0;
    }
    if (cptLEDyellow > 5) {
      digitalWrite(LEDyellow[LEDgroupe], L);
    } else {
      digitalWrite(LEDyellow[LEDgroupe], H);
    }
    if (cptLEDgreen > 5) {
      digitalWrite(LEDgreen[LEDgroupe], L);
    } else {
      digitalWrite(LEDgreen[LEDgroupe], H);
    }
  }
}

void PrintScroll(String m) {
  TelnetPrintln(m);
}
