/* Paquet de 52 cartes.
 * Identifiant de carte : id = couleur*13 + rang, avec couleur 0=♠ 1=♥ 2=♦ 3=♣
 * et rang 0=A, 1=2 … 8=9, 9=10, 10=V, 11=D, 12=R.
 * Cet ordre est celui des images dans assets/cards (cards.bin et cards_<c>.gfx). */
#ifndef CARDS_H
#define CARDS_H
#include <stdint.h>

#define DECK_SIZE 52
#define CARD_BACK 52          /* index de l'image du dos */
#define HAND_SIZE 5

typedef uint8_t card_t;

typedef struct {
    card_t cards[DECK_SIZE];
    uint8_t next;             /* prochaine carte à distribuer */
} deck_t;

/* Générateur pseudo-aléatoire injectable (xorshift 16 bits, reproductible). */
typedef struct { uint16_t state; } rng_t;
void rng_seed(rng_t *r, uint16_t seed);
uint16_t rng_next(rng_t *r);

static inline uint8_t card_suit(card_t c) { return c / 13; }
static inline uint8_t card_rank(card_t c) { return c % 13; }   /* 0=A … 12=R */
static inline uint8_t card_is_red(card_t c) { uint8_t s = card_suit(c); return s == 1 || s == 2; }   /* ♥ ♦ */

void deck_init(deck_t *d);
void deck_shuffle(deck_t *d, rng_t *r);   /* Fisher-Yates */
card_t deck_draw(deck_t *d);

#endif
