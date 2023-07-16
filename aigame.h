#ifndef __AIGAME_H__
#define __AIGAME_H__

#include <stdint.h>

void init_game (int difficulty);
void draw_field (void);
int shift (void);
void up (void);
void down (void);
void lookahead(uint8_t vis[]);

#endif
