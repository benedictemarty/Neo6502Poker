/* Textes du jeu, français et anglais. */
#ifndef LANG_H
#define LANG_H
#include <stdint.h>
#include "engine/hand.h"

typedef enum { LANG_FR = 0, LANG_EN = 1 } lang_t;

enum {
    S_SUBTITLE, S_CHOOSE, S_CREDIT, S_BET, S_HELP_BET, S_HELP_HOLD, S_NEXT, S_OVER,
    S_HOLD, S_CHANGE, S_QUIT, S_WON, S_GUESS, S_WIN, S_DOUBLE_WIN, S_DOUBLE_LOSE,
    S_TITLE_HELP, S_TITLE_PLAY, S_HELP_FOOT, S_HELP_FOOT_LAST, S_ZOOM, S_COUNT
};

extern lang_t lang;
const char *T(uint8_t id);                 /* texte dans la langue courante */
const char *hand_label(hand_cat_t cat);    /* nom de la combinaison */

#define HELP_PAGES 5
const char *help_title(uint8_t page);
const char *const *help_lines(uint8_t page);   /* lignes terminées par NULL */

#endif
