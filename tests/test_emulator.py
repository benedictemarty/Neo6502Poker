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
CARDS = os.path.join(ROOT, "assets", "cards", "56x80", "cards.bin")
GOLDEN = os.path.join(ROOT, "tests", "golden")

pytestmark = pytest.mark.skipif(not os.path.exists(EMU), reason="Phosphoneo absent")


@pytest.fixture(scope="module")
def sd():
    subprocess.run(["make", "-s", "neo"], cwd=ROOT, check=True)
    d = tempfile.mkdtemp(prefix="neo-sd-")
    shutil.copy(CARDS, d)
    yield d
    shutil.rmtree(d)


def run(sd, keys, cycles, out, at=8_000_000):
    cmd = [EMU, NEO, "--headless", "--storage", sd, "--cycles", str(cycles),
           "--type-keys", f"{at}:{keys}", "--screenshot", out]
    r = subprocess.run(cmd, capture_output=True, text=True, timeout=600)
    assert r.returncode == 0, r.stderr[-2000:]
    return open(out, "rb").read()


@pytest.mark.parametrize("name,keys,cycles,at", [
    ("title", "", 7_000_000, 8_000_000),             # écran de présentation (français)
    ("title_en", "E", 12_000_000, 8_000_000),        # écran de présentation en anglais
    ("help1", "H", 16_000_000, 8_000_000),           # pages d'aide
    ("help2", "H ", 16_000_000, 8_000_000),
    ("help3", "H  ", 16_000_000, 8_000_000),
    ("help4", "H   ", 16_000_000, 8_000_000),
    ("help5", "H    ", 16_000_000, 8_000_000),
    ("help5_en", "EH    ", 16_000_000, 8_000_000),
    ("help3_en", "EH  ", 16_000_000, 8_000_000),
    ("holding", "F MMD", 60_000_000, 8_000_000),     # français, mise 2, donne
    ("showdown", "F MMD5D", 90_000_000, 8_000_000),  # change la 5e carte, abattage
    ("showdown_en", "E MMD5D", 90_000_000, 8_000_000),  # même partie en anglais
    # graine différente (espace à 9,35 M cycles) : full, 4 doublements gagnés puis encaissement
    ("double_collect", " MMDDDNDNDRDRDRQ", 60_000_000, 9_350_000),
    # même donne, 4e pari perdu (N sur un K de carreau)
    ("double_lost", " MMDDDNDNDRDRDN", 60_000_000, 9_350_000),
    # même donne, capture pendant les 3 s de clignotement : la ligne FULL est en surbrillance
    ("win_blink", " MMDD", 15_600_000, 9_350_000),
])
def test_golden(sd, name, keys, cycles, at, tmp_path):
    out = str(tmp_path / f"{name}.ppm")
    got = run(sd, keys, cycles, out, at)
    ref = open(os.path.join(GOLDEN, f"{name}.ppm"), "rb").read()
    assert got == ref, f"capture différente de tests/golden/{name}.ppm (voir {out})"


def test_cards_in_package_folder(tmp_path):
    """cards.bin absent du répertoire courant mais présent dans poker/ (paquet Prophet lancé
    par ProphetGui sans changement de répertoire) : l'écran-titre est identique."""
    subprocess.run(["make", "-s", "neo"], cwd=ROOT, check=True)
    d = tmp_path / "storage"; (d / "poker").mkdir(parents=True)
    shutil.copy(CARDS, d / "poker")
    got = run(str(d), "", 7_000_000, str(tmp_path / "sub.ppm"))
    assert got == open(os.path.join(GOLDEN, "title.ppm"), "rb").read()


def test_cards_missing(tmp_path):
    """Sans cards.bin nulle part : message explicite (golden nocards) au lieu de cartes brouillées."""
    subprocess.run(["make", "-s", "neo"], cwd=ROOT, check=True)
    d = tmp_path / "storage"; d.mkdir()
    got = run(str(d), "", 7_000_000, str(tmp_path / "nocards.ppm"))
    assert got == open(os.path.join(GOLDEN, "nocards.ppm"), "rb").read()
