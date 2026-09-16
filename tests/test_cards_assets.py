import os

ROOT = os.path.join(os.path.dirname(__file__), "..")
CARDS = os.path.join(ROOT, "assets", "cards")


def _check_size(w, h):
    d = os.path.join(CARDS, f"{w}x{h}")
    per_card = w * h // 2  # 4 bpp
    tiles_per_card = (w // 16) * (h // 16)
    # binaire brut : 52 cartes + dos
    assert os.path.getsize(os.path.join(d, "cards.bin")) == 53 * per_card
    # 53 PNG
    pngs = os.listdir(os.path.join(d, "png"))
    assert len(pngs) == 53 and "back.png" in pngs and "As.png" in pngs and "Kc.png" in pngs
    # un .gfx par couleur : 13 cartes + dos, en-tête Neo6502 256 octets
    for s in "shdc":
        data = open(os.path.join(d, f"cards_{s}.gfx"), "rb").read()
        assert data[0] == 1
        n = 14 * tiles_per_card
        assert data[1] == min(n, 128)          # tiles 16x16 ($00-$7F)
        assert data[2] == max(0, n - 128)      # suite en sprites 16x16 ($80+)
        assert data[2] <= 64 and data[3] == 0
        assert len(data) == 256 + n * 128
        assert len(data) < 32768 - 256


def test_cards_32x48():
    _check_size(32, 48)


def test_cards_48x64():
    _check_size(48, 64)


def test_nearest_palette():
    import sys
    sys.path.insert(0, os.path.join(ROOT, "tools"))
    from make_cards import nearest
    assert nearest((0, 0, 0)) == 8          # noir opaque, jamais 0 (transparent)
    assert nearest((255, 0, 77)) == 1       # rouge
    assert nearest((255, 241, 232)) == 7    # blanc
