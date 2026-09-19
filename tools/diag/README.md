# Diagnostic du stockage sur la carte réelle

Cartes brouillées et textes illisibles **sur la carte** alors que `neo` et Phosphoneo sont
corrects : le jeu ne fait rien de différent, seuls le chargement du `.neo` et la lecture de
`cards.bin` passent par un chemin propre à la carte (clé USB / SD, TinyUSB, FatFs).
`diag.neo` mesure ces deux chemins avec des empreintes 16 bits.

## Sur le PC
```
make diag            # construit build/diag.neo et imprime les valeurs attendues
make test            # tests/test_diag.py vérifie que Phosphoneo donne ces valeurs
```

## Sur la carte
1. Copier `build/diag.neo` et `assets/cards/56x80/cards.bin` à la racine du stockage
   (le même `cards.bin` que celui du PC : `md5sum` identique).
2. Lancer `diag.neo` comme `poker.neo` (menu de démarrage ou `load "diag.neo"` sous BASIC).
3. Comparer l'écran à la sortie de `make diag`, ligne à ligne.

| Ligne | Ce qu'elle mesure | Si elle diffère |
|---|---|---|
| `VERSION` | firmware (API 1,1) | à noter avec le résultat |
| `IMAGE len hash` | l'image chargée en RAM (`$0200`, `len` octets) | le chargeur `.neo` (lecture octet par octet) ou le `.neo` copié n'est pas celui du PC |
| `SIZE` | taille de `cards.bin` | fichier absent ou différent sur le stockage |
| `Cnn hash` | chaque carte : seek + lecture d'un bloc de 2240 octets (le chemin du jeu) | lecture par blocs non alignés sur les secteurs (FatFs `FF_FS_TINY`, pilote MSC) |
| `BYTES` | carte 0 relue octet par octet | lecture par octet (celle du chargeur) |
| `SEQ` | tout le fichier par blocs de 512 octets | lecture alignée / multi-secteurs |
| `OPEN ERR 11` | le fichier ne s'ouvre pas (`11` = introuvable) | `cards.bin` absent du répertoire courant — cas constaté sur carte le 2026-09-19 (paquet Prophet lancé depuis `poker/`) ; le jeu ≥ 0.9.2 cherche aussi `poker/cards.bin` et affiche un message sinon |

Une différence sur `IMAGE` seule explique les textes brouillés du jeu (tables de chaînes
mal chargées) ; une différence sur les `Cnn` seuls explique les cartes brouillées.
