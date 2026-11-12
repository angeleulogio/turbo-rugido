/* font.h - fuente de mapa de bits 5x7 dibujada con rectangulos */
#ifndef FONT_H
#define FONT_H

#include "core.h"

float text_width(const char *s, float scale);
void text_draw(float x, float y, const char *s, float scale, Col c);           /* con sombra */
void text_draw_center(float cx, float y, const char *s, float scale, Col c);
void text_draw_right(float rx, float y, const char *s, float scale, Col c);

#endif
