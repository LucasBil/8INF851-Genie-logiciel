# Module io

Le module contient les contrôleurs d'entrée utilisateur et de démonstration, ainsi qu'un contrôleur de sortie. Les flux injectés rendent l'interface utilisateur déterministe et testable.

Le contrôleur de sortie affiche la trace d'exécution, le coût de chaque opération et le total de chaque configuration. Il indique la configuration la moins coûteuse et permet d'enregistrer ce même rapport texte via `TraceSerializer`.

`UserInputController` propose un menu pour lister et charger une configuration, en créer et sauvegarder une, entrer une matrice, lancer le système ou quitter. Une configuration doit définir les 12 coûts positifs du modèle. Les erreurs de saisie et de fichiers sont affichées sans quitter le menu. L'instance actuelle de MSS ne peut charger qu'une matrice initiale et le noyau ne fournit pas encore d'API pour programmer une opération; le contrôleur refuse donc de présenter une trace vide comme un bilan réussi.

`DemoInputController` charge une matrice 2x2 et affiche les résultats sans demander de saisie ni de choix de sauvegarde. Il signale une erreur si le noyau ne produit aucune opération.

Les sous-totaux CPU/RAM et le lancement d'opérations restent à définir dans le noyau : le modèle de coût ne précise pas de répartition par composant et `MonSuperSysteme` n'expose pas encore de mécanisme de programmation d'opérations.
