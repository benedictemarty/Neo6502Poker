#include <stddef.h>
#include "neo/lang.h"

lang_t lang = LANG_FR;

static const char *const STR[2][S_COUNT] = {
    { "VIDEO-POKER POUR NEO6502", "F : FRANCAIS    E : ENGLISH", "CREDIT", "ENJEU",
      "M MISER  C ANNULER  D DONNER", "1-5 GARDER/CHANGER   D TIRER",
      "ESPACE : MANCHE SUIVANTE", "VOUS ETES LESSIVE.  Q QUITTER",
      "GARDE", "CHANGE", "Q QUITTER",
      "Q : ENCAISSER   D : DOUBLER", "R : ROUGE   N : NOIR", "GAIN",
      "GAGNE ! GAIN DOUBLE", "PERDU...",
      "H : AIDE", "ESPACE : JOUER", "ESPACE : SUITE     Q : RETOUR", "ESPACE : RETOUR", "LOUPE" },
    { "VIDEO POKER FOR THE NEO6502", "F : FRANCAIS    E : ENGLISH", "CREDIT", "BET",
      "M BET  C CANCEL  D DEAL", "1-5 HOLD/DISCARD   D DRAW",
      "SPACE : NEXT HAND", "YOU ARE BROKE.  Q QUIT",
      "HOLD", "DRAW", "Q QUIT",
      "Q : COLLECT   D : DOUBLE", "R : RED   B : BLACK", "WIN",
      "RIGHT! WIN DOUBLED", "LOST...",
      "H : HELP", "SPACE : PLAY", "SPACE : NEXT     Q : BACK", "SPACE : BACK", "ZOOM" },
};

static const char *const HANDS[2][HAND_COUNT] = {
    { "RIEN", "PAIRE HABILLEE", "DOUBLE PAIRE", "BRELAN", "SUITE",
      "COULEUR", "FULL", "CARRE", "QUINTE FLUSH", "FLUSH ROYALE" },
    { "NOTHING", "JACKS OR BETTER", "TWO PAIR", "THREE OF A KIND", "STRAIGHT",
      "FLUSH", "FULL HOUSE", "FOUR OF A KIND", "STRAIGHT FLUSH", "ROYAL FLUSH" },
};

const char *T(uint8_t id) { return STR[lang][id]; }
const char *hand_label(hand_cat_t cat) { return HANDS[lang][cat]; }

/* ---- pages d'aide (53 colonnes max, ASCII) ---------------------------------- */
static const char *const HELP_TITLES[2][HELP_PAGES] = {
    { "COMMENT JOUER", "L'ECART", "QUITTE OU DOUBLE", "LES GAINS", "CREDITS" },
    { "HOW TO PLAY", "THE DRAW", "DOUBLE OR NOTHING", "THE PAYOUTS", "CREDITS" },
};

static const char *const HELP_FR_1[] = {
    "Vous jouez seul contre la machine, avec 20 credits.",
    "Une manche se deroule en trois temps :",
    " 1. la mise : M ajoute 1 credit a l'enjeu (10 maxi),",
    "    C annule la mise, D donne les cartes.",
    "    Sans mise, D reprend la mise precedente.",
    " 2. l'ecart : vous changez de 0 a 5 cartes (page 2).",
    " 3. l'abattage : la main est payee selon la table",
    "    des gains (page 4), puis quitte ou double (p. 3).",
    "La partie s'arrete quand le credit tombe a zero.",
    NULL };
static const char *const HELP_EN_1[] = {
    "You play alone against the machine, with 20 credits.",
    "A hand has three steps:",
    " 1. the bet: M adds 1 credit to the bet (10 max),",
    "    C cancels the bet, D deals the cards.",
    "    With no bet, D repeats the previous bet.",
    " 2. the draw: change 0 to 5 cards (page 2).",
    " 3. the showdown: the hand is paid from the payout",
    "    table (page 4), then double or nothing (page 3).",
    "The game ends when your credit reaches zero.",
    NULL };
static const char *const HELP_FR_2[] = {
    "Apres la donne, chaque carte porte GARDE.",
    "Les touches 1 a 5 basculent la carte correspondante",
    "entre GARDE (conservee) et CHANGE (remplacee).",
    "D tire les nouvelles cartes et compare la main.",
    NULL };
