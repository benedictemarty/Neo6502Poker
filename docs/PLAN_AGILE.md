# Plan agile — Neo6502Poker

## Vision
Un poker 32 cartes joueur-contre-machine sur Neo6502, fidèle à l'esprit des Poker Oric de 1983.

## Backlog produit (ordre de priorité)
1. **US-01** Étude de l'existant Oric (analyse du code, règles, barème) — *fait, sprint 0*.
2. **US-02** Récupérer la version ASN Diffusion (oric.org n° 1760) et la comparer — *fait, sprint 0* (`docs/analyse-poker-asn.md`).
3. **US-03** Chaîne d'outils : **C avec llvm-mos** (décision du 2026-09-16), moteur testé en natif — *fait*.
4. **US-04** Moteur 52 cartes : paquet, donne, écart, évaluateur (test exhaustif) — *fait, sprint 1* ; variante 32 cartes (Talvas) à faire.
4b. **US-04b** Jalon 1 : vidéo-poker façon ASN — *machine à états et binaire faits, sprint 1* ; validé sur Phosphoneo, quitte ou double et sons faits (sprint 3).
5. **US-05** Enchères + IA de mise.
6. **US-06** Affichage des cartes (API graphique Neo6502) — *assets prêts, sprint 0* (`docs/graphismes-neo6502.md`) ; reste : chargement `.gfx`/`.bin` et affichage sur cible.
7. **US-07** Fin de partie, score, aide intégrée.

## Sprints
| Sprint | Dates | Objectif | État |
|---|---|---|---|
| 0 | 2026-09-16 | Récupération + analyse du Poker Oric, outillage TAP/BASIC, dépôt git | terminé |
| 1 | 2026-09-16 | US-03, US-04, US-04b (vidéo-poker jouable en C) | terminé — validé sur Phosphoneo, puis sur carte réelle le 2026-09-19 (0.9.3) |
| 2 | 2026-09-16 | écran-titre, FR/EN, retournement animé, qualité des cartes (56×80, index pixel) | terminé |
| 3 | 2026-09-16 | quitte ou double, sons | terminé |
| 4 | 2026-09-16 | aide intégrée FR/EN illustrée (4 pages) | terminé |
| 4b | 2026-09-16 | clignotement de la ligne gagnante (3 s) après le son de victoire | terminé |
| 4c | 2026-09-19 | **anomalie carte réelle** (cartes et textes brouillés, émulateurs corrects) : outil `tools/diag` pour localiser le défaut (chargement du `.neo` ou lecture de `cards.bin`) | terminé, **validé sur carte** — deux causes : `cards.bin` introuvable depuis le répertoire courant (paquet Prophet → repli `poker/cards.bin` + message, 0.9.2) ; textes faux quand la chaîne est construite sur la pile par le SDK (→ tampon statique, 0.9.3) |
| 5 | à planifier | poker 2 joueurs façon Talvas : 32 cartes, enchères, IA (US-05) | à faire |

## Définition de « fini »
Code testé (pytest ou tests d'émulation), CHANGELOG et documentation à jour, commit signé bmarty.
