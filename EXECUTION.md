# Guide d'exécution MSS

Ce guide décrit les façons de compiler, lancer et vérifier le prototype MSS sous Windows, plateforme principale du projet. Linux/WSL est présenté uniquement pour l'analyse mémoire facultative avec Valgrind. Toutes les commandes sont à exécuter depuis la racine du dépôt.

## Ce que fait l'application aujourd'hui

Le prototype demande le nombre de lignes et de colonnes, lit les valeurs de la matrice, la charge à l'adresse mémoire zéro, puis la réaffiche. Il ne simule pas encore les opérations et ne calcule pas les coûts CPU/RAM : les règles correspondantes doivent être confirmées.

## Linux/WSL — analyse mémoire facultative

La compilation et les tests automatiques de la CI sont exécutés sous Windows. Linux/WSL sert ici à lancer manuellement Valgrind, qui n'est pas disponible nativement sous Windows.

### Installer les outils

```bash
sudo apt update
sudo apt install -y build-essential cmake valgrind
```

Google Test est téléchargé par CMake lors de la première configuration; une connexion Internet est donc nécessaire.

### Compiler les tests et lancer Valgrind

```bash
cmake -S . -B build-valgrind -DCMAKE_BUILD_TYPE=Debug
cmake --build build-valgrind
ctest --test-dir build-valgrind --output-on-failure
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
  ./build-valgrind/tests/mss_tests
```

Cette vérification détecte les fuites et accès mémoire invalides pendant les tests. Elle est manuelle et facultative; elle ne fait pas partie du workflow GitHub Actions.

## Windows

### Prérequis

- CMake 3.20 ou plus récent.
- g++ et les outils MinGW-w64 (par exemple l'environnement MSYS2 UCRT64).
- Les outils CMake/MinGW doivent être accessibles dans le `PATH`.
- Une connexion Internet lors de la première configuration, pour télécharger Google Test.

Ouvrir PowerShell à la racine du dépôt.

### Compiler et lancer l'application

Le script local `run-mss.bat` configure puis compile MSS et lance immédiatement
le programme interactif. Il peut être exécuté depuis l'Explorateur Windows ou
depuis PowerShell à la racine du dépôt :

```powershell
.\run-mss.bat
```

Le script configure `build-mingw` avec le générateur MinGW Makefiles; les
prérequis ci-dessus doivent donc être installés. Le script est ignoré par Git
et n'est pas inclus dans les clones du dépôt.

Sans le script, les commandes équivalentes sont :

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build-mingw
.\build-mingw\mss.exe
```

Le programme interactif attend les dimensions et les valeurs de la matrice. L'exécutable `mss.exe` est produit dans `build-mingw`.

### Générer `main.exe` à la racine

Le fichier `main.exe` déjà présent à la racine peut provenir d'une ancienne compilation. Pour générer la version actuelle, compile MSS puis copie l'exécutable CMake :

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build-mingw --target mss
Copy-Item .\build-mingw\mss.exe .\main.exe -Force
.\main.exe
```

Le programme CMake s'appelle `mss`; `main.exe` est seulement une copie de `mss.exe`. Les exécutables générés sont ignorés par Git et ne doivent pas être commités.

### Lancer les tests unitaires

Le script local `run-tests.bat` configure puis compile la cible de tests et les
exécute avec CTest :

```powershell
.\run-tests.bat
```

Comme `run-mss.bat`, ce script est ignoré par Git et n'est pas inclus dans les
clones du dépôt.

Sans le script, les commandes équivalentes sont :

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build-mingw
ctest --test-dir build-mingw --output-on-failure
```

### Mesurer la couverture de code

La couverture nécessite GCC/MinGW-w64 (avec `gcov`) et l'outil Python `gcovr`.
Installe `gcovr` une fois dans PowerShell :

```powershell
python -m pip install gcovr
```

Vérifie que `cmake`, `gcov` et `gcovr` sont accessibles dans le `PATH`, puis
lance le script local :

```powershell
.\run-coverage.bat
```

Le script configure un build séparé dans `build-coverage`, compile les tests
avec l'instrumentation `ENABLE_COVERAGE`, les exécute et affiche un résumé de
couverture. Le rapport HTML détaillé est écrit dans
`build-coverage\coverage.html`. Le script est ignoré par Git et n'est pas
inclus dans les clones du dépôt.

Il faut exécuter ces commandes dans PowerShell/Windows. Ne lance pas `ctest` sur un dossier configuré depuis Linux/WSL : ses fichiers contiennent des chemins absolus propres à cet environnement et ne sont pas portables. Si CMake indique que `build-mingw` a été créé avec un autre générateur, configure un nouveau dossier (`build-windows`) avec les mêmes commandes en remplaçant `build-mingw` par `build-windows`.

### Utiliser Visual Studio Code

Avec les extensions C/C++ et CMake/MinGW installées, ouvrir le dépôt dans VS Code :

1. Choisir **Terminal > Run Task... > Compiler MSS** pour configurer puis compiler.
2. Choisir **Run and Debug > Déboguer MSS (Windows)** pour démarrer l'application sous GDB.

La tâche compile dans `build-mingw`; le débogueur lance ensuite `build-mingw/mss.exe`.

### Erreur CTest avec un chemin `/mnt/c/...`

Une erreur indiquant que CTest ne trouve pas un fichier `mss_tests_..._include.cmake` sous `/mnt/c/...` signifie généralement que CTest Windows utilise un dossier configuré depuis Linux/WSL. Reconfigure et reconstruis le dossier Windows comme indiqué ci-dessus, puis exécute `ctest --test-dir build-mingw --output-on-failure`. Garde un dossier de build distinct par environnement.

La CI GitHub Actions est centrée sur Windows : elle compile le programme et lance les tests sur les Pull Requests et les pushes vers `main`. L'analyse Valgrind sous Linux/WSL est une vérification manuelle séparée.

## Répertoires de build

Les répertoires `build-mingw` et `build-valgrind` sont séparés afin de ne pas mélanger les fichiers produits par Windows et Linux/WSL. Après une modification du code, relancer la compilation puis les tests du répertoire correspondant.
