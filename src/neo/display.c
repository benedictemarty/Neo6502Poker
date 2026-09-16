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
static uint8_t frame[CARD_BYTES];
static struct blitter_area area;

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

uint8_t display_load_card(uint8_t slot, uint8_t card) {
    if (slot >= CARD_SLOTS || card > CARD_BACK) return 0;
    neo_file_open(1, CARDS_FILE, 0);            /* lecture seule */
    if (neo_api_error()) return 0;
    neo_file_seek(1, (uint32_t)card * CARD_BYTES);
    uint16_t n = neo_file_read(1, slots[slot], CARD_BYTES);
    neo_file_close(1);
    return n == CARD_BYTES;
}

static void blit(const uint8_t *data, int16_t x, int16_t y, uint8_t w) {
    area.address = (uint16_t)(uintptr_t)data;
    area.page = 0;
    area.padding = 0;
    area.stride = w / 2;
    area.format = 1;
    area.transparent = 0;
    area.solid = 0;
    area.height = CARD_H;
    area.width = w;
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
    blit(slots[slot], x, y, CARD_W);
}

/* Compresse horizontalement l'image du slot à la largeur w (paire) dans `frame`. */
static void squeeze(uint8_t slot, uint8_t w) {
    const uint8_t *src = slots[slot];
    uint8_t *dst = frame;
    for (uint8_t y = 0; y < CARD_H; y++) {
        for (uint8_t ox = 0; ox < w; ox += 2) {
            uint8_t sx0 = (uint8_t)(((uint16_t)ox * CARD_W) / w);
            uint8_t sx1 = (uint8_t)(((uint16_t)(ox + 1) * CARD_W) / w);
            uint8_t b0 = src[sx0 >> 1], b1 = src[sx1 >> 1];
            uint8_t p0 = (sx0 & 1) ? (b0 & 0x0F) : (b0 >> 4);
            uint8_t p1 = (sx1 & 1) ? (b1 & 0x0F) : (b1 >> 4);
            *dst++ = (uint8_t)((p0 << 4) | p1);
        }
        src += CARD_W / 2;
    }
}

static const uint8_t flip_widths[] = { 44, 32, 20, 8 };

static void flip_frame(uint8_t slot, uint8_t w, int16_t x, int16_t y) {
    squeeze(slot, w);
    display_clear_rect(x, y, x + CARD_W - 1, y + CARD_H - 1, COL_TABLE);
    blit(frame, x + (CARD_W - w) / 2, y, w);
    display_wait_ticks(2);
}

void display_flip(uint8_t from, uint8_t to, int16_t x, int16_t y) {
    for (uint8_t i = 0; i < sizeof flip_widths; i++) flip_frame(from, flip_widths[i], x, y);
    for (uint8_t i = sizeof flip_widths; i-- > 0;) flip_frame(to, flip_widths[i], x, y);
    display_blit_slot(to, x, y);
}

void display_text(uint16_t x, uint16_t y, uint8_t colour, const char *s) {
    display_big_text(x, y, colour, 1, s);
}

void display_big_text(uint16_t x, uint16_t y, uint8_t colour, uint8_t size, const char *s) {
    neo_graphics_set_color(colour);
    neo_graphics_set_solid_flag(0);             /* texte sans fond */
    neo_graphics_set_draw_size(size);
    neo_graphics_draw_text(x, y, s);
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
