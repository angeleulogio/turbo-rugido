/* game.h - menus, flujo del juego y dibujo */
#ifndef GAME_H
#define GAME_H

#include "plat.h"

void game_init(void);
void game_frame(const PadIn pads[2], float dt);   /* actualiza y dibuja un cuadro */
int game_wants_exit(void);

/* para pruebas en PC */
int game_state(void);
void game_debug_start_race(int track, int mode, int nplayers, int car0, int car1);

#endif
