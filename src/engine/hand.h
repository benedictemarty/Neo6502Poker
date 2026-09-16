/* Évaluation d'une main de 5 cartes (52 cartes) selon la table du Poker ASN (1983). */
#ifndef HAND_H
#define HAND_H
#include "cards.h"

typedef enum {
    HAND_NONE = 0,
    HAND_JACKS_OR_BETTER,   /* paire habillée : V, D, R ou A */
    HAND_TWO_PAIR,
    HAND_THREE,
    HAND_STRAIGHT,
    HAND_FLUSH,
    HAND_FULL,
    HAND_FOUR,
    HAND_STRAIGHT_FLUSH,
    HAND_ROYAL,
    HAND_COUNT
} hand_cat_t;

extern const uint16_t hand_payout[HAND_COUNT];   /* multiplicateurs ASN : 0,1,2,3,5,7,10,40,100,500 */
extern const char *const hand_name[HAND_COUNT];  /* libellés français */

hand_cat_t hand_eval(const card_t cards[HAND_SIZE]);

#endif
