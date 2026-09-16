/* Tests natifs du moteur : énumération exhaustive des 2 598 960 mains + cas ciblés. */
#include <stdio.h>
#include <string.h>
#include "../../src/engine/hand.h"

static int failures = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("ECHEC: %s\n", msg); failures++; } } while (0)

/* id = couleur*13 + rang ; rang 0=A 1=2 … 9=10 10=V 11=D 12=R */
static card_t C(uint8_t suit, uint8_t rank) { return suit * 13 + rank; }

int main(void) {
    /* Comptes de référence (poker 5 cartes, 52 cartes). */
    static const unsigned long expected[HAND_COUNT] = {
        1302540 + 760320,  /* rien + paires basses (2..10 : 9 rangs sur 13 -> 1098240*9/13) */
        337920,            /* paires V D R A : 1098240*4/13 */
        123552, 54912, 10200, 5108, 3744, 624, 36, 4
    };
    unsigned long count[HAND_COUNT] = {0};
    card_t h[5];
    for (h[0] = 0; h[0] < 52; h[0]++)
      for (h[1] = h[0] + 1; h[1] < 52; h[1]++)
        for (h[2] = h[1] + 1; h[2] < 52; h[2]++)
          for (h[3] = h[2] + 1; h[3] < 52; h[3]++)
            for (h[4] = h[3] + 1; h[4] < 52; h[4]++)
              count[hand_eval(h)]++;
    unsigned long total = 0;
    for (int i = 0; i < HAND_COUNT; i++) {
        total += count[i];
        printf("%-16s %8lu (attendu %8lu)\n", hand_name[i], count[i], expected[i]);
        CHECK(count[i] == expected[i], hand_name[i]);
    }
    CHECK(total == 2598960UL, "total des mains");

    /* Cas ciblés, dont les deux bugs du Poker ASN d'origine. */
    card_t aces[5] = { C(0,0), C(1,0), C(2,1), C(3,4), C(0,7) };
    CHECK(hand_eval(aces) == HAND_JACKS_OR_BETTER, "paire d'As = paire habillee");
    card_t tens[5] = { C(0,9), C(1,9), C(2,1), C(3,4), C(0,7) };
    CHECK(hand_eval(tens) == HAND_NONE, "paire de 10 ne paie pas");
    card_t sf9[5] = { C(2,8), C(2,9), C(2,10), C(2,11), C(2,12) };
    CHECK(hand_eval(sf9) == HAND_STRAIGHT_FLUSH, "9-10-V-D-R assortis = quinte flush, pas royale");
    card_t royal[5] = { C(3,9), C(3,10), C(3,11), C(3,12), C(3,0) };
    CHECK(hand_eval(royal) == HAND_ROYAL, "10-V-D-R-A assortis = flush royale");
    card_t wheel[5] = { C(0,0), C(1,1), C(2,2), C(3,3), C(0,4) };
    CHECK(hand_eval(wheel) == HAND_STRAIGHT, "A-2-3-4-5 = suite");
    card_t wrap[5] = { C(0,11), C(1,12), C(2,0), C(3,1), C(0,2) };
    CHECK(hand_eval(wrap) == HAND_NONE, "D-R-A-2-3 n'est pas une suite");
    card_t full[5] = { C(0,5), C(1,5), C(2,5), C(3,8), C(0,8) };
    CHECK(hand_eval(full) == HAND_FULL, "full");

    /* Paquet : 52 cartes distinctes après mélange, reproductible. */
    deck_t d; rng_t r; rng_seed(&r, 1234);
    deck_init(&d); deck_shuffle(&d, &r);
    uint8_t seen[52] = {0};
    for (int i = 0; i < 52; i++) seen[d.cards[i]]++;
    for (int i = 0; i < 52; i++) CHECK(seen[i] == 1, "carte manquante ou doublee");
    deck_t d2; rng_t r2; rng_seed(&r2, 1234); deck_init(&d2); deck_shuffle(&d2, &r2);
    CHECK(memcmp(d.cards, d2.cards, 52) == 0, "melange reproductible");

    printf(failures ? "%d ECHEC(S)\n" : "OK\n", failures);
    return failures ? 1 : 0;
}
