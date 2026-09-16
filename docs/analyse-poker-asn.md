# Analyse du « Poker » ASN Diffusion pour Oric-1 (1983)

Date : 2026-09-16 — Sprint 0 (US-02).

## 1. Identification

| Élément | Valeur |
|---|---|
| Source | fichier fourni par bmarty (`poker-asn.tap`, téléchargé depuis oric.org, fiche n° 1760) |
| Copie locale | `reference/poker-oric/POKER_ASN.tap` (sha256 `ed2e8ae3…c504a22`, 5 548 octets) |
| Listing | `reference/poker-oric/POKER_ASN.bas` (179 lignes) |
| Format | TAP, 1 fichier `POKER`, BASIC, autorun, `$0501`–`$1A99` (5 529 octets) |
| Auteur | non indiqué dans le code (aucune signature) — **je ne sais pas** qui l'a écrit |
| Éditeur | ASN Diffusion (d'après oric.org) ; pas de mention dans le programme |

**Ce programme n'a rien de commun avec le Poker de V. Talvas / Loriciels** (voir
`analyse-poker-oric.md`) : ce n'est pas un poker à deux joueurs mais un **vidéo-poker**
(« poker à machine ») : un joueur, une mise, une donne, un écart, et un gain selon une
table de paiement, avec option « quitte ou double ».

## 2. Règles implémentées

- **52 cartes** (`DIM A(51)`, l. 610). Carte `X` : rang `Y = INT(X/4)` (0 = As, 1 = 2 … 9 = 10,
  10 = V, 11 = D, 12 = R), couleur `Z = X MOD 4` (0-1 rouges, 2-3 noires — cf. l. 1020-1040).
  L'As est affiché « 1 » (police `"1234567890VDR"`, l. 130).
- **Crédit** initial `C = 20` (l. 610). Touches (l. 640-700) : `M` = miser 1 (répétable),
  `C` = annuler la mise, `D` = donner. La mise précédente `M1` est reprise par défaut (l. 625-635).
- Mélange (l. 720) puis donne des 5 premières cartes ; évaluation immédiate (l. 745) avec
  mémorisation du rang initial `L`.
- **Écart** (l. 760-810) : touches `1`-`5` retournent la carte à remplacer, `C` = annuler l'écart,
  `M` = **augmenter la mise après avoir vu les cartes**, `D` = tirer. Les cartes remplacées
  sont prises dans `A(5…)` (l. 850-870).
- Paiement (l. 880-900) : si la mise a été augmentée après la donne (`M<>M1`), le gain n'est
  versé que si la main s'est **améliorée** (`L < GA`, l. 890) — c'est la parade anti-triche.
- **Quitte ou double** (l. 910-1080) : `Q` = encaisser `GA×M`, `D` = doubler : on devine
  la couleur (`R` rouge / `N` noir) de la carte suivante ; bon → `M` doublé et on peut
  recommencer, faux → mise perdue.
- Partie finie quand `C = 0` (l. 630 → 2700 « VOULEZ-VOUS REJOUER ? »).

## 3. Table de paiement (l. 530-550) et évaluateur (l. 2200-2550)

| `GA` | Libellé | Multiplicateur | Détection |
|---|---|---|---|
| 1 | PAIRE HABIL. | 1 | paire de V, D ou R (`Y(W)>9`, l. 2550) |
| 2 | DOUBLE PAIRE | 2 | deux paires adjacentes après tri (`V=2`) |
| 3 | BRELAN | 3 | l. 2330 |
| 4 | SUITE | 5 | rangs consécutifs (l. 2360-2390), As-10-V-D-R accepté |
| 5 | COULEUR | 7 | 5 mêmes `Z` (l. 2394) |
| 6 | FULL | 10 | l. 2310-2320 |
| 7 | CARRE | 40 | l. 2300 |
| 8 | QUINTE FLUSH | 100 | suite + couleur |
| 9 | FLUSH ROYALE | 500 | As-10-V-D-R + couleur (l. 2400) |

