#include <stdlib.h>
/* main_pc.c - pruebas automaticas en PC: simula carreras y guarda capturas */
#include <stdio.h>
#include "../source/plat.h"
#include "../source/game.h"
#include "../source/race.h"
#include "../source/track.h"
#include "../source/cars.h"
#include <string.h>

void pc_set_pads(const PadIn p[MAX_PLAYERS]);
void pc_save_ppm(const char *path);
extern int pc_tris_drawn;

static void press(unsigned btn, int frames_after) {
    PadIn p[MAX_PLAYERS];
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
    int calor5 = mode >= 200, suv_max = 0, embestidas = 0, suv_prev_cerca[SUV_MAX] = {0};
    int police = mode >= 100, close_f = 0, busts = 0, maxheat = 0, rb = 0, pcrash = 0;
    if (calor5) mode -= 100;
    if (police) mode -= 100;
    static Track tr;
    static Race r;
    int cars[MAX_PLAYERS] = {car, 1, 2, 3};
    int f, laps_done = 0, walls = 0, crashes = 0, nan = 0;
    float lap_t[8] = {0}, last = 0, maxspd = 0;
    track_build(&tr, trk);
    race_setup(&r, &tr, (PhysMode)mode, 1, cars, police);
    if (getenv("VUELTAS")) r.laps = atoi(getenv("VUELTAS"));
    if (calor5) r.heat_t = 185.0f;   /* prueba: busqueda 5 desde el inicio */
    for (f = 0; f < 60 * 420; f++) {
        PadIn p[MAX_PLAYERS];
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
            if (pl->crashed && !prev_crash) { crashes++; if (pl->crash_t < 0) busts++; }
            if (r.heat > maxheat) maxheat = r.heat;
            if (r.siren > 0.85f) close_f++;
            {   /* camionetas activas y golpes contra el jugador */
                int q, act = 0;
                for (q = 0; q < SUV_MAX; q++) {
                    Entity *e = &r.ent[SUV_BASE + q];
                    int cerca;
                    if (!e->active) { suv_prev_cerca[q] = 0; continue; }
                    if (e->suv_player == 0) act++;
                    cerca = vlen(v3(e->pos.x - pl->pos.x, 0, e->pos.z - pl->pos.z)) < 4.6f;
                    if (cerca && !suv_prev_cerca[q]) embestidas++;
                    suv_prev_cerca[q] = cerca;
                }
                if (act > suv_max) suv_max = act;
            }
            { int q; for (q = POLICE_BASE; q < ENT_MAX; q++) { if (r.ent[q].roadblock && r.ent[q].rb_t > 29.98f) rb++; if (r.ent[q].crashed && r.ent[q].crash_t < 0.02f && r.ent[q].crash_t > 0) pcrash++; } }
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
        if (calor5) printf(" | CAMIONETAS max=%d embestidas=%d TAKEDOWNS=%d", suv_max, embestidas, r.suv_takedowns);
        if (r.police) printf(" | SUPER=%d", r.super_police);
        if (police) printf(" | POLICIA calor=%d detenido=%d retenes=%d patrullasfuera=%d cerca=%.0f%%", maxheat, busts, rb, pcrash, 100.0f * close_f / (r.time * 60.0f));
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
    PadIn none[MAX_PLAYERS];
    memset(none, 0, sizeof(none));
    if (argc > 1 && strcmp(argv[1], "sim") == 0) {
        for (t = 0; t < TRACK_COUNT; t++)
            for (m = 0; m < MODE_COUNT; m++) sim_test(t, m, m == 2 ? 1 : 0);
        sim_test(4, 0, 5); sim_test(6, 2, 5); sim_test(7, 1, 5); sim_test(21, 0, 0); sim_test(21, 1, 7); sim_test(21, 2, 4); sim_test(22, 0, 0); sim_test(22, 1, 7); sim_test(22, 2, 4);
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "uno") == 0) { sim_test(atoi(argv[2]), atoi(argv[3]), argc > 4 ? atoi(argv[4]) : 0); return 0; }
    if (argc > 1 && strcmp(argv[1], "police") == 0) {
        for (t = 0; t < TRACK_COUNT; t += 3)
            for (m = 0; m < MODE_COUNT; m++) sim_test(t, 100 + m, m == 2 ? 1 : 0);
        return 0;
    }
    if (argc > 2 && strcmp(argv[1], "recorrido") == 0) {
        int tk = atoi(argv[2]), f, q = 0, every = argc > 3 ? atoi(argv[3]) : 8, from = argc > 4 ? atoi(argv[4]) : 4;
        extern Race *pc_race(void);
        game_init();
        game_debug_start_race(tk, 0, 1, 1, 2);
        for (f = 0; f < 60 * 100; f++) {
            PadIn p[MAX_PLAYERS];
            Race *r = pc_race();
            int k;
            memset(p, 0, sizeof(p));
            for (k = 0; k < RACERS; k++) if (r->ent[k].is_player) {
                CarInput in = race_autopilot(r, &r->ent[k], 1.0f/60.0f);
                p[0].sx = in.steer; p[0].trig_r = in.throttle; p[0].trig_l = in.brake;
            }
            game_frame(p, 1.0f / 60.0f);
            if (f > 60 * from && f % (60 * every) == 0) { char nm[64]; snprintf(nm, sizeof(nm), "r%02d_%02d", tk, q++); shot(nm); }
        }
        return 0;
    }
    if (argc > 7 && strcmp(argv[1], "camara") == 0) {
        /* camara libre: pista ex ey ez tx tz (alturas fijas) */
        extern int g_dbg_cam; extern V3 g_dbg_eye, g_dbg_tgt;
        game_init();
        game_debug_start_race(atoi(argv[2]), 0, 1, 1, 2);
        g_dbg_cam = 1;
        g_dbg_eye = v3(atof(argv[3]), atof(argv[4]), atof(argv[5]));
        g_dbg_tgt = v3(atof(argv[6]), 0, atof(argv[7]));
        press(0, 3);
        shot(argc > 8 ? argv[8] : "camara");
        return 0;
    }
    if (argc > 3 && strcmp(argv[1], "multi") == 0) {
        /* carrera de varios jugadores: cada mando lo maneja el piloto automatico */
        int tk = atoi(argv[2]), np = atoi(argv[3]), f, fin = 0;
        extern Race *pc_race(void);
        char nm[32];
        game_init();
        int maxp[MAX_PLAYERS] = {0}, cam = argc > 5;
        game_debug_start_race(tk, argc > 4 ? atoi(argv[4]) : 0, np, 4, 9);
        if (cam) pc_race()->heat_t = 185.0f;   /* prueba: busqueda 5 desde el inicio */
        for (f = 0; f < 60 * 400 && !fin; f++) {
            PadIn p[MAX_PLAYERS];
            Race *r = pc_race();
            int k;
            memset(p, 0, sizeof(p));
            for (k = 0; k < RACERS; k++) {
                Entity *e = &r->ent[k];
                if (!e->is_player) continue;
                {
                    CarInput in = race_autopilot(r, e, 1.0f / 60.0f);
                    p[e->player].sx = in.steer; p[e->player].trig_r = in.throttle; p[e->player].trig_l = in.brake;
                    if (e->veh.boost > 0.5f) p[e->player].held |= BTN_X;
                }
            }
            game_frame(p, 1.0f / 60.0f);
            {
                int q, cnt[MAX_PLAYERS] = {0};
                for (q = 0; q < SUV_MAX; q++) if (r->ent[SUV_BASE + q].active) cnt[r->ent[SUV_BASE + q].suv_player]++;
                for (q = 0; q < MAX_PLAYERS; q++) if (cnt[q] > maxp[q]) maxp[q] = cnt[q];
            }
            if (f == 60 * 25) { snprintf(nm, sizeof(nm), "multi%d_%d%s", np, tk, cam ? "_cam" : ""); shot(nm); }
            if (cam && f == 60 * 40) { snprintf(nm, sizeof(nm), "multi%d_%d_cam2", np, tk); shot(nm); }
            if (r->state == RS_FINISHED) fin = 1;
        }
        {
            Race *r = pc_race();
            int k, n = 0;
            for (k = 0; k < RACERS; k++) if (r->ent[k].is_player && r->ent[k].finished) n++;
            printf("pista %d, %d jugadores: terminaron %d, tiempo %.1f s, estado %d", tk, np, n, r->time, r->state);
            if (cam) printf(" | camionetas maximas por jugador: %d %d %d %d", maxp[0], maxp[1], maxp[2], maxp[3]);
            printf("\n");
        }
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "nuevas") == 0) {
        /* super patrulla, camara con palanca C y selector de vueltas */
        int f, n = 0;
        extern Race *pc_race(void);
        game_init();
        game_debug_start_race(0, 100, 1, 8, 9);   /* TAURO V10 (clase A) con policia */
        for (f = 0; f < 60 * 200 && n < 2; f++) {
            PadIn p[MAX_PLAYERS];
            Race *r = pc_race();
            Entity *pl = NULL;
            int k;
            memset(p, 0, sizeof(p));
            for (k = 0; k < RACERS; k++) if (r->ent[k].is_player) pl = &r->ent[k];
            if (pl && !pl->crashed) {
                CarInput in = race_autopilot(r, pl, 1.0f / 60.0f);
                p[0].sx = in.steer; p[0].trig_r = in.throttle * 0.75f; p[0].trig_l = in.brake;
            }
            game_frame(p, 1.0f / 60.0f);
            if (pl && f > 60 * 4 && f % 10 == 0)
                for (k = 0; k < POLICE_MAX; k++) {
                    Entity *e = &r->ent[POLICE_BASE + k];
                    V3 fw = v3(sinf(pl->yaw), 0, -cosf(pl->yaw)), dv = vsub(e->pos, pl->pos);
                    float d = vlen(v3(dv.x, 0, dv.z));
                    if (e->active && !e->crashed && d > 6 && d < 24 && vdot(dv, fw) > 4.0f) {
                        char nm[32]; snprintf(nm, sizeof(nm), "n_super%d", n++); shot(nm); f += 200; break;
                    }
                }
        }
        {   /* palanca C: derecha y abajo (mirar atras) */
            static const float cxs[2] = {1, 0}, cys[2] = {0, -1};
            int q;
            for (q = 0; q < 2; q++) {
                for (f = 0; f < 40; f++) {
                    PadIn p[MAX_PLAYERS];
                    memset(p, 0, sizeof(p));
                    p[0].trig_r = 0.5f; p[0].cx = cxs[q]; p[0].cy = cys[q];
                    game_frame(p, 1.0f / 60.0f);
                }
                shot(q == 0 ? "n_camC_derecha" : "n_camC_atras");
            }
        }
        game_init();
        game_frame(none, 1.0f / 60.0f);
        press(BTN_START, 10); press(BTN_A, 10); press(BTN_A, 10); press(BTN_A, 10);
        press(BTN_RIGHT, 8); press(BTN_RIGHT, 8); press(BTN_RIGHT, 8); press(0, 20);
        shot("n_vueltas");
        printf("capturas patrulla: %d\n", n);
        return 0;
    }
    if (argc > 2 && strcmp(argv[1], "suvfoto") == 0) {
        int tk = atoi(argv[2]), f, n = 0;
        extern Race *pc_race(void);
        game_init();
        game_debug_start_race(tk, 100, 1, 4, 9);
        for (f = 0; f < 60 * 240 && n < 3; f++) {
            PadIn p[MAX_PLAYERS];
            Race *r = pc_race();
            Entity *pl = NULL;
            int k;
            memset(p, 0, sizeof(p));
            if (r->heat_t < 185.0f) r->heat_t = 185.0f;
            for (k = 0; k < RACERS; k++) if (r->ent[k].is_player) pl = &r->ent[k];
            if (pl && !pl->crashed) {
                CarInput in = race_autopilot(r, pl, 1.0f / 60.0f);
                p[0].sx = in.steer; p[0].trig_r = in.throttle; p[0].trig_l = in.brake;
            }
            game_frame(p, 1.0f / 60.0f);
            if (pl && f > 60 * 8 && f % 20 == 0)
                for (k = 0; k < SUV_MAX; k++) {
                    Entity *e = &r->ent[SUV_BASE + k];
                    V3 fw = v3(sinf(pl->yaw), 0, -cosf(pl->yaw)), dv = vsub(e->pos, pl->pos);
                    float d = vlen(v3(dv.x, 0, dv.z));
                    if (e->active && !e->crashed && d > 5 && d < 22 && vdot(dv, fw) > 3.0f) {
                        char nm[32]; snprintf(nm, sizeof(nm), "suv%d_%d", tk, n++); shot(nm); f += 120; break;
                    }
                }
        }
        printf("capturas: %d\n", n);
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "pintura") == 0) {
        game_init();
        game_frame(none, 1.0f / 60.0f);
        press(BTN_START, 10); press(BTN_A, 10); press(BTN_A, 10); press(BTN_A, 10); press(BTN_A, 10);
        press(BTN_X, 10); press(BTN_X, 30); shot("m01_pintura");
        press(BTN_RIGHT, 6); press(BTN_RIGHT, 6); press(BTN_X, 30); shot("m02_pintura");
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "clases") == 0) {
        int q;
        game_init();
        game_frame(none, 1.0f / 60.0f);
        press(BTN_START, 10); press(BTN_A, 10); press(BTN_A, 10); press(BTN_A, 10); press(BTN_A, 10);
        shot("k01_claseB");
        press(BTN_UP, 6); for (q = 0; q < 5; q++) press(BTN_RIGHT, 6);
        press(0, 30); shot("k02_claseA");
        press(BTN_UP, 6); for (q = 0; q < 7; q++) press(BTN_RIGHT, 6);
        press(0, 30); shot("k03_claseC");
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "garage") == 0) {
        int q;
        game_init();
        game_frame(none, 1.0f / 60.0f);
        press(0, 10); shot("g00_titulo");
        press(BTN_DOWN, 2); press(BTN_A, 30);
        for (q = 0; q < 8; q++) press(BTN_RIGHT, 2);
        for (q = 8; q < CAR_COUNT; q++) {
            char nm[32];
            press(0, 75);
            snprintf(nm, sizeof(nm), "g%02d_garage", q);
            shot(nm);
            press(BTN_RIGHT, 2);
        }
        press(BTN_X, 30); shot("g20_pintura");
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "fotos_choques") == 0) {
        int f, got_crash = 0, got_wreck = 0, got_hit = 0, q;
        extern Race *pc_race(void);
        game_init();
        game_frame(none, 1.0f / 60.0f);
        press(BTN_START, 2); press(BTN_A, 2); press(BTN_A, 2); press(BTN_A, 2); press(BTN_A, 30);
        for (q = 0; q < 8; q++) press(BTN_RIGHT, 2);
        shot("c00_patrulla");
        { extern void game_debug_damage_preview(int car); game_debug_damage_preview(CAR_POLICE); }
        press(0, 40); shot("c05_abollado");
        press(0, 50); shot("c06_abollado2");
        game_debug_start_race(0, 1, 1, CAR_POLICE, 1);
        for (f = 0; f < 60 * 200 && !(got_crash && got_wreck && got_hit >= 2); f++) {
            PadIn p[MAX_PLAYERS];
            Race *r = pc_race();
            Entity *pl = NULL;
            int k;
            memset(p, 0, sizeof(p));
            for (k = 0; k < RACERS; k++) if (r->ent[k].is_player) pl = &r->ent[k];
            if (!pl->crashed && !pl->finished && r->state == RS_RUNNING) {
                CarInput in = race_autopilot(r, pl, 1.0f/60.0f);
                p[0].sx = in.steer; p[0].trig_r = in.throttle; p[0].trig_l = in.brake;
                if (pl->veh.boost > 0.5f) p[0].held |= BTN_X;
            }
            game_frame(p, 1.0f / 60.0f);
            if (!got_crash && pl->crashed && pl->crash_t > 0.45f && pl->crash_t < 0.5f) { shot("c01_choque_ruedas"); got_crash = 1; }
            for (k = 0; k < WRECK_MAX && !got_wreck; k++) {
                Wreck *w = &r->wreck[k];
                float rel = w->s - pl->s;
                if (w->active && !pl->crashed && rel > 7.0f && rel < 16.0f && fabsf(w->d - pl->d) < 6.0f) { shot("c02_restos"); got_wreck = 1; }
            }
            if (got_hit < 2 && pl->crashed == 0 && pl->invuln <= 0 && strstr(r->view[0].msg, "RESTOS") && r->view[0].msg_t > 1.9f) {
                shot(got_hit ? "c04_restos_nitro2" : "c03_restos_nitro"); got_hit++;
            }
        }
        printf("choque=%d restos=%d nitro_restos=%d\n", got_crash, got_wreck, got_hit);
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "fotos_policia") == 0) {
        static const int tk[4] = {3, 13, 18, 10}, md[4] = {3, 0, 1, 3};
        int q;
        game_init();
        game_frame(none, 1.0f / 60.0f);
        press(BTN_START, 2); press(BTN_A, 2);
        press(BTN_DOWN, 1); press(BTN_DOWN, 1); press(BTN_DOWN, 2); shot("p01_fisica");
        press(BTN_A, 2); press(BTN_DOWN, 2); shot("p02_policia");
        for (q = 0; q < 4; q++) {
            int f;
            char nm[64];
            game_debug_start_race(tk[q], 100 + md[q], 1, q * 2, 1);
            for (f = 0; f < 60 * (40 + q * 15); f++) {
                PadIn p[MAX_PLAYERS];
                extern Race *pc_race(void);
                Race *r = pc_race();
                int k;
                memset(p, 0, sizeof(p));
                for (k = 0; k < RACERS; k++) {
                    Entity *e = &r->ent[k];
                    if (e->is_player) {
                        CarInput in = race_autopilot(r, e, 1.0f/60.0f);
                        p[0].sx = in.steer; p[0].trig_r = in.throttle; p[0].trig_l = in.brake;
                        if (e->veh.boost > 0.4f) p[0].held |= BTN_X;
                        if (r->siren > 0.7f && r->view[0].slow > 0.3f && f > 60 * 30) p[0].held |= BTN_Y;
                    }
                }
                game_frame(p, 1.0f / 60.0f);
            }
            snprintf(nm, sizeof(nm), "p1%d_persecucion", q);
            shot(nm);
        }
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
                PadIn p[MAX_PLAYERS];
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
