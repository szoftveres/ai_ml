#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "aigame.h"

void
draw_field (gamefield_t *game) {
    int x;
    int y;
    for (x = 0; x != COLS; x++) {
        printf("-");
    }
    printf("\n");
    for (y = 0; y != ROWS; y++) {
        for (x = 0; x != COLS; x++) {
            if ((MECOL == x) && (game->me == y)) {
                printf("@");
            } else {
                printf("%c", game->field[x][y] ? '-' : ' ');
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
shift (gamefield_t *game) {
    int x;
    for (x = 1; x != COLS; x++) {
        memcpy(game->field[x-1], game->field[x], sizeof(game->field[0]));
    }
    memset(game->field[x-1], 0x00, sizeof(game->field[0]));
    /* adding obstacles */
    for (x = 0; x != game->diff; x++) {
        game->field[COLS-1][rand() % ROWS] = 1;
    }

    return ((game->me < 0) || (game->me >= ROWS) || game->field[MECOL][game->me]);
}

void
up (gamefield_t *game) {
    game->me--;
}

void
down (gamefield_t *game) {
    game->me++;
}

gamefield_t *
init_game (int difficulty) {
    int i;
    gamefield_t *game = malloc(sizeof(gamefield_t));
    if (!game) {
        exit(1);
    }
    game->diff = difficulty;

    memset(game->field, 0x00, sizeof(game->field));
    game->me = ROWS/2;

    for (i = 0; i != (MECOL-10); i++) {
        shift(game);
    }
    return game;
}

void
lookahead (gamefield_t *game, int size, uint8_t vis[]) {
    int x = 0;
    int it = 0;
    while (1) {
        int y;
        for (y = game->me - (x + 1); y != (game->me + x + 2); y++) {
            vis[it] = game->field[MECOL + x][y] ? 1 : 0x00;
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
