#!/usr/bin/env python3
"""Génère les faces de cartes pour le Neo6502 (mode 0, 320x240, sprites/tiles 4 bpp).

Entrées : assets/source/english_pattern_playing_cards_deck.svg et
          assets/source/atlas_deck_card_back_blue_and_brown.svg (CC0, Dmitry Fomin).
Sorties (assets/cards/<W>x<H>/) :
  - png/<rang><couleur>.png      cartes quantifiées sur la palette Neo6502 (16 couleurs)
  - deck.png                     planche de contrôle (52 cartes + dos)
  - cards.bin                    53 images 4 bpp (2 pixels/octet, quartet haut = 1er pixel),
                                 ordre : ♠A..K, ♥A..K, ♦A..K, ♣A..K, dos (index 52)
  - cards_<s|h|d|c>.gfx          fichier graphique Neo6502 : 13 cartes + dos, chaque carte =
                                 (W/16)*(H/16) blocs 16x16 ligne par ligne ; blocs 0-127 en tiles
                                 (id $00-$7F), suivants en sprites 16x16 (id $80 + index-128)

Usage : python3 tools/make_cards.py [--size 56x80] [--size 48x64] [--density 96]
Dépendances : ImageMagick (`convert`) pour rasteriser le SVG, Pillow.
"""
import os
import subprocess
import sys

from PIL import Image

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
SVG = os.path.join(ROOT, "assets", "source", "english_pattern_playing_cards_deck.svg")
BACK_SVG = os.path.join(ROOT, "assets", "source", "atlas_deck_card_back_blue_and_brown.svg")
OUT = os.path.join(ROOT, "assets", "cards")

# Palette par défaut du Neo6502 (firmware/common/include/interface/palette.h).
# 0 = transparent pour les sprites, 8 = noir opaque.
PALETTE = [
    (0, 0, 0), (255, 0, 77), (0, 228, 54), (255, 236, 39),
    (29, 43, 83), (126, 37, 83), (41, 173, 255), (255, 241, 232),
    (0, 0, 0), (95, 87, 79), (0, 135, 81), (255, 163, 0),
    (171, 82, 54), (131, 118, 156), (255, 204, 170), (194, 195, 199),
]
SUITS = "shdc"          # ordre des lignes de la planche : ♠ ♥ ♦ ♣
RANKS = "A23456789TJQK"  # ordre des colonnes (T = 10)
GREEN = (85, 170, 85)    # fond de la planche SVG


def rasterize(density):
    png = os.path.join(OUT, f"_deck_density{density}.png")
    if not os.path.exists(png):
        subprocess.run(["convert", "-density", str(density), SVG, png], check=True)
    return Image.open(png).convert("RGB")


def grid(im):
    """Détecte les colonnes/lignes de cartes (zones non vertes)."""
    w, h = im.size
    px = im.load()

    def runs(flags):
        r, s = [], None
        for i, bg in enumerate(flags):
            if not bg and s is None:
                s = i
            if bg and s is not None:
                r.append((s, i - 1))
                s = None
        if s is not None:
            r.append((s, len(flags) - 1))
        return r

    cols = runs([sum(px[x, y] == GREEN for y in range(0, h, 3)) > (h / 3) * 0.9 for x in range(w)])
    rows = runs([sum(px[x, y] == GREEN for x in range(0, w, 3)) > (w / 3) * 0.9 for y in range(h)])
    assert len(cols) == 13 and len(rows) == 4, (len(cols), len(rows))
    return cols, rows


def nearest(rgb):
    best, sel = None, 0
    for i in range(1, 16):  # jamais 0 (transparent)
        p = PALETTE[i]
        s = sum((a - b) ** 2 for a, b in zip(rgb, p))
        if best is None or s < best:
            best, sel = s, i
    return sel


# Les SVG source n'utilisent que 5 couleurs à plat ; les autres valeurs sont de
# l'anticrénelage. On classe chaque pixel vers la couleur de base la plus proche
# (le noir est favorisé pour préserver les traits fins), puis vers la palette Neo.
FLAT = [((255, 255, 255), 7), ((0, 0, 0), 8), ((255, 85, 85), 1),
        ((85, 85, 170), 13), ((255, 255, 85), 3), ((159, 159, 207), 15)]   # 13 lavande, 15 gris = liseré
