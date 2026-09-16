# Graphismes des cartes pour le Neo6502 (mode 0)

Date : 2026-09-16 — Sprint 0 (US-06 préparatoire).

## 1. Contraintes du Neo6502 (vérifiées dans le firmware `paulscottrobson/neo6502-firmware`)

| Élément | Valeur | Source |
|---|---|---|
| Mode 0 | 320×240 pixels, 8 bits/pixel en mémoire écran ; console 53×30, police 6×8 | `firmware/common/sources/interface/graphics.cpp` |
| Palette par défaut | 16 couleurs Pico-8 (0 noir/transparent, 1 rouge, 2 vert, 3 jaune, 4 bleu foncé, 5 pourpre, 6 bleu, 7 blanc, 8 noir opaque, 9 gris foncé, 10 vert foncé, 11 orange, 12 brun, 13 lavande, 14 pêche, 15 gris clair) | `firmware/common/include/interface/palette.h` |
| Sprites / tiles | 4 bits/pixel (2 pixels/octet, quartet haut = 1er pixel) ; tiles 16×16 (128 o), sprites 16×16 (128 o) et 32×32 (512 o) ; couleur 0 = transparent pour les sprites | `basic/scripts/gconvert.py` |
| Fichier `.gfx` | en-tête 256 octets : `[0]=1`, `[1]=nb tiles`, `[2]=nb sprites 16`, `[3]=nb sprites 32`, puis les données ; **32 Ko max** de mémoire graphique (`GFX_MEMORY_SIZE 0x8000`) | `gconvert.py`, `memory.h` |

Conséquence : un jeu complet de 52 cartes en 32×48 (768 o/carte) = 39,9 Ko, ou en 48×64
(1 536 o/carte) = 79,9 Ko : **impossible dans un seul `.gfx`**. Deux solutions sont livrées :
1. un `.gfx` **par couleur** (13 cartes + dos) — 11 Ko en 32×48, 21,8 Ko en 48×64 ;
2. un binaire brut `cards.bin` (53 images 4 bpp) à charger par morceaux depuis la carte SD
   dans la RAM 6502 puis à copier avec le blitter / `Draw Image`.

## 2. Sources graphiques (open source, CC0)

- Faces : « English pattern playing cards deck » — Dmitry Fomin, CC0.
- Dos : « Atlas deck card back blue and brown » — Dmitry Fomin, CC0.

Détails et liens : `assets/source/LICENCE.md`.

## 3. Génération

```
python3 tools/make_cards.py            # 32x48 et 48x64
python3 tools/make_cards.py --size 48x64
```
Pipeline : rasterisation du SVG par ImageMagick (`convert -density 48`), détection de la grille
13×4 sur le fond vert, redimensionnement LANCZOS, quantification au plus proche sur la palette
Neo6502 (jamais l'indice 0, noir = 8), puis empaquetage 4 bpp.

Sorties dans `assets/cards/<W>x<H>/` :

| Fichier | Contenu |
|---|---|
| `png/<rang><couleur>.png` | 52 cartes (`A 2 … 9 T J Q K` × `s h d c`) + `back.png`, images indexées |
| `deck.png` | planche de contrôle |
| `cards.bin` | 53 images 4 bpp ; index = `couleur*13 + rang` (♠=0 ♥=1 ♦=2 ♣=3, A=0 … K=12), dos = 52 |
| `cards_<s|h|d|c>.gfx` | 14 cartes en blocs 16×16, ligne par ligne ; carte n → blocs `n*T … n*T+T-1` (T = 6 en 32×48, 12 en 48×64), dos = carte 13. `Draw Image` n'adresse que 128 tiles (`$00-$7F`) : les blocs ≥ 128 (48×64 seulement) sont stockés en sprites 16×16, id = `$80 + (bloc − 128)` |

## 3b. Qualité (v0.5.0)
- Le SVG n'utilise que 5 couleurs à plat (blanc, noir, rouge, bleu, jaune) : chaque pixel réduit est
  classé vers la plus proche (noir favorisé ×0,6 pour garder les traits), puis vers la palette Neo :
  blanc 7, noir 8, rouge 1, bleu → lavande 13, jaune 3, liseré gris 15.
- Index de coin redessinés (police pixel 5×7, `FONT` dans `make_cards.py`) dans les deux coins.
- Format **56×80** utilisé par le jeu (blitter, pas de contrainte de tiles) : 2 240 octets/carte,
  `cards.bin` = 118 720 octets, chargé carte par carte depuis la SD.

## 4. Choix recommandé

**56×80** pour le jeu (blitter). 48×64 et 32×48 restent générés pour un usage en tiles/sprites (`.gfx`).

## 5. Limites connues

- Quantification sans tramage : les bleus des figures deviennent lavande (13) / bleu foncé (4).
- Le dos rasterisé perd sa bordure blanche (motif plein cadre).
- Les index de coin en 32×48 sont difficilement lisibles.