Méthode : tri à bulles des 5 cartes par index (l. 2200-2240), décomposition rang/couleur
(l. 2250), puis tests par positions sur la main triée (technique classique du poker à 5
cartes triées) ; `GA` est calculé par **chute à travers une cascade de `GA=GA+1`**
(l. 2410-2480) — le point d'entrée dans la cascade fixe la valeur. Le gain est ensuite
surligné dans la table à l'écran (l. 2505-2510) avec un son (l. 2490).

## 4. Affichage (HIRES)

- Titre « POKER » agrandi ×6 à partir de la police ROM, hachuré (l. 3000-3120).
- **Symboles de couleur** : 4 symboles de 3×3 caractères définis en `DATA` (l. 10-120),
  pokés dans la police HIRES standard `#9AE0-#9BFF` (caractères 92 à 127), l. 5.
- **Grands chiffres** : la police ROM (`#9800+code*8`) de `1234567890VDR` est agrandie ×2
  en 4 caractères dans la police alternative `#9D00` (l. 130-220), utilisée avec `CHAR …,1,1`.
- Carte : cadre 32×64 px (l. 400-450), pas de 48 px ; rang en haut à gauche, symboles au centre
  (l. 240-310) ; cartes retournées = fond hachuré `FILL … 255`.
- Panneau droit : ENJEU / CREDIT et menu contextuel (l. 570-680, 910-980).
- À la sortie (l. 2600, 2720) la police texte `#B6E0…` est restaurée depuis la ROM
  (`#FE50-#FF6F`) — car `HIMEM #977F` protège les polices mais le programme a écrasé
  les caractères 92-127.

## 5. Défauts / bugs repérés

1. **Paire d'As non payée** : l'As vaut 0, or le test « paire habillée » est `Y(W)>9` (l. 2550) ;
   seules V/D/R paient. Volontaire ou bug ? Je ne sais pas ; le libellé suggère un bug.
2. **l. 2405** `IF Y(0)=8 THEN GA=1` : une quinte flush 9-10-V-D-R est payée **FLUSH ROYALE**
   (500) au lieu de QUINTE FLUSH (100).
3. Mélange l. 720 : `INT(RND(1)*51)` ne tire jamais l'index 51 — biais mineur.
4. `2360` : si deux rangs identiques la suite est écartée correctement, mais l'As-bas
   (A-2-3-4-5) n'est pas reconnu comme suite (As = 0 puis 1,2,3,4 : `Y(0)=Y(1)-1` → reconnu
   en fait, l. 2380). Donc As-bas **accepté**, As-haut accepté aussi (l. 2390).
5. Aucune protection contre `M > C` autre que `2100` (mise incrémentale de 1) ; OK.

## 6. Comparaison des deux Poker Oric

| | ASN Diffusion (1983) | V. Talvas / Loriciels |
|---|---|---|
| Genre | vidéo-poker, 1 joueur vs table de gains | poker fermé, joueur vs ordinateur |
| Jeu | 52 cartes | 32 cartes |
| Enchères | mise fixe + quitte ou double | deux tours de relances, IA de mise |
| Taille | 5,5 Ko, 179 lignes | 12,4 Ko, 440 lignes |
| Évaluateur | main triée, cascade `GA` | histogramme + machine à états `S$` |
| Graphisme | polices redéfinies (×2, symboles 3×3) | tracés vectoriels `DATA` |

## 7. Enseignements pour Neo6502

- Les deux évaluateurs sont portables ; celui de l'ASN (main triée + tests positionnels)
  est le plus compact pour de l'assembleur 6502.
- Le vidéo-poker est un **premier jalon** naturel (pas d'IA) ; le poker 2 joueurs de Talvas
  peut réutiliser ensuite le même moteur de cartes.
- Astuce d'affichage réutilisable : rang ×2 et symboles 3×3 en tuiles — se transpose
  directement en tuiles/sprites Neo6502.
