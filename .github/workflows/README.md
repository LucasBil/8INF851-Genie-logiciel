# Workflows GitHub Actions

Le workflow `ci.yml` compile et teste le projet sous Windows lors des Pull Requests et des pushes vers `main`. Linux n'est pas utilisé dans la CI; WSL/Ubuntu sert uniquement aux analyses Valgrind lancées manuellement.
