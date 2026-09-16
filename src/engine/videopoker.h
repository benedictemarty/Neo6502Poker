/* Vidéo-poker façon ASN Diffusion (1983) : un joueur, mise, donne, écart, table de gains. */
#ifndef VIDEOPOKER_H
#define VIDEOPOKER_H
#include "cards.h"
#include "hand.h"

#define VP_START_CREDIT 20
#define VP_MAX_BET 10

typedef enum { VP_BETTING, VP_HOLDING, VP_SHOWDOWN, VP_OVER } vp_state_t;

typedef struct {
    deck_t deck;
    rng_t rng;
    card_t hand[HAND_SIZE];
    uint8_t held[HAND_SIZE];   /* 1 = carte conservée, 0 = à changer */
    uint16_t credit;
    uint8_t bet;
    uint8_t last_bet;
    hand_cat_t result;
    uint16_t win;
    vp_state_t state;
} videopoker_t;

void vp_init(videopoker_t *g, uint16_t seed);
uint8_t vp_bet_inc(videopoker_t *g);     /* +1 crédit dans l'enjeu (M) */
void vp_bet_cancel(videopoker_t *g);     /* remboursement de la mise (C) */
uint8_t vp_deal(videopoker_t *g);        /* donne (D) — reprend la mise précédente si 0 */
void vp_toggle_hold(videopoker_t *g, uint8_t i);
void vp_draw(videopoker_t *g);           /* écart puis évaluation et paiement */
void vp_next_round(videopoker_t *g);

#endif
