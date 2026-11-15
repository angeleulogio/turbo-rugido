/* race.h - estado de la carrera: jugadores, IA, trafico, vueltas */
#ifndef RACE_H
#define RACE_H

#include "core.h"
#include "track.h"
#include "vehicle.h"
#include "plat.h"

#define RACERS      4
#define TRAFFIC_MAX 12
#define POLICE_MAX  4
#define POLICE_BASE (RACERS + TRAFFIC_MAX)
#define ENT_MAX     (RACERS + TRAFFIC_MAX + POLICE_MAX)
#define WRECK_MAX   6     /* restos de autos chocados que quedan en la pista (CHOQUES) */
#define DEBRIS_MAX  16    /* ruedas volando */
#define RACE_LAPS   3

enum { RS_COUNTDOWN = 0, RS_RUNNING, RS_FINISHED };

typedef struct {
    int active;
    int is_player, player;      /* indice de jugador humano (0/1) */
    int is_traffic;
    int is_police;              /* patrulla */
    int roadblock;              /* patrulla detenida en un reten */
    float rb_t;
    int car;                    /* indice en CAR_SPECS (racers) o tipo (trafico) */
    void *gpu;                  /* malla en la plataforma */
    Vehicle veh;                /* solo jugadores */
    /* cinematico (IA y trafico) */
    float s, d, v, vd, target_d, skill, lane;
    /* comun */
    V3 pos;
    float yaw, pitch;
    int lap;
    float prev_s, progress;
    int finished;
    float finish_time;
    int place;
    /* choque */
    int crashed;
    float crash_t, invuln;
    V3 crash_vel, crash_rot, crash_spin;
    float crash_y, crash_vy;
    Col paint, accent;
    float cap_v;                /* limite de velocidad por bloqueo (IA) */
    float bump_cool;
    float stuck_t, reverse_t;   /* piloto automatico */
    /* danos (CHOQUES): el render deforma la malla cuando dmg_dirty = 1 */
    int dmg_dirty;
    V3 dmg_dir;                 /* direccion local del golpe (x derecha, z atras) */
    float dmg_amt;              /* 0..1 */
    int wheels_lost;            /* bits: 0 del-izq, 1 del-der, 2 tras-izq, 3 tras-der */
    int dmg_reset;              /* al reaparecer: volver a la malla original */
    float hit_cd;               /* pausa entre premios por chocar con sus restos */
    int busted;                 /* parado por la policia (no deja restos) */
} Entity;

typedef struct {
    int active;
    int src;                    /* entidad de la que salio la malla */
    int owner;                  /* jugador que choco (-1 si fue la IA o el trafico) */
    int mesh_pending;           /* el render debe copiar la malla deformada */
    int flipped;
    float s, d, t, hit_cd;
    V3 pos;
    float yaw;
} Wreck;

typedef struct {
    int active;
    V3 pos, vel, rot, spin;
    float t, s;
} Debris;

typedef struct {
    float x, y;  /* camara suavizada */
    V3 eye, target;
    float yaw;
    int far_cam;
    float fov;
    char msg[32];
    float msg_t;
    Col msg_col;
    float best_lap, lap_start;
    float shake;
    float prev_rel[TRAFFIC_MAX];
    float bust;                 /* 0..3 s: cuanto falta para ser detenido */
    float slow;                 /* medidor de camara lenta (CALLE) */
    int slowing;
} PlayerView;

typedef struct {
    Track *track;
    PhysMode mode;
    int nplayers;
    int ntraffic;
    Entity ent[ENT_MAX];
    PlayerView view[2];
    int state;
    float countdown;
    float time;
    float finish_wait;
    float slowmo;
    float vlim[TRACK_MAX_PTS];
    float line[TRACK_MAX_PTS];
    int finish_order;
    int last_beep;
    /* persecucion policial */
    int police;
    int heat;                   /* nivel de busqueda 1..5 */
    float heat_t, roadblock_cd, evade_t;
    float siren;                /* volumen de la sirena para el jugador 1 */
    float timescale;            /* camara lenta del jugador (CALLE) */
    Wreck wreck[WRECK_MAX];
    int wreck_next;
    Debris debris[DEBRIS_MAX];
} Race;

void race_setup(Race *r, Track *tr, PhysMode mode, int nplayers, const int cars[2], int police);
void race_update(Race *r, const PadIn pads[2], float dt);
void race_camera(Race *r, int p, M34 view, float *fov);
void race_entity_matrix(const Race *r, const Entity *e, M34 out);
void race_wreck_matrix(const Wreck *w, M34 out);
void race_debris_matrix(const Debris *d, M34 out);
CarInput race_pad_to_input(const PadIn *pad);
CarInput race_autopilot(const Race *r, Entity *e, float dt);
int race_all_players_done(const Race *r);

#endif
