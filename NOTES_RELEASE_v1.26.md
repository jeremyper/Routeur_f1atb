# v1.26 — Fiabilité réseau et lisibilité

Cette version corrige deux défauts qui touchaient toutes les installations, et surtout
le manque de mémoire qui empêchait la **prévision météo** et le **tarif Tempo** de
fonctionner. Sur mon installation, la prévision solaire remonte pour la première fois.

> ⚠️ **Mise à jour recommandée** si vous êtes en v1.20-beta ou v1.20-beta2 : ces
> préversions contiennent une page Réglages tronquée et n'ont jamais reçu les correctifs
> réseau ci-dessous.

## Prévision météo et tarif Tempo : ils fonctionnent enfin

Les appels vers RTE et Open-Meteo échouaient, silencieusement pour la météo. La cause
était le manque de mémoire, mais elle a mis du temps à se laisser nommer : c'est le code
d'erreur de mbedTLS qui a tranché.

Une négociation TLS réclame entre 55 et 90 Ko de tas sur un ESP32, et la session
maintenue vers le broker MQTT en retient déjà une cinquantaine. Les deux ne tiennent pas
ensemble. Le routeur **libère désormais sa session MQTT juste avant un appel HTTPS
sortant**, puis la rétablit ; une fenêtre de dix secondes empêche entre-temps toute
reconnexion, sinon la poignée de main du broker refragmente le tas et l'appel suivant
échoue à son tour.

**Ce que vous verrez :** le broker perd sa connexion deux à trois fois par jour, une
dizaine de secondes. La déconnexion est propre — le *Last Will* ne se déclenche pas,
Home Assistant ne voit pas le routeur passer indisponible.

Le plancher de mémoire libre passe de **488 octets à plus de 16 Ko**.

## Page Données brutes illisible

Le tableau s'affichait en texte pâle sur fond pâle avec le thème sombre : la feuille de
style forçait la couleur du texte et écrasait celle de la palette. Les cellules en
héritent de nouveau, la palette par défaut des tableaux passe au sombre, et les cellules
d'unité (W, Wh, Hz…) reçoivent automatiquement un texte contrasté avec leur fond, quelle
que soit la couleur que vous choisissez.

Votre palette enregistrée bascule toute seule **si vous n'aviez jamais touché aux
couleurs des tableaux** ; sinon les vôtres sont conservées.

## Tableau de bord : il dit ce qui cloche

L'accueil restait sur « Connexion… » indéfiniment sans distinguer un routeur injoignable,
un refus d'accès ou une réponse tronquée. Il nomme maintenant la cause après trois échecs.
Il n'affiche par ailleurs plus « Tout est calme » à partir de puissances à zéro tant
qu'aucune donnée n'est arrivée — il affirmait sans rien savoir.

## Diagnostic

Les messages d'échec réseau indiquent le tas libre, le plus gros bloc contigu et le code
d'erreur mbedTLS. Une sonde signale toute opération qui abaisse le plancher mémoire, et
reste silencieuse sinon.

## Application de suivi

Le synoptique de [l'application web](https://jeremyper.github.io/Routeur_f1atb/) est
refait : quatre anneaux à icône autour d'un moyeu, valeurs à l'extérieur, sens du courant
visible au défilement. L'installation sur iPhone est corrigée (icônes PNG et
`apple-mobile-web-app-capable` manquaient, d'où un simple raccourci au lieu d'une
application autonome).

---

## Installation

Le binaire `Routeur_Soleo_v1.26.bin` est attaché à cette release. Deux voies :

- **Sans USB** — page **Mise à jour** du routeur, qui propose directement cette version.
- **Par USB** — voir le [guide d'installation](INSTALLATION.md).

Exportez votre configuration (page **Import / Export**) avant de flasher.
