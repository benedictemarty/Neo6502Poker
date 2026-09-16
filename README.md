# Neo6502Poker

Portage / réécriture d'un jeu de poker (32 cartes, joueur contre machine) pour le **Neo6502**,
inspiré des « Poker » édités pour l'Oric au début des années 80.

## Contenu

- `docs/analyse-poker-asn.md` — analyse du Poker **ASN Diffusion** (vidéo-poker 52 cartes, 1983).
- `docs/analyse-poker-oric.md` — analyse du Poker Oric de V. Talvas (Loriciels) : règles,
  évaluateur de mains, IA, graphismes, bugs.
- `docs/PLAN_AGILE.md` — backlog et sprints.
- `reference/poker-oric/` — TAP d'origine (TOSEC) et listing BASIC détokenisé.
- `tools/oric_tap.py` — lecteur TAP + détokeniseur BASIC Oric.
- `tests/` — tests pytest (`python3 -m pytest -q tests`).
- `CHANGELOG.md` — journal des modifications.

## Note sur « Poker ASN »

Deux Poker Oric sont étudiés : celui d'**ASN Diffusion** (oric.org n° 1760, 1983, vidéo-poker
52 cartes) et celui de V. Talvas / Loriciels (TOSEC, poker fermé à 2 joueurs). Ce sont deux
programmes sans rapport ; voir `docs/analyse-poker-asn.md` §6 pour la comparaison.
