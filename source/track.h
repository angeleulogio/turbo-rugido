/* track.h - pistas procedurales (diseno original) */
#ifndef TRACK_H
#define TRACK_H

#include "core.h"

#define TRACK_MAX_PTS   1400
#define TRACK_MAX_CHUNKS 64
#define TRACK_SPACING   3.0f

enum { TRACK_OCHO = 0, TRACK_FORTALEZA, TRACK_VALLE, TRACK_COUNT };

typedef struct {
    V3 c;          /* centro */
    float r;       /* radio */
    Mesh mesh;     /* geometria del trozo */
    void *gpu;     /* manejador de plataforma (display list) */
} TrackChunk;

typedef struct {
    int id;
    const char *name;
    int n;                       /* numero de muestras (cerrado) */
    float length;                /* longitud total */
    float ds;                    /* distancia entre muestras */
    float hw;                    /* medio ancho del asfalto */
    float wall;                  /* distancia lateral al muro */
    V3 p[TRACK_MAX_PTS];         /* centro */
    V3 t[TRACK_MAX_PTS];         /* tangente (3D) */
    V3 r[TRACK_MAX_PTS];         /* derecha (horizontal) */
    float curv[TRACK_MAX_PTS];   /* curvatura con signo (rad/m), + = gira a la derecha */
    unsigned char tunnel[TRACK_MAX_PTS];
    unsigned char bridge[TRACK_MAX_PTS];
    V3 center;                   /* centro aproximado del circuito */
    float extent;                /* radio aproximado */
    Col sky_top, sky_horizon, fog;
    TrackChunk chunks[TRACK_MAX_CHUNKS];
    int nchunks;
    Mesh world;                  /* suelo y montanas (siempre visible) */
    void *world_gpu;
} Track;

typedef struct {
    int idx;      /* segmento */
    float s;      /* distancia recorrida desde la meta */
    float d;      /* desplazamiento lateral (+ = derecha) */
    float h;      /* altura del asfalto */
    V3 tan;       /* tangente interpolada */
    V3 right;
} TrackPos;

const char *track_name(int id);
void track_build(Track *tr, int id);           /* genera centro + mallas */
void track_build_centerline(Track *tr, int id);/* solo el trazado (minimapa) */
void track_free_meshes(Track *tr);
TrackPos track_locate(const Track *tr, V3 pos, int hint);  /* hint < 0 => busqueda completa */
V3 track_point(const Track *tr, float s, float d);         /* posicion en el mundo */
V3 track_tangent(const Track *tr, float s);
float track_wrap_s(const Track *tr, float s);
float track_heading(V3 tan);                   /* yaw de una direccion */

#endif
