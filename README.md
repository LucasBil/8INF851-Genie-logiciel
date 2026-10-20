# MSS — Mon Super Système

Prototype C++17 du simulateur MSS. Cette première version fournit la structure CMake en trois modules, les types de base, une saisie de matrice et les tests. Les opérations, leurs coûts et la sérialisation ne sont pas encore activés tant que leurs règles et les champs de configuration ne sont pas confirmés.

## Prérequis

- Windows 10/11, CMake 3.20 ou plus récent et g++ (MinGW-w64/MSYS2 UCRT64).
- Git et une connexion Internet lors de la première configuration (Google Test est téléchargé par CMake).
- Linux avec g++/CMake peut aussi être utilisé pour compiler et exécuter les tests.

## Compiler et lancer

Pour le détail des parcours Linux/Windows, tests, couverture et débogage VS Code, voir [EXECUTION.md](EXECUTION.md).

Depuis la racine du dépôt, dans PowerShell :

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build-mingw
.\build-mingw\mss.exe
```

L'application demande le nombre de lignes et de colonnes, puis les valeurs de la matrice. Elle vérifie et réaffiche la matrice chargée à l'adresse mémoire zéro. Elle retourne un code d'erreur et un message explicite si l'entrée est invalide.

Pour exécuter les tests :

```powershell
ctest --test-dir build-mingw --output-on-failure
```

Sous Windows, configure et construis d'abord `build-mingw` avec les commandes ci-dessus. N'utilise pas le dossier `build-linux` depuis Windows/PowerShell : les fichiers CMake générés contiennent des chemins spécifiques à Linux/WSL.

Sous Linux, utiliser le dossier `build-linux` et omettre le générateur :

```bash
cmake -S . -B build-linux -DCMAKE_BUILD_TYPE=Debug
cmake --build build-linux
./build-linux/mss
ctest --test-dir build-linux --output-on-failure
```

## Couverture

```bash
cmake -S . -B build-cov -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build-cov
ctest --test-dir build-cov --output-on-failure
gcovr -r . build-cov --object-directory build-cov --filter 'src/(core|io)' --fail-under-line 75 --print-summary
```

Google Test est récupéré avec `FetchContent`. Le code de couverture nécessite GCC ou Clang et `gcovr`.

## État du prototype

- **Disponible** : `Matrix`, adresses de RAM, matrice initiale MSS, enum `OperationName`, lecture/affichage via flux injectables, tests unitaires.
- **En attente** : sémantique de `execute`/`run`, règles des opérations, coûts CPU/RAM, champs de configuration JSON et outils de sérialisation.

`Cpu::getOperationCost` et `Ram::getOperationCost` signalent explicitement que leur modèle n'est pas encore spécifié; ils ne renvoient pas de coût fictif.
