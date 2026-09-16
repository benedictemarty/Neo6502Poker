/* Neo6502Poker — vidéo-poker (jalon 1). Touches : M miser, C annuler, D donner/tirer,
 * 1-5 garder/changer une carte, Espace manche suivante, Q quitter. */
#include <neo/api.h>
#include <stdio.h>
#include <string.h>
#include "engine/videopoker.h"
#include "neo/display.h"
#include "neo/lang.h"
#include "neo/help.h"

#define DOUBLE_POS 2                 /* position de la carte du quitte ou double */
static void sfx(uint8_t id) { neo_sound_play_effect(0, id); }

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

static void title_texts(void) {
    display_clear_rect(0, 144, 319, 239, COL_TABLE);
    centered(148, COL_WHITE, lang == LANG_FR ? "Base sur le video-poker de la societe ASN (Oric 1)"
                                             : "Based on the ASN video poker (Oric 1)");
    centered(164, COL_WHITE, lang == LANG_FR ? "Programmation Benedicte MARTY" : "Programming Benedicte MARTY");
    centered(174, COL_LIGHT, lang == LANG_FR ? "avec l'aide de Claude Code" : "with the help of Claude Code");
    centered(198, COL_YELLOW, T(S_CHOOSE));
    centered(212, COL_YELLOW, T(S_TITLE_HELP));
    centered(226, COL_YELLOW, T(S_TITLE_PLAY));
}

static void title_draw(void) {
    display_clear_rect(0, 0, 319, 239, COL_TABLE);
    display_big_text(100 + 3, 12 + 3, COL_BLACK, 4, "POKER");     /* ombre */
    display_big_text(100, 12, COL_YELLOW, 4, "POKER");
    for (uint8_t i = 0; i < HAND_SIZE; i++) {                     /* éventail centré : 4*50+56 = 256 */
        display_load_card(i, fan[i]);
        display_blit_slot(i, 32 + i * 50, 56);
        display_wait_ticks(8);
    }
    title_texts();
}

static void title_screen(void) {
    title_draw();
    for (;;) {
        char k = read_key();
        if (k == 'F' || k == 'E') { lang = (k == 'F') ? LANG_FR : LANG_EN; title_texts(); }
        else if (k == 'H') { help_show(); title_draw(); }
        else if (k == ' ' || k == 13) break;
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
    case VP_BETTING:  display_text(8, STATUS_Y + 16, COL_WHITE, T(S_HELP_BET)); display_text(8, STATUS_Y + 28, COL_LIGHT, T(S_TITLE_HELP)); break;
    case VP_HOLDING:  display_text(8, STATUS_Y + 16, COL_WHITE, T(S_HELP_HOLD)); break;
    case VP_WON:
        if (game.double_card != CARD_BACK)
            snprintf(line, sizeof line, "%s   %s %u", T(S_DOUBLE_WIN), T(S_WIN), game.win);
        else
            snprintf(line, sizeof line, "%s   %s %u", hand_label(game.result), T(S_WIN), game.win);
        display_text(8, STATUS_Y + 16, COL_YELLOW, line);
        display_text(8, STATUS_Y + 28, COL_WHITE, T(S_WON));
        break;
    case VP_DOUBLE:
        snprintf(line, sizeof line, "%s %u", T(S_WIN), game.win);
        display_text(8, STATUS_Y + 16, COL_YELLOW, line);
        display_text(8, STATUS_Y + 28, COL_WHITE, T(S_GUESS));
        break;
    case VP_SHOWDOWN:
        if (game.double_lost)
            display_text(8, STATUS_Y + 16, COL_RED, T(S_DOUBLE_LOSE));
        else {
            snprintf(line, sizeof line, "%s  +%u", hand_label(game.result), game.win);
            display_text(8, STATUS_Y + 16, game.win ? COL_YELLOW : COL_LIGHT, line);
        }
        display_text(8, STATUS_Y + 28, COL_WHITE, T(S_NEXT));
        break;
    case VP_OVER:     display_text(8, STATUS_Y + 16, COL_RED, T(S_OVER)); break;
    }
    if (game.state == VP_BETTING || game.state == VP_OVER)
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

/* Affiche les 5 positions : celles dont l'image change sont retournées ensemble. */
static void show_cards(const card_t *target) {
    uint8_t n = 0, slot[HAND_SIZE], card[HAND_SIZE];
    int16_t x[HAND_SIZE];
    for (uint8_t i = 0; i < HAND_SIZE; i++) {
        if (shown[i] != target[i]) {
            if (shown[i] == CARD_BACK) display_load_card(i, CARD_BACK);   /* face actuelle = dos */
            slot[n] = i; card[n] = target[i]; x[n] = card_x(i); n++;
            shown[i] = target[i];
        }
    }
    if (n) display_flip_many(n, slot, card, x, HAND_Y);
    for (uint8_t i = 0; i < HAND_SIZE; i++) draw_label(i);
}

static void show_hand(void) { show_cards(game.hand); }

/* Vue du quitte ou double : dos partout, la carte tirée au centre. */
static void show_double(void) {
    card_t view[HAND_SIZE];
    for (uint8_t i = 0; i < HAND_SIZE; i++) view[i] = CARD_BACK;
    view[DOUBLE_POS] = game.double_card;
    show_cards(view);
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
        if (k == 'Q' && (game.state == VP_BETTING || game.state == VP_OVER)) break;   /* ailleurs Q = encaisser */
        switch (game.state) {
        case VP_BETTING:
            if (k == 'H') { help_show(); draw_table(); for (uint8_t i = 0; i < HAND_SIZE; i++) display_blit_slot(i, card_x(i), HAND_Y); }
            else if (k == 'M') { if (vp_bet_inc(&game)) sfx(API_SFX_COIN); else sfx(API_SFX_REJECT); }
            else if (k == 'C') vp_bet_cancel(&game);
            else if (k == 'D' && vp_deal(&game)) { sfx(API_SFX_CONFIRM); draw_status(); show_hand(); }
            break;
        case VP_HOLDING:
            if (k >= '1' && k <= '5') { vp_toggle_hold(&game, k - '1'); draw_label(k - '1'); sfx(API_SFX_POSITIVE); }
            else if (k == 'D') {
                vp_draw(&game); draw_status(); show_hand();
                if (game.result >= HAND_FOUR) sfx(API_SFX_FANFARE);
                else if (game.win) sfx(API_SFX_VICTORY);
                else sfx(API_SFX_NEGATIVE);
            }
            break;
        case VP_WON:
            if (k == 'Q') { vp_collect(&game); sfx(API_SFX_COIN); }
            else if (k == 'D' && vp_double_start(&game)) { sfx(API_SFX_SWEEP); draw_status(); show_double(); }
            break;
        case VP_DOUBLE:
            if (k == 'R' || k == 'N' || k == 'B') {
                uint8_t won = vp_double_guess(&game, k == 'R');
                draw_status(); show_double();
                sfx(won ? API_SFX_POWERUP : API_SFX_DEFEAT);
            }
            break;
        case VP_SHOWDOWN:
            if (k == ' ') {
                vp_next_round(&game);
                if (game.state == VP_OVER) sfx(API_SFX_DEFEAT);
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
