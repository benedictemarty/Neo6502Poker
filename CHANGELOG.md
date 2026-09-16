# Changelog

Format inspiré de [Keep a Changelog](https://keepachangelog.com/fr/1.1.0/).

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
