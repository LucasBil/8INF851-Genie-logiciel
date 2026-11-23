# Guide utilisateur MSS

Ce guide décrit l'utilisation du programme interactif MSS : les configurations de coûts, la saisie des matrices, l'exécution des opérations et la lecture du bilan. Pour compiler et lancer le programme, consultez le [README principal](README.md) et [EXECUTION.md](EXECUTION.md).

Lancez le programme depuis la racine du dépôt. Les répertoires `configurations/` et `trace/` sont recherchés relativement au répertoire courant.

Les diagnostics de chargement/sauvegarde et les erreurs d'exécution sont consignés dans `logs/mss.log`. Le fichier est créé au besoin; les matrices et leurs valeurs ne sont pas écrites dans le journal.

## Parcours général

Au démarrage, le menu propose :

| Choix | Action |
|---|---|
| `0` | Lancer la démonstration automatique |
| `1` | Charger une ou plusieurs configurations existantes |
| `2` | Créer et sauvegarder une configuration |
| `3` | Entrer une matrice |
| `4` | Choisir et exécuter des opérations |
| `5` | Quitter |

Une configuration doit être chargée ou créée avant la démonstration ou l'exécution. Il est possible de charger plusieurs configurations afin de comparer leurs coûts.

## Configurations et sérialisation

Les configurations sont des fichiers texte du répertoire `configurations/`. Le programme les affiche par ordre alphabétique avec un indice commençant à zéro. Entrez l'indice à charger; répétez pour en charger d'autres, puis entrez `q` pour revenir au menu. Les indices reflètent la liste affichée et peuvent changer si des fichiers sont ajoutés ou retirés.

Pour créer une configuration, entrez un coût entier strictement positif pour chaque opération demandée, puis un nom de fichier simple. Le programme ajoute `.txt` si cette extension n'est pas déjà présente. Les coûts servent à comparer les configurations dans le bilan; ils ne changent pas le résultat mathématique des opérations.

Le format d'un fichier est une opération et son coût par ligne, par exemple :

```text
Add 3
Multiply 8
Clean 1
```

Le nom d'opération doit être reconnu et chaque coût doit être positif. Une configuration doit contenir les coûts des opérations évaluées dans le bilan. Évitez de modifier manuellement les fichiers, sauf si vous respectez ce format.

## Saisie des matrices

Choisissez `3`, entrez une dimension carrée qui est une puissance de deux et au moins égale à 2 (par exemple 2, 4 ou 8), puis saisissez les valeurs entières ligne par ligne. Le programme affiche la matrice et son adresse RAM.

La première matrice est chargée à l'adresse 0 si la RAM simulée est vide. Les matrices créées ensuite occupent une adresse disponible. Les résultats d'opérations restent aussi en RAM et peuvent être proposés lors d'une exécution ultérieure.

## Exécution interactive

Choisissez `4`. Le programme affiche les matrices disponibles avec leurs adresses RAM, puis demande une suite d'opérations. Entrez le numéro d'une opération pour l'ajouter, puis `q` pour terminer la saisie et lancer la suite.

| Numéro | Opération | Entrées et effet |
|---|---|---|
| `1` | `Split` | Sépare une matrice en quatre quadrants : haut-gauche, haut-droit, bas-gauche, bas-droit. |
| `2` | `Fuse` | Réunit quatre matrices de même taille en une matrice plus grande. |
| `3` | `Add` | Additionne deux matrices de même dimension. |
| `4` | `Multiply` | Multiplie deux matrices de même dimension. |
| `5` | `Rotate` | Effectue une rotation horaire. |
| `6` | `Mirror` | Effectue un miroir gauche-droite. |
| `7` | `CalcDeterminant` | Calcule le déterminant; le résultat est une matrice 1×1. |
| `8` | `Clean` | Nettoie les matrices temporaires inutilisées; conserve les matrices et résultats de l'utilisateur. |
| `9` | `Look` | Affiche le contenu d'une adresse RAM indiquée. |
| À venir | `Full_Clean` | Commande prévue pour vider toute la RAM simulée|

Les matrices utilisées par les opérations sont choisies automatiquement parmi celles affichées au début de cette exécution, dans l'ordre croissant de leurs adresses RAM :

