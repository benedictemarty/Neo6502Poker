import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "tools"))
from oric_tap import TOKENS, detokenize, parse_tap  # noqa: E402

ROOT = os.path.join(os.path.dirname(__file__), "..")
TAP = os.path.join(ROOT, "reference", "poker-oric", "POKER_Talvas_Loriciels.tap")
BAS = os.path.join(ROOT, "reference", "poker-oric", "POKER_Talvas_Loriciels.bas")


def test_table_tokens_complete():
    assert len(TOKENS) == 119  # $80..$F6 (MID$ est le dernier token)


def test_parse_tap_poker():
    files = parse_tap(open(TAP, "rb").read())
    assert len(files) == 1
    f = files[0]
    assert f["name"] == "POKER"
    assert f["type"] == "BASIC"
    assert f["autorun"] == 0xC7
    assert (f["start"], f["end"]) == (0x0501, 0x359F)
    assert len(f["body"]) == 0x359F - 0x0501 + 1


def test_detokenize_matches_reference_listing():
    body = parse_tap(open(TAP, "rb").read())[0]["body"]
    lines = detokenize(body)
    assert len(lines) == 440
    assert lines[0] == "1 HIMEM #97FF:OO=1E3:JJ=OO:PAPER 0:INK 7"
    assert lines == open(BAS).read().splitlines()


def test_detokenize_minimal_program():
    # 10 PRINT "A"   -> next(2) num(2) $BA '"' 'A' '"' 0 ; fin 0 0
    body = bytes([0x0B, 0x05, 10, 0, 0xBA, 0x22, 0x41, 0x22, 0, 0, 0])
    assert detokenize(body) == ['10 PRINT "A"']


def test_parse_tap_asn():
    tap = os.path.join(ROOT, "reference", "poker-oric", "POKER_ASN.tap")
    f = parse_tap(open(tap, "rb").read())[0]
    assert f["name"] == "POKER" and f["type"] == "BASIC"
    assert (f["start"], f["end"]) == (0x0501, 0x1A99)
    lines = detokenize(f["body"])
    assert len(lines) == 179
    assert lines[0].startswith("3 HIMEM #977F:CLEAR :TEXT :PAPER 0:INK 1:HIRES ")
    bas = os.path.join(ROOT, "reference", "poker-oric", "POKER_ASN.bas")
    assert lines == open(bas).read().splitlines()
