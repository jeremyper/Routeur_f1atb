# Suivre son routeur depuis l'extérieur

Sans ouvrir de port sur votre box, sans Home Assistant, sans serveur à installer.
Comptez une vingtaine de minutes.

## Le principe

Votre box laisse passer sans difficulté les connexions qui **partent** de votre réseau —
c'est ce qui permet à un téléphone de consulter un site web. En revanche elle bloque
celles qui **entrent**, sauf configuration explicite.

Les objets connectés du commerce exploitent cette asymétrie : ils ouvrent eux-mêmes une
connexion vers l'extérieur et la maintiennent ouverte. Le routeur fait pareil : il se
connecte à un service de messagerie appelé **broker MQTT**, et y dépose ses mesures.
Votre téléphone va simplement les y lire.

Personne n'entre chez vous. Le routeur sort déposer ses données, vous allez les chercher.

```
   Routeur  ──── dépose ────►  Broker (HiveMQ)  ◄──── consulte ────  Téléphone
   (chez vous)                   (internet)                          (n'importe où)
```

> **Lecture seule.** En mode Standard, le routeur publie ses mesures mais n'accepte
> aucune commande venue de l'extérieur. Vous pouvez surveiller, pas piloter. C'est
> volontaire : même si quelqu'un interceptait la liaison, il ne pourrait rien allumer
> chez vous.

---

## Étape 1 — Créer un compte HiveMQ Cloud

HiveMQ propose une offre gratuite largement suffisante pour un routeur.
**Chacun crée son propre compte** : vos données ne transitent par aucune infrastructure
partagée.

1. Rendez-vous sur **https://www.hivemq.com/cloud/** et créez un compte
2. Créez un cluster en choisissant l'offre **gratuite**
3. Sélectionnez une région proche (Europe)

La création prend une minute.

## Étape 2 — Relever l'adresse du cluster

Une fois le cluster créé, sa page affiche son adresse, de la forme :

```
a1b2c3d4e5f6g7h8.s1.eu.hivemq.cloud
```

Notez-la, ainsi que le port **8883** (le port des connexions chiffrées).

> Cette adresse est un **nom d'hôte**, pas une adresse IP. C'est pour cela que le routeur
> dispose d'un champ distinct : le champ « adresse IP » historique ne peut pas la contenir.

## Étape 3 — Créer deux accès distincts

C'est le point qui protège votre installation. Dans la gestion des accès de votre cluster
(rubrique *Access Management* ou équivalent), créez **deux** jeux d'identifiants :

| Nom suggéré | Permission | Pour qui |
|---|---|---|
| `routeur` | **Publication** seule | Le routeur, qui dépose ses mesures |
| `telephone` | **Souscription** seule | Votre téléphone, qui les lit |

Choisissez des mots de passe différents et notez-les.

**Pourquoi deux ?** L'identifiant du téléphone sera saisi dans une application, donc plus
exposé. Avec une permission de lecture seule, quelqu'un qui le récupérerait pourrait voir
votre consommation — sans jamais pouvoir publier de fausses mesures ni perturber le
routeur.

## Étape 4 — Configurer le routeur

Sur la page **Réglages** du routeur, ouvrez **🌐 Réseau & domotique** :

| Champ | Valeur |
|---|---|
| Serveur domotique (adresse IP) | *laisser vide* |
| Broker distant (nom d'hôte) | l'adresse relevée à l'étape 2 |
| Connexion chiffrée (TLS) | **activée** |
| Port MQTT | `8883` |
| Identifiant de connexion | `routeur` |
| Mot de passe | celui de l'accès publication |
| Période d'envoi | `10` (secondes) |

⚠️ **La période d'envoi doit être supérieure à zéro**, sinon le routeur ne publie rien.
C'est l'oubli le plus fréquent.

Enregistrez. Le routeur redémarre et se connecte.

## Étape 5 — Lire les mesures sur le téléphone

Installez une application MQTT — il en existe plusieurs gratuites sur Android et iOS,
cherchez « MQTT dashboard » ou « MQTT panel ».

Configurez la connexion :

| Réglage | Valeur |
|---|---|
| Serveur | l'adresse du cluster |
| Port | `8883` |
| TLS / SSL | activé |
| Identifiant | `telephone` |
| Mot de passe | celui de l'accès souscription |

Puis abonnez-vous au topic :

```
homeassistant/routeur_rms_state
```

> Si vous avez personnalisé le préfixe ou le nom d'appareil dans les réglages MQTT,
> adaptez : le topic vaut `<préfixe d'état>/<nom d'appareil>_state`.

### Ce que vous recevrez

**Un seul message**, contenant toutes les mesures au format JSON :

```json
{
  "Puissance_M_Soutiree": 1240,
  "Puissance_M_Injectee": 0,
  "EnergieJour_M_Soutiree": 4820,
  "Temperature_0": 51.9,
  "Ouverture_Triac": 62,
  "Economie_Jour": 1.34,
  "Code_Tarifaire": "BLEU HP",
  "ESP32_On": 137.4
}
```

C'est important pour la configuration : ce n'est **pas** un topic par mesure. Dans votre
application, créez un afficheur par valeur en pointant le même topic et en indiquant la
clé JSON à extraire. La plupart des applications appellent cela *JSON path* ou *clé*.

Un second topic indique si le routeur est joignable :

```
homeassistant/routeur_rms/Available   →   "online" ou "offline"
```

Si le routeur perd le courant ou le réseau, le broker publie automatiquement `offline` à
sa place. Vous savez donc distinguer « tout va bien, rien ne bouge » de « le routeur ne
répond plus ».

---

## En cas de problème

**Rien n'arrive sur le téléphone**
- La période d'envoi est-elle bien supérieure à zéro ?
- Le port est-il bien `8883` et le TLS activé ? HiveMQ n'accepte aucune connexion en clair.
- Le champ « adresse IP » doit rester vide : renseigné, il prendrait le pas.

**Le routeur ne se connecte pas**
Consultez le journal du routeur en Telnet. Le message `connecté au broker MQTT` confirme
la connexion ; son absence répétée indique un problème d'identifiants ou d'adresse.

**Vérifier le compte plutôt que le routeur**
La console HiveMQ affiche le nombre de clients connectés. Si le routeur y apparaît, le
problème vient de la configuration du téléphone, pas du routeur.

---

## Ce que cette solution ne fait pas

- **Pas d'interface web à distance.** Vous recevez des mesures, pas les pages du routeur.
  Les graphiques, les Réglages et l'éditeur d'automatismes restent accessibles uniquement
  depuis chez vous.
- **Pas de pilotage.** Lecture seule en mode Standard, délibérément.
- **Pas d'historique.** Le broker ne conserve que la dernière valeur ; il ne constitue pas
  une base de données.

Si vous voulez l'interface complète à distance, la réponse n'est pas MQTT mais un réseau
privé virtuel (Tailscale, WireGuard) installé sur une machine allumée en permanence chez
vous. C'est une autre démarche, sans modification du routeur.
