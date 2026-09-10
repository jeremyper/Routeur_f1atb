# Installation — guide pas à pas

Ce firmware **remplace** le firmware officiel F1ATB sur votre ESP32. Ce n'est pas un
module à ajouter : on flashe l'un *à la place* de l'autre. Votre carte, votre câblage et
vos sondes ne changent pas.

Comptez 20 minutes la première fois.

---

## Étape 0 — Vérifier que c'est compatible avec votre installation

**C'est l'étape à ne pas sauter.** Ce fork gère moins de matériel que le firmware officiel.

Ouvrez la page **Réglages** de votre routeur actuel et regardez comment il mesure la
puissance :

| Votre source de mesure | Compatible ? |
|---|---|
| **UxI** (transformateur + pince ampèremétrique) | ✅ oui |
| **UxIx2** (module JSY-MK-194T) | ✅ oui |
| **UxIx3** (module JSY-MK-333, triphasé) | ✅ oui |
| Linky (téléinformation) | ❌ non |
| Enphase Envoy | ❌ non |
| Shelly EM / Shelly Pro EM | ❌ non |
| SmartGateways, HomeWizard | ❌ non |
| Puissance reçue par MQTT | ❌ non |
| ESP externe (RMS distant) | ❌ non |

Sont également retirés : les **écrans LCD tactiles** (Sunton / CYD) et la carte
**Ethernet WT32-ETH01**. Seules les LED d'état restent gérées.

> Si votre ligne est en ❌, **restez sur le firmware officiel**. Le routeur ne saurait
> plus lire votre consommation, et vous devriez tout refaire en sens inverse.

---

## Étape 1 — Sauvegarder votre configuration

Sur votre routeur actuel : page **Réglages → Import / Export**, bouton
**⬇ Télécharger** en face de `parametres.json`.

Rangez ce fichier quelque part de sûr. Il contient vos actions, vos plages horaires, vos
réglages réseau — tout ce que vous avez patiemment paramétré.

**Ne sautez pas cette étape.** C'est votre seul filet.

---

## Étape 2 — Obtenir le firmware

### Voie A — binaire prêt à l'emploi (recommandée)

Rendez-vous sur la page [**Releases**](https://github.com/jeremyper/Routeur_f1atb/releases)
du dépôt et téléchargez le fichier `.bin` de la dernière version.

C'est tout. Passez à l'étape 3.

### Voie B — compiler soi-même

Si aucune release n'est encore publiée, ou si vous voulez modifier le code :

1. Téléchargez le dépôt : bouton vert **Code → Download ZIP**, puis décompressez.
2. **Renommez le dossier** en `Solar_Router_V17_16`.
   Arduino IDE exige que le dossier porte le nom du fichier principal. Si l'IDE vous
   propose de « créer un dossier » à l'ouverture, **refusez** : il ne déplacerait qu'un
   fichier sur la trentaine du projet.
3. Ouvrez `Solar_Router_V17_16.ino`. Tous les onglets doivent apparaître en haut.
4. Dans **Outils** :
   - Type de carte : **ESP32 Dev Module**
   - Partition Scheme : **Custom** ← indispensable, sinon la compilation échoue à 129 %
   - Flash Size : **4MB**
5. Installez les bibliothèques **ArduinoJson**, **PubSubClient**, **DallasTemperature**.
   ⚠️ N'installez **pas** OneWire : une version spécifique est fournie dans le dépôt.
6. Cliquez sur **Vérifier** (✓). Si ça compile, vous êtes bon.
7. **Croquis → Exporter les binaires compilés**. Le fichier apparaît dans
   `build/esp32.esp32.esp32/` sous le nom `Solar_Router_V17_16.ino.bin`.

> ⚠️ Prenez bien le fichier `.ino.bin`. Le `.ino.merged.bin` contient le chargeur de
> démarrage et échouera à la mise à jour.

Avec « Custom », l'IDE affiche ensuite un pourcentage trompeur (« 10 % … maximum
16777216 ») : c'est normal, il ne sait pas déduire la taille réelle. L'occupation
véritable est d'environ 87 %.

---

## Étape 3 — Installer sur le routeur

### Par le réseau (sans câble)

1. Ouvrez la page **Mise à jour** de votre routeur (bouton dans Réglages, ou `/OTA`).
2. **Choisir un fichier** → sélectionnez votre `.bin`.
3. **⬆ Mettre à jour**. La barre progresse jusqu'à 100 %.
4. Un message confirme la réception et le redémarrage.
5. Patientez ~20 secondes, puis rechargez la page avec **Ctrl+F5**.

Ce `Ctrl+F5` compte : votre navigateur garde en mémoire l'ancienne feuille de style.
Sans vidage du cache, l'affichage peut sembler cassé alors que tout va bien.

### Par USB

Branchez l'ESP32, sélectionnez le port dans **Outils → Port**, puis **Téléverser**.

---

## Étape 4 — Vérifier et restaurer

Après redémarrage, votre configuration est normalement conservée : la mise à jour
n'écrit que la zone programme, pas la zone de données.

Si quelque chose manque, page **Import / Export → Importer** et reprenez le
`parametres.json` de l'étape 1.

Vérifiez ensuite que les mesures de puissance remontent bien sur la page d'accueil.

---

## Revenir au firmware officiel

À tout moment :

1. Téléchargez le binaire officiel sur [f1atb.fr](https://f1atb.fr), section Domotique.
2. Envoyez-le par la même page **Mise à jour**.
3. Réimportez votre `parametres.json` d'origine.

Si le routeur ne répond plus du tout, un flash par **USB** le récupère toujours. C'est
pourquoi il vaut mieux, la toute première fois, faire la manipulation avec l'ESP32
accessible plutôt qu'enfermé dans un tableau électrique.

---

## En cas de problème

- **La compilation échoue à 129 %** → Partition Scheme n'est pas sur *Custom*.
- **L'IDE ne trouve que quelques fichiers** → le dossier n'est pas nommé
  `Solar_Router_V17_16`.
- **L'affichage semble cassé après mise à jour** → `Ctrl+F5`.
- **La mise à jour est refusée** → une clé d'accès est définie ; ouvrez d'abord une page
  Réglages et saisissez-la.
- **Plus aucune mesure de puissance** → votre source n'était probablement pas dans la
  liste de l'étape 0. Revenez au firmware officiel.

Pour tout le reste, ouvrez une
[issue](https://github.com/jeremyper/Routeur_f1atb/issues) sur le dépôt : c'est plus
pratique qu'un fil de commentaires, et ça sert aux suivants.
