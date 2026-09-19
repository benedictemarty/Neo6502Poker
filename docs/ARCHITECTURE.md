# Architecture (C, llvm-mos)

```
src/engine/   moteur pur C, sans dépendance matérielle — compilé en natif (gcc) pour les tests
              et en 6502 (llvm-mos) pour la cible
  cards.[ch]        paquet 52 cartes, RNG xorshift injectable, mélange Fisher-Yates
  hand.[ch]         évaluateur 5 cartes + table de gains ASN (paire habillée V/D/R/A … flush royale)
  videopoker.[ch]   machine à états : BETTING → HOLDING → (WON ⇄ DOUBLE) → SHOWDOWN → (BETTING | OVER)
src/neo/      couche Neo6502 (API via neo/api.h et ControlPort)
  display.[ch]      recherche de cards.bin (CARDS_PATHS : ., poker/, games/poker/), chargement d'une carte, blitter, retournement animé
  lang.[ch]         textes français / anglais, pages d'aide
  help.[ch]         aide illustrée (cadres « loupe » reproduisant des zones du jeu)
  main.c            écran-titre, boucle de jeu, clavier, dessin de la table
tests/c/      tests natifs (exécutés par `make test-engine`, et via pytest `tests/test_engine_c.py`)
tools/diag/   diag.c : diagnostic du stockage sur carte (empreintes de l'image chargée et de cards.bin),
              expected.py : valeurs attendues ; tests/test_diag.py les vérifie dans Phosphoneo
```

## Identifiants de cartes
`id = couleur*13 + rang`, couleur 0=♠ 1=♥ 2=♦ 3=♣, rang 0=A, 1=2 … 9=10, 10=V, 11=D, 12=R.
C'est aussi l'index de l'image dans `assets/cards/56x80/cards.bin` (dos = 52).

## Affichage des cartes
Une main mélange les couleurs, donc les `.gfx` par couleur ne suffisent pas. Le programme
lit l'image de chaque carte (2 240 octets, 4 bpp, 56×80) dans `cards.bin` via `neo_file_open/seek/read`
et l'affiche avec **Blit Image** (API groupe 12, fonction 4, format source 1 = quartets,
page 0 = RAM 6502). Le SDK llvm-mos ne wrappe pas encore le blitter : `display.c` écrit
directement dans `ControlPort` (`$FF00`).

## Compilation
```
make neo           # build/poker.neo (mos-neo6502-clang, ~7,5 Ko)
make test-engine   # tests natifs (2 598 960 mains + machine à états)
make test          # tests natifs + pytest
```
Émulateurs : `make run` lance l'émulateur officiel `~/Neo6502firmware/bin/neo` (fenêtre SDL2, jouable ;
stockage = `build/storage/`), `make shot` fait une capture headless avec Phosphoneo (`~/Phosphoneo`,
sans affichage) ; `tests/test_emulator.py` compare
des captures de référence (le jeu est déterministe dans l'émulateur).

Déploiement : copier `build/poker.neo` et `assets/cards/56x80/cards.bin` à la racine de la SD,
puis `load "poker.neo"` / `run` (ou via l'émulateur).

## Retournement des cartes
`display_flip_many` : pour chaque étape k ∈ {2, 4, 8}, la carte est blittée avec un `stride` de
k lignes et une hauteur 80/k (compression verticale gratuite), 20 ms par image ; les nouvelles
faces sont chargées entre la fermeture et l'ouverture. Toutes les cartes qui changent sont
animées dans la même passe.

## Abattage gagnant
Son (`API_SFX_VICTORY`, fanfare dès le carré) puis `blink_payline(rank)` : la ligne de la table
des gains est redessinée 12 fois en alternant bandeau jaune / fond vert, 250 ms chacune (3 s),
et retrouve son état normal. L'animation est bloquante ; les touches frappées pendant restent
dans la file clavier du firmware (64 entrées).

## Écarts assumés par rapport au Poker ASN
- La paire d'As paie (bug corrigé) ; 9-10-V-D-R assortis = quinte flush (bug corrigé).
- Mise plafonnée à 10 ; doublement plafonné à 30 000 (l'ASN doublait sans limite).
