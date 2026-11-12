/* race.h - estado de la carrera: jugadores, IA, trafico, vueltas */
#ifndef RACE_H
#define RACE_H

#include "core.h"
#include "track.h"
#include "vehicle.h"
#include "plat.h"

#define RACERS      4
#define TRAFFIC_MAX 12
#define ENT_MAX     (RACERS + TRAFFIC_MAX)
#define RACE_LAPS   3

enum { RS_COUNTDOWN = 0, RS_RUNNING, RS_FINISHED };

typedef struct {
    int active;
    int is_player, player;      /* indice de jugador humano (0/1) */
    int is_traffic;
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
} Entity;

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
} Race;

void race_setup(Race *r, Track *tr, PhysMode mode, int nplayers, const int cars[2]);
void race_update(Race *r, const PadIn pads[2], float dt);
void race_camera(Race *r, int p, M34 view, float *fov);
void race_entity_matrix(const Race *r, const Entity *e, M34 out);
CarInput race_pad_to_input(const PadIn *pad);
CarInput race_autopilot(const Race *r, Entity *e, float dt);
int race_all_players_done(const Race *r);

#endif
