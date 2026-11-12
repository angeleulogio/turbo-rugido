/* main_pc.c - pruebas automaticas en PC: simula carreras y guarda capturas */
#include <stdio.h>
#include "../source/plat.h"
#include "../source/game.h"
#include "../source/race.h"
#include "../source/track.h"

void pc_set_pads(const PadIn p[2]);
void pc_save_ppm(const char *path);
extern int pc_tris_drawn;

static void press(unsigned btn, int frames_after) {
    PadIn p[2];
    int i;
    memset(p, 0, sizeof(p));
    p[0].connected = 1;
    p[0].held = btn; p[0].pressed = btn;
    pc_set_pads(p);
    game_frame(p, 1.0f / 60.0f);
    memset(p, 0, sizeof(p));
    for (i = 0; i < frames_after; i++) game_frame(p, 1.0f / 60.0f);
}

/* simulacion sin graficos: un jugador con piloto automatico */
static void sim_test(int trk, int mode, int car) {
    static Track tr;
    static Race r;
    int cars[2] = {car, 1};
    int f, laps_done = 0, walls = 0, crashes = 0, nan = 0;
    float lap_t[8] = {0}, last = 0, maxspd = 0;
    track_build(&tr, trk);
    race_setup(&r, &tr, (PhysMode)mode, 1, cars);
    for (f = 0; f < 60 * 420; f++) {
        PadIn p[2];
        Entity *pl = &r.ent[RACERS - 1];
        memset(p, 0, sizeof(p));
        if (r.state != RS_COUNTDOWN && !pl->finished) {
            CarInput in = race_autopilot(&r, pl, 1.0f/60.0f);
            p[0].sx = in.steer; p[0].trig_r = in.throttle; p[0].trig_l = in.brake;
            p[0].held = in.boost ? BTN_X : 0;
            if (mode == MODE_CHOQUES && pl->veh.boost > 0.5f) p[0].held |= BTN_X;
        }
        {
            int prev_lap = pl->lap, prev_crash = pl->crashed;
            race_update(&r, p, 1.0f / 60.0f);
            if (pl->lap > prev_lap && pl->lap >= 2) { lap_t[laps_done++ & 7] = r.time - last; last = r.time; }
            else if (pl->lap > prev_lap) last = r.time;
            if (pl->crashed && !prev_crash) crashes++;
        }
        if (pl->veh.wall_impact > 3.0f) walls++;
        {
            float sp = mode == MODE_SIM ? pl->veh.vf : pl->veh.speed;
            if (sp > maxspd) maxspd = sp;
            if (sp != sp || pl->pos.x != pl->pos.x) nan = 1;
        }
        if (r.state == RS_FINISHED) break;
    }
    {
        int i;
        printf("pista=%-16s modo=%-15s auto=%d len=%5.0fm n=%d fin=%s t=%6.1fs vueltas:", tr.name, mode_name(mode), car, tr.length, tr.n,
               r.state == RS_FINISHED ? "SI" : "NO", r.time);
        for (i = 0; i < laps_done && i < 4; i++) printf(" %.1f", lap_t[i]);
        printf(" | pos=%d muros=%d choques=%d vmax=%.0fkmh nan=%d | IA:", r.ent[RACERS - 1].place, walls, crashes, maxspd * 3.6f, nan);
        for (i = 0; i < RACERS - 1; i++) printf(" %d%s", r.ent[i].place, r.ent[i].finished ? "" : "*");
        printf("\n");
    }
    {
        int i, tris = 0;
        for (i = 0; i < tr.nchunks; i++) tris += tr.chunks[i].mesh.n / 3;
        printf("   triangulos pista=%d mundo=%d chunks=%d\n", tris, tr.world.n / 3, tr.nchunks);
    }
    track_free_meshes(&tr);
}

static void shot(const char *name) {
    char b[128];
    snprintf(b, sizeof(b), "shots/%s.ppm", name);
    pc_save_ppm(b);
}

int main(int argc, char **argv) {
    int t, m;
    PadIn none[2];
    memset(none, 0, sizeof(none));
    if (argc > 1 && strcmp(argv[1], "sim") == 0) {
        for (t = 0; t < TRACK_COUNT; t++)
            for (m = 0; m < MODE_COUNT; m++) sim_test(t, m, m == 2 ? 1 : 0);
        sim_test(2, 0, 3); sim_test(0, 2, 2); sim_test(1, 1, 2);
        return 0;
    }
    game_init();
    /* menus */
    game_frame(none, 1.0f / 60.0f); shot("01_titulo");
    press(BTN_START, 2); shot("02_jugadores");
    press(BTN_A, 2); shot("03_fisica");
    press(BTN_A, 2); shot("04_pista");
    press(BTN_DOWN, 2); shot("04b_pista2");
    press(BTN_DOWN, 2); shot("04c_pista3");
    press(BTN_UP, 1); press(BTN_UP, 1);
    press(BTN_A, 30); shot("05_auto");
    /* carreras: cada pista en cada modo, con piloto automatico */
    for (t = 0; t < TRACK_COUNT; t++) {
        for (m = 0; m < MODE_COUNT; m++) {
            int f;
            char nm[64];
            game_debug_start_race(t, m, (t == 1 && m == 1) ? 2 : 1, t, (t + 2) % 4);
            for (f = 0; f < 60 * 3; f++) game_frame(none, 1.0f / 60.0f);
            if (m == 0) { snprintf(nm, sizeof(nm), "10_t%d_salida", t); shot(nm); }
            for (f = 0; f < 60 * 22 + t * 300; f++) {
                PadIn p[2];
                extern Race *pc_race(void);
                memset(p, 0, sizeof(p));
                {
                    Race *r = pc_race();
                    int k;
                    for (k = 0; k < RACERS; k++) {
                        Entity *e = &r->ent[k];
                        if (e->is_player) {
                            CarInput in = race_autopilot(r, e, 1.0f/60.0f);
                            p[e->player].sx = in.steer; p[e->player].trig_r = in.throttle; p[e->player].trig_l = in.brake;
                            if (m == 1 && e->veh.boost > 0.3f) p[e->player].held |= BTN_X;
                        }
                    }
                }
                game_frame(p, 1.0f / 60.0f);
            }
            snprintf(nm, sizeof(nm), "20_t%d_m%d", t, m);
            shot(nm);
            printf("%s triangulos dibujados=%d\n", nm, pc_tris_drawn);
        }
    }
    return 0;
}
