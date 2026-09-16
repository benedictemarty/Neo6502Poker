#!/usr/bin/env python3
"""Lecture de fichiers TAP Oric et détokenisation du BASIC Oric 1.0/1.1.

Usage : python3 tools/oric_tap.py fichier.tap [--list] [--bas SORTIE]
"""
import sys

# Table des tokens BASIC Oric (codes $80..$FF) — même jeu en ROM 1.0 et 1.1.
TOKENS = [
    "END", "EDIT", "STORE", "RECALL", "TRON", "TROFF", "POP", "PLOT", "PULL",
    "LORES", "DOKE", "REPEAT", "UNTIL", "FOR", "LLIST", "LPRINT", "NEXT", "DATA",
    "INPUT", "DIM", "CLS", "READ", "LET", "GOTO", "RUN", "IF", "RESTORE", "GOSUB",
    "RETURN", "REM", "HIMEM", "GRAB", "RELEASE", "TEXT", "HIRES", "SHOOT",
    "EXPLODE", "ZAP", "PING", "SOUND", "MUSIC", "PLAY", "CURSET", "CURMOV",
    "DRAW", "CIRCLE", "PATTERN", "FILL", "CHAR", "PAPER", "INK", "STOP", "ON",
    "WAIT", "CLOAD", "CSAVE", "DEF", "POKE", "PRINT", "CONT", "LIST", "CLEAR",
    "GET", "CALL", "!", "NEW", "TAB(", "TO", "FN", "SPC(", "@", "AUTO", "ELSE",
    "THEN", "NOT", "STEP", "+", "-", "*", "/", "^", "AND", "OR", ">", "=", "<",
    "SGN", "INT", "ABS", "USR", "FRE", "POS", "HEX$", "&", "SQR", "RND", "LN",
    "EXP", "COS", "SIN", "TAN", "ATN", "PEEK", "DEEK", "LOG", "LEN", "STR$",
    "VAL", "ASC", "CHR$", "PI", "TRUE", "FALSE", "KEY$", "SCRN", "POINT",
    "LEFT$", "RIGHT$", "MID$",
]


def parse_tap(data: bytes):
    """Retourne la liste des fichiers contenus dans un TAP : dict(name, type, autorun, start, end, body)."""
    files = []
    i = 0
    while True:
        j = data.find(b"\x24", i)
        if j < 0:
            break
        k = j + 1
        hdr = data[k:k + 9]
        if len(hdr) < 9:
            break
        typ, auto = hdr[2], hdr[3]
        end = hdr[4] << 8 | hdr[5]
        start = hdr[6] << 8 | hdr[7]
        name_end = data.find(b"\x00", k + 9)
        name = data[k + 9:name_end].decode("latin-1")
        body = name_end + 1
        size = end - start + 1
        files.append({
            "name": name, "type": "BASIC" if typ == 0 else "MC",
            "autorun": auto, "start": start, "end": end,
            "body": data[body:body + size],
        })
        i = body + size
    return files


def detokenize(body: bytes):
    """Détokenise un programme BASIC Oric (liste de chaînes 'numéro texte')."""
    lines = []
    i = 0
    while i + 4 <= len(body):
        nxt = body[i] | body[i + 1] << 8
        if nxt == 0:
            break
        ln = body[i + 2] | body[i + 3] << 8
        j = i + 4
        s = ""
        instr = False
        while body[j] != 0:
            c = body[j]
            if c == 0x22:
                instr = not instr
            if c >= 0x80 and not instr:
                t = TOKENS[c - 0x80]
                s += t + (" " if t.isalpha() or t.endswith("$") else "")
            else:
                s += chr(c) if 32 <= c < 127 else f"<{c:02x}>"
            j += 1
        lines.append(f"{ln} {s}")
        i = j + 1
    return lines


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 1
    data = open(argv[1], "rb").read()
    files = parse_tap(data)
    for n, f in enumerate(files):
        print(f"file {n}: {f['name']!r} {f['type']} autorun={f['autorun']:02x} "
              f"start=${f['start']:04X} end=${f['end']:04X} size={len(f['body'])}")
    if "--bas" in argv:
        out = argv[argv.index("--bas") + 1]
        lines = detokenize(files[0]["body"])
        open(out, "w").write("\n".join(lines) + "\n")
        print(f"{len(lines)} lignes -> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
