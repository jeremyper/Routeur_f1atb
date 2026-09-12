# v1.26 — Supervision à distance, application web, et fiabilité réseau

Première version stable depuis la **v1.10**. Les v1.20-beta et beta2 n'étaient que des
préversions, et contenaient encore une page Réglages tronquée : cette release les
remplace toutes les deux.

Deux nouveautés majeures, et la correction du manque de mémoire qui empêchait la
**prévision météo** et le **tarif Tempo** de fonctionner.

---

# 🌍 Nouveau — Suivre son routeur depuis l'extérieur

**Sans ouvrir de port sur votre box, sans Home Assistant, sans serveur à installer.**

Votre box bloque les connexions qui entrent, mais laisse passer celles qui sortent. Les
objets connectés du commerce exploitent cette asymétrie ; le routeur fait désormais
pareil : il se connecte lui-même à un service de messagerie (**broker MQTT**) et y dépose
ses mesures. Votre téléphone va simplement les y lire.

```
   Routeur  ──── dépose ────►  Broker (HiveMQ)  ◄──── consulte ────  Téléphone
   (chez vous)                   (internet)                          (n'importe où)
```

Personne n'entre chez vous. Le routeur sort déposer ses données, vous allez les chercher.

**Chacun crée son propre compte** chez HiveMQ — l'offre gratuite suffit largement pour un
routeur. Aucune infrastructure partagée, aucun quota commun, aucun identifiant qui
circule. Je n'héberge rien et n'ai accès à rien.

> **Lecture seule.** En mode Standard, le routeur publie ses mesures mais n'accepte aucune
> commande venue de l'extérieur. Vous surveillez, vous ne pilotez pas. C'est volontaire :
> même si quelqu'un interceptait la liaison, il ne pourrait rien allumer chez vous.
> Le guide fait créer **deux accès distincts** — un en écriture seule pour le routeur, un
> en lecture seule pour le téléphone.

📖 **[Guide pas à pas](SUPERVISION_DISTANTE.md)** — comptez une vingtaine de minutes.

Pour ceux qui ont déjà Home Assistant, rien ne change : la publication MQTT et
l'auto-découverte fonctionnent comme avant, et peuvent coexister avec un broker distant.

# 📱 Nouveau — Application de suivi

**[jeremyper.github.io/Routeur_f1atb](https://jeremyper.github.io/Routeur_f1atb/)**

Une page web installable sur téléphone, qui se connecte à **votre** broker. Vos
identifiants restent sur votre appareil : ils ne sont envoyés qu'à votre propre broker,
jamais ailleurs. Rien n'est hébergé de mon côté.

- Synoptique du flux d'énergie en direct — quatre anneaux autour d'un moyeu, le sens du
  courant se lit au défilement des pointillés
- Production, réseau, maison, eau chaude, tarif Tempo, économies du mois
- Thème clair / sombre, fonctionne hors ligne une fois installée

Installation : bouton **⬇️** dans l'application, ou *menu → Ajouter à l'écran d'accueil*.
Un **diagnostic intégré** explique point par point ce qui bloque, le cas échéant.

---

# 🔧 Corrections

## Prévision météo et tarif Tempo : ils fonctionnent enfin

Les appels vers RTE et Open-Meteo échouaient — silencieusement pour la météo. La cause
était le manque de mémoire, mais elle a mis du temps à se laisser nommer : c'est le code
d'erreur de mbedTLS qui a tranché, après plusieurs fausses pistes.

Une négociation TLS réclame entre 55 et 90 Ko de tas sur un ESP32, et la session
maintenue vers le broker en retient déjà une cinquantaine. Les deux ne tiennent pas
ensemble. Le routeur **libère donc sa session MQTT juste avant un appel HTTPS sortant**,
puis la rétablit ; une fenêtre de dix secondes empêche entre-temps toute reconnexion,
sinon la poignée de main du broker refragmente le tas et l'appel suivant échoue à son tour.

**Ce que vous verrez :** le broker perd sa connexion deux à trois fois par jour, une
dizaine de secondes. La déconnexion est propre — le *Last Will* ne se déclenche pas,
Home Assistant ne voit pas le routeur passer indisponible.

Le plancher de mémoire libre passe de **488 octets à plus de 16 Ko**.

## Page Données brutes illisible

Le tableau s'affichait en texte pâle sur fond pâle avec le thème sombre : la feuille de
style forçait la couleur du texte et écrasait celle de la palette. Les cellules en
héritent de nouveau, la palette par défaut des tableaux passe au sombre, et les cellules
d'unité (W, Wh, Hz…) reçoivent automatiquement un texte contrasté avec leur fond, quelle
que soit la couleur choisie.

Votre palette enregistrée bascule toute seule **si vous n'aviez jamais touché aux
couleurs des tableaux** ; sinon les vôtres sont conservées.

## Tableau de bord : il dit ce qui cloche

L'accueil restait sur « Connexion… » indéfiniment, sans distinguer un routeur injoignable,
un refus d'accès ou une réponse tronquée. Il nomme maintenant la cause après trois échecs.
Il n'affiche par ailleurs plus « Tout est calme » à partir de puissances à zéro tant
qu'aucune donnée n'est arrivée — il affirmait sans rien savoir.

## Diagnostic

Les messages d'échec réseau indiquent le tas libre, le plus gros bloc contigu et le code
d'erreur mbedTLS. Une sonde signale toute opération qui abaisse le plancher mémoire, et
reste silencieuse sinon.

---

## Installation

Le binaire `Routeur_Soleo_v1.26.bin` est attaché à cette release. Deux voies :

- **Sans USB** — page **Mise à jour** du routeur, qui propose directement cette version.
- **Par USB** — voir le [guide d'installation](INSTALLATION.md).

Exportez votre configuration (page **Import / Export**) avant de flasher.

> ⚠️ **Fork personnel, sans garantie.** Testé sur une seule installation, avec un
> périmètre matériel réduit (sources UxI, UxIx2, UxIx3 uniquement). Ce n'est pas un
> remplacement du firmware officiel **[F1ATB](https://github.com/F1ATB/Solar-Router-F1ATB)**,
> dont ce projet est un fork de la V17.16.
