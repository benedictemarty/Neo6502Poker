# Neo6502Poker

Dépôts : [GitHub](https://github.com/benedictemarty/Neo6502Poker) · [Framagit](https://framagit.org/benedictemarty/Neo6502Poker)

Portage / réécriture d'un jeu de poker (32 cartes, joueur contre machine) pour le **Neo6502**,
inspiré des « Poker » édités pour l'Oric au début des années 80.

## Compilation et tests

```
make neo           # binaire Neo6502 (llvm-mos dans ~/llvm-mos)
make test          # tests natifs C + pytest (dont captures golden sur Phosphoneo)
make run           # jouer dans l'émulateur officiel neo (SDL2) — H : aide intégrée FR/EN
make diag          # diagnostic du stockage pour la carte réelle (tools/diag/README.md)
```
Sur la carte : `cards.bin` doit être dans le répertoire courant ou dans `poker/` (paquet Prophet) ;
sinon le jeu affiche « cards.bin introuvable ».
Voir `docs/ARCHITECTURE.md`.

## Contenu

- `docs/analyse-poker-asn.md` — analyse du Poker **ASN Diffusion** (vidéo-poker 52 cartes, 1983).
- `docs/analyse-poker-oric.md` — analyse du Poker Oric de V. Talvas (Loriciels) : règles,
  évaluateur de mains, IA, graphismes, bugs.
- `src/engine/`, `src/neo/` — moteur C et couche Neo6502 ; `tests/c/` — tests natifs.
- `docs/ARCHITECTURE.md` — organisation du code C.
- `docs/graphismes-neo6502.md` — contraintes graphiques du Neo6502 et génération des cartes.
- `docs/PLAN_AGILE.md` — backlog et sprints.
- `reference/poker-oric/` — TAP d'origine et listings BASIC détokenisés.
- `assets/source/` — SVG CC0 des cartes (faces + dos) ; `assets/cards/` — cartes converties (PNG, `.bin`, `.gfx`).
- `tools/make_cards.py` — génération des cartes pour le Neo6502 (mode 0, 4 bpp).
- `tools/oric_tap.py` — lecteur TAP + détokeniseur BASIC Oric.
- `tools/diag/` — `diag.neo` : empreintes du `.neo` chargé et de `cards.bin` lus sur la carte, à comparer
  avec `make diag` (cartes ou textes brouillés sur la carte réelle).
- `tests/` — tests pytest (`python3 -m pytest -q tests`).
- `CHANGELOG.md` — journal des modifications.

## Crédits

Voir [CREDITS.md](CREDITS.md) : jeu (Bénédicte MARTY, d'après ASN Diffusion 1983), machine Neo6502
(Olimex ; firmware Paul Robson, MIT), llvm-mos, Phosphoneo, cartes CC0 de Dmitry Fomin. La page 5 de
l'aide intégrée (touche H) reprend ces crédits en français et en anglais.

## Note sur « Poker ASN »

Deux Poker Oric sont étudiés : celui d'**ASN Diffusion** (oric.org n° 1760, 1983, vidéo-poker
52 cartes) et celui de V. Talvas / Loriciels (TOSEC, poker fermé à 2 joueurs). Ce sont deux
programmes sans rapport ; voir `docs/analyse-poker-asn.md` §6 pour la comparaison.
