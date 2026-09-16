# Changelog

Format inspiré de [Keep a Changelog](https://keepachangelog.com/fr/1.1.0/).

## [0.9.0] - 2026-09-16
### Ajouté
- À l'abattage gagnant : après le son (victoire, fanfare dès le carré), la ligne de la combinaison
  gagnée clignote 3 s dans la table des gains (12 alternances bandeau jaune / normal, `blink_payline`).
- Capture golden `win_blink` prise pendant le clignotement (24 tests).

## [0.8.0] - 2026-09-16
### Ajouté
- Page « Crédits » (5ᵉ page de l'aide, FR/EN) : logiciel, machine Neo6502/Olimex, firmware (Paul Robson,
  MIT), llvm-mos, Phosphoneo, cartes CC0, sons ; `CREDITS.md` et section dans le README.

## [0.7.2] - 2026-09-16
### Ajouté
- Publication sur GitHub et Framagit (remotes `github` et `framegit`), liens dans le README.

## [0.7.1] - 2026-09-16
### Ajouté
- `docs/JAQUETTE.md` : prompts (EN/FR, variante pixel art) pour générer la jaquette avec Nano Banana.

## [0.7.0] - 2026-09-16
### Ajouté
- **Aide intégrée** (`src/neo/help.c`), 4 pages en français ou anglais selon le sélecteur F/E :
  comment jouer, l'écart, quitte ou double, les gains. Chaque page est illustrée par une
  reproduction de la zone d'écran concernée dans un cadre « loupe » (barre de statut agrandie ×2,
  vraies cartes avec GARDE/CHANGE, carte du pari, ligne de la table des gains).
  Accès : H à l'écran-titre et pendant la mise ; Espace page suivante, Q retour.
- Écran-titre : F/E change la langue immédiatement (textes retraduits), H aide, Espace joue.
- Captures golden des 4 pages (FR), d'une page EN et du titre EN (21 tests).

## [0.6.0] - 2026-09-16
### Ajouté
- **Quitte ou double** (règle ASN) : après un gain, Q encaisse ou D propose de deviner la couleur
  (R rouge / N noir, B en anglais) de la carte suivante ; gain doublé à chaque bonne réponse,
  perdu sinon ; plafond de doublement 30 000. Nouveaux états `VP_WON`, `VP_DOUBLE`.
- **Sons** (effets du firmware) : pièce à la mise et à l'encaissement, confirmation à la donne,
  victoire / fanfare (carré et plus) / négatif à l'abattage, power-up ou défaite au pari, défaite
  en fin de partie.
- Captures golden `double_collect` et `double_lost` (graine différente via le moment de la frappe).
### Corrigé
- `card_is_red` : ♥ et ♦ sont rouges (♠/♥ auparavant) — détecté par le scénario émulé.
- Q n'interrompt le jeu qu'en phase de mise ou en fin de partie (sinon Q = encaisser).
- Capture golden `holding` reprise à l'état final (l'ancienne était prise pendant l'animation).

## [0.5.1] - 2026-09-16
### Modifié
- Retournement des cartes 6× plus court (721 ms → ~120 ms pour une donne) : animation autour
  de l'axe horizontal réalisée par le pas de lignes du blitter (80 → 40 → 20 → 10 lignes, puis
  ouverture), sans calcul sur le 6502, et toutes les cartes qui changent sont retournées ensemble.

## [0.5.0] - 2026-09-16
### Ajouté
- Écran de présentation (titre, éventail de cartes, crédits « Basé sur le vidéo-poker de la société
  ASN (Oric 1) — Programmation Bénédicte MARTY avec l'aide de Claude Code », choix de langue).
- Version **française et anglaise** (`src/neo/lang.[ch]`, touche F/E à l'écran-titre).
- Animation de retournement des cartes dans les deux sens (dos → face à la donne et à l'écart,
  face → dos à la manche suivante) : compression horizontale en RAM puis blitter, 8 images.
- Cartes **56×80** (le blitter n'impose pas des multiples de 16) ; 5 cartes = 312 px.
### Modifié
- Qualité des cartes : classification vers les 5 couleurs à plat du SVG (fini les pixels pêche/gris),
  bleu des figures en lavande, liseré gris, **index de coin redessinés en police pixel 5×7** (les
  glyphes du SVG étaient illisibles), coins nettoyés ; rasterisation à 96 dpi.
- Captures golden : titre, donne, abattage FR et EN.

## [0.4.2] - 2026-09-16
### Ajouté
- `make run` : lance le jeu dans l'émulateur officiel `neo` (SDL2) ; Phosphoneo reste l'outil headless (`make shot`, tests).

## [0.4.1] - 2026-09-16
### Vérifié
- `poker.neo` exécuté dans **Phosphoneo** (émulateur local headless) : chargement des cartes depuis
  `cards.bin`, blitter, donne, écart, abattage — une manche complète validée par captures.
### Ajouté
- Tests golden `tests/test_emulator.py` (captures `tests/golden/*.ppm`, déterministes), cibles
  `make run-emu` / `make shot`.
### Corrigé
- Texte dessiné sans fond noir ; alignement de la table des gains (printf llvm-mos sans largeur).

## [0.4.0] - 2026-09-16
### Ajouté
- Chaîne C (llvm-mos, cible neo6502) : `Makefile`, `src/engine/` (cartes, évaluateur, vidéo-poker),
  `src/neo/` (affichage par blitter depuis `cards.bin`, boucle de jeu), `build/poker.neo`.
- Tests natifs : énumération exhaustive des 2 598 960 mains (comptes de référence), cas des deux
  bugs ASN corrigés, machine à états ; intégrés à pytest.
- `docs/ARCHITECTURE.md`.

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
