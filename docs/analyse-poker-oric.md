# Analyse du jeu « Poker » pour Oric (V. Talvas / Loriciels)

Date : 2026-09-16 — Sprint 0 (étude de l'existant).

## 1. Provenance et identification

| Élément | Valeur |
|---|---|
| Source | TOSEC « Tangerine Oric-1 & Atmos » (2012-04-23), archive.org, fichier `ZZZ-UNK-Poker (F) (19xx).tap` |
| Copie locale | `reference/poker-oric/POKER_Talvas_Loriciels.tap` (sha256 `8abf911a…a1a1c936`) |
| Listing détokenisé | `reference/poker-oric/POKER_Talvas_Loriciels.bas` (440 lignes) |
| Format | TAP Oric, 1 seul fichier `POKER`, BASIC, autorun `$C7`, `$0501`–`$359F` (12 447 octets) |
| Auteur | V. Talvas (« V.TALVAS PRESENTE » l. 2, « V.TALVAS ADAP. » l. 15030) |
| Éditeur | **Loriciels** (mention de propriété lignes 20002-20005) |
| Fiche oric.org correspondante | n° 2065 « Poker », auteur V Talvas |

### ⚠️ Ce n'est PAS (à coup sûr) le « Poker » d'ASN Diffusion

La base oric.org recense 4 « Poker » :

| id | Éditeur | Auteur | Notes |
|---|---|---|---|
| 768 | – | B. d'Armagnac | TAP « modified by JB » |
| **1760** | **ASN Diffusion** | (non renseigné) | 1983, « Oric-1 ONLY », pas de manuel |
| 2061 | – | Frédéric Daniel | |
| 2065 | UNKNOWN | V. Talvas | ← correspond au TAP analysé ici |

Le téléchargement de la fiche 1760 (ASN) exige un compte oric.org ; aucun miroir public
n'a été trouvé (TOSEC ne contient qu'un seul Poker). **Je ne sais pas** si le programme
ASN 1983 est le même que celui de Talvas/Loriciels ; le listing indique Loriciels, pas ASN.
Pour obtenir la version ASN, il faut se connecter sur
<https://www.oric.org/software/poker-1760.html>.

## 2. Règles implémentées (d'après le code et l'aide intégrée l. 15000-15930)

- Jeu de **32 cartes** (7, 8, 9, 10, Valet, Dame, Roi, As × 4 couleurs), indices 1..32 :
  `valeur = INT((E-1)/4)` (0 = 7 … 7 = As), couleur = `(E-1) MOD 4` (♠ `}`, ♥ `[`, ♦ `{`, ♣ `]`).
- **2 joueurs** : humain (`JJ`, 1000 F) contre ordinateur (`OO`, 1000 F). Pot `PO` initial 10 F.
- Une manche : donne de 5 cartes chacun (l. 220-232, tirage sans remise via tableau `N()`),
  **1er tour d'enchères**, **échange de 0 à 3 cartes** (l. 13000), **2e tour d'enchères**,
  **abattage** (l. 16200).
- Enchères : relance minimale de **10 F** au-dessus de l'adversaire, mise maximale **100 F**,
  miser 0 = passer, égaliser = passer à l'étape suivante (`PR` compte les égalisations ; à `PR=2`
  on compare les mains).
- Fin de partie quand un joueur a moins de 10 F (l. 18550 / 18700) ; nombre de manches `SW`.

## 3. Évaluation des mains (l. 4000-4310)

Chaque main reçoit une valeur numérique `U` (plus grand = meilleur) :

| Combinaison | Base `U` | Détail |
|---|---|---|
| Rien | somme des indices | `E(1)+…+E(5)` |
| Paire | 1000 | `+100*(rang+1) + somme indices` |
| Double paire | 2000 | idem |
| Brelan | 3000 | idem |
| Quinte | 4000 | `+ indice de la plus petite carte` |
| Full | 5000 | idem paire |
| **Couleur** | **6000** | `+ indice` — la couleur bat le full (règle du poker à 32 cartes) |
| Carré | 7000 | |
| Quinte flush | 8000 | |

Algorithme :
1. Recherche de la carte d'indice minimal (l. 4010, `D`).
2. Quinte / quinte flush : on construit les 5 cartes attendues `A$(E(D)+4*P)` et on compte
   les correspondances exactes (`S`, quinte flush) ou de rang seul (`T`, quinte) ; vérification
   qu'aucun rang n'est doublé (l. 4090).
