"""Compile et exécute les tests natifs du moteur C (gcc)."""
import os
import subprocess

ROOT = os.path.join(os.path.dirname(__file__), "..")


def test_engine_native():
    r = subprocess.run(["make", "-s", "test-engine"], cwd=ROOT, capture_output=True, text=True)
    print(r.stdout)
    assert r.returncode == 0, r.stdout + r.stderr
    assert "OK" in r.stdout
