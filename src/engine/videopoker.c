#include "videopoker.h"

void vp_init(videopoker_t *g, uint16_t seed) {
    rng_seed(&g->rng, seed);
    g->credit = VP_START_CREDIT;
    g->bet = 0; g->last_bet = 1;
    g->result = HAND_NONE; g->win = 0;
    g->state = VP_BETTING;
    for (uint8_t i = 0; i < HAND_SIZE; i++) { g->hand[i] = CARD_BACK; g->held[i] = 0; }
}

uint8_t vp_bet_inc(videopoker_t *g) {
    if (g->state != VP_BETTING || g->credit == 0 || g->bet >= VP_MAX_BET) return 0;
    g->bet++; g->credit--;
    return 1;
}

void vp_bet_cancel(videopoker_t *g) {
    if (g->state != VP_BETTING) return;
    g->credit += g->bet; g->bet = 0;
}

uint8_t vp_deal(videopoker_t *g) {
    if (g->state != VP_BETTING) return 0;
    if (g->bet == 0) {                                   /* reprise de la mise précédente */
        uint8_t b = g->last_bet;
        if (b > g->credit) b = g->credit;
        if (b == 0) return 0;
        g->bet = b; g->credit -= b;
    }
    g->last_bet = g->bet;
    deck_init(&g->deck);
    deck_shuffle(&g->deck, &g->rng);
    for (uint8_t i = 0; i < HAND_SIZE; i++) { g->hand[i] = deck_draw(&g->deck); g->held[i] = 1; }
    g->state = VP_HOLDING;
    return 1;
}

void vp_toggle_hold(videopoker_t *g, uint8_t i) {
    if (g->state == VP_HOLDING && i < HAND_SIZE) g->held[i] ^= 1;
}

void vp_draw(videopoker_t *g) {
    if (g->state != VP_HOLDING) return;
    for (uint8_t i = 0; i < HAND_SIZE; i++)
        if (!g->held[i]) g->hand[i] = deck_draw(&g->deck);
    g->result = hand_eval(g->hand);
    g->win = (uint16_t)hand_payout[g->result] * g->bet;
    g->credit += g->win;
    g->state = VP_SHOWDOWN;
}

void vp_next_round(videopoker_t *g) {
    if (g->state != VP_SHOWDOWN) return;
    g->bet = 0; g->win = 0;
    g->state = (g->credit == 0) ? VP_OVER : VP_BETTING;
}