- Une opération à une matrice (`Split`, `Rotate`, `Mirror`, `CalcDeterminant`) prend la dernière matrice disponible.
- `Add` et `Multiply` prennent les deux dernières matrices disponibles.
- `Fuse` prend les quatre dernières matrices disponibles.
- `Look` demande explicitement une adresse RAM.
- `Clean` ne prend aucune matrice en entrée.

Par exemple, si les cinq matrices disponibles occupent les adresses 0 à 4, `Add` utilise celles des adresses 3 et 4. Le programme ne demande pas de sélectionner les matrices une par une. La liste est capturée avant la saisie de la suite : un résultat créé pendant cette suite ne peut donc pas servir d'entrée à une opération ajoutée dans cette même suite. Pour utiliser un résultat, lancez une nouvelle exécution.

`Add` et `Multiply` exigent des matrices carrées de même dimension. `Fuse` exige quatre matrices carrées de même dimension. Les opérations matricielles acceptent les dimensions prises en charge par MSS, qui sont des puissances de deux.

### Portée de `Clean`

`Clean` supprime uniquement les matrices temporaires devenues inutiles. Les matrices d'entrée et les résultats destinés à l'utilisateur restent en RAM. `Full_Clean` est la commande prévue pour vider toute la RAM simulée, mais elle n'est pas encore disponible dans le menu; son implémentation dans `Core/` reste à faire.

## Démonstration automatique

La démonstration requiert au moins une configuration. Elle exécute deux scénarios prédéfinis :

1. Deux matrices 2×2, `[[1, 2], [3, 4]]` et `[[5, 6], [7, 8]]` : `Add` et `Multiply` sur ces deux matrices, puis `Split` et `Fuse` des quadrants de la première.
2. Deux matrices 4×4, l'une contenant les valeurs de 1 à 16 par ligne et l'autre ces valeurs en ordre inverse : `Add` et `Multiply` sur cette paire.

Chaque scénario affiche son bilan et enregistre automatiquement une trace distincte dans `trace/`. Les noms commencent par `demo_input_2x2_` ou `demo_input_4x4_`; un suffixe unique évite d'écraser une trace existante.

## Lire le bilan et enregistrer une trace

Après l'exécution, le bilan présente :

1. La liste numérotée des opérations exécutées et leur composant (`RAM`, `CPU` ou `E/S`).
2. Pour chaque configuration, le coût total, les sous-totaux par composant et le coût de chaque opération.
3. La configuration ayant le coût total le plus bas.

Les coûts sont les valeurs définies dans les configurations, additionnées pour les opérations de la trace. Ils ne représentent ni une durée d'exécution ni les valeurs numériques des matrices résultantes. Dans le parcours interactif, la trace est conservée pendant la session : le bilan d'une exécution ultérieure peut donc inclure les opérations des exécutions précédentes. L'exécution interactive affiche les matrices disponibles avant de lancer les opérations, mais le bilan ne réaffiche pas les matrices calculées. Celles-ci restent disponibles en RAM pour une exécution ultérieure.

Après un bilan interactif, répondez `o`/`oui` pour sauvegarder la trace ou `n`/`non` pour ne pas la sauvegarder. Si vous choisissez de sauvegarder, entrez un nom ou un chemin de fichier sans espaces. La démonstration, elle, enregistre les traces sans demander de confirmation. Une trace sauvegardée contient le bilan et les coûts, pas les valeurs des matrices résultantes.

## Messages et erreurs courants

- **Aucune configuration active** : chargez une configuration avec le choix `1` ou créez-en une avec le choix `2`.
- **Pas assez de matrices disponibles** : l'opération demandée nécessite plus de matrices que celles affichées; créez-en ou produisez-en lors d'une exécution précédente.
- **Dimension ou valeurs invalides** : les dimensions doivent être des puissances de deux supérieures ou égales à 2, et les valeurs saisies doivent être des entiers.
- **Erreur de configuration** : vérifiez que le fichier est lisible, que chaque ligne contient un nom d'opération reconnu suivi d'un coût positif, et qu'aucune opération n'est répétée.
