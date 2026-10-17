# Soleo — routeur solaire

Firmware ESP32 pour routeur photovoltaïque : automatismes autour du chauffe-eau,
prévision météo, tarification Tempo et protection du disjoncteur, avec une interface web
pensée pour le mobile.

> **Soleo est un fork du routeur RMS de [F1ATB](https://f1atb.fr)**, basé sur sa V17.16.
> Le cœur de régulation, les pilotes de mesure et l'essentiel du firmware sont son œuvre ;
> ce dépôt n'ajoute que des fonctions périphériques et une nouvelle interface.
> Documentation d'origine : **https://f1atb.fr**, section Domotique.

> ⚠️ **Fork personnel, sans garantie.** Testé sur une seule installation, avec un périmètre
> matériel réduit (voir plus bas). Ce n'est pas un remplacement du firmware officiel.
> Exportez votre configuration (page **Import / Export**) avant de flasher, et gardez de
> quoi revenir en arrière.
>
> 📌 **Base : V17.16.** Le firmware officiel poursuit son développement de son côté
> ([V17.26 au 09/2026](https://github.com/F1ATB/Solar-Router-F1ATB)) : les correctifs et
> nouveautés publiés depuis ne sont pas repris ici.

## Versions

Ce fork a **sa propre numérotation, repartie à 1.00**, pour éviter toute confusion avec
celle du firmware officiel — qui est déjà en 17.26 et continue d'avancer. Le fichier
principal garde son nom d'origine (`Solar_Router_V17_16.ino`), qui indique la base amont,
pas la version du fork.

| Incrément | Signification |
|---|---|
| `1.01`, `1.02` | Correctif |
| `1.10`, `1.20` | Nouvelle fonctionnalité |
| `2.00` | Changement majeur ou rupture du format de configuration |

Les binaires compilés sont publiés dans les
[Releases](https://github.com/jeremyper/Routeur_f1atb/releases) et accessibles directement
depuis la page **Mise à jour** du routeur.

## Licence

**GNU AGPL-3.0-or-later**, comme le firmware d'origine — voir [LICENSE](LICENSE).
Toute redistribution, modifiée ou non, doit rester sous la même licence et fournir le code source.

---

> 📖 **Vous voulez l'essayer ?** Suivez le [**guide d'installation pas à pas**](INSTALLATION.md).
> Commencez par l'étape 0 : elle vérifie en une minute si votre installation est compatible.
>
> 📡 **Suivre son routeur depuis l'extérieur**, sans ouvrir de port ni installer Home Assistant :
> voir [**Supervision à distance**](SUPERVISION_DISTANTE.md).
>
> 📱 **Application de suivi** — installable sur téléphone, se connecte à *votre* broker :
> [jeremyper.github.io/Routeur_f1atb](https://jeremyper.github.io/Routeur_f1atb/)
>
> 🌡️ **Dimensionner son relais statique** — dissipateur, calibre, ventilateur, câblage :
> [calculateur en ligne](https://jeremyper.github.io/Routeur_f1atb/dimensionnement.html)

## Aperçu

| Tableau de bord | Mesures |
|---|---|
| ![Tableau de bord](docs/captures/accueil.png) | ![Mesures](docs/captures/mesures.png) |
| Flux d'énergie en direct, tarif Tempo du jour, économies du mois | Prévision de production PV sur 48 h (Open-Meteo) |

| Automatismes | Réglages |
|---|---|
| ![Actions](docs/captures/actions.png) | ![Réglages](docs/captures/reglages.png) |
| Assistant de création en langage naturel | Ballon d'eau chaude et chauffe prédictive |

---

## Ce que ce fork ajoute

### Eau chaude sanitaire
- **Ballon intelligent** — estime la consommation réelle d'eau chaude en détectant les
  puisages (chutes de température), et n'entretient que la réserve réellement nécessaire
  au lieu de toujours viser la température cible
- **Sécurité anti-légionelle** — chauffe complète forcée après N jours sans montée en
  température, y compris en mode absence
- **Forçage adaptatif** — croise le besoin du ballon avec le surplus solaire attendu
- **Deux sondes** — une en bas (obligatoire), une en haut (facultative) pour mieux estimer
  la réserve d'eau chaude ; voir [où placer les sondes](#placement-des-sondes-du-ballon)

### Prévision météo solaire
- Source **Open-Meteo** (gratuite, sans clé API) : latitude, longitude, puissance crête
- Production estimée aujourd'hui / demain, rafraîchie périodiquement
- **Nouvelle condition de période** : « actif si prévision demain/aujourd'hui < ou ≥ seuil »
  — par exemple *ne forcer le chauffe-eau la nuit que si demain s'annonce couvert*
- Auto-apprentissage du coefficient de surplus routable, historique prévision/production

### Tarification et suivi
- **Tarifs Tempo RTE** : 6 prix configurables selon la couleur du jour et HP/HC
- Suivi des **économies en €** (jour / mois / total)
- **Journal d'événements** en langage naturel

### Protection et confort
- **Délestage d'abonnement** — bride progressivement les sorties quand la puissance
  soutirée approche le calibre du disjoncteur, marches forcées comprises
- **Mode absence** — suspend les automatismes, manuellement ou sur plage de dates
- **Ventilateur SSR thermorégulé** — rampe de vitesse pilotée par une sonde sur le dissipateur

### Onduleur
- **SMA Sunny Boy** en Modbus TCP : production instantanée et énergie du jour

### Interface web
- Refonte complète : tableau de bord avec flux d'énergie, page Actions en langage naturel,
  Réglages en accordéons thématiques
- Thème clair / sombre, pensée mobile
- Feuille de style et bascule de thème mutualisées (`/commun.css`, `/theme.js`)

### Robustesse
- Appels réseau bloquants (RTE, météo, SMA) déportés sur le cœur 0 pour ne pas perturber
  la régulation, avec verrous sur les données partagées entre cœurs
- Contrôle d'accès étendu aux points d'entrée qui modifient l'état ou exposent des secrets
  (voir *Sécurité d'accès* dans [FONCTIONNALITES.md](FONCTIONNALITES.md))
- Fermeture de sécurité des sorties sur mesure de puissance périmée

---

## Périmètre matériel : plus étroit que l'original

Ce fork est **recentré sur une seule configuration matérielle**, la mienne. Plusieurs
sources et périphériques gérés par le firmware d'origine ont été retirés. Si votre
installation utilise l'un d'eux, **restez sur le firmware officiel F1ATB**.

### Sources de mesure conservées
| Source | État |
|---|---|
| **UxI** (transformateur + pince ampèremétrique) | ✅ |
| **UxIx2** (module JSY-MK-194T) | ✅ |
| **UxIx3** (module JSY-MK-333, triphasé) | ✅ |

### Sources retirées
Linky TIC, Enphase Envoy-S, Shelly EM et Pro EM, SmartGateways, HomeWizard,
puissance reçue par MQTT, ESP externe.
*(des libellés résiduels peuvent subsister dans l'interface : ils ne correspondent
à aucun pilote de lecture)*

### Autres retraits
- Écrans LCD tactiles (Sunton / CYD) et pilotes associés — seules les LED d'état restent
- Carte Ethernet WT32-ETH01

## Ce qui est conservé de l'original

- Régulation **PID** éprouvée (intégrateur dominant, P/D optionnels)
- Modes de sortie **Multi-sinus**, **Train de sinus**, **On/Off**, découpe triac
- Jusqu'à **10 actions × 8 périodes**, avec conditions horaire, température, tarif,
  météo et état d'une autre action
- **Publication MQTT** avec auto-découverte Home Assistant
- Multi-routeurs (ESP-RMS distants), historiques, **OTA**, Telnet, mode AP / WPS
- Sondes de température **DS18B20** (jusqu'à 4 canaux), internes ou externes

---

## Compilation

- **Arduino IDE 2.x** ou arduino-cli, core **esp32 by Espressif 3.3.x**
- Carte : `ESP32 Dev Module`
- Partition Scheme : **Custom** ⚠️ (voir ci-dessous)
- Bibliothèques : `ArduinoJson`, `PubSubClient`, `DallasTemperature`
  (+ `OneWire` fourni dans le dépôt — version spécifique, ne pas remplacer par celle du
  gestionnaire de bibliothèques)

Empreinte : **1 719 160 octets (88 %)** de flash, 83 508 octets de RAM statique.

### ⚠️ Le dossier doit être renommé

Arduino IDE exige que le dossier porte le nom du croquis principal. Après clonage,
renommez le dossier en **`Solar_Router_V17_16`**. Si l'IDE propose de « créer un dossier »
à l'ouverture, refusez : il ne déplacerait qu'un seul fichier sur la trentaine du projet.

### ⚠️ Partition Scheme : « Custom » obligatoire

Avec le schéma par défaut (*Default 4MB with spiffs*, 1,2 Mo applicatifs), la compilation
échoue :

```
Le croquis utilise 1699336 octets (129%) ... Le maximum est de 1310720 octets.
text section exceeds available space in board
```

Sélectionnez **Outils → Partition Scheme → Custom** : le `partitions.csv` fourni réserve
1900 Ko par partition applicative, avec OTA. Vérifiez aussi que **Flash Size** est sur
**4MB** — le découpage fourni totalise ~3,97 Mo.

Avec « Custom », l'IDE affiche ensuite un pourcentage trompeur :

```
Le croquis utilise 1699336 octets (10%) ... Le maximum est de 16777216 octets.
```

C'est normal : le core ESP32 ne sait pas déduire la taille utile d'un `partitions.csv`
quelconque et se rabat sur la taille maximale d'une puce flash. L'occupation réelle est
de **87 %** des 1900 Ko de la partition `app0`.

Ne choisissez pas un autre schéma qui « rentrerait » : la mise à jour OTA n'écrit que la
partition applicative et conserve la table de partitions déjà en place. Un découpage
différent déplacerait la zone LittleFS et ferait perdre configuration, actions et
historiques au redémarrage.

---

## Exemple : forçage intelligent du chauffe-eau

1. **Réglages** → activer la prévision météo, renseigner latitude / longitude / kWc.
2. **Réglages** → renseigner le ballon : sonde, volume, température cible, puissance.
3. **Actions** → action « Chauffe-eau (SSR) », mode **Multi-sinus**.
4. Période 9h–17h : type **Routage** (seuil 0 W) — absorbe le surplus solaire.
5. Période 2h–6h : type **ON**, condition tarif **Heures Creuses**, condition météo
   « **Prévision demain < 8 kWh** ».

Le forçage nocturne ne se déclenche alors que si la journée suivante ne permettra pas
de chauffer l'eau au solaire. En activant en plus la **chauffe prédictive**, le ballon
n'entretient que le volume correspondant à votre consommation habituelle.

---

## Placement des sondes du ballon

![Schéma : sonde du bas au tiers inférieur, sonde du haut près de la sortie d'eau chaude](docs/schema_sondes_ballon.svg)

Dans un ballon, l'eau se range par couches : la chaude en haut, la froide en bas. Quand on
tire de l'eau chaude, l'eau froide entre **par le bas** et l'eau chaude sort **par le haut**.
Le bas refroidit donc tout de suite, le haut reste chaud jusqu'à ce que le ballon soit
presque vide.

| | ② Sonde du **bas** | ① Sonde du **haut** |
|---|---|---|
| Obligatoire ? | **Oui** — c'est elle qui active la chauffe intelligente | Non, facultative |
| Où | Au **tiers inférieur**, contre la cuve sous l'isolant, ou dans le doigt de gant du thermostat | Près de la sortie d'eau chaude, contre la cuve sous l'isolant |
| Sert à | Détecter les **puisages** (l'eau froide arrive par le bas) et juger l'**anti-légionelle** (quand le bas est chaud, tout le ballon l'est) | Mieux estimer la **réserve** d'eau chaude, et afficher la température qui sort au robinet |
| Réglage | **Réglages** → Ballon → *Sonde du bas du ballon* | **Réglages** → Ballon → *Sonde du haut du ballon* |

**Comment le routeur s'en sert.** Il découpe le ballon en deux moitiés de même volume : la
moitié basse est à la température du bas, la moitié haute à celle du haut. Chaque moitié
compte à part, si bien qu'un bas froid n'efface pas l'eau chaude encore disponible en haut.

- **Sans sonde du haut**, tout le ballon est supposé à la température du bas. Le routeur
  sous-estime alors la réserve et chauffe un peu plus que nécessaire : c'est le côté
  confort, avec un peu moins d'économies.
- **Avec la sonde du haut**, la réserve et la consommation apprise sont plus justes : la
  chauffe intelligente entretient au plus près de votre besoin.
- **Une seule sonde, en haut : à éviter.** Les puisages passent inaperçus, la réserve est
  surestimée (risque de douche froide) et l'anti-légionelle s'arrête alors que le bas est
  encore tiède.

**Réglages recommandés** (page **Réglages**) :

| Réglage | Valeur | Pourquoi |
|---|---|---|
| Température souhaitée *(L'essentiel → Ballon)* | **55 à 60 °C** | Sous 55 °C, les légionelles se développent ; au-delà de 60 °C, plus de calcaire et de pertes |
| Température minimale utile *(L'essentiel → Ballon)* | **40 °C** | Une douche se prend vers 38–40 °C. Champ visible avec la chauffe prédictive |
| Chauffe anti-légionelle *(Mode absence → Sécurité sanitaire)* | **7 jours** | Chauffe complète forcée si la cible n'est pas atteinte depuis 7 jours. Active aussi hors absence quand la chauffe prédictive est en marche : ne pas mettre 0, le ballon peut rester tiède |

Gardez au moins **10 °C d'écart** entre la température souhaitée et la minimale utile : la
réserve d'eau chaude se calcule sur cet écart. Trop serré, la réserve paraît toujours vide et
le routeur chauffe sans arrêt.

**Pose.** Coupez le disjoncteur du ballon avant d'ouvrir le capot : il y a du 230 V. Ne
retirez jamais le thermostat, c'est une sécurité. Plaquez la sonde avec un peu de pâte
thermique et remettez l'isolant par-dessus, sinon elle mesure surtout la pièce. Jamais sur le
tuyau d'eau chaude : au repos il est à la température de la pièce, et il chauffe pendant un
puisage, l'inverse de ce que le routeur attend.

**Vérifier.** Ouvrez un robinet d'eau chaude 2 minutes : la sonde du bas doit perdre plus de
0,3 °C par minute, sinon le puisage n'est pas vu. Le lendemain matin, le journal doit afficher
*« Ballon : usage estimé du jour … kWh »*. Dans **Réglages** → Ballon, la ligne *Diagnostic
du jour* affiche les deux températures et signale des sondes probablement inversées.

---

## Documentation

[**FONCTIONNALITES.md**](FONCTIONNALITES.md) décrit l'ensemble des fonctions, paramètres
et pages web en détail.

## Crédits

Firmware d'origine : **F1ATB** — https://f1atb.fr
Bibliothèque OneWire : Jim Studt, Paul Stoffregen et contributeurs.
