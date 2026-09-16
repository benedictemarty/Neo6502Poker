#include "hand.h"

const uint16_t hand_payout[HAND_COUNT] = { 0, 1, 2, 3, 5, 7, 10, 40, 100, 500 };

const char *const hand_name[HAND_COUNT] = {
    "RIEN", "PAIRE HABILLEE", "DOUBLE PAIRE", "BRELAN", "SUITE",
    "COULEUR", "FULL", "CARRE", "QUINTE FLUSH", "FLUSH ROYALE"
};

/* Valeur ordonnée d'un rang : 2=0 … R=11, A=12 (l'As est haut). */
static uint8_t rank_value(uint8_t rank) { return rank == 0 ? 12 : rank - 1; }

hand_cat_t hand_eval(const card_t cards[HAND_SIZE]) {
    uint8_t count[13] = {0};
    uint8_t v[HAND_SIZE];
    uint8_t pairs = 0, three = 0, four = 0, pair_value = 0;
    uint8_t flush = 1;

    for (uint8_t i = 0; i < HAND_SIZE; i++) {
        v[i] = rank_value(card_rank(cards[i]));
        count[v[i]]++;
        if (card_suit(cards[i]) != card_suit(cards[0])) flush = 0;
    }
    for (uint8_t r = 0; r < 13; r++) {
        if (count[r] == 4) four = 1;
        else if (count[r] == 3) three = 1;
        else if (count[r] == 2) { pairs++; pair_value = r; }
    }
    if (four) return HAND_FOUR;
    if (three && pairs) return HAND_FULL;

    /* tri par insertion des 5 valeurs */
    for (uint8_t i = 1; i < HAND_SIZE; i++) {
        uint8_t x = v[i]; int8_t j = i - 1;
        while (j >= 0 && v[j] > x) { v[j + 1] = v[j]; j--; }
        v[j + 1] = x;
    }
    uint8_t straight = 0;
    if (!pairs && !three) {
        if (v[4] - v[0] == 4) straight = 1;                                   /* 5 valeurs consécutives distinctes */
        else if (v[4] == 12 && v[0] == 0 && v[3] == 3) straight = 1;         /* A-2-3-4-5 */
    }
    if (straight && flush) return (v[0] == 8) ? HAND_ROYAL : HAND_STRAIGHT_FLUSH;  /* 10-V-D-R-A */
    if (flush) return HAND_FLUSH;
    if (straight) return HAND_STRAIGHT;
    if (three) return HAND_THREE;
    if (pairs == 2) return HAND_TWO_PAIR;
    if (pairs == 1) return (pair_value >= 9) ? HAND_JACKS_OR_BETTER : HAND_NONE;  /* V=9 D=10 R=11 A=12 */
    return HAND_NONE;
}
