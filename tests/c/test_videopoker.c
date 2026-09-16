/* Tests natifs de la machine à états du vidéo-poker. */
#include <stdio.h>
#include "../../src/engine/videopoker.h"

static int failures = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("ECHEC: %s\n", msg); failures++; } } while (0)

int main(void) {
    videopoker_t g;
    vp_init(&g, 42);
    CHECK(g.credit == 20 && g.state == VP_BETTING, "etat initial");

    /* mise / annulation */
    CHECK(vp_bet_inc(&g) && vp_bet_inc(&g) && g.bet == 2 && g.credit == 18, "miser 2");
    vp_bet_cancel(&g);
    CHECK(g.bet == 0 && g.credit == 20, "annuler rembourse");
    for (int i = 0; i < 15; i++) vp_bet_inc(&g);
    CHECK(g.bet == VP_MAX_BET && g.credit == 20 - VP_MAX_BET, "mise plafonnee");
    vp_bet_cancel(&g);

    /* donne avec reprise de la mise precedente */
    CHECK(vp_deal(&g) && g.bet == 1 && g.credit == 19 && g.state == VP_HOLDING, "donne mise par defaut 1");
    uint8_t seen[52] = {0};
    for (int i = 0; i < 5; i++) { CHECK(g.hand[i] < 52, "carte valide"); seen[g.hand[i]]++; CHECK(g.held[i] == 1, "gardee par defaut"); }
    for (int i = 0; i < 52; i++) CHECK(seen[i] <= 1, "pas de doublon");

    /* ecart : les cartes changees sont differentes des precedentes */
    card_t before[5]; for (int i = 0; i < 5; i++) before[i] = g.hand[i];
    vp_toggle_hold(&g, 0); vp_toggle_hold(&g, 4);
    vp_draw(&g);
    CHECK(g.state == VP_SHOWDOWN, "abattage");
    CHECK(g.hand[1] == before[1] && g.hand[2] == before[2] && g.hand[3] == before[3], "cartes gardees inchangees");
    CHECK(g.hand[0] != before[0] && g.hand[4] != before[4], "cartes changees");
    CHECK(g.win == (uint16_t)hand_payout[g.result] * 1, "gain = table x mise");
    CHECK(g.credit == 19 + g.win, "credit mis a jour");
    vp_next_round(&g);
    CHECK(g.state == VP_BETTING && g.bet == 0, "manche suivante");

    /* fin de partie : credit epuise */
    g.credit = 1; g.last_bet = 5;
    CHECK(vp_deal(&g) && g.bet == 1 && g.credit == 0, "mise par defaut bornee au credit");
    for (int i = 0; i < 5; i++) g.hand[i] = i * 13;  /* 4 As... + 2 : carre */
    g.hand[4] = 1;
    g.state = VP_HOLDING; vp_draw(&g);
    CHECK(g.result == HAND_FOUR && g.win == 40, "carre paye 40");
    g.credit = 0; vp_next_round(&g);
    CHECK(g.state == VP_OVER, "lessive");

    printf(failures ? "%d ECHEC(S)\n" : "OK\n", failures);
    return failures ? 1 : 0;
}
