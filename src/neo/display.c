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

void display_blit_slot(uint8_t slot, int16_t x, int16_t y) {
    area.address = (uint16_t)(uintptr_t)slots[slot];
    area.page = 0;
    area.padding = 0;
    area.stride = CARD_W / 2;
    area.format = 1;
    area.transparent = 0;
    area.solid = 0;
    area.height = CARD_H;
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

void display_text(uint16_t x, uint16_t y, uint8_t colour, const char *s) {
    neo_graphics_set_color(colour);
    neo_graphics_draw_text(x, y, s);
}

void display_clear_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t colour) {
    neo_graphics_set_color(colour);
    neo_graphics_set_solid_flag(1);
    neo_graphics_draw_rectangle(x1, y1, x2, y2);
}
