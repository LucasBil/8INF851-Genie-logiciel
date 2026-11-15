# Module io

Le module contient les contrôleurs d'entrée utilisateur et de démonstration, ainsi qu'un contrôleur de sortie. Les flux injectés rendent l'interface utilisateur déterministe et testable.

Le contrôleur de sortie affiche la trace d'exécution, le coût de chaque opération et le total de chaque configuration. Il indique la configuration la moins coûteuse et permet d'enregistrer ce même rapport texte via `TraceSerializer`.

`UserInputController` propose un menu pour lancer une démonstration automatique, charger ou créer une configuration, entrer une matrice, lancer le système ou quitter. Une configuration doit définir les coûts des opérations qu'elle évalue. Les erreurs de saisie et de fichiers sont affichées sans quitter le menu.

`DemoInputController` exécute deux scénarios sans interaction utilisateur : opérations de base (addition, multiplication, split et fuse) sur des matrices 2x2, puis addition et multiplication récursives de deux matrices 4x4. Chaque scénario affiche et enregistre sa trace dans `trace/` sous un nom commençant par `demo_input_`, avec la dimension dans le nom.

Le contrôleur de sortie attribue les opérations aux composants et affiche leurs sous-totaux en plus du coût total par configuration.
