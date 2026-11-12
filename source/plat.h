/* plat.h - interfaz entre el juego y la plataforma (GameCube o PC de pruebas) */
#ifndef PLAT_H
#define PLAT_H

#include "core.h"

#define SCREEN_W 640
#define SCREEN_H 480

typedef struct { int x, y, w, h; } Viewport;

/* botones (mismos significados en todas las plataformas) */
enum {
    BTN_A = 1 << 0, BTN_B = 1 << 1, BTN_X = 1 << 2, BTN_Y = 1 << 3,
    BTN_Z = 1 << 4, BTN_START = 1 << 5, BTN_UP = 1 << 6, BTN_DOWN = 1 << 7,
    BTN_LEFT = 1 << 8, BTN_RIGHT = 1 << 9, BTN_L = 1 << 10, BTN_R = 1 << 11
};

typedef struct {
    int connected;
    float sx, sy;          /* palanca -1..1 (sy + = arriba) */
    float trig_l, trig_r;  /* gatillos analogicos 0..1 */
    unsigned held, pressed;
} PadIn;

/* graficos */
void *plat_mesh_upload(const Mesh *m);      /* devuelve manejador (o NULL si vacia) */
void plat_mesh_release(void *h);
void plat_frame_begin(void);
void plat_frame_end(void);
void plat_sky(const Viewport *vp, Col top, Col horizon);
void plat_view_begin(const Viewport *vp, float fovy, float znear, float zfar, M34 view,
                     Col fog, float fog_start, float fog_end);
void plat_draw(void *mesh, M34 model);        /* model NULL = identidad */
void plat_2d_begin(void);
void plat_rect(float x, float y, float w, float h, Col c);
void plat_2d_end(void);

/* entrada */
void plat_read_pads(PadIn pads[2]);

/* audio */
enum { SFX_BEEP = 0, SFX_GO, SFX_CRASH, SFX_BUMP, SFX_BOOST, SFX_SELECT, SFX_COUNT };
void plat_engine(int player, int on, float rpm01, float load);
void plat_sfx(int id);

#endif
