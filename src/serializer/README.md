# Module serializer

Le module expose `SerializerTool`, `FileExplorer` et `ConfigurationSerializer`. Les configurations texte sont sauvegardees dans `configurations/`, en fichiers `.txt`. Chaque ligne contient le nom d'une operation et son cout, separes par un espace (ex. `Rotate 4`). L'index de chargement est zero-based et suit le tri alphabetique des noms de fichiers. FileExplorer conserve la liste des chemins obtenue au moment de l'affichage; une nouvelle liste actualise ces indices. Une sauvegarde remplace le fichier du meme nom.
