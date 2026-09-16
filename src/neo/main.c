/* Neo6502Poker — vidéo-poker (jalon 1). Touches : M miser, C annuler, D donner/tirer,
 * 1-5 garder/changer une carte, Espace manche suivante, Q quitter. */
#include <neo/api.h>
#include <stdio.h>
#include <string.h>
#include "engine/videopoker.h"
#include "neo/display.h"

#define HAND_Y 96
#define HAND_X0 20
#define HAND_STEP 56

static videopoker_t game;
static char line[40];

static void draw_table(void) {
    display_clear_rect(0, 0, 319, 239, COL_GREEN);
    display_text(8, 8, COL_WHITE, "NEO6502 POKER");
    for (uint8_t i = 0; i < HAND_COUNT - 1; i++) {
        display_text(200, 8 + i * 9, COL_LIGHT, hand_name[i + 1]);
        snprintf(line, sizeof line, "%u", hand_payout[i + 1]);   /* aligné à droite, colonne 6 car. */
        display_text(312 - 6 * (uint16_t)strlen(line), 8 + i * 9, COL_YELLOW, line);
    }
}

static void draw_status(void) {
    display_clear_rect(0, 176, 319, 239, COL_GREEN);
    snprintf(line, sizeof line, "CREDIT %3u   ENJEU %2u", game.credit, game.bet);
    display_text(8, 180, COL_YELLOW, line);
    switch (game.state) {
    case VP_BETTING:  display_text(8, 200, COL_WHITE, "M MISER  C ANNULER  D DONNER"); break;
    case VP_HOLDING:  display_text(8, 200, COL_WHITE, "1-5 GARDER/CHANGER   D TIRER"); break;
    case VP_SHOWDOWN:
        snprintf(line, sizeof line, "%s  +%u", hand_name[game.result], game.win);
        display_text(8, 200, game.win ? COL_YELLOW : COL_LIGHT, line);
        display_text(8, 212, COL_WHITE, "ESPACE : MANCHE SUIVANTE");
        break;
    case VP_OVER:     display_text(8, 200, COL_RED, "VOUS ETES LESSIVE.  Q QUITTER"); break;
    }
}

static void draw_hand(void) {
    for (uint8_t i = 0; i < HAND_SIZE; i++) {
        int16_t x = HAND_X0 + i * HAND_STEP;
        display_load_card(i, game.hand[i]);
        display_blit_slot(i, x, HAND_Y);
        display_clear_rect(x, HAND_Y + CARD_H + 2, x + CARD_W - 1, HAND_Y + CARD_H + 10, COL_GREEN);
        if (game.state == VP_HOLDING)
            display_text(x + 6, HAND_Y + CARD_H + 2, game.held[i] ? COL_WHITE : COL_RED,
                         game.held[i] ? "GARDE" : "CHANGE");
    }
}

int main(void) {
    display_init();
    vp_init(&game, (uint16_t)neo_system_timer());
    draw_table();
    draw_hand();
    draw_status();
    for (;;) {
        char k = neo_console_read_char();
        if (!k) continue;
        if (k >= 'a' && k <= 'z') k -= 32;
        if (k == 'Q') break;
        switch (game.state) {
        case VP_BETTING:
            if (k == 'M') vp_bet_inc(&game);
            else if (k == 'C') vp_bet_cancel(&game);
            else if (k == 'D' && vp_deal(&game)) draw_hand();
            break;
        case VP_HOLDING:
            if (k >= '1' && k <= '5') { vp_toggle_hold(&game, k - '1'); draw_hand(); }
            else if (k == 'D') { vp_draw(&game); draw_hand(); if (game.win) neo_sound_beep(); }
            break;
        case VP_SHOWDOWN:
            if (k == ' ') vp_next_round(&game);
            break;
        case VP_OVER:
            break;
        }
        draw_status();
    }
    neo_console_clear_screen();
    return 0;
}
