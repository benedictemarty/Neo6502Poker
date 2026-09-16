# Crédits

## Logiciel
- **Neo6502Poker** — programmation Bénédicte MARTY (bmarty <bmarty@mailo.com>), 2026, avec l'aide de
  Claude Code (Anthropic). Licence EUPL-1.2.
- D'après le **vidéo-poker d'ASN Diffusion** pour Oric 1 (1983), étudié dans `docs/analyse-poker-asn.md` ;
  le Poker de V. Talvas / Loriciels a servi de seconde référence (`docs/analyse-poker-oric.md`).

## Machine
- **Neo6502** — carte open hardware Olimex (W65C02S + RP2040), <https://www.olimex.com/Products/Retro-Computers/Neo6502/>.
- **Firmware et API** — Paul Robson et contributeurs, licence MIT,
  <https://github.com/paulscottrobson/neo6502-firmware>. Les effets sonores du jeu sont ceux du firmware.

## Outils
- **llvm-mos** — compilateur C pour 6502, cible `neo6502`, licence Apache-2.0 avec exceptions LLVM,
  <https://github.com/llvm-mos/llvm-mos-sdk>.
- **Phosphoneo** — émulateur Neo6502 headless de bmarty (tests golden), EUPL-1.2.
- Émulateur officiel `neo` (dépôt du firmware) pour jouer.
- Python 3, Pillow, ImageMagick pour la génération des cartes.

## Graphismes
- Faces : « English pattern playing cards deck », dos : « Atlas deck card back blue and brown » —
  Dmitry Fomin, **CC0**, Wikimedia Commons (détails dans `assets/source/LICENCE.md`).
