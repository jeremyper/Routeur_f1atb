# v1.28 — Sorties GPIO, ventilateur sur relais, historique à distance

Cette version corrige une régression signalée par un utilisateur venant du firmware F1ATB
(plus aucune sortie GPIO active), ajoute un vrai mode **relais** au ventilateur du SSR, et
fait publier par le routeur son propre historique pour l'application de suivi.

Elle inclut la v1.27, qui n'a pas fait l'objet d'une release séparée.

---

## 🔧 Sorties GPIO perdues en venant de F1ATB

Deux défauts, reproduits puis corrigés :

- **Le type de carte retombait à « non défini ».** Le firmware F1ATB propose des cartes
  que Soleo ne gère pas (écrans CYD, WT32-ETH01). Un tel type ne pouvait pas s'afficher
  dans les Réglages : le premier enregistrement le remettait à zéro, et dans ce cas
  **aucune sortie d'action n'était activée**. Le type est désormais conservé, et les
  sorties des actions sont activées quel que soit le type de carte.
- **Une action pouvait être supprimée à l'enregistrement.** Si sa broche n'était pas dans
  la liste proposée pour la carte, le champ restait vide et la page Actions effaçait
  l'action entière, sans prévenir. La broche est maintenant conservée, et l'action 0
  (chauffe-eau) est de nouveau protégée contre l'effacement, comme dans le firmware F1ATB.

> ⚠️ **Si vous avez installé Soleo en venant du firmware F1ATB**, vérifiez après la mise à
> jour : **Réglages** → type de carte, puis **Actions** → que vos actions et leurs broches
> sont toujours là. Une action déjà effacée par l'ancien comportement est à recréer.

## 🌀 Ventilateur du SSR : mode relais

La fonction ventilateur ne savait piloter qu'en PWM (vitesse variable, dès 30 %). Un relais
ne suit pas un PWM : il grésille ou ne colle pas.

- Nouveau mode **« module relais » marche / arrêt**, avec 3 °C d'écart entre démarrage et
  arrêt pour éviter les claquements
- Choix du **niveau actif** — la plupart des modules relais s'enclenchent sur un niveau bas
- Broches **16 à 23** ajoutées (les plus courantes sur les cartes relais) ; la **12**,
  broche de démarrage de l'ESP32, est retirée
- Changement de broche ou de mode pris en compte **sans redémarrage**, et conflit avec une
  action, la sonde ou le port série **signalé au journal**

🌡️ Avant d'ajouter un ventilateur, vérifiez le dissipateur : un SSR meurt de chaleur, pas
d'un manque d'ampères. Le **[calculateur de dimensionnement](https://jeremyper.github.io/Routeur_f1atb/dimensionnement.html)**
donne le dissipateur, le calibre du SSR et le seuil de ventilateur adaptés à votre installation.

## 📱 Historique dans l'application de suivi

Une page web ne tourne pas en arrière-plan : fermée, elle n'enregistre rien. Le routeur
publie donc lui-même ses **24 dernières heures** (au pas de 15 minutes) dans un message MQTT
**retenu**. Les courbes 1 h / 6 h / 24 h de l'[application](https://jeremyper.github.io/Routeur_f1atb/)
sont là dès l'ouverture, même si le téléphone n'a pas été allumé depuis deux jours.

L'application calcule aussi ce que le routeur ne publie pas : part du surplus réellement
valorisée, prix du kWh évité déduit de vos économies, gain et coût en €/h, vitesse de
chauffe du ballon et détection de puisage.

> Nouveau sujet MQTT : `<préfixe>/<appareil>/Historique`. Si vous aviez restreint à la main
> les droits de vos accès HiveMQ sujet par sujet, autorisez-le (en écriture pour le routeur,
> en lecture pour le téléphone). Avec les droits du [guide](https://github.com/jeremyper/Routeur_f1atb/blob/main/SUPERVISION_DISTANTE.md),
> il n'y a rien à faire.

## 🏠 Accueil : kWh d'eau chaude

La tuile **Eau chaude** affiche l'énergie consommée par le chauffe-eau dans la journée, et
la part venue du soleil — par exemple *« 4,8 kWh aujourd'hui · dont 3,1 solaires »*. Le
total nécessite la seconde voie de mesure (source UxIx2, JSY-MK-194T).

---

## Installation

Le binaire `Routeur_Soleo_v1.28.bin` est attaché à cette release. Deux voies :

- **Sans USB** — page **Mise à jour** du routeur, qui propose directement cette version.
- **Par USB** — voir le [guide d'installation](https://github.com/jeremyper/Routeur_f1atb/blob/main/INSTALLATION.md).

Exportez votre configuration (page **Import / Export**) avant de flasher.

> ⚠️ **Fork personnel, sans garantie.** Testé sur une seule installation, avec un périmètre
> matériel réduit (sources UxI, UxIx2, UxIx3 ; pas d'écran ni de carte Ethernet). Ce n'est
> pas un remplacement du firmware officiel **[F1ATB](https://github.com/F1ATB/Solar-Router-F1ATB)**,
> dont ce projet est un fork de la V17.16.