BLACK_BIAS = 0.6   # < 1 : un pixel gris devient noir plus tôt


def classify(rgb):
    best, sel = None, 7
    for base, idx in FLAT:
        s = sum((a - b) ** 2 for a, b in zip(rgb, base))
        if idx == 8:
            s *= BLACK_BIAS
        if best is None or s < best:
            best, sel = s, idx
    return sel


def quantize(im, mapper=classify):
    """Image RGB -> image 'P' d'indices Neo6502."""
    out = Image.new("P", im.size)
    flat = [0] * 768
    for i, c in enumerate(PALETTE):
        flat[i * 3:i * 3 + 3] = c
    out.putpalette(flat)
    src = im.load()
    dst = out.load()
    for y in range(im.size[1]):
        for x in range(im.size[0]):
            dst[x, y] = mapper(src[x, y])
    return out


# Police pixel 5x7 pour les index de coin (les glyphes du SVG sont illisibles une fois réduits).
FONT = {
    "A": ["01110", "10001", "10001", "11111", "10001", "10001", "10001"],
    "2": ["01110", "10001", "00001", "00110", "01000", "10000", "11111"],
    "3": ["11110", "00001", "00001", "01110", "00001", "00001", "11110"],
    "4": ["00010", "00110", "01010", "10010", "11111", "00010", "00010"],
    "5": ["11111", "10000", "11110", "00001", "00001", "10001", "01110"],
    "6": ["01110", "10000", "10000", "11110", "10001", "10001", "01110"],
    "7": ["11111", "00001", "00010", "00100", "01000", "01000", "01000"],
    "8": ["01110", "10001", "10001", "01110", "10001", "10001", "01110"],
    "9": ["01110", "10001", "10001", "01111", "00001", "00001", "01110"],
    "J": ["00111", "00010", "00010", "00010", "00010", "10010", "01100"],
    "Q": ["01110", "10001", "10001", "10001", "10101", "10010", "01101"],
    "K": ["10001", "10010", "10100", "11000", "10100", "10010", "10001"],
    "1": ["010", "110", "010", "010", "010", "010", "111"],
    "0": ["010", "101", "101", "101", "101", "101", "010"],
}
INDEX_BOX = (0, 0, 11, 13)   # zone du rang en haut à gauche (56x80), le symbole reste en dessous


def draw_index(imgP, rank, colour):
    """Rang net dans les deux coins (bas droit tourné de 180°) ; seulement en 56x80."""
    w, h = imgP.size
    if (w, h) != (56, 80):
        return
    px = imgP.load()
    glyphs = ["1", "0"] if rank == "T" else [rank]
    for gx in range(INDEX_BOX[0], INDEX_BOX[2]):
        for gy in range(INDEX_BOX[1], INDEX_BOX[3]):
            px[gx, gy] = 7
            px[w - 1 - gx, h - 1 - gy] = 7
    x = 2
    for g in glyphs:
        rows = FONT[g]
        for gy, row in enumerate(rows):
            for gx, bit in enumerate(row):
                if bit == "1":
                    px[x + gx, 3 + gy] = colour
                    px[w - 1 - (x + gx), h - 1 - (3 + gy)] = colour
        x += len(rows[0]) + 1
    # coins arrondis : on efface les résidus gris du liseré
    for cx, cy in ((0, 0), (w - 1, 0), (0, h - 1), (w - 1, h - 1)):
        for dx in range(3):
            for dy in range(3):
                gx, gy = abs(cx - dx), abs(cy - dy)
                if px[gx, gy] == 15:
                    px[gx, gy] = 7


def make_back(w, h, density):
    """Dos de carte : rasterisation du SVG CC0 (fond transparent -> blanc)."""
    png = os.path.join(OUT, f"_back_density{density}.png")
    if not os.path.exists(png):
        subprocess.run(["convert", "-density", str(density), "-background", "white",
                        "-flatten", BACK_SVG, png], check=True)
    return Image.open(png).convert("RGB").resize((w, h), Image.LANCZOS)


def pack4bpp(imgP):
    """Image 'P' -> octets 4 bpp (2 pixels/octet, quartet haut = premier pixel)."""
    px = imgP.load()
    w, h = imgP.size
    data = bytearray()
    for y in range(h):
        for x in range(0, w, 2):
            data.append((px[x, y] << 4) | px[x + 1, y])
    return bytes(data)


