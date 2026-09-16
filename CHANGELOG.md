# Changelog

Format inspiré de [Keep a Changelog](https://keepachangelog.com/fr/1.1.0/).

## [0.3.1] - 2026-09-16
### Corrigé
- `.gfx` 48×64 : `Draw Image` n'adresse que 128 tiles ; les blocs 128-167 sont désormais
  stockés en sprites 16×16 (`$80+`) pour rester affichables. Tests et doc mis à jour.

## [0.3.0] - 2026-09-16
### Ajouté
- Faces et dos de cartes open source (CC0, Dmitry Fomin) dans `assets/source/`.
- `tools/make_cards.py` : conversion en 32×48 et 48×64 pour le Neo6502 mode 0 (palette 16 couleurs,
  4 bpp) ; sorties PNG, `cards.bin` (53 images) et un `.gfx` par couleur (13 cartes + dos).
- `docs/graphismes-neo6502.md` : contraintes du Neo6502 (mode 0, palette, format .gfx, 32 Ko) et format des sorties.
- Tests `tests/test_cards_assets.py`.

## [0.2.0] - 2026-09-16
### Ajouté
- Poker ASN Diffusion (oric.org n° 1760) fourni par l'utilisateur : TAP et listing dans
  `reference/poker-oric/POKER_ASN.*`, analyse `docs/analyse-poker-asn.md` (vidéo-poker 52 cartes,
  table de gains, quitte ou double, bugs) et comparaison avec la version Talvas/Loriciels.
- Test `test_parse_tap_asn`.
### Modifié
- README, plan agile (US-02 terminée, jalon vidéo-poker ajouté).

## [0.1.0] - 2026-09-16
### Ajouté
- Récupération du TAP « Poker » Oric (V. Talvas / Loriciels) depuis la collection TOSEC (archive.org)
  et listing BASIC détokenisé (`reference/poker-oric/`).
- Analyse complète du jeu : `docs/analyse-poker-oric.md` (règles, barème des mains, IA, graphismes, bugs).
- Outil `tools/oric_tap.py` (lecture TAP Oric, détokenisation BASIC) et tests `tests/test_oric_tap.py`.
- Documentation projet : `README.md`, `docs/PLAN_AGILE.md`.
### Note
- Le Poker d'ASN Diffusion (oric.org n° 1760) n'a pas pu être récupéré (compte oric.org requis).
