"""Le diagnostic de stockage (tools/diag) donne dans Phosphoneo les valeurs calculées sur le PC.

Sur la carte réelle, ces mêmes lignes doivent apparaître ; toute différence localise le défaut
(chargement du .neo, lecture par blocs, lecture par octet ou lecture séquentielle).
"""
import os
import shutil
import subprocess
import sys
import tempfile

import pytest

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, os.path.join(ROOT, "tools", "diag"))
from expected import expected_lines, neo_payload, h16  # noqa: E402

EMU = os.path.expanduser("~/Phosphoneo/build/phosphoneo")
NEO = os.path.join(ROOT, "build", "diag.neo")
CARDS = os.path.join(ROOT, "assets", "cards", "56x80", "cards.bin")


def test_hash_reference():
    assert h16(b"") == 0 and h16(b"\x01") == 1 and h16(b"\x01\x02") == 33


def test_neo_header():
    subprocess.run(["make", "-s", NEO], cwd=ROOT, check=True)
    neo = open(NEO, "rb").read()
    assert neo[9] | (neo[10] << 8) == 0x0200          # adresse de chargement
    assert len(neo_payload(neo)) == len(neo) - 14      # un seul bloc, sans commentaire


@pytest.mark.skipif(not os.path.exists(EMU), reason="Phosphoneo absent")
def test_diag_matches_expected(tmp_path):
    subprocess.run(["make", "-s", NEO], cwd=ROOT, check=True)
    sd = tempfile.mkdtemp(prefix="neo-diag-")
    try:
        shutil.copy(CARDS, sd)
        out = str(tmp_path / "console.txt")
        r = subprocess.run([EMU, NEO, "--headless", "--storage", sd, "--cycles", "80000000",
                            "--screenshot-text", out], capture_output=True, text=True, timeout=600)
        assert r.returncode == 0, r.stderr[-2000:]
    finally:
        shutil.rmtree(sd)
    got = [l.rstrip() for l in open(out).read().splitlines() if l.strip()]
    assert got[0].startswith("VERSION ")
    assert got[1:] == expected_lines(open(NEO, "rb").read(), open(CARDS, "rb").read())
