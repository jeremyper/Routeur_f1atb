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
   Routeur  ──── dépose ────►  Broker (EMQX)    ◄──── consulte ────  Téléphone
   (chez vous)                   (internet)                          (n'importe où)
```

> **Lecture seule.** En mode Standard, le routeur publie ses mesures mais n'accepte
> aucune commande venue de l'extérieur. Vous pouvez surveiller, pas piloter. C'est
> volontaire : même si quelqu'un interceptait la liaison, il ne pourrait rien allumer
> chez vous.

---

## Étape 1 — Créer un compte EMQX Cloud

EMQX propose une offre **Serverless** gratuite, largement suffisante pour un routeur :
1 million de minutes de connexion et 1 Go de trafic par mois, soit une vingtaine
d'appareils connectés en permanence. **Chacun crée son propre compte** : vos données ne
transitent par aucune infrastructure partagée.

1. Rendez-vous sur **https://cloud.emqx.com** et créez un compte
2. Créez un déploiement **Serverless**
3. Sélectionnez une région proche (Europe, par exemple Francfort)
4. **Laissez la limite de dépense mensuelle à 0** : vous restez dans le quota gratuit, et
   si par extraordinaire il était dépassé, le service s'arrête au lieu de vous facturer

La création prend une minute.

> **Vous utilisiez HiveMQ Cloud ?** Son offre gratuite (Serverless) n'est plus proposée
> depuis le 30 septembre 2026, les clusters existants cessent de fonctionner le
> 31 décembre 2026 et sont supprimés le 7 janvier 2027. Suivez ce guide puis remplacez,
> dans le routeur et sur le téléphone, l'adresse et les identifiants : rien d'autre ne
> change.

## Étape 2 — Relever l'adresse du déploiement

La page **Overview** du déploiement affiche son adresse, de la forme :

```
a1b2c3d4.ala.eu-central-1.emqxsl.com
```

Notez-la, ainsi que les deux ports :

| Port | Pour quoi |
|---|---|
| **8883** | MQTT chiffré : le routeur et les applications MQTT du téléphone |
| **8084** | MQTT par WebSocket chiffré : l'application web Soleo dans un navigateur |

> Cette adresse est un **nom d'hôte**, pas une adresse IP. C'est pour cela que le routeur
> dispose d'un champ distinct : le champ « adresse IP » historique ne peut pas la contenir.

## Étape 3 — Créer deux accès distincts

C'est le point qui protège votre installation. Dans **Access Control → Authentication**,
créez **deux** jeux d'identifiants :

| Nom suggéré | Pour qui |
|---|---|
| `routeur` | Le routeur, qui dépose ses mesures |
| `telephone` | Votre téléphone, qui les lit |

Choisissez des mots de passe différents et notez-les.

Puis, dans **Access Control → Authorization**, limitez chaque accès à son rôle :

| Identifiant | Topic | Action | Autoriser |
|---|---|---|---|
| `telephone` | `#` | Publish | **Deny** |
| `routeur` | `#` | Subscribe | **Deny** |

**Pourquoi deux ?** L'identifiant du téléphone sera saisi dans une application, donc plus
exposé. Privé du droit de publier, quelqu'un qui le récupérerait pourrait voir votre
consommation — sans jamais pouvoir publier de fausses mesures ni perturber le routeur.

> Les intitulés de la console EMQX évoluent : si la rubrique *Authorization* n'apparaît
> pas sous ce nom, cherchez les règles d'accès (ACL) de votre déploiement.

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
| Serveur | l'adresse du déploiement |
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
- Le port est-il bien `8883` et le TLS activé ? EMQX Serverless n'accepte aucune connexion en clair.
- Dans l'application web Soleo (navigateur), le port est `8084`, pas `8883` : un navigateur
  ne parle MQTT qu'à travers un WebSocket.
- Le champ « adresse IP » doit rester vide : renseigné, il prendrait le pas.

**Le service s'est arrêté en cours de mois**
Le quota gratuit est épuisé (cas très improbable avec un seul routeur). La console EMQX
l'indique ; le service reprend le mois suivant. Une période d'envoi de 10 s ou plus
ménage le trafic.

**Le routeur ne se connecte pas**
Consultez le journal du routeur en Telnet. Le message `connecté au broker MQTT` confirme
la connexion ; son absence répétée indique un problème d'identifiants ou d'adresse.

**Vérifier le compte plutôt que le routeur**
La page **Overview** du déploiement EMQX affiche le nombre de clients connectés. Si le routeur y apparaît, le
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
