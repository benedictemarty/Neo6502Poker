/* Neo6502Poker — vidéo-poker (jalon 1). Touches : M miser, C annuler, D donner/tirer,
 * 1-5 garder/changer une carte, Espace manche suivante, Q quitter. */
#include <neo/api.h>
#include <stdio.h>
#include <string.h>
#include "engine/videopoker.h"
#include "neo/display.h"
#include "neo/lang.h"

#define HAND_Y 96
#define HAND_X0 4
#define HAND_STEP 62
#define LABEL_Y (HAND_Y + CARD_H + 3)
#define STATUS_Y 196

static videopoker_t game;
static uint8_t shown[HAND_SIZE];     /* image affichée à chaque position */
static char line[40];

static int16_t card_x(uint8_t i) { return HAND_X0 + i * HAND_STEP; }

static char read_key(void) {
    char k;
    while (!(k = neo_console_read_char())) { }
    if (k >= 'a' && k <= 'z') k -= 32;
    return k;
}

/* ---- écran de présentation ------------------------------------------------- */
static const uint8_t fan[HAND_SIZE] = { 0, 25, 37, 49, 9 };   /* A♠ R♥ D♦ V♣ 10♠ */

static void centered(uint16_t y, uint8_t colour, const char *s) {
    display_text((320 - 6 * (uint16_t)strlen(s)) / 2, y, colour, s);
}

static void title_screen(void) {
    display_clear_rect(0, 0, 319, 239, COL_TABLE);
    display_big_text(100 + 3, 12 + 3, COL_BLACK, 4, "POKER");     /* ombre */
    display_big_text(100, 12, COL_YELLOW, 4, "POKER");
    for (uint8_t i = 0; i < HAND_SIZE; i++) {                     /* éventail centré : 4*50+56 = 256 */
        display_load_card(i, fan[i]);
        display_blit_slot(i, 32 + i * 50, 56);
        display_wait_ticks(8);
    }
    centered(148, COL_WHITE, "Base sur le video-poker de la societe ASN (Oric 1)");
    centered(166, COL_WHITE, "Programmation Benedicte MARTY");
    centered(176, COL_LIGHT, "avec l'aide de Claude Code");
    centered(206, COL_YELLOW, T(S_CHOOSE));
    for (;;) {
        char k = read_key();
        if (k == 'F') { lang = LANG_FR; break; }
        if (k == 'E') { lang = LANG_EN; break; }
    }
}

/* ---- table de jeu ---------------------------------------------------------- */
static void draw_table(void) {
    display_clear_rect(0, 0, 319, 239, COL_TABLE);
    display_big_text(8, 8, COL_YELLOW, 2, "POKER");
    display_text(8, 28, COL_LIGHT, "NEO6502");
    for (uint8_t i = 0; i < HAND_COUNT - 1; i++) {
        display_text(190, 4 + i * 9, COL_LIGHT, hand_label(i + 1));
        snprintf(line, sizeof line, "%u", hand_payout[i + 1]);   /* aligné à droite */
        display_text(316 - 6 * (uint16_t)strlen(line), 4 + i * 9, COL_YELLOW, line);
    }
}

static void draw_status(void) {
    display_clear_rect(0, STATUS_Y - 4, 319, 239, COL_TABLE);
    snprintf(line, sizeof line, "%s %3u   %s %2u", T(S_CREDIT), game.credit, T(S_BET), game.bet);
    display_text(8, STATUS_Y, COL_YELLOW, line);
    switch (game.state) {
    case VP_BETTING:  display_text(8, STATUS_Y + 16, COL_WHITE, T(S_HELP_BET)); break;
    case VP_HOLDING:  display_text(8, STATUS_Y + 16, COL_WHITE, T(S_HELP_HOLD)); break;
    case VP_SHOWDOWN:
        snprintf(line, sizeof line, "%s  +%u", hand_label(game.result), game.win);
        display_text(8, STATUS_Y + 16, game.win ? COL_YELLOW : COL_LIGHT, line);
        display_text(8, STATUS_Y + 28, COL_WHITE, T(S_NEXT));
        break;
    case VP_OVER:     display_text(8, STATUS_Y + 16, COL_RED, T(S_OVER)); break;
    }
    display_text(316 - 6 * (uint16_t)strlen(T(S_QUIT)), STATUS_Y, COL_LIGHT, T(S_QUIT));
}

static void draw_label(uint8_t i) {
    int16_t x = card_x(i);
    display_clear_rect(x, LABEL_Y, x + CARD_W - 1, LABEL_Y + 8, COL_TABLE);
    if (game.state == VP_HOLDING) {
        const char *s = T(game.held[i] ? S_HOLD : S_CHANGE);
        display_text(x + (CARD_W - 6 * (uint16_t)strlen(s)) / 2, LABEL_Y,
                     game.held[i] ? COL_WHITE : COL_RED, s);
    }
}

/* Affiche la main : les positions dont l'image change sont retournées ensemble. */
static void show_hand(void) {
    uint8_t n = 0, slot[HAND_SIZE], card[HAND_SIZE];
    int16_t x[HAND_SIZE];
    for (uint8_t i = 0; i < HAND_SIZE; i++) {
        if (shown[i] != game.hand[i]) {
            if (shown[i] == CARD_BACK) display_load_card(i, CARD_BACK);   /* face actuelle = dos */
            slot[n] = i; card[n] = game.hand[i]; x[n] = card_x(i); n++;
            shown[i] = game.hand[i];
        }
    }
    if (n) display_flip_many(n, slot, card, x, HAND_Y);
    for (uint8_t i = 0; i < HAND_SIZE; i++) draw_label(i);
}

int main(void) {
    display_init();
    display_load_card(SLOT_BACK, CARD_BACK);
    title_screen();
    vp_init(&game, (uint16_t)neo_system_timer());
    draw_table();
    for (uint8_t i = 0; i < HAND_SIZE; i++) { shown[i] = CARD_BACK; display_blit_slot(SLOT_BACK, card_x(i), HAND_Y); }
    draw_status();
    for (;;) {
        char k = read_key();
        if (k == 'Q') break;
        switch (game.state) {
        case VP_BETTING:
            if (k == 'M') vp_bet_inc(&game);
            else if (k == 'C') vp_bet_cancel(&game);
            else if (k == 'D' && vp_deal(&game)) { draw_status(); show_hand(); }
            break;
        case VP_HOLDING:
            if (k >= '1' && k <= '5') { vp_toggle_hold(&game, k - '1'); draw_label(k - '1'); }
            else if (k == 'D') { vp_draw(&game); draw_status(); show_hand(); if (game.win) neo_sound_beep(); }
            break;
        case VP_SHOWDOWN:
            if (k == ' ') {
                vp_next_round(&game);
                for (uint8_t i = 0; i < HAND_SIZE; i++) game.hand[i] = CARD_BACK;   /* retour des dos */
                show_hand();
            }
            break;
        case VP_OVER:
            break;
        }
        draw_status();
    }
    neo_console_clear_screen();
    return 0;
}
