#!/usr/bin/env python3
"""Valeurs attendues du diagnostic tools/diag/diag.c, calculées sur le PC.

Usage : python3 tools/diag/expected.py [build/diag.neo] [assets/cards/56x80/cards.bin]
Imprime les mêmes lignes que diag.neo sur la console Neo6502 (sauf VERSION).
"""
import os
import sys

CARD_BYTES = 2240
CARD_COUNT = 53


def h16(data, h=0):
    for b in data:
        h = (h * 31 + b) & 0xFFFF
    return h


def neo_payload(neo):
    """Contenu chargé en RAM d'un .neo à un seul bloc (en-tête de 14 octets)."""
    assert neo[:4] == b"\x03NEO", "pas un fichier .neo"
    size = neo[11] | (neo[12] << 8)
    assert neo[13] == 0, "commentaire non vide : en-tête plus long"
    return neo[14:14 + size]


def expected_lines(neo, cards):
    img = neo_payload(neo)
    lines = [f"IMAGE {len(img):04X} {h16(img):04X}", f"SIZE {len(cards)}"]
    row = ""
    for c in range(CARD_COUNT):
        row += f"C{c:02d} {h16(cards[c * CARD_BYTES:(c + 1) * CARD_BYTES]):04X}"
        row += "\n" if c % 5 == 4 else "  "
        if c % 5 == 4:
            lines.append(row.rstrip("\n"))
            row = ""
    lines.append(row.rstrip())
    lines.append(f"BYTES {h16(cards[:CARD_BYTES]):04X}")
    lines.append(f"SEQ {h16(cards):04X}")
    lines.append("END")
    return lines


if __name__ == "__main__":
    root = os.path.join(os.path.dirname(__file__), "..", "..")
    neo = sys.argv[1] if len(sys.argv) > 1 else os.path.join(root, "build", "diag.neo")
    cards = sys.argv[2] if len(sys.argv) > 2 else os.path.join(root, "assets", "cards", "56x80", "cards.bin")
    print("\n".join(expected_lines(open(neo, "rb").read(), open(cards, "rb").read())))
