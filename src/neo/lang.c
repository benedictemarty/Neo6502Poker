#include "neo/lang.h"

lang_t lang = LANG_FR;

static const char *const STR[2][S_COUNT] = {
    { "VIDEO-POKER POUR NEO6502", "F : FRANCAIS    E : ENGLISH", "CREDIT", "ENJEU",
      "M MISER  C ANNULER  D DONNER", "1-5 GARDER/CHANGER   D TIRER",
      "ESPACE : MANCHE SUIVANTE", "VOUS ETES LESSIVE.  Q QUITTER",
      "GARDE", "CHANGE", "Q QUITTER" },
    { "VIDEO POKER FOR THE NEO6502", "F : FRANCAIS    E : ENGLISH", "CREDIT", "BET",
      "M BET  C CANCEL  D DEAL", "1-5 HOLD/DISCARD   D DRAW",
      "SPACE : NEXT HAND", "YOU ARE BROKE.  Q QUIT",
      "HOLD", "DRAW", "Q QUIT" },
};

static const char *const HANDS[2][HAND_COUNT] = {
    { "RIEN", "PAIRE HABILLEE", "DOUBLE PAIRE", "BRELAN", "SUITE",
      "COULEUR", "FULL", "CARRE", "QUINTE FLUSH", "FLUSH ROYALE" },
    { "NOTHING", "JACKS OR BETTER", "TWO PAIR", "THREE OF A KIND", "STRAIGHT",
      "FLUSH", "FULL HOUSE", "FOUR OF A KIND", "STRAIGHT FLUSH", "ROYAL FLUSH" },
};

const char *T(uint8_t id) { return STR[lang][id]; }
const char *hand_label(hand_cat_t cat) { return HANDS[lang][cat]; }
