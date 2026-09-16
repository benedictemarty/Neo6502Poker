# Neo6502Poker

Portage / réécriture d'un jeu de poker (32 cartes, joueur contre machine) pour le **Neo6502**,
inspiré des « Poker » édités pour l'Oric au début des années 80.

## Contenu

- `docs/analyse-poker-oric.md` — analyse du Poker Oric de V. Talvas (Loriciels) : règles,
  évaluateur de mains, IA, graphismes, bugs.
- `docs/PLAN_AGILE.md` — backlog et sprints.
- `reference/poker-oric/` — TAP d'origine (TOSEC) et listing BASIC détokenisé.
- `tools/oric_tap.py` — lecteur TAP + détokeniseur BASIC Oric.
- `tests/` — tests pytest (`python3 -m pytest -q tests`).
- `CHANGELOG.md` — journal des modifications.

## Note sur « Poker ASN »

Le Poker d'**ASN Diffusion** (oric.org n° 1760, 1983, Oric-1 seulement) n'est téléchargeable
qu'avec un compte oric.org ; le seul Poker disponible publiquement (TOSEC) est celui de
V. Talvas / Loriciels. Voir `docs/analyse-poker-oric.md` §1.
