# v1.29 — Deux sondes sur le ballon, recalibrage, suppression des automatismes

Cette version affine le ballon intelligent avec une seconde sonde facultative en haut du
ballon. Elle ajoute aussi un assistant pour recalibrer les sondes de température et rend
enfin possible la suppression d'un automatisme depuis un téléphone.

---

## 🚿 Ballon : une sonde en bas, une sonde en haut

Une seule sonde ne peut pas tout faire. En bas, elle voit arriver l'eau froide (puisages) et
dit quand tout le ballon est chaud (anti-légionelle). En revanche, elle sous-estime la
réserve : le haut reste chaud bien après que le bas a refroidi.

- Nouveau réglage **Sonde du haut du ballon (facultative)** dans **Réglages** → L'essentiel →
  Ballon. L'ancien réglage devient **Sonde du bas du ballon**.
- Le routeur voit le ballon comme deux moitiés : la basse à la température du bas, la haute
  à celle du haut. Chacune compte à part : un bas froid n'efface plus l'eau chaude du haut.
  La réserve et la consommation apprise sont plus justes, et la chauffe prédictive
  entretient au plus près du besoin.
- La détection des puisages et l'anti-légionelle **restent sur la sonde du bas**.
- **Sans sonde du haut, rien ne change** : les calculs sont exactement ceux de la v1.28.
- **Tableau de bord** : la tuile Eau chaude affiche la température du haut (celle qui sort
  au robinet), avec celle du bas en petit. La jauge suit la moyenne des deux.
- **Diagnostic** (Réglages → Ballon) : températures du haut et du bas, avec une alerte si le
  bas reste plus chaud que le haut (sondes probablement inversées).

📐 Le **[README](https://github.com/jeremyper/Routeur_f1atb#placement-des-sondes-du-ballon)**
contient un schéma du ballon : où poser chaque sonde, où ne jamais la poser, et les
réglages recommandés.

| Réglage | Valeur conseillée |
|---|---|
| Température souhaitée | 55 à 60 °C |
| Température minimale utile | 40 °C |
| Chauffe anti-légionelle | 7 jours, jamais 0 avec la chauffe prédictive |

> Si vous ajoutez une sonde du haut, la consommation moyenne déjà apprise va baisser
> doucement pendant environ 5 jours : avec une seule sonde, elle était surestimée.

## 🌡️ Recalibrer une sonde de température

Une sonde DS18B20 peut se tromper d'un demi-degré, ce qui fausse l'écart haut/bas et le
seuil de la température souhaitée.

- Sous chaque sonde DS18B20 : **Recalibrer avec un thermomètre**. Mettez la sonde et un
  thermomètre au même endroit (un verre d'eau), saisissez la température lue, puis
  **Calculer** : le routeur relit sa mesure et propose la correction. Enregistrez :
  elle s'applique immédiatement, sans redémarrage.
- La **correction de mesure** accepte désormais les dixièmes de degré. Avant, le champ
  n'acceptait que des degrés entiers, et une valeur comme 0,29 °C était enregistrée 0,28 °C.

## 🗑️ Supprimer un automatisme

Sur la page **Actions**, le bouton 🗑 ouvrait l'éditeur sans rien supprimer. La vraie
méthode (effacer le titre) n'était indiquée que dans une infobulle, invisible au doigt.

- Le bouton 🗑 **supprime vraiment** l'automatisme, après confirmation.
- Un bouton **Supprimer cette action** sous le titre de chaque action, dans l'éditeur.
- L'**action 0** pilote le chauffe-eau (boutons Forcer et Arrêter de l'accueil) et reste
  protégée. Le bouton 🗑 explique maintenant pourquoi, et comment la passer en mode
  **Inactif**.
- Une sauvegarde qui échoue (mot de passe refusé, routeur injoignable) affiche enfin un
  message. Avant, rien ne s'affichait et on croyait avoir enregistré.
- Supprimer une action décale le numéro des suivantes. Une condition « état d'une action »
  restait sur l'ancien numéro et visait donc **la mauvaise action**. Les conditions suivent
  maintenant la renumérotation, et celle qui visait l'action supprimée est retirée.

> ⚠️ Si vous avez déjà supprimé une action alors qu'une autre en dépendait (condition
> « état d'une action »), vérifiez cette condition : elle vise peut-être la mauvaise action.

## 📡 Supervision à distance : EMQX Cloud remplace HiveMQ

L'offre gratuite HiveMQ Cloud n'est plus proposée, et les clusters existants s'arrêtent le
31/12/2026. Le [guide](https://github.com/jeremyper/Routeur_f1atb/blob/main/SUPERVISION_DISTANTE.md)
passe à **EMQX Cloud Serverless**, avec une note de migration. L'application web propose le
port WebSocket 8084 par défaut.

---

## Installation

Le binaire `Routeur_Soleo_v1.29.bin` est attaché à cette release. Deux voies :

- **Sans USB** — page **Mise à jour** du routeur, qui propose directement cette version.
- **Par USB** — voir le [guide d'installation](https://github.com/jeremyper/Routeur_f1atb/blob/main/INSTALLATION.md).

Exportez votre configuration (page **Import / Export**) avant de flasher. Aucun réglage n'est
à refaire : la sonde du haut est facultative et désactivée par défaut.

> ⚠️ **Fork personnel, sans garantie.** Testé sur une seule installation, avec un périmètre
> matériel réduit (sources UxI, UxIx2, UxIx3 ; pas d'écran ni de carte Ethernet). Ce n'est
> pas un remplacement du firmware officiel [F1ATB](https://github.com/F1ATB/Solar-Router-F1ATB),
> dont ce projet est un fork de la V17.16.
