# Plan agile — Neo6502Poker

## Vision
Un poker 32 cartes joueur-contre-machine sur Neo6502, fidèle à l'esprit des Poker Oric de 1983.

## Backlog produit (ordre de priorité)
1. **US-01** Étude de l'existant Oric (analyse du code, règles, barème) — *fait, sprint 0*.
2. **US-02** Récupérer la version ASN Diffusion (oric.org n° 1760) et la comparer — *fait, sprint 0* (`docs/analyse-poker-asn.md`).
3. **US-03** Choisir la chaîne d'outils Neo6502 (BASIC Neo, asm 6502, C llvm-mos/cc65).
4. **US-04** Moteur de jeu : paquet de cartes, donne, écart, évaluateur de mains (52 cartes façon ASN puis 32 cartes façon Talvas) avec tests unitaires.
4b. **US-04b** Jalon 1 : vidéo-poker façon ASN (table de gains, quitte ou double).
5. **US-05** Enchères + IA de mise.
6. **US-06** Affichage des cartes (API graphique Neo6502) — *assets prêts, sprint 0* (`docs/graphismes-neo6502.md`) ; reste : chargement `.gfx`/`.bin` et affichage sur cible.
7. **US-07** Fin de partie, score, aide intégrée.

## Sprints
| Sprint | Dates | Objectif | État |
|---|---|---|---|
| 0 | 2026-09-16 | Récupération + analyse du Poker Oric, outillage TAP/BASIC, dépôt git | terminé |
| 1 | à planifier | US-03, US-04 | à faire |

## Définition de « fini »
Code testé (pytest ou tests d'émulation), CHANGELOG et documentation à jour, commit signé bmarty.
