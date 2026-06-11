# Routeur Solaire — fork allégé du Routeur RMS F1ATB

Fork personnalisé du [routeur photovoltaïque F1ATB](https://f1atb.fr) (base V17.16),
recentré sur **un seul cas d'usage matériel** et enrichi d'une **gestion du forçage
pilotée par la prévision météo**.

## Matériel cible

- ESP32 DevKit (Wroom), sans écran
- Mesure de puissance : **UxI** (transformateur + pince ampèremétrique) ou module JSY (UxIx2 / UxIx3)
- Sorties : **SSR zéro-crossing** (routage proportionnel) + relais ou modules HTTP externes (On/Off)
- LEDs d'état optionnelles
- Sondes de température DS18B20 optionnelles (thermostat chauffe-eau)

## Différences avec le firmware F1ATB d'origine

### Supprimé
- Sources de mesure : Shelly EM/Pro, Enphase Envoy, Linky TIC, HomeWizard, SmartGateways, MQTT, ESP externe
- Écran LCD tactile (Sunton/CYD), OLED, carte Ethernet WT32-ETH01
- **Découpe de phase triac** et son interruption 100 µs (gain CPU permanent), modes PWM et demi-sinus
- Le « slot triac » réservé : l'action 0 est une action GPIO normale

### Conservé
- Cœur de régulation PID éprouvé (intégrateur dominant + P/D optionnels)
- Modes de sortie propres en harmoniques, compatibles SSR zéro-crossing :
  **Multi-sinus** (recommandé), **Train de sinus**, **On/Off**
- Jusqu'à 10 actions × 8 périodes avec conditions : heure, température,
  tarif (HP/HC, Tempo RTE), état d'une autre action
- MQTT + auto-découverte Home Assistant
- Historiques, OTA, Telnet, mode AP/WPS

### Ajouté
- **Prévision météo solaire (Open-Meteo, gratuit, sans clé API)** :
  - Paramètres : latitude, longitude, puissance crête (kWc)
  - Production estimée aujourd'hui / demain (kWh), rafraîchie toutes les 2 h
  - **Nouvelle condition de période** : « actif si prévision demain/aujourd'hui < ou ≥ seuil »
    — ex. *forcer le chauffe-eau la nuit en heures creuses uniquement si demain sera couvert*
  - Prévisions affichées sur l'accueil, la page Actions et publiées en MQTT
- Interface modernisée (CSS sombre, responsive mobile, popup de période centrée)
- Suppression des `delay()` bloquants sur les chemins d'erreur réseau

## Compilation

- Arduino IDE 2.x ou arduino-cli, core **esp32 by Espressif 3.3.x**
- Carte : `ESP32 Dev Module`, Partition Scheme : **custom** (`partitions.csv` fourni,
  2 × 1900 Ko app + 140 Ko SPIFFS) ou `Minimal SPIFFS`
- Bibliothèques : `ArduinoJson`, `PubSubClient`, `DallasTemperature`
  (+ `OneWire` fourni dans le projet)

Empreinte actuelle : ~1,59 Mo (80 %) de flash, ~82 Ko de RAM statique.

## Exemple : forçage intelligent du chauffe-eau

1. **Paramètres** → activer la prévision météo, renseigner latitude/longitude/kWc.
2. **Actions** → action « Chauffe-eau (SSR) », mode **Multi-sinus**.
3. Période 9h–17h : type **Routage** (seuil 0 W) — route le surplus solaire.
4. Période 2h–6h (heures creuses) : type **ON**, condition tarif HC,
   condition météo « **Prévision demain < 8 kWh** » :
   le forçage nocturne ne se déclenche que si la journée suivante ne
   permettra pas de chauffer l'eau avec le surplus solaire.
