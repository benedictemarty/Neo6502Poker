/* Affichage des cartes sur Neo6502 (mode 0, 320x240) via le blitter (API groupe 12). */
#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#include "engine/cards.h"

#define CARD_W 48
#define CARD_H 64
#define CARD_BYTES (CARD_W * CARD_H / 2)   /* 4 bpp */
#define CARD_SLOTS 6                       /* 5 cartes de la main + 1 (dos / tirage) */
#define CARDS_FILE "cards.bin"             /* assets/cards/48x64/cards.bin copié sur la SD */

/* Palette Neo6502 utilisée par les cartes. */
#define COL_RED 1
#define COL_YELLOW 3
#define COL_WHITE 7
#define COL_BLACK 8
#define COL_GREEN 10
#define COL_LIGHT 15

void display_init(void);
/* Charge l'image de la carte `card` (0..51, ou CARD_BACK) dans le slot `slot`. */
uint8_t display_load_card(uint8_t slot, uint8_t card);
/* Affiche le slot à la position (x, y). */
void display_blit_slot(uint8_t slot, int16_t x, int16_t y);
void display_text(uint16_t x, uint16_t y, uint8_t colour, const char *s);
void display_clear_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t colour);

#endif
