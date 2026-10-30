# v1.30 — Prévision juste, bilans mesurés, anti-légionelle qui sait s'arrêter

Cette version part d'un audit d'une installation réelle (3 kWc, ballon 300 L, une sonde au
tiers bas, onduleur SMA) sur deux semaines d'octobre. Plusieurs calculs se trompaient dans le
même sens : le routeur chauffait la nuit au réseau ce que le soleil aurait fait le lendemain.

---

## ☀️ Prévision solaire : le rayonnement sur les panneaux

La prévision partait du rayonnement **horizontal**. En automne, le soleil bas frappe bien mieux
un panneau incliné que le sol : la production réelle dépassait la prévision de 35 % en
médiane, jusqu'à 80 %.

- Le routeur demande désormais à Open-Meteo le rayonnement **sur le plan des panneaux**,
  heure par heure.
- Deux nouveaux réglages dans **Réglages** → Prévision solaire : **Inclinaison** (30° par
  défaut) et **Orientation** (0 = plein sud, −90 = est, 90 = ouest). Avec un relevé à la
  boussole (sud = 180°), retirez 180 : 223° donne 43.
- Sur l'installation de l'audit, le 30/10 : 7,6 kWh prévus avant, **12,2 kWh** maintenant,
  pour 11,25 kWh produits.

> ⚠️ Les prévisions affichées augmentent en automne et en hiver. Si vous utilisez une
> condition « prévision < seuil » ou « ≥ seuil », revoyez le seuil.

## 🚿 Bilans du ballon mesurés, plus estimés

- **Soleil envoyé au ballon et € économisés** : calcul continu, mesure par mesure
  (puissance du ballon − puissance soutirée). Avant, rien n'était compté dès que la maison
  soutirait plus de 20 W ; comme la régulation vise 0 W, la moitié environ du soleil envoyé
  au ballon était oubliée (1,7 kWh annoncés pour ~4 kWh le 30/10).
- **Coefficient routable** : appris sur le surplus réellement disponible (soleil reçu par le
  ballon + réinjection) rapporté à la production. Avant, il prenait le temps d'ouverture de
  la sortie × la puissance de la résistance, ce qui comptait la marche forcée au réseau et
  les heures où le thermostat avait coupé : 9,3 kWh « envoyés au ballon » pour 4,7 mesurés,
  et un coefficient passé de 33 à 70 % en 3 jours.
- **Journal** : « Bilan du jour : X kWh de soleil dans le ballon, Y kWh réinjectés, sur
  Z kWh produits ».
- Sans voie T (source à une seule mesure), la puissance du ballon est estimée par
  l'ouverture de la sortie.

## 🧠 Chauffe prédictive : ne chauffe plus tout le ballon pour rien

La chauffe prédictive comptait ce qui manque d'eau **utile** (au-dessus de la température
minimale), mais pas l'énergie qu'il faut d'abord pour remonter l'eau froide jusque-là. Tant
que le bas restait sous 40 °C, le besoin ne baissait pas : la marche forcée de nuit chauffait
tout le ballon, même avec du soleil prévu. Exemple réel : 7,5 kWh pris au réseau entre 2 h et
5 h 30, puis 3,5 kWh réinjectés l'après-midi, ballon plein.

- Le besoin compte maintenant la montée jusqu'à la température minimale, et ne dépasse
  jamais celui du mode standard : le mode intelligent ne chauffe jamais plus que lui.
- Même nuit recalculée : environ 1 kWh de forçage, le soleil fait le reste.
- Le journal « besoin détecté » n'oscille plus (7 lignes en 2 h le 28/10).

## 🦠 Anti-légionelle : elle sait s'arrêter

La chauffe de sécurité ne s'arrêtait qu'à la température souhaitée. Si la sonde ne peut pas
l'atteindre (thermostat du ballon, sonde dans une zone plus froide), le ballon restait en
marche forcée jour et nuit : plus de routage solaire, et le bouton Arrêter n'y pouvait rien.

- Elle s'arrête aussi quand le **thermostat du ballon a coupé** : sortie grande ouverte,
  ballon à 0 W pendant 15 min (mesuré sur la voie T). Le journal indique la température de la
  sonde à ce moment : c'est la bonne valeur pour la **température souhaitée**.
- Elle s'arrête au plus tard après **8 h**, avec un message au journal.
- La surveillance s'applique aussi dès que le chauffe-eau a une **marche forcée adaptative**
  (condition météo « Adaptatif »), qui peut garder le ballon tiède comme le mode intelligent.

## ⚡ Marche forcée sous condition : le routage continue

Une période **ON** dont les conditions n'étaient pas remplies **arrêtait** l'action. Une
marche forcée conditionnelle en plein jour coupait donc le routage solaire.

- Sur une sortie **Multi-sinus** ou **Train de sinus**, la période régule désormais le
  surplus quand ses conditions ne sont pas remplies.
- Exemple : heures creuses de 12 h 30 à 14 h 30, **ON si sonde < 35 °C, jusqu'à 42 °C** : les
  jours gris, le ballon rattrape au tarif creux ; les jours de soleil, il route le surplus.
- Les relais On/Off et les actions à distance gardent l'ancien comportement.
- Chaque période garde son propre état d'hystérésis.

## 🔧 Correctifs

- **Onduleur SMA** : une réponse Modbus arrivée en retard pouvait être prise pour la
  suivante ; la production du jour avait affiché **14 707 kWh** le 26/10. Chaque requête porte
  maintenant son identifiant, la réponse doit être complète, et un compteur total qui recule
  ou bondit est écarté. À minuit, sans relevé valide depuis le démarrage, le compteur du jour
  se recale sur celui de l'onduleur au lieu de compter tout le compteur total.
- **Actions inactives** : une action inactive laissée sur le GPIO du chauffe-eau (héritage
  F1ATB) remettait cette sortie à l'arrêt toutes les 200 ms ; le relais statique perdait des
  demi-alternances. Elle n'y touche plus.
- **Mesures absentes** : quand le compteur (JSY) ne répond plus, le routeur redémarrait toutes
  les 3 minutes, indéfiniment (330 fois en 16 h le 17/10). Après 3 essais, il reste en marche
  — page web accessible, régulation fermée — et retente toutes les 30 minutes. Le journal
  conseille de couper l'alimentation, seul remède quand le JSY est bloqué.

---

## Après la mise à jour

1. **Réglages** → Prévision solaire : renseignez l'inclinaison et l'orientation.
2. Vérifiez la **température souhaitée** : elle doit être atteignable **sur la sonde du bas**
   quand le thermostat du ballon coupe.

Binaire : `Routeur_Soleo_v1.30.bin` (attaché automatiquement par la compilation).
