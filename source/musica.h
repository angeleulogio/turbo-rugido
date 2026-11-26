/* musica.h - musica sintetizada: tema del menu y jingle de meta */
#ifndef MUSICA_H
#define MUSICA_H

int musica_menu_muestras(int rate);
void musica_menu_render(short *out, int n, int rate);
int musica_jingle_muestras(int rate);
void musica_jingle_render(short *out, int n, int rate);

#endif