def tiles16(imgP):
    """Découpe une image en tiles 16x16 (ordre ligne par ligne), chacune 128 octets."""
    w, h = imgP.size
    assert w % 16 == 0 and h % 16 == 0
    return [pack4bpp(imgP.crop((x, y, x + 16, y + 16)))
            for y in range(0, h, 16) for x in range(0, w, 16)]


MAX_TILES = 128  # Draw Image adresse les tiles $00-$7F, puis les sprites 16x16 $80-$BF


def write_gfx(path, tiles):
    """Format .gfx Neo6502 (cf. basic/scripts/gconvert.py) : en-tête 256 octets
    [0]=1 (format), [1]=nb tiles 16x16, [2]=nb sprites 16x16, [3]=nb sprites 32x32.
    Au-delà de 128 tiles, les blocs suivants sont stockés comme sprites 16x16 (même
    format de données) afin de rester adressables : id = $80 + (index - 128)."""
    assert len(tiles) <= MAX_TILES + 64
    header = bytearray(256)
    header[0] = 1
    header[1] = min(len(tiles), MAX_TILES)
    header[2] = max(0, len(tiles) - MAX_TILES)
    data = header + b"".join(tiles)
    assert len(data) < 32768 - 256, "Trop d'images pour la mémoire graphique (32 Ko)"
    open(path, "wb").write(data)


def build(size, density):
    w, h = size
    outdir = os.path.join(OUT, f"{w}x{h}")
    os.makedirs(os.path.join(outdir, "png"), exist_ok=True)
    deck = rasterize(density)
    cols, rows = grid(deck)

    cards = []  # (nom, image P) dans l'ordre ♠A..K ♥ ♦ ♣
    for si, (y0, y1) in enumerate(rows):
        for ri, (x0, x1) in enumerate(cols):
            card = deck.crop((x0, y0, x1 + 1, y1 + 1)).resize((w, h), Image.LANCZOS)
            q = quantize(card)
            draw_index(q, RANKS[ri], 1 if si in (1, 2) else 8)
            name = f"{RANKS[ri]}{SUITS[si]}"
            q.save(os.path.join(outdir, "png", name + ".png"))
            cards.append((name, q))
    back = quantize(make_back(w, h, density), nearest)
    back.save(os.path.join(outdir, "png", "back.png"))
    cards.append(("back", back))

    # planche de contrôle
    sheet = Image.new("RGB", (13 * (w + 2), 5 * (h + 2)), GREEN)
    for i, (_, q) in enumerate(cards):
        sheet.paste(q.convert("RGB"), ((i % 13) * (w + 2), (i // 13) * (h + 2)))
    sheet.save(os.path.join(outdir, "deck.png"))

    # binaire brut 4 bpp
    with open(os.path.join(outdir, "cards.bin"), "wb") as f:
        for _, q in cards:
            f.write(pack4bpp(q))

    # un .gfx par couleur (13 cartes + dos) — seulement si la taille est en tiles 16x16
    tiles = []
    for si, s in enumerate(SUITS):
        if w % 16 or h % 16:
            break
        tiles = []
        for _, q in cards[si * 13:si * 13 + 13] + [cards[52]]:
            tiles += tiles16(q)
        write_gfx(os.path.join(outdir, f"cards_{s}.gfx"), tiles)
    print(f"{w}x{h} : 53 images, {w * h // 2} octets/carte"
          + (f", {len(tiles)} blocs 16x16 par .gfx ({min(len(tiles), MAX_TILES)} tiles + "
             f"{max(0, len(tiles) - MAX_TILES)} sprites, {len(tiles) * 128 + 256} octets)" if tiles else ""))


def main(argv):
    sizes, density = [], 96
    i = 1
    while i < len(argv):
        if argv[i] == "--size":
            sizes.append(tuple(int(v) for v in argv[i + 1].split("x")))
            i += 2
        elif argv[i] == "--density":
            density = int(argv[i + 1])
            i += 2
        else:
            print(__doc__)
            return 1
    for size in sizes or [(32, 48), (48, 64), (56, 80)]:
        build(size, density)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
