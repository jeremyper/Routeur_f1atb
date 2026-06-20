# Routeur Solaire F1ATB — Fonctionnalités complètes

**Version firmware : 17.16** · Plateforme : ESP32 · Interface : Soleo (web responsive)

---

## Table des matières

1. [Vue d'ensemble](#1-vue-densemble)
2. [Sources de mesure de puissance](#2-sources-de-mesure-de-puissance)
3. [Sorties de routage (Actions)](#3-sorties-de-routage-actions)
4. [Conditions et déclenchement des actions](#4-conditions-et-déclenchement-des-actions)
5. [Capteurs de température](#5-capteurs-de-température)
6. [Onduleur SMA Sunny Boy (Modbus TCP)](#6-onduleur-sma-sunny-boy-modbus-tcp)
7. [Prévision météo solaire](#7-prévision-météo-solaire)
8. [Ballon eau chaude intelligent](#8-ballon-eau-chaude-intelligent)
9. [Mode Absence](#9-mode-absence)
10. [Tarification électrique](#10-tarification-électrique)
11. [Horloge et synchronisation](#11-horloge-et-synchronisation)
12. [Connectivité réseau](#12-connectivité-réseau)
13. [MQTT](#13-mqtt)
14. [Multi-routeurs (ESP-RMS distants)](#14-multi-routeurs-esp-rms-distants)
15. [Ventilateur thermorégulé SSR](#15-ventilateur-thermorégulé-ssr)
16. [Historiques et économies](#16-historiques-et-économies)
17. [Journal d'événements](#17-journal-dévénements)
18. [Import / Export de configuration](#18-import--export-de-configuration)
19. [Mise à jour OTA (Over-The-Air)](#19-mise-à-jour-ota-over-the-air)
20. [Interface web Soleo](#20-interface-web-soleo)
21. [Sécurité d'accès](#21-sécurité-daccès)
22. [Récapitulatif des pages web](#22-récapitulatif-des-pages-web)

---

## 1. Vue d'ensemble

Le **Routeur Solaire F1ATB** (RMS = Routeur Multi Sources) est un firmware ESP32 open-source qui réoriente automatiquement l'excédent de production photovoltaïque vers des charges électriques (chauffe-eau, piscine, batterie…) afin d'éviter d'injecter gratuitement sur le réseau.

**Principe :**
- Mesure en temps réel le flux de puissance à l'entrée du tableau électrique
- Calcule l'excédent PV disponible
- Pilote jusqu'à **4 sorties** (SSR / relais / Triac) pour absorber cet excédent
- S'adapte aux contraintes tarifaires, météo et aux besoins thermiques

---

## 2. Sources de mesure de puissance

Le routeur supporte **10 sources** de mesure, sélectionnables dans les paramètres :

| Source | Description |
|--------|-------------|
| **UxI** | Mesure directe tension × courant via transformateur + sonde ampèremétrique |
| **JSY-MK-194T** | Module avec mesure tension secteur + 2 sondes ampèremétriques (maison + triac) |
| **JSY-MK-333** | Module pour installation triphasée |
| **Linky** | Lecture téléinfo standard (TIC mode standard) — index, puissances, LTARF |
| **Enphase Envoy-S** | Passerelle Enphase firmware V5, V7, V8 |
| **Shelly EM** | Compteur Shelly EM |
| **Shelly Pro EM** | Compteur Shelly Pro EM |
| **SmartGateways** | Compteur SmartG |
| **HomeWizard** | Compteur HomeWizard |
| **MQTT** | Puissance reçue par abonnement MQTT |
| **ESP Externe** | Données relayées depuis un autre ESP32-RMS |

Toutes les sources exposent les mêmes variables internes (`PuissanceSM`, `PuissanceST`, etc.) utilisées par le moteur de routage.

### Calibration
- Offset de calibration tension (`CalibU`) et courant (`CalibI`)
- Nommage personnalisé des sondes (`nomSondeFixe`, `nomSondeMobile`)

---

## 3. Sorties de routage (Actions)

Jusqu'à **4 actions** indépendantes, chacune pilotant une sortie physique.

### Modes de sortie

| Mode | Description |
|------|-------------|
| **OFF forcé** | Sortie toujours ouverte (désactivée) |
| **ON forcé** | Sortie toujours fermée (100% de la puissance) |
| **Puissance fixe** | Commutation tout/rien à un seuil configurable |
| **Multi-sinus** | Découpe sinusoïdale progressive 0→100% (SSR recommandé) |
| **Train de sinus** | Salves de cycles complets (convient aux relais) |

### Paramétrage par action
- **Nom** personnalisé affiché sur le tableau de bord
- **GPIO** de sortie configurable (broche physique ESP32)
- **Puissance nominale** de la charge (en W), utilisée pour les calculs d'économies
- **Polarité** : actif haut (3,3 V) ou actif bas (0 V)
- **Sondes de température** : la sonde à surveiller pour l'action (arrêt par température)
- **Compteur H_Ouvre** : cumul du temps équivalent d'ouverture depuis 6h00

### Forçage manuel
Depuis le tableau de bord, chaque action peut être :
- **Forcée ON** (prioritaire pendant X minutes)
- **Forcée OFF** (bloquée)
- Revenue en **mode automatique**

---

## 4. Conditions et déclenchement des actions

Chaque action peut être configurée avec des **tranches horaires** (jusqu'à plusieurs par action), chacune définissant :

### Plage horaire
- Heure de début / fin (format décimal hhmm)
- Jours de la semaine (lundi→dimanche, tout sélectionnable)

### Conditions météo (`MeteoCond`)

| Code | Condition |
|------|-----------|
| 0 | Toujours actif (pas de condition météo) |
| 1 | Actif si prévision solaire **bonne** (surplus attendu) |
| 2 | Actif si prévision solaire **mauvaise** (pour chauffe en HC) |
| 3 | Actif si température ballon **insuffisante** (besoin détecté) |
| 4 | Actif si **Linky en heures creuses** (HC/CREUSE) |
| 5 | **Adaptatif ballon** : chauffe si déficit prévu (mode ballon intelligent) |

### Conditions de puissance
- Seuil de puissance importée minimum pour déclencher l'action
- Seuil de puissance exportée maximum

### Conditions de température
- Canal de sonde (0–3) à surveiller
- Température **inférieure** déclenchante (Tinf)
- Température **supérieure** d'arrêt (Tsup)
- **Hystérésis** automatique si Tinf < Tsup

### Conditionnement par d'autres actions
- Une action peut être déclenchée **seulement si une autre action est ON** (même ESP32 ou ESP distant)

---

## 5. Capteurs de température

### DS18B20 (bus 1-Wire interne)
- Jusqu'à **4 capteurs** sur un seul bus
- Adressage automatique à la mise sous tension
- Offset de correction configurable par canal (°C)
- Nom personnalisé par canal

### Sources externes
- **DS18B20 via ESP Externe** : températures d'un autre routeur RMS sur le réseau
- **MQTT** : souscription à un topic de température (format numérique)
- **Watchdog** : si une mesure n'est plus reçue, le canal est invalidé

### Affichage
- Température affichée sur le tableau de bord et la page Mesures
- Utilisée comme condition d'action et pour le suivi du ballon

---

## 6. Onduleur SMA Sunny Boy (Modbus TCP)

Connexion directe à un onduleur **SMA Sunny Boy** via le protocole Modbus TCP (port 502, Unit ID 3).

### Données lues
- **Puissance AC instantanée** (registre 30775, S32) en W
- **Énergie totale produite** (registre 30529, U32) en Wh
- **Énergie journalière** calculée par différence depuis 6h00

### Comportement
- Lecture toutes les **20 secondes**
- Si l'onduleur est injoignable (nuit — certains modèles coupent) : retry toutes les 5 minutes
- Valeur NaN SMA (`0x80000000`) interprétée comme 0 W (veille)

### Activation
Serveur Modbus à activer dans l'interface web SMA :
> Paramètres → Communication externe → Modbus → TCP On

---

## 7. Prévision météo solaire

Prévision de production solaire basée sur l'API **Open-Meteo** (gratuite, sans clé API).

### Principe
- Récupération du **rayonnement global quotidien** (`shortwave_radiation_sum` en MJ/m²)
- Conversion en production estimée :
  `kWh = MJ/m² ÷ 3.6 × Puissance_crête_kWc × 0.80`
- Prévisions **J** (aujourd'hui) et **J+1** (demain) rafraîchies toutes les **2 heures**

### Paramètres configurables
- **Latitude / Longitude** GPS (avec bouton de géolocalisation automatique)
- **Puissance crête PV** installée (kWc)
- **Coefficient routable** (`BallonCoefAuto`) : fraction de la production effectivement dispatchable vers le ballon (auto-appris)

### Auto-apprentissage du coefficient
Chaque soir à minuit, le routeur compare :
- Production SMA réelle du jour (kWh)
- Énergie réellement routée vers le ballon (kWh)

Le coefficient est ajusté par **moyenne pondérée (70/30)** et mémorisé dans `/coefauto.txt`.

### Historique prévision/production
Fichier `/histmeteo.csv` : 60 dernières lignes, une par jour.
Format : `date;prévision_kWh;production_kWh;routée_kWh;coef`

---

## 8. Ballon eau chaude intelligent

Système de gestion prédictive du chauffe-eau ECS.

### Paramètres
| Paramètre | Description |
|-----------|-------------|
| `BallonCanal` | Canal de sonde DS18B20 mesurant la température du ballon |
| `BallonVolume` | Volume du ballon en litres |
| `BallonTcible` | Température cible (°C) |
| `BallonPuissance` | Puissance de la résistance (W) |
| `BallonTmin` | Température minimale exploitable (°C), par défaut 40°C |
| `BallonModeIntel` | Active le mode intelligent (apprentissage usage) |

### Calculs en temps réel
- **Besoin** (kWh) = Volume × 1,163/1000 × (Tcible − Tballon)
- **Réserve** (kWh) = Volume × 1,163/1000 × (Tballon − Tmin)
- **Surplus prévu** (kWh) = Prévision météo × Coefficient routable / 100
- **Déficit** = Besoin − Surplus prévu → déclenche le **forçage adaptatif** si > 0

### Mode standard
Le ballon est chauffé lorsque le déficit est positif (besoin > surplus solaire attendu).

### Mode intelligent (`BallonModeIntel = 1`)
- Le routeur **apprend l'usage habituel d'eau chaude** en détectant les chutes de température (signe de puisage).
- Chute mesurée = ΔT sur 60 secondes × Volume × 1,163/1000
- Moyenne glissante sur ~5 jours : `UsageMoyen = 0.8 × ancien + 0.2 × nouveau`
- Le déficit vise à couvrir `UsageMoyen × 1.3 − Réserve` (marge 30%)
- **Objectif** : ne maintenir que la réserve nécessaire, pas toujours chasser la température cible

### Anti-légionelle
- Si le ballon n'a pas chauffé depuis `BallonAntiLegioJours` jours : chauffe forcée à 100%
- Valable en mode présence **et** en mode absence
- Le compteur `AbsenceJoursSansChauffe` est incrémenté chaque nuit à minuit

---

## 9. Mode Absence

Suspension complète des automatismes pendant une période de vacances.

### Activation
- **Manuelle** : bouton 🏨 sur le tableau de bord (activation/désactivation instantanée)
- **Automatique** : dates de début et fin configurables (`AbsenceDebut`, `AbsenceFin` au format AAAAMMJJ)
- Les deux modes peuvent être combinés

### Comportement
- Toutes les actions sont forcées **OFF** (sauf anti-légionelle)
- Bannière orange visible sur le tableau de bord
- Bouton 🏨 mis en surbrillance

### Anti-légionelle en absence
- Paramètre `AbsenceAntiLegio` : nombre de jours sans chauffe toléré (0 = désactivé, 3/5/7/14 jours)
- Quand le seuil est atteint : la résistance du ballon est forcée ON à 100% jusqu'à atteindre la température cible
- Le compteur est remis à zéro après la chauffe

---

## 10. Tarification électrique

### Mode Base (HP/HC)
- **Prix Heures Pleines** (`PrixHP`) en €/kWh
- **Prix Heures Creuses** (`PrixHC`) en €/kWh
- Détection des HC via le Linky (champ LTARF contenant "CREUSE" ou "HC")

### Mode Tempo EDF (6 prix)
Activé par `TempoRTEon`. Couleur du jour récupérée depuis **RTE** (réseau de transport d'électricité).

| Couleur | Heures Pleines | Heures Creuses |
|---------|---------------|----------------|
| 🔵 Bleu | `PrixBleuHP` (déf. 0,1609 €) | `PrixBleuHC` (déf. 0,1296 €) |
| ⚪ Blanc | `PrixBlancHP` (déf. 0,1894 €) | `PrixBlancHC` (déf. 0,1486 €) |
| 🔴 Rouge | `PrixRougeHP` (déf. 0,7562 €) | `PrixRougeHC` (déf. 0,1568 €) |

La couleur du jour est affichée en badge dans l'en-tête (ex. "Tempo Bleu · HC").

### Calcul des économies
- `PrixKwhActuel()` : retourne le prix de l'instant (selon LTARF + Tempo si activé)
- Économie journalière = énergie routée (kWh) × prix courant
- Cumul mensuel et annuel dans l'historique

---

## 11. Horloge et synchronisation

### Sources d'horloge (sélectionnables)
| Mode | Description |
|------|-------------|
| **Internet (NTP)** | Serveur NTP configurable (défaut : pool.ntp.org) |
| **Linky** | Heure extraite de la téléinfo |
| **Interne** | Saisie manuelle JJ/MM/AAAA HH:MN |
| **IT 100 Hz** | Synchronisation sur les interruptions secteur (Triac) |
| **IT 50 Hz** | Synchronisation sur les fronts secteur |
| **ESP Externe** | Heure reçue d'un autre ESP32-RMS |

### Fuseau horaire
7 zones disponibles : Europe centrale, Guadeloupe/Martinique, Guyane, Réunion, Mayotte, Nouvelle-Calédonie, Wallis-et-Futuna.

---

## 12. Connectivité réseau

### Modes WiFi
| Mode | Description |
|------|-------------|
| **STA + Internet** | Connexion à un réseau WiFi local avec accès internet (NTP, météo, RTE) |
| **STA sans Internet** | WiFi local sans sortie internet |
| **Access Point (AP)** | ESP32 crée son propre hotspot pour configuration initiale |

### Options réseau
- **DHCP** ou adresse IP fixe (avec Gateway, masque, DNS configurables)
- **Hostname mDNS** : accès via `<hostname>.local`
- Scan WiFi au boot pour faciliter le changement de réseau
- **Veille WiFi** (`WifiSleep`) activable pour économiser de l'énergie

### ESP32-ETH01 (Ethernet)
Modèle ESP32 avec interface Ethernet physique (mode réseau 10 = ETH01).

---

## 13. MQTT

### Publication (topics sortants)
Le routeur publie régulièrement :
- Puissances mesurées (importée, exportée, produite)
- État des actions (ON/OFF/%, nom)
- Températures des sondes (°C)
- Économies (€/jour)
- Énergie routée (Wh)
- Température CPU ESP32

Préfixes séparés pour la **déclaration** (`MQTTPrefix`) et la **publication d'état** (`MQTTPrefixEtat`).

### Souscription (topics entrants)
Le routeur peut recevoir :
- **Puissance externe** (source de mesure alternative)
- **Températures externes** (jusqu'à 4 canaux)
- **Commande ON/OFF** des actionneurs (forçage via MQTT)

### Paramètres MQTT
| Paramètre | Description |
|-----------|-------------|
| `MQTTIP` | Adresse IP du broker |
| `MQTTPort` | Port (défaut 1883) |
| `MQTTUser` / `MQTTPwd` | Authentification |
| `MQTTdeviceName` | Nom de l'appareil |
| `MQTTPrefix` | Préfixe topics de déclaration |
| `MQTTPrefixEtat` | Préfixe topics d'état |
| Reconnexion | Tentative toutes les 5 minutes si déconnecté |

---

## 14. Multi-routeurs (ESP-RMS distants)

Jusqu'à **8 ESP32-RMS** peuvent être interconnectés.

### Fonctionnalités
- Un ESP32 maître peut lire les mesures de puissance d'un ESP esclave
- Les températures d'un esclave peuvent être utilisées comme source pour les conditions d'action du maître
- Les actions d'un esclave peuvent conditionner les actions du maître
- Suivi de la qualité des échanges (notes de communication)

---

## 15. Ventilateur thermorégulé SSR

Gestion d'un ventilateur de refroidissement pour le SSR/Triac.

### Paramètres
| Paramètre | Description |
|-----------|-------------|
| `FanGpio` | Broche GPIO de commande du ventilateur |
| `FanCanalTemp` | Canal de sonde de température à surveiller |
| `FanTdemarrage` | Température de démarrage du ventilateur (°C) |
| `FanTmax` | Température à pleine vitesse (°C) |

### Comportement
- Commande **PWM proportionnelle** entre Tdémarrage et Tmax
- Arrêt automatique sous Tdémarrage
- Permet de réduire le bruit en dehors des périodes de forte dissipation

---

## 16. Historiques et économies

### Historique 48h
- Puissance mesurée toutes les 2 secondes, moyennée sur 10 minutes
- Graphiques SVG des puissances (importée, exportée, routée)
- Graphiques des températures (4 canaux)
- Graphiques des ouvertures SSR (%, par action)

### Historique mensuel
- Énergie routée par jour (Wh/kWh)
- Énergie importée par jour
- Économies cumulées (€)
- Fichier `/histo_mensuel.csv` géré en LittleFS

### Économies en temps réel
- Économie de la journée en cours (€)
- Cumul du mois (€)
- Affiché sur le tableau de bord et la page Mesures
- Calcul utilisant `PrixKwhActuel()` (prix tenant compte HP/HC et Tempo)

### Historique météo / production
- Fichier `/histmeteo.csv` : 60 jours, prévision vs production vs routé vs coefficient

---

## 17. Journal d'événements

Journal en langage naturel, accessible depuis le tableau de bord.

**Événements journalisés :**
- Démarrage et redémarrage du routeur
- Chargement / sauvegarde des paramètres
- Changements d'état des actions (ON/OFF/forçage)
- Détection de besoin ballon (chauffe en cours / satisfaite)
- Bilan journalier SMA (production vs routé vs coefficient)
- Apprentissage de l'usage eau chaude
- Anti-légionelle déclenchée
- Mode absence activé / désactivé
- Erreurs de connexion (SMA, météo, RTE)

Les 20 derniers événements sont conservés en mémoire et affichés sur le tableau de bord.

---

## 18. Import / Export de configuration

### Export
- Téléchargement de tout fichier stocké sur le LittleFS de l'ESP32 :
  - `parametres.json` : tous les réglages
  - Historiques CSV
  - `/coefauto.txt` : coefficient appris
  - `/histmeteo.csv` : historique météo
- Nom automatique avec version et adresse IP (`parametres_V17.16_IP42.json`)
- Indicateur d'espace disque (total / utilisé / pourcentage)

### Import
- Chargement d'un fichier `.json` (paramètres) ou `.csv` (données mensuelles)
- Un **Restart** est recommandé après import de paramètres

### Effacement
- Effacement individuel de chaque fichier avec confirmation

---

## 19. Mise à jour OTA (Over-The-Air)

Mise à jour du firmware via le navigateur web, sans câble USB.

### Procédure
1. Sauvegarder les paramètres (si changement de version majeure)
2. Télécharger le `.bin` depuis le site F1ATB.fr
3. Uploader via la page OTA
4. Restart + Ctrl+F5 navigateur

### Suivi
- Version actuelle affichée
- Barre de progression en temps réel
- Fichiers binaires listés sur f1atb.fr directement dans la page

---

## 20. Interface web Soleo

Interface responsive **dark/light** accessible depuis n'importe quel navigateur (PC, tablette, smartphone).

### Tableau de bord (`/`)
- **Flux d'énergie SVG** animé : soleil → maison → eau chaude → réseau
- **Tuiles** : production PV du jour, consommation maison, ballon (avec barre de niveau), économies du mois
- **Liste des automatismes** avec état en temps réel (chip coloré)
- **Journal** des 20 derniers événements
- **Badge tarifaire** : couleur Tempo + HP/HC en cours
- **Indicateur de connexion** (point clignotant vert/gris)
- **Bouton absence** 🏨 + bannière orange si mode actif
- **Bascule thème** 🌙/☀️ (persistant dans le navigateur)

### Page Mesures (`/mesures`)
- Tableau en temps réel des grandeurs électriques (toutes sources)
- Graphiques SVG 48h (puissances, températures, ouvertures)
- Tableau multi-actions avec forçage ON/OFF par action
- Données Linky brutes (téléinfo standard)
- Informations ESP32 (RAM, WiFi, uptime, version)

### Page Données brutes (`/Brute`)
- Données brutes de chaque source (JSY, Enphase, Shelly, etc.)
- Tableau Linky ligne par ligne
- Informations système ESP32 (RAM, core charge, WiFi dBm, MAC, IP)

### Page Actions (`/Actions`)
- Vue cartes des automatismes configurés
- Création / modification / suppression d'actions
- Définition des tranches horaires et conditions
- Assistant de création (langage naturel)

### Page Réglages (`/Para`)
- Accordéons thématiques (réseau, sources, tarifs, météo, SMA, ballon, absence, températures, actions, avancé)
- Mode standard / Expert (masque les réglages techniques)
- Sauvegarde flottante avec toast de confirmation

### Pages utilitaires
| URL | Page |
|-----|------|
| `/Heure` | Réglage horloge et fuseau |
| `/Export` | Import / export fichiers |
| `/OTA` | Mise à jour firmware |
| `/Couleurs` | Personnalisation couleurs tableaux |
| `/Wifi` | Configuration WiFi (mode AP) |

---

## 21. Sécurité d'accès

- **Mot de passe** configurable pour protéger l'accès aux pages Paramètres et Actions
- Stocké en cookie navigateur (`CleAcces`)
- Les pages de consultation (tableau de bord, mesures) restent accessibles sans mot de passe

---

## 22. Récapitulatif des pages web

| URL | Titre | Accès |
|-----|-------|-------|
| `/` | Tableau de bord (temps réel) | Libre |
| `/mesures` | Mesures détaillées + graphiques | Libre |
| `/Brute` | Données brutes (diagnostic) | Libre |
| `/Actions` | Gestion des automatismes | Protégé |
| `/Para` | Paramètres généraux | Protégé |
| `/Heure` | Réglage horloge | Protégé |
| `/Export` | Import / Export | Protégé |
| `/OTA` | Mise à jour firmware | Protégé |
| `/Couleurs` | Couleurs tableaux | Protégé |
| `/Wifi` | Configuration WiFi | — (AP uniquement) |

---

## Annexe — Variables persistées (LittleFS)

| Fichier | Contenu |
|---------|---------|
| `parametres.json` | Tous les paramètres du routeur (réseau, actions, tarifs, ballon…) |
| `/coefauto.txt` | Coefficient routable auto-appris + usage moyen eau chaude |
| `/histmeteo.csv` | 60 jours de prévision / production / routé / coefficient |
| `/histo_mensuel.csv` | Énergie routée et économies par jour (mois courant) |
| `/journal.txt` | Journal des événements |

---

*Document généré le 20/06/2026 — Firmware v17.16*
