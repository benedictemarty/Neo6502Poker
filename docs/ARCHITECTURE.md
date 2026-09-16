# Architecture (C, llvm-mos)

```
src/engine/   moteur pur C, sans dépendance matérielle — compilé en natif (gcc) pour les tests
              et en 6502 (llvm-mos) pour la cible
  cards.[ch]        paquet 52 cartes, RNG xorshift injectable, mélange Fisher-Yates
  hand.[ch]         évaluateur 5 cartes + table de gains ASN (paire habillée V/D/R/A … flush royale)
  videopoker.[ch]   machine à états : BETTING → HOLDING → SHOWDOWN → (BETTING | OVER)
src/neo/      couche Neo6502 (API via neo/api.h et ControlPort)
  display.[ch]      chargement d'une carte depuis cards.bin (SD) et affichage par le blitter
  main.c            boucle de jeu, clavier, dessin de la table
tests/c/      tests natifs (exécutés par `make test-engine`, et via pytest `tests/test_engine_c.py`)
```

## Identifiants de cartes
`id = couleur*13 + rang`, couleur 0=♠ 1=♥ 2=♦ 3=♣, rang 0=A, 1=2 … 9=10, 10=V, 11=D, 12=R.
C'est aussi l'index de l'image dans `assets/cards/48x64/cards.bin` (dos = 52).

## Affichage des cartes
Une main mélange les couleurs, donc les `.gfx` par couleur ne suffisent pas. Le programme
lit l'image de chaque carte (1 536 octets, 4 bpp) dans `cards.bin` via `neo_file_open/seek/read`
et l'affiche avec **Blit Image** (API groupe 12, fonction 4, format source 1 = quartets,
page 0 = RAM 6502). Le SDK llvm-mos ne wrappe pas encore le blitter : `display.c` écrit
directement dans `ControlPort` (`$FF00`).

## Compilation
```
make neo           # build/poker.neo (mos-neo6502-clang, ~7,5 Ko)
make test-engine   # tests natifs (2 598 960 mains + machine à états)
make test          # tests natifs + pytest
```
Déploiement : copier `build/poker.neo` et `assets/cards/48x64/cards.bin` à la racine de la SD,
puis `load "poker.neo"` / `run` (ou via l'émulateur).

## Écarts assumés par rapport au Poker ASN
- La paire d'As paie (bug corrigé) ; 9-10-V-D-R assortis = quinte flush (bug corrigé).
- Mise plafonnée à 10 ; « quitte ou double » non encore implémenté (US-05).
