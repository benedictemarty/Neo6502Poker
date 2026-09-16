"""Tests d'intégration sur Phosphoneo (émulateur Neo6502 headless, ~/Phosphoneo).

Le jeu est déterministe dans l'émulateur (graine = compteur système au démarrage),
donc les captures de référence de tests/golden/ doivent être reproduites à l'identique.
Ignoré si l'émulateur n'est pas présent.
"""
import os
import shutil
import subprocess
import tempfile

import pytest

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
EMU = os.path.expanduser("~/Phosphoneo/build/phosphoneo")
NEO = os.path.join(ROOT, "build", "poker.neo")
CARDS = os.path.join(ROOT, "assets", "cards", "48x64", "cards.bin")
GOLDEN = os.path.join(ROOT, "tests", "golden")

pytestmark = pytest.mark.skipif(not os.path.exists(EMU), reason="Phosphoneo absent")


@pytest.fixture(scope="module")
def sd():
    subprocess.run(["make", "-s", "neo"], cwd=ROOT, check=True)
    d = tempfile.mkdtemp(prefix="neo-sd-")
    shutil.copy(CARDS, d)
    yield d
    shutil.rmtree(d)


def run(sd, keys, cycles, out):
    cmd = [EMU, NEO, "--headless", "--storage", sd, "--cycles", str(cycles),
           "--type-keys", f"6000000:{keys}", "--screenshot", out]
    r = subprocess.run(cmd, capture_output=True, text=True, timeout=600)
    assert r.returncode == 0, r.stderr[-2000:]
    return open(out, "rb").read()


@pytest.mark.parametrize("name,keys,cycles", [
    ("holding", "MMD", 60_000_000),      # mise 2, donne : 5s 6s 3s As Kh
    ("showdown", "MMD5D", 90_000_000),   # change la 5e carte, abattage : RIEN +0
])
def test_golden(sd, name, keys, cycles, tmp_path):
    out = str(tmp_path / f"{name}.ppm")
    got = run(sd, keys, cycles, out)
    ref = open(os.path.join(GOLDEN, f"{name}.ppm"), "rb").read()
    assert got == ref, f"capture différente de tests/golden/{name}.ppm (voir {out})"
