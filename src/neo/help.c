#include <neo/api.h>
#include <stdio.h>
#include <string.h>
#include "engine/videopoker.h"
#include "neo/display.h"
#include "neo/lang.h"
#include "neo/help.h"

#define LINE_H 9

static char read_key(void) {
    char k;
    while (!(k = neo_console_read_char())) { }
    if (k >= 'a' && k <= 'z') k -= 32;
    return k;
}

static uint16_t text_w(const char *s, uint8_t size) { return 6 * size * (uint16_t)strlen(s); }

/* Cadre « loupe » : fond blanc, bordure noire, petite loupe dessinée en haut à droite. */
static void zoom_box(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    display_clear_rect(x1, y1, x2, y2, COL_BLACK);
    display_clear_rect(x1 + 2, y1 + 2, x2 - 2, y2 - 2, COL_WHITE);
    neo_graphics_set_solid_flag(0);
    neo_graphics_set_color(COL_BLACK);
    neo_graphics_draw_ellipse(x2 - 20, y1 - 6, x2 - 8, y1 + 6);      /* verre */
    neo_graphics_draw_line(x2 - 9, y1 + 5, x2 - 2, y1 + 12);        /* manche */
    display_text(x2 - 24 - text_w(T(S_ZOOM), 1), y1 - 3, COL_YELLOW, T(S_ZOOM));
}

static uint8_t draw_lines(uint8_t page, uint16_t y) {
    const char *const *l = help_lines(page);
    for (; *l; l++, y += LINE_H) display_text(4, y, COL_WHITE, *l);
    return (uint8_t)y;
}

/* Page 1 : la barre de statut agrandie x2. */
static void illus_status(uint16_t y) {
    zoom_box(8, y, 311, y + 46);
    display_big_text(16, y + 8, COL_YELLOW, 2, lang == LANG_FR ? "CREDIT  20   ENJEU  0" : "CREDIT  20   BET  0");
    display_text(16, y + 30, COL_BLACK, T(S_HELP_BET));
}

/* Page 2 : deux vraies cartes avec leurs etiquettes, comme dans le jeu. */
static void illus_hold(uint16_t y) {
    zoom_box(8, y, 311, y + CARD_H + 26);
    display_load_card(0, 25);                       /* R de coeur */
    display_load_card(1, 9);                        /* 10 de pique */
    for (uint8_t i = 0; i < 2; i++) {
        uint16_t x = 20 + i * 106;
        display_blit_slot(i, x, y + 6);
        const char *s = T(i ? S_CHANGE : S_HOLD);
        display_text(x + (CARD_W - text_w(s, 1)) / 2, y + CARD_H + 10, i ? COL_RED : COL_BLACK, s);
        display_big_text(x + CARD_W + 6, y + CARD_H / 2 - 4, COL_BLACK, 2, i ? "2" : "1");
    }
    display_text(240, y + 24, COL_BLACK, "1-5 :");
    display_text(240, y + 36, COL_BLACK, T(S_HOLD));
    display_text(240, y + 48, COL_BLACK, "<->");
    display_text(240, y + 60, COL_RED, T(S_CHANGE));
}

/* Page 3 : un dos, la carte du pari retournee, et les invites a droite. */
static void illus_double(uint16_t y) {
    zoom_box(8, y, 311, y + CARD_H + 14);
    display_load_card(0, 33);                       /* 8 de carreau */
    display_blit_slot(SLOT_BACK, 20, y + 6);
    display_blit_slot(0, 86, y + 6);
    display_text(148, y + 22, COL_BLACK, T(S_WON));
    display_text(148, y + 52, COL_BLACK, T(S_GUESS));
}

/* Page 5 : la machine, en pixels : l'eventail du titre et le logo. */
static void illus_credits(uint16_t y) {
    zoom_box(8, y, 311, y + 40);
    display_big_text(16, y + 8, COL_BLACK, 2, "NEO6502");
    display_text(120, y + 10, COL_BLACK, "W65C02S @ 6.25 MHz");
    display_text(120, y + 20, COL_BLACK, "320x240 - 16 couleurs / colours");
}

/* Page 4 : une ligne de la table des gains agrandie. */
static void illus_payout(uint16_t y) {
    zoom_box(8, y, 311, y + 30);
    display_big_text(16, y + 8, COL_BLACK, 2, hand_label(HAND_JACKS_OR_BETTER));
    display_big_text(311 - 8 - 12, y + 8, COL_RED, 2, "1");
}

static void draw_page(uint8_t page) {
    display_clear_rect(0, 0, 319, 239, COL_TABLE);
    char t[24];
    snprintf(t, sizeof t, "%u/%u", page + 1, HELP_PAGES);
    display_big_text(4, 4, COL_YELLOW, 2, help_title(page));
    display_text(316 - text_w(t, 1), 8, COL_LIGHT, t);
    uint16_t y = draw_lines(page, 28) + 10;
    switch (page) {
    case 0: illus_status(y); break;
    case 1: illus_hold(y); break;
    case 2: illus_double(y); break;
    case 3: illus_payout(y); break;
    case 4: illus_credits(y); break;
    }
    const char *f = T(page + 1 < HELP_PAGES ? S_HELP_FOOT : S_HELP_FOOT_LAST);
    display_text((320 - text_w(f, 1)) / 2, 228, COL_YELLOW, f);
}

void help_show(void) {
    uint8_t page = 0;
    display_load_card(SLOT_BACK, CARD_BACK);
    for (;;) {
        draw_page(page);
        char k = read_key();
        if (k == 'Q' || k == 27) return;
        if (k == ' ' || k == 13 || k == 'N') {
            if (page + 1 >= HELP_PAGES) return;
            page++;
        } else if (k == 'P' && page > 0) page--;
    }
}
