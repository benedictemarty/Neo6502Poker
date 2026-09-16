#include "cards.h"

void rng_seed(rng_t *r, uint16_t seed) { r->state = seed ? seed : 0xACE1; }

uint16_t rng_next(rng_t *r) {
    uint16_t x = r->state;
    x ^= x << 7; x ^= x >> 9; x ^= x << 8;
    r->state = x;
    return x;
}

void deck_init(deck_t *d) {
    for (uint8_t i = 0; i < DECK_SIZE; i++) d->cards[i] = i;
    d->next = 0;
}

void deck_shuffle(deck_t *d, rng_t *r) {
    for (uint8_t i = DECK_SIZE - 1; i > 0; i--) {
        uint8_t j = rng_next(r) % (i + 1);
        card_t t = d->cards[i]; d->cards[i] = d->cards[j]; d->cards[j] = t;
    }
    d->next = 0;
}

card_t deck_draw(deck_t *d) { return d->cards[d->next++]; }