static const char *const HELP_EN_2[] = {
    "After the deal, every card is marked HOLD.",
    "Keys 1 to 5 toggle the matching card between",
    "HOLD (kept) and DRAW (replaced).",
    "D draws the new cards and evaluates the hand.",
    NULL };
static const char *const HELP_FR_3[] = {
    "Apres un gain, Q encaisse ou D propose le pari :",
    "les cartes se retournent, devinez la couleur de",
    "la carte suivante avec R (rouge) ou N (noir).",
    "Bonne reponse : le gain double, et vous pouvez",
    "recommencer. Mauvaise reponse : le gain est perdu.",
    NULL };
static const char *const HELP_EN_3[] = {
    "After a win, Q collects or D offers the bet:",
    "the cards turn over; guess the colour of the",
    "next card with R (red) or B (black).",
    "Right: the win doubles and you may go again.",
    "Wrong: the win is lost.",
    NULL };
static const char *const HELP_FR_4[] = {
    "Le gain est le multiplicateur de la table x l'enjeu.",
    "PAIRE HABILLEE  paire de V, D, R ou A            x1",
    "DOUBLE PAIRE    deux paires                      x2",
    "BRELAN          trois cartes de meme valeur      x3",
    "SUITE           cinq valeurs qui se suivent      x5",
    "COULEUR         cinq cartes de la meme couleur   x7",
    "FULL            un brelan et une paire          x10",
    "CARRE           quatre cartes de meme valeur    x40",
    "QUINTE FLUSH    suite de la meme couleur       x100",
    "FLUSH ROYALE    10 V D R A de la meme couleur  x500",
    NULL };
static const char *const HELP_EN_4[] = {
    "The win is the table multiplier times the bet.",
    "JACKS OR BETTER pair of J, Q, K or A             x1",
    "TWO PAIR        two pairs                        x2",
    "THREE OF A KIND three cards of the same rank     x3",
    "STRAIGHT        five ranks in sequence           x5",
    "FLUSH           five cards of the same suit      x7",
    "FULL HOUSE      three of a kind and a pair      x10",
    "FOUR OF A KIND  four cards of the same rank     x40",
    "STRAIGHT FLUSH  a straight in one suit         x100",
    "ROYAL FLUSH     10 J Q K A in one suit         x500",
    NULL };

static const char *const HELP_FR_5[] = {
    "JEU        Programmation Benedicte MARTY, 2026,",
    "           avec l'aide de Claude Code (Anthropic).",
    "           D'apres le video-poker d'ASN Diffusion",
    "           pour Oric 1 (1983). Licence EUPL-1.2.",
    "MACHINE    Neo6502 : carte Olimex (W65C02S + RP2040),",
    "           firmware et API de Paul Robson et",
    "           contributeurs (licence MIT).",
    "OUTILS     Compilateur llvm-mos (Apache-2.0),",
    "           emulateur Phosphoneo pour les tests.",
    "CARTES     Dessins de Dmitry Fomin (CC0),",
    "           Wikimedia Commons.",
    "SONS       Effets sonores du firmware Neo6502.",
    NULL };
static const char *const HELP_EN_5[] = {
    "GAME       Programming Benedicte MARTY, 2026,",
    "           with the help of Claude Code (Anthropic).",
    "           After the ASN Diffusion video poker",
    "           for the Oric 1 (1983). EUPL-1.2 licence.",
    "MACHINE    Neo6502: Olimex board (W65C02S + RP2040),",
    "           firmware and API by Paul Robson and",
    "           contributors (MIT licence).",
    "TOOLS      llvm-mos compiler (Apache-2.0),",
    "           Phosphoneo emulator for the tests.",
    "CARDS      Artwork by Dmitry Fomin (CC0),",
    "           Wikimedia Commons.",
    "SOUNDS     Sound effects from the Neo6502 firmware.",
    NULL };

static const char *const *const HELP[2][HELP_PAGES] = {
    { HELP_FR_1, HELP_FR_2, HELP_FR_3, HELP_FR_4, HELP_FR_5 },
    { HELP_EN_1, HELP_EN_2, HELP_EN_3, HELP_EN_4, HELP_EN_5 },
};

const char *help_title(uint8_t page) { return HELP_TITLES[lang][page]; }
const char *const *help_lines(uint8_t page) { return HELP[lang][page]; }
