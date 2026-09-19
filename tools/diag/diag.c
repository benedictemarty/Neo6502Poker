/* Neo6502Poker — diagnostic du stockage sur carte réelle.
 *
 * Symptôme visé (2026-09-19) : cartes brouillées et textes illisibles sur la carte, alors que
 * les émulateurs sont corrects. Le programme vérifie les deux chemins qui diffèrent entre
 * carte et émulateurs : le chargement du .neo lui-même et la lecture de cards.bin.
 * Chaque ligne est une empreinte 16 bits à comparer avec `python3 tools/diag/expected.py`.
 *
 *   VERSION a.b.c     version du firmware (API 1,1)
 *   IMAGE   len hash  empreinte de l'image chargée [$0200, $0200+len) — détecte un .neo mal lu
 *   SIZE    nnnnnn    taille de cards.bin (décimal)
 *   Cnn     hash      chaque carte lue par seek + lecture de 2240 octets (chemin du jeu)
 *   BYTES   hash      carte 0 relue octet par octet (chemin du chargeur .neo)
 *   SEQ     hash      tout le fichier lu séquentiellement par blocs de 512 octets
 *   END
 */
#include <neo/api.h>
#include <stdio.h>
#include <stdint.h>

#define CARDS "cards.bin"
#define CARD_BYTES 2240
#define CARD_COUNT 53

extern char _start[];          /* adresse de chargement du .neo (0x0200) */
extern char __data_end[];      /* fin de l'image chargée (code + rodata + data) */

static uint8_t buf[CARD_BYTES];
static uint16_t image_hash;

/* Empreinte sensible à l'ordre : h = h*31 + octet (mod 65536). */
static uint16_t hash(uint16_t h, const uint8_t *p, uint16_t n) {
    while (n--) h = (uint16_t)((h << 5) - h + *p++);
    return h;
}

static uint8_t open_cards(void) {
    neo_file_open(1, CARDS, 0);
    return neo_api_error() == 0;
}

int main(void) {
    /* Empreinte de l'image avant tout appel susceptible de modifier .data (printf). */
    image_hash = hash(0, (const uint8_t *)_start, (uint16_t)(__data_end - _start));

    neo_console_clear_screen();
    neo_version_t v;
    neo_system_version(&v);
    printf("VERSION %u.%u.%u\n", v.major, v.minor, v.patch);
    printf("IMAGE %04X %04X\n", (unsigned)(__data_end - _start), image_hash);

    if (!open_cards()) { printf("OPEN ERR %02X\nEND\n", neo_api_error()); for (;;) { } }
    uint32_t size = neo_file_size(1);
    printf("SIZE %lu\n", (unsigned long)size);

    /* Chemin du jeu : seek + lecture d'un bloc de 2240 octets par carte. */
    for (uint8_t c = 0; c < CARD_COUNT; c++) {
        neo_file_seek(1, (uint32_t)c * CARD_BYTES);
        uint16_t n = neo_file_read(1, buf, CARD_BYTES);
        printf("C%02u %04X%s", c, hash(0, buf, n), (c % 5 == 4) ? "\n" : "  ");
        if (n != CARD_BYTES) printf("(n=%u)", n);
    }
    printf("\n");

    /* Chemin du chargeur .neo : octet par octet. */
    neo_file_seek(1, 0);
    uint16_t h = 0;
    for (uint16_t i = 0; i < CARD_BYTES; i++) {
        uint8_t b;
        if (neo_file_read(1, &b, 1) != 1) break;
        h = hash(h, &b, 1);
    }
    printf("BYTES %04X\n", h);

    /* Lecture séquentielle alignée sur les secteurs. */
    neo_file_seek(1, 0);
    h = 0;
    for (uint32_t off = 0; off < size; off += 512) {
        uint16_t n = neo_file_read(1, buf, 512);
        h = hash(h, buf, n);
        if (n == 0) break;
    }
    printf("SEQ %04X\n", h);
    neo_file_close(1);
    printf("END\n");
    for (;;) { }
}