3. Couleur : 5 mêmes suffixes (l. 4150-4190).
4. Histogramme des rangs `V(0..7)` puis machine à états sur `S$` (PAIRE → DOUBLE PAIRE,
   BRELAN+PAIRE → FULL, etc.).
5. Les libellés (`T$`, `U$`) servent à l'affichage : « FULL AUX AS PAR LES DAMES ».

## 4. Intelligence artificielle de l'ordinateur

- **Mise** (l. 5030-5080) : `DISPO = OO / (20000/UU) * 1.6` — budget proportionnel à la force
  de la main `UU` et à la caisse ; mise de base `BAS = DISPO / (RND*4+6)` ; relance `MJ+10+RND*10`
  si nécessaire. Il passe si `DISPO - MJ < -40` sauf main forte (l. 18100), égalise sinon.
  Le premier joueur est tiré au sort (`TI`).
- **Échange de cartes** (l. 16000-16160) : main ≥ quinte → ne change rien ; paire/brelan… →
  garde les cartes des rangs `XY$`/`XZ$` et change les autres ; rien → change 3 cartes.
- Il ne bluffe pas ; il « connaît » sa main uniquement (l. 5010 évalue les cartes 6-10).

## 5. Affichage (HIRES 240×200)

- Cartes 39×71 px, pas de 49 px (l. 240-300), dessinées avec `CURSET/CURMOV/DRAW/CHAR`.
- Symboles de couleur : 4 glyphes redéfinis par `POKE` à 46808/46824/47064/47080
  (`$B6D8`, `$B6E8`, `$B7D8`, `$B7E8` = caractères `[`, `]`, `{`, `}` du jeu de caractères
  texte à `$B400`).
- Figures V/D/R : tracés vectoriels stockés en `DATA` (l. 2995-3044, terminés par 777),
  `99` = préfixe « DRAW », sinon `CURMOV` ; dessin puis miroir (`A3=-1`) pour la moitié
  inférieure (l. 6000-6630).
- Cartes 7/8 : disposition des symboles l. 370-385 ; 9/10 : l. 335-352 ; As : l. 7000.
- Texte : accents encodés `{`=é, `}`=è, `@`=à, `\`=ç (jeu de caractères Oric-1 français).
- Ligne d'état : « POKER » poké en 48013-48025 (`$BB8D`…, ligne 0 de l'écran texte).

## 6. Défauts / bugs repérés dans le code

1. **l. 5090** `IF JJ<10 THEN PRINT 18550` : imprime « 18550 » au lieu de `GOTO 18550`
   (le message « Vous êtes lessivé » n'est jamais atteint par ce chemin).
2. **l. 16035-16055** (l'ordinateur change 3 cartes sans combinaison) : la position `YO` est
   tirée au hasard (peut tomber deux fois sur la même carte), la nouvelle carte n'est pas
   marquée dans `N()` et l'ancienne n'est pas libérée → doublons possibles avec le joueur.
3. **l. 4010** : la boucle cherche en fait la carte d'indice **minimal** (commentaire trompeur),
   ce qui est bien ce qu'exige le test de quinte.
4. Pas de quinte « As-7-8-9-10 » ; l'As n'est jamais bas.
5. Ligne 5107 (« vous ne possédez que ») est testée **après** la soustraction en 5100.
6. `POKE 853,230` (l. 4) : effet non identifié — je ne sais pas ce qu'il fait.

## 7. Enseignements pour le portage Neo6502

- Modèle de données minimal : 32 indices de cartes, tableau « déjà tirée », histogramme
  de rangs — trivial à porter en 6502 asm ou en C (llvm-mos / cc65).
- L'évaluateur de mains est la partie la plus réutilisable ; le barème `U` (§3) peut être
  conservé tel quel (il respecte la hiérarchie du poker à 32 cartes, couleur > full).
- L'IA de mise est un simple ratio caisse/force ; à réécrire proprement mais l'esprit est simple.
- Les graphismes vectoriels (`DATA` 2995-3044) peuvent être rejoués sur Neo6502 avec les
  primitives ligne/cercle de l'API graphique, ou remplacés par des sprites.
