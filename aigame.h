#ifndef __AIGAME_H__
#define __AIGAME_H__

#include <stdint.h>

void init_game (void);
void draw_field (void);
void shift (void);
void up (void);
void down (void);
int evaluate (void);
void lookahead(uint8_t vis[]);

#endif
