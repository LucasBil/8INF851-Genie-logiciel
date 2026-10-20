# Guide d'exécution MSS

Ce guide décrit les façons de compiler, lancer et vérifier le prototype MSS sous Linux et Windows. Toutes les commandes sont à exécuter depuis la racine du dépôt.

## Ce que fait l'application aujourd'hui

Le prototype demande le nombre de lignes et de colonnes, lit les valeurs de la matrice, la charge à l'adresse mémoire zéro, puis la réaffiche. Il ne simule pas encore les opérations et ne calcule pas les coûts CPU/RAM : les règles correspondantes doivent être confirmées.

## Linux

### Prérequis

Installer un compilateur C++, CMake et les outils nécessaires :

```bash
sudo apt update
sudo apt install -y build-essential cmake
```

Google Test est téléchargé par CMake lors de la première configuration; une connexion Internet est donc nécessaire.

### Compiler et lancer l'application

```bash
cmake -S . -B build-linux -DCMAKE_BUILD_TYPE=Debug
cmake --build build-linux
./build-linux/mss
```

Cette exécution interactive sert à saisir et vérifier une matrice. Entrer d'abord les dimensions, par exemple `2 2`, puis les valeurs, par exemple `1 2 3 4`.

### Lancer les tests unitaires

```bash
ctest --test-dir build-linux --output-on-failure
```

Les tests vérifient les matrices, la mémoire virtuelle, l'initialisation du système, les erreurs attendues et la lecture/affichage des matrices. `--output-on-failure` montre le détail uniquement si un test échoue.

### Mesurer la couverture

Installer gcovr, puis créer un build de couverture distinct :

```bash
sudo apt install -y gcovr
cmake -S . -B build-cov -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build-cov
ctest --test-dir build-cov --output-on-failure
gcovr -r . build-cov --object-directory build-cov \
  --filter 'src/(core|io)' --fail-under-line 75 --print-summary
```

Cette commande mesure la couverture des modules `core` et `io` et échoue si la couverture des lignes est inférieure à 75 %. Elle nécessite GCC ou Clang et gcovr.

### Qualité et mémoire

Si les outils sont installés, les contrôles de formatage et d'analyse statique peuvent être exécutés ainsi :

```bash
find src include tests -type f \( -name '*.cpp' -o -name '*.hpp' \) \
  -print0 | xargs -0 clang-format --dry-run --Werror
clang-tidy -p build-linux src/core/*.cpp src/io/*.cpp
```

Valgrind sert à détecter les fuites mémoire et les accès invalides :

```bash
sudo apt install -y valgrind
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
  ./build-linux/tests/mss_tests
```

## Windows

### Prérequis

- CMake 3.20 ou plus récent.
- g++ et les outils MinGW-w64 (par exemple l'environnement MSYS2 UCRT64).
- Les outils CMake/MinGW doivent être accessibles dans le `PATH`.
- Une connexion Internet lors de la première configuration, pour télécharger Google Test.

Ouvrir PowerShell à la racine du dépôt.

### Compiler et lancer l'application

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build-mingw
.\build-mingw\mss.exe
```

Le programme interactif attend les dimensions et les valeurs de la matrice. L'exécutable `mss.exe` est produit dans `build-mingw`.

### Lancer les tests unitaires

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build-mingw
ctest --test-dir build-mingw --output-on-failure
```

Il faut exécuter ces commandes dans PowerShell/Windows. Ne lance pas `ctest` sur `build-linux` depuis Windows : ce dossier contient les chemins absolus générés par CMake sous Linux (par exemple `/mnt/c/...`) et n'est pas portable entre systèmes. Si CMake indique que `build-mingw` a été créé avec un autre générateur, configure un nouveau dossier (`build-windows`) avec les mêmes commandes en remplaçant `build-mingw` par `build-windows`.

### Utiliser Visual Studio Code

Avec les extensions C/C++ et CMake/MinGW installées, ouvrir le dépôt dans VS Code :

1. Choisir **Terminal > Run Task... > Compiler MSS** pour configurer puis compiler.
2. Choisir **Run and Debug > Déboguer MSS (Windows)** pour démarrer l'application sous GDB.

La tâche compile dans `build-mingw`; le débogueur lance ensuite `build-mingw/mss.exe`.

### Erreur CTest avec un chemin `/mnt/c/...`

Une erreur indiquant que CTest ne trouve pas un fichier `mss_tests_..._include.cmake` sous `/mnt/c/...` signifie généralement que CTest Windows utilise un dossier configuré depuis Linux/WSL. Reconfigure et reconstruis le dossier Windows comme indiqué ci-dessus, puis exécute `ctest --test-dir build-mingw --output-on-failure`. Garde un dossier de build distinct par environnement.

La couverture Gcov, clang-tidy et Valgrind décrits ci-dessus sont prévus pour l'environnement Linux. La CI couvre les deux plateformes : compilation et tests sous Windows, puis contrôles qualité, couverture et Valgrind sous Linux.

## Répertoires de build

Les répertoires `build-linux`, `build-mingw` et `build-cov` sont séparés afin de ne pas mélanger les fichiers produits par des plateformes ou options de compilation différentes. Après une modification du code, relancer la compilation puis les tests du répertoire correspondant.
