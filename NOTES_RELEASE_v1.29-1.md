# v1.29-1 — Boutons de l'accueil sur le bon chauffe-eau

Correctif de la v1.29, même numéro de version firmware (1.29).

## 🚿 Accueil : Forcer / Arrêter pilotent enfin le chauffe-eau

Les boutons **Forcer** et **Arrêter** de la tuile Eau chaude, et l'état « En chauffe »,
visaient toujours l'**action 0**. Chez qui a gardé en action 0 une action inactive héritée
de F1ATB (par exemple « Titre Triac ») avec le chauffe-eau plus loin dans la liste, les
boutons n'avaient aucun effet et la tuile n'affichait jamais la chauffe.

Le routeur désigne maintenant lui-même l'action du chauffe-eau, avec la même règle que la
sécurité anti-légionelle : l'action liée à une sonde du ballon, sinon la première action
en Multi-sinus ou Train de sinus. L'action 0 ne sert plus qu'à défaut.

## 🗑️ L'action 0 peut être supprimée

L'action 0 n'a plus de rôle particulier : le bouton 🗑 la supprime comme les autres, après
confirmation. Seule la **dernière** action restante ne peut pas être supprimée. La
protection contre un effacement **involontaire** (titre vide, broche « pas choisi ou
supprimer ») reste en place pour l'action 0.

---

Binaire : `Routeur_Soleo_v1.29-1.bin` (attaché automatiquement par la compilation).
