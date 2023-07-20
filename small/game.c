#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "aigame.h"

#define ROWS (22)
#define COLS (80)

static char field[ROWS][COLS];

#define MECOL (COLS/2)
static int me;

static int diff;

void
draw_field (void) {
    int x;
    int y;
    for (x = 0; x != COLS; x++) {
        printf("-");
    }
    printf("\n");
    for (y = 0; y != ROWS; y++) {
        for (x = 0; x != COLS; x++) {
            if ((MECOL == x) && (me == y)) {
                printf("@");
            } else {
                printf("%c", field[y][x] ? '-' : ' ');
            }
        }
        printf("\n");
    }
    for (x = 0; x != COLS; x++) {
        printf("-");
    }
    printf("\n");
    fflush(0);
}

int
shift (void) {
    int x;
    int y;
    for (x = 1; x != COLS; x++) {
        for (y = 0; y != ROWS; y++) {
            field[y][x-1] = field[y][x];
        }
    }
    for (y = 0; y != ROWS; y++) {
            field[y][x-1] = 0;
    }
    /* adding obstacles */
    for (x = 0; x != diff; x++) {
        field[rand() % ROWS][COLS-1] = 1;
    }

    return ((me < 0) || (me >= ROWS) || field[me][MECOL]);
}

void
up (void) {
    me--;
}

void
down (void) {
    me++;
}

void
init_game (int difficulty) {
    int i;
    diff = difficulty;

    memset(field, 0x00, sizeof(field));
    me = ROWS/2;

    for (i = 0; i != (MECOL-10); i++) {
        shift();
    }
}

void
lookahead (int size, uint8_t vis[]) {
    int x = 0;
    int it = 0;
    while (1) {
        int y;
        for (y = me - (x + 1); y != (me + x + 2); y++) {
            vis[it] = field[y][MECOL + x] ? 1 : 0x00;
            if ((y <= 0) || (y >= ROWS)) {
                vis[it] = 1;
            }
            it++;
            if (it == size) {
                return;
            }
        }
        x++;
    }
}
