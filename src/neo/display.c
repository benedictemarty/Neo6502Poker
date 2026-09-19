#include <neo/api.h>
#include <string.h>
#include "neo/display.h"

/* Descripteur de zone du blitter (cf. group12_blitter.inc du firmware). */
struct blitter_area {
    uint16_t address;
    uint8_t page;        /* 0 = RAM 6502 */
    uint8_t padding;
    int16_t stride;      /* octets entre deux lignes */
    uint8_t format;      /* 1 = paires de quartets (4 bpp) */
    uint8_t transparent;
    uint8_t solid;
    uint8_t height;
    uint16_t width;      /* en pixels pour le format 1 */
} __attribute__((packed));

static uint8_t slots[CARD_SLOTS][CARD_BYTES];
static struct blitter_area area;
static const char *const cards_paths[] = CARDS_PATHS;
static const char *cards_file;   /* chemin retenu par display_find_cards */

static void api_call(uint8_t group, uint8_t function) {
    ControlPort.function = function;
    ControlPort.command = group;
    while (ControlPort.command != 0) { }
}

void display_init(void) {
    neo_console_clear_screen();
    neo_console_set_cursor_visibility(0);
    neo_graphics_set_defaults(0xFF, 0x00, 1, 1, 0);
}

const char *display_find_cards(void) {
    for (uint8_t i = 0; i < sizeof cards_paths / sizeof *cards_paths; i++) {
        neo_file_open(1, cards_paths[i], 0);
        if (!neo_api_error()) { neo_file_close(1); cards_file = cards_paths[i]; return cards_file; }
    }
    return NULL;
}

uint8_t display_load_card(uint8_t slot, uint8_t card) {
    if (slot >= CARD_SLOTS || card > CARD_BACK || !cards_file) return 0;
    neo_file_open(1, cards_file, 0);            /* lecture seule */
    if (neo_api_error()) return 0;
    neo_file_seek(1, (uint32_t)card * CARD_BYTES);
    uint16_t n = neo_file_read(1, slots[slot], CARD_BYTES);
    neo_file_close(1);
    return n == CARD_BYTES;
}

static void blit_rows(const uint8_t *data, int16_t x, int16_t y, uint8_t row_step, uint8_t h) {
    area.address = (uint16_t)(uintptr_t)data;
    area.page = 0;
    area.padding = 0;
    area.stride = (int16_t)(CARD_W / 2) * row_step;
    area.format = 1;
    area.transparent = 0;
    area.solid = 0;
    area.height = h;
    area.width = CARD_W;
    uint16_t a = (uint16_t)(uintptr_t)&area;
    ControlPort.params[0] = 0;                  /* action : copie */
    ControlPort.params[1] = a & 0xFF;
    ControlPort.params[2] = a >> 8;
    ControlPort.params[3] = x & 0xFF;
    ControlPort.params[4] = (uint16_t)x >> 8;
    ControlPort.params[5] = y & 0xFF;
    ControlPort.params[6] = (uint16_t)y >> 8;
    ControlPort.params[7] = 0;                  /* écriture d'octets entiers */
    api_call(API_GROUP_BLITTER, 4);             /* Blit Image */
}

void display_blit_slot(uint8_t slot, int16_t x, int16_t y) {
    blit_rows(slots[slot], x, y, 1, CARD_H);
}

/* Retournement autour de l'axe horizontal : la carte est blittée avec un pas de `k` lignes
 * (stride = k * 28 octets, hauteur 80/k), ce qui la compresse verticalement sans calcul CPU. */
static void flip_frame(uint8_t slot, uint8_t k, int16_t x, int16_t y) {
    uint8_t h = CARD_H / k;
    display_clear_rect(x, y, x + CARD_W - 1, y + CARD_H - 1, COL_TABLE);
    blit_rows(slots[slot], x, y + (CARD_H - h) / 2, k, h);
}

static const uint8_t flip_steps[] = { 2, 4, 8 };   /* 40, 20, 10 lignes */

void display_flip_many(uint8_t n, const uint8_t *slot, const uint8_t *card, const int16_t *x, int16_t y) {
    for (uint8_t f = 0; f < sizeof flip_steps; f++) {                /* fermeture des anciennes faces */
        for (uint8_t i = 0; i < n; i++) flip_frame(slot[i], flip_steps[f], x[i], y);
        display_wait_ticks(2);
    }
    for (uint8_t i = 0; i < n; i++) display_load_card(slot[i], card[i]);
    for (uint8_t f = sizeof flip_steps; f-- > 0;) {                   /* ouverture des nouvelles */
        for (uint8_t i = 0; i < n; i++) flip_frame(slot[i], flip_steps[f], x[i], y);
        display_wait_ticks(2);
    }
    for (uint8_t i = 0; i < n; i++) display_blit_slot(slot[i], x[i], y);
}

void display_text(uint16_t x, uint16_t y, uint8_t colour, const char *s) {
    display_big_text(x, y, colour, 1, s);
}

/* Chaîne longueur-préfixée dans un tampon statique en RAM basse : le SDK (`neo_graphics_draw_text`)
 * la construit sur la pile logicielle vers $F5xx, ce qui donne des textes faux sur la carte réelle
 * (Trinity 0.2.0, 2026-09-19) alors que les émulateurs sont corrects ; ProphetGui, qui utilise un
 * tampon statique, affiche bien. Cause non élucidée, la différence est supprimée. */
static struct { uint8_t length; char data[63]; } ptext;

void display_big_text(uint16_t x, uint16_t y, uint8_t colour, uint8_t size, const char *s) {
    uint8_t n = 0;
    while (s[n] && n < sizeof ptext.data) { ptext.data[n] = s[n]; n++; }
    if (!n) return;
    ptext.length = n;
    neo_graphics_set_color(colour);
    neo_graphics_set_solid_flag(0);             /* texte sans fond */
    neo_graphics_set_draw_size(size);
    neo_graphics_draw_text_p(x, y, (const neo_pstring_t *)&ptext);
    while (ControlPort.command != 0) { }        /* la chaîne doit rester lisible jusqu'à la fin */
    neo_graphics_set_draw_size(1);
}

void display_clear_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t colour) {
    neo_graphics_set_color(colour);
    neo_graphics_set_solid_flag(1);
    neo_graphics_draw_rectangle(x1, y1, x2, y2);
}

void display_wait_ticks(uint8_t ticks) {
    long t0 = neo_system_timer();
    while (neo_system_timer() - t0 < ticks) { }
}
