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
    CHECK(g.state == (g.win ? VP_WON : VP_SHOWDOWN), "abattage : gain en attente ou rien");
    CHECK(g.hand[1] == before[1] && g.hand[2] == before[2] && g.hand[3] == before[3], "cartes gardees inchangees");
    CHECK(g.hand[0] != before[0] && g.hand[4] != before[4], "cartes changees");
    CHECK(g.win == (uint16_t)hand_payout[g.result] * 1, "gain = table x mise");
    CHECK(g.credit == 19, "credit inchange avant encaissement");
    if (g.state == VP_WON) vp_collect(&g);
    CHECK(g.credit == 19 + g.win && g.state == VP_SHOWDOWN && !g.double_lost, "encaissement");
    vp_next_round(&g);
    CHECK(g.state == VP_BETTING && g.bet == 0, "manche suivante");

    /* fin de partie : credit epuise */
    g.credit = 1; g.last_bet = 5;
    CHECK(vp_deal(&g) && g.bet == 1 && g.credit == 0, "mise par defaut bornee au credit");
    for (int i = 0; i < 5; i++) g.hand[i] = i * 13;  /* 4 As... + 2 : carre */
    g.hand[4] = 1;
    g.state = VP_HOLDING; vp_draw(&g);
    CHECK(g.result == HAND_FOUR && g.win == 40 && g.state == VP_WON, "carre paye 40, en attente");

    /* couleurs : ♠ noir, ♥ rouge, ♦ rouge, ♣ noir */
    CHECK(!card_is_red(0) && card_is_red(13) && card_is_red(26) && !card_is_red(39), "rouge = coeur et carreau");

    /* quitte ou double : on force la carte suivante du paquet */
    CHECK(vp_double_start(&g) && g.state == VP_DOUBLE, "doubler");
    g.deck.cards[g.deck.next] = 13;             /* As de coeur : rouge */
    CHECK(vp_double_guess(&g, 1) == 1 && g.win == 80 && g.state == VP_WON && g.double_card == 13, "rouge gagne : 80");
    CHECK(vp_double_start(&g), "redoubler");
    g.deck.cards[g.deck.next] = 33;             /* 8 de carreau : rouge */
    CHECK(vp_double_guess(&g, 1) == 1 && g.win == 160, "carreau est rouge : 160");
    CHECK(vp_double_start(&g), "redoubler");
    g.deck.cards[g.deck.next] = 40;             /* 3 de trefle : noir */
    CHECK(vp_double_guess(&g, 1) == 0 && g.win == 0 && g.state == VP_SHOWDOWN && g.double_lost, "rouge perd : 0");
    CHECK(g.credit == 0, "rien encaisse");
    vp_next_round(&g);
    CHECK(g.state == VP_OVER, "lessive");

    /* plafond de doublement */
    vp_init(&g, 7); g.state = VP_WON; g.win = VP_MAX_WIN + 1;
    CHECK(vp_double_start(&g) == 0, "pas de doublement au-dela du plafond");
    g.win = 100; vp_double_start(&g); CHECK(g.state == VP_DOUBLE, "doublement sous le plafond");

    printf(failures ? "%d ECHEC(S)\n" : "OK\n", failures);
    return failures ? 1 : 0;
}
