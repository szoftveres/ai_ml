#ifndef __AIGAME_H__
#define __AIGAME_H__

#include <stdint.h>

#define ROWS (22)
#define COLS (80)
#define MECOL (COLS/2)

typedef struct gamefield_s {
    char    field[COLS][ROWS];
    int     me;
    int     diff;
} gamefield_t;

gamefield_t *init_game (int difficulty);
void draw_field (gamefield_t *game);
int shift (gamefield_t *game);
void up (gamefield_t *game);
void down (gamefield_t *game);
void lookahead (gamefield_t *game, int size, uint8_t vis[]);

#endif
