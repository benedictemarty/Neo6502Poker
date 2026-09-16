/* Affichage des cartes sur Neo6502 (mode 0, 320x240) via le blitter (API groupe 12). */
#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#include "engine/cards.h"

#define CARD_W 56
#define CARD_H 80
#define CARD_BYTES (CARD_W * CARD_H / 2)   /* 4 bpp */
#define SLOT_BACK 5                        /* dos, chargé une fois */
#define SLOT_TEMP 6                        /* image cible pendant un retournement */
#define CARD_SLOTS 7
#define CARDS_FILE "cards.bin"             /* assets/cards/56x80/cards.bin copié sur la SD */

/* Palette Neo6502. */
#define COL_RED 1
#define COL_YELLOW 3
#define COL_WHITE 7
#define COL_BLACK 8
#define COL_GREEN 10
#define COL_LIGHT 15
#define COL_TABLE COL_GREEN

void display_init(void);
uint8_t display_load_card(uint8_t slot, uint8_t card);   /* card 0..51 ou CARD_BACK */
void display_blit_slot(uint8_t slot, int16_t x, int16_t y);
/* Retournement : l'image du slot `from` se referme, celle du slot `to` s'ouvre. */
void display_flip(uint8_t from, uint8_t to, int16_t x, int16_t y);
void display_text(uint16_t x, uint16_t y, uint8_t colour, const char *s);
void display_big_text(uint16_t x, uint16_t y, uint8_t colour, uint8_t size, const char *s);
void display_clear_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t colour);
void display_wait_ticks(uint8_t ticks);   /* 1 tick = 10 ms */

#endif
