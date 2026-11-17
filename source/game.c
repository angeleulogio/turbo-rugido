/* game.c - flujo de pantallas, HUD y render de la carrera */
#include <stdio.h>
#include "game.h"
#include "core.h"
#include "track.h"
#include "cars.h"
#include "vehicle.h"
#include "race.h"
#include "font.h"

enum { GS_TITLE = 0, GS_GARAGE, GS_PLAYERS, GS_MODE, GS_POLICE, GS_TRACK, GS_CAR, GS_LOADING, GS_RACE, GS_PAUSE, GS_RESULTS };

static struct {
    int st, sel, nplayers, mode, track, cars[2], car_player, police;
    int g_car, g_paint, g_built_car, g_built_paint, g_auto;   /* garage */
    float g_yaw, g_zoom;
    float t;
    int pause_sel, res_sel;
    int loaded;
    float nav_cool[2];
    float prev_sx[2], prev_sy[2];
} G;

static Track g_track;                 /* pista de la carrera */
static Track g_preview[TRACK_COUNT];  /* trazados para el minimapa */
static Race g_race;
static void *g_car_preview[CAR_COUNT];
static void *g_ent_gpu[ENT_MAX];
static void *g_menu_floor;
static void *g_garage_mesh;   /* auto del garage con la pintura elegida */
static void *g_lightbar[2];   /* torreta de la patrulla: dos fases del destello */
/* danos (CHOQUES): copia original y deformada de cada auto, restos y ruedas sueltas */
static int g_dmg_on;
static Mesh g_ent_orig[ENT_MAX], g_ent_def[ENT_MAX];
static int g_ent_wf[ENT_MAX][4], g_ent_wc[ENT_MAX][4];
static void *g_wreck_gpu[WRECK_MAX];
static void *g_wheel_gpu;

static void mesh_clone(Mesh *dst, const Mesh *src) {
    mesh_init(dst);
    mesh_append(dst, src);
}

static unsigned pos_hash(const MVtx *v) {
    unsigned h = (unsigned)(int)floorf(v->x * 9.0f) * 73856093u ^ (unsigned)(int)floorf(v->y * 9.0f) * 19349663u ^ (unsigned)(int)floorf(v->z * 9.0f) * 83492791u;
    return (h >> 7) & 255u;
}

/* abolla la malla hacia dentro alrededor del punto de impacto y oscurece la pintura;
   solo depende de la posicion, asi los vertices compartidos se mueven igual y no se abren huecos */
static void deform_mesh(int i, V3 dir, float amt, int lost) {
    Mesh *m = &g_ent_def[i];
    V3 P = v3(dir.x * 1.0f, 0.75f, dir.z * 2.3f);
    int k, w, skip;
    for (k = 0; k < m->n; k++) {
        MVtx *v = &m->v[k];
        float dx, dy, dz, dist, f, push, hn;
        skip = 0;
        for (w = 0; w < 4; w++) if (k >= g_ent_wf[i][w] && k < g_ent_wf[i][w] + g_ent_wc[i][w]) skip = 1;
        if (skip) continue;
        dx = v->x - P.x; dy = v->y - P.y; dz = v->z - P.z;
        dist = sqrtf(dx * dx + dy * dy + dz * dz);
        f = 1.0f - dist / 1.6f;
        if (f <= 0) continue;
        hn = pos_hash(v) / 255.0f;
        push = amt * 0.55f * f * f * (0.7f + 0.6f * hn);
        v->x -= dir.x * push;
        v->z -= dir.z * push;
        if (v->y > 0.4f) v->y -= amt * 0.22f * f * hn;
        {
            float sh = 1.0f - 0.4f * f * amt;
            v->r = (unsigned char)(v->r * sh); v->g = (unsigned char)(v->g * sh); v->b = (unsigned char)(v->b * sh);
        }
    }
    /* ruedas que salieron volando: se colapsan */
    for (w = 0; w < 4; w++) {
        if (!(lost & (1 << w))) continue;
        for (k = g_ent_wf[i][w]; k < g_ent_wf[i][w] + g_ent_wc[i][w] && k < m->n; k++) {
            m->v[k].x = m->v[g_ent_wf[i][w]].x; m->v[k].y = m->v[g_ent_wf[i][w]].y; m->v[k].z = m->v[g_ent_wf[i][w]].z;
        }
    }
}

static void process_damage(void) {
    Race *r = &g_race;
    int i;
    if (!g_dmg_on) return;
    /* restos nuevos: copian la malla deformada antes de que el auto se repare */
    for (i = 0; i < WRECK_MAX; i++) {
        Wreck *w = &r->wreck[i];
        if (!w->mesh_pending) continue;
        w->mesh_pending = 0;
        plat_mesh_release(g_wreck_gpu[i]);
        g_wreck_gpu[i] = g_ent_def[w->src].n ? plat_mesh_upload(&g_ent_def[w->src]) : NULL;
    }
    for (i = 0; i < ENT_MAX; i++) {
        Entity *e = &r->ent[i];
        int changed = 0;
        if (!g_ent_orig[i].n) { e->dmg_dirty = e->dmg_reset = 0; continue; }
        if (e->dmg_reset) {
            e->dmg_reset = 0;
            memcpy(g_ent_def[i].v, g_ent_orig[i].v, sizeof(MVtx) * g_ent_orig[i].n);
            changed = 1;
        }
        if (e->dmg_dirty) {
            e->dmg_dirty = 0;
            deform_mesh(i, e->dmg_dir, e->dmg_amt, e->wheels_lost);
            changed = 1;
        }
        if (changed) {
            plat_mesh_release(g_ent_gpu[i]);
            g_ent_gpu[i] = plat_mesh_upload(&g_ent_def[i]);
        }
    }
}

static const Col WHITE = {255, 255, 255, 255};
static const Col YELLOW = {255, 220, 50, 255};
static const Col GREY = {170, 170, 180, 255};
static const Col ORANGE = {255, 140, 30, 255};

int game_state(void) { return G.st; }
int game_wants_exit(void) { return 0; }

/* ------------------------------------------------------------ */

static void fmt_time(char *b, int n, float t) {
    int m, s, c;
    if (t < 0) t = 0;
    m = (int)(t / 60.0f);
    s = (int)(t - m * 60);
    c = (int)((t - m * 60 - s) * 100.0f);
    snprintf(b, n, "%d:%02d.%02d", m, s, c);
}

static unsigned nav(const PadIn *p, int i) {
    /* direcciones del menu: cruceta o palanca (con repeticion) */
    unsigned out = p->pressed & (BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT);
    float sx = p->sx, sy = p->sy;
    if (G.nav_cool[i] > 0) G.nav_cool[i] -= 1.0f / 60.0f;
    if ((fabsf(sx) > 0.6f && fabsf(G.prev_sx[i]) <= 0.6f) || (fabsf(sx) > 0.6f && G.nav_cool[i] <= 0)) {
        out |= sx > 0 ? BTN_RIGHT : BTN_LEFT;
        G.nav_cool[i] = 0.25f;
    }
    if ((fabsf(sy) > 0.6f && fabsf(G.prev_sy[i]) <= 0.6f) || (fabsf(sy) > 0.6f && G.nav_cool[i] <= 0)) {
        out |= sy > 0 ? BTN_UP : BTN_DOWN;
        G.nav_cool[i] = 0.25f;
    }
    G.prev_sx[i] = sx; G.prev_sy[i] = sy;
    return out;
}

/* ------------------------------------------------------------ */

void game_init(void) {
    int i;
    memset(&G, 0, sizeof(G));
    G.nplayers = 1;
    G.cars[0] = 0; G.cars[1] = 1;
    for (i = 0; i < TRACK_COUNT; i++) track_build_centerline(&g_preview[i], i);
    for (i = 0; i < CAR_COUNT; i++) {
        Mesh m;
        car_build_mesh(&m, CAR_SPECS[i].shape, CAR_SPECS[i].paint, CAR_SPECS[i].accent);
        g_car_preview[i] = plat_mesh_upload(&m);
        mesh_free(&m);
    }
    {
        Mesh m;
        int k;
        mesh_init(&m);
        for (k = 0; k < 16; k++) {
            float a0 = 2 * (float)M_PI * k / 16, a1 = 2 * (float)M_PI * (k + 1) / 16;
            mesh_tri_flat(&m, v3(0, 0, 0), v3(cosf(a0) * 4.2f, 0, sinf(a0) * 4.2f), v3(cosf(a1) * 4.2f, 0, sinf(a1) * 4.2f),
                          (k & 1) ? col(60, 60, 70) : col(75, 75, 88));
        }
        g_menu_floor = plat_mesh_upload(&m);
        mesh_free(&m);
    }
}

static void unload_race(void) {
    int i;
    if (!G.loaded) return;
    for (i = 0; i < g_track.nchunks; i++) { plat_mesh_release(g_track.chunks[i].gpu); g_track.chunks[i].gpu = NULL; }
    plat_mesh_release(g_track.world_gpu); g_track.world_gpu = NULL;
    for (i = 0; i < ENT_MAX; i++) { plat_mesh_release(g_ent_gpu[i]); g_ent_gpu[i] = NULL; }
    for (i = 0; i < 2; i++) { plat_mesh_release(g_lightbar[i]); g_lightbar[i] = NULL; }
    for (i = 0; i < ENT_MAX; i++) { mesh_free(&g_ent_orig[i]); mesh_free(&g_ent_def[i]); }
    for (i = 0; i < WRECK_MAX; i++) { plat_mesh_release(g_wreck_gpu[i]); g_wreck_gpu[i] = NULL; }
    plat_mesh_release(g_wheel_gpu); g_wheel_gpu = NULL;
    g_dmg_on = 0;
    track_free_meshes(&g_track);
    G.loaded = 0;
}

static int mode_menu_index(int mode) {
    return mode == MODE_CHOQUES ? 1 : 0;
}

static void restart_race(void) {
    int i;
    race_setup(&g_race, &g_track, (PhysMode)G.mode, G.nplayers, G.cars, G.police);
    for (i = 0; i < ENT_MAX; i++) g_race.ent[i].dmg_reset = 1;
    process_damage();
}

static void load_race(void) {
    int i;
    unload_race();
    track_build(&g_track, G.track);
    for (i = 0; i < g_track.nchunks; i++) {
        g_track.chunks[i].gpu = plat_mesh_upload(&g_track.chunks[i].mesh);
        mesh_free(&g_track.chunks[i].mesh);
    }
    g_track.world_gpu = plat_mesh_upload(&g_track.world);
    mesh_free(&g_track.world);
    race_setup(&g_race, &g_track, (PhysMode)G.mode, G.nplayers, G.cars, G.police);
    for (i = 0; i < ENT_MAX; i++) {
        Entity *e = &g_race.ent[i];
        Mesh m;
        if (!e->active && !e->is_police) continue;
        if (e->is_traffic) traffic_build_mesh(&m, e->car, e->paint);
        else car_build_mesh(&m, CAR_SPECS[e->car].shape, e->paint, e->accent);
        g_ent_gpu[i] = plat_mesh_upload(&m);
        if (G.mode == MODE_CHOQUES) {
            int w;
            for (w = 0; w < 4; w++) { g_ent_wf[i][w] = g_wheel_first[w]; g_ent_wc[i][w] = g_wheel_count[w]; }
            mesh_clone(&g_ent_orig[i], &m);
            mesh_clone(&g_ent_def[i], &m);
        }
        mesh_free(&m);
    }
    g_dmg_on = (G.mode == MODE_CHOQUES);
    {
        Mesh m;
        wheel_build_mesh(&m);
        g_wheel_gpu = plat_mesh_upload(&m);
        mesh_free(&m);
    }
    for (i = 0; i < 2; i++) {
        Mesh m;
        Col on1 = i ? col(255, 40, 40) : col(90, 20, 20), on2 = i ? col(30, 30, 90) : col(40, 110, 255);
        mesh_init(&m);
        mesh_box(&m, v3(-0.62f, 1.37f, 0.10f), v3(-0.02f, 1.55f, 0.40f), on1, on1);
        mesh_box(&m, v3(0.02f, 1.37f, 0.10f), v3(0.62f, 1.55f, 0.40f), on2, on2);
        g_lightbar[i] = plat_mesh_upload(&m);
        mesh_free(&m);
    }
    G.loaded = 1;
}

void game_debug_start_race(int track, int mode, int nplayers, int car0, int car1) {
    G.police = (mode >= 100); if (mode >= 100) mode -= 100;
    G.track = track; G.mode = mode; G.nplayers = nplayers; G.cars[0] = car0; G.cars[1] = car1;
    load_race();
    G.st = GS_RACE;
}

/* ------------------------------------------------------------ */
/* dibujo de menus                                               */
/* ------------------------------------------------------------ */

static void menu_background(int car, float ypos) {
    Viewport vp = {0, 0, SCREEN_W, SCREEN_H};
    M34 view, model;
    plat_sky(&vp, col(20, 24, 70), col(200, 90, 60));
    m34_lookat(view, v3(0, 2.4f, 7.5f), v3(0, 1, 0), v3(0, 0.6f, 0));
    plat_view_begin(&vp, 45.0f, 0.5f, 100.0f, view, col(200, 90, 60), 60.0f, 90.0f);
    {
        M34 t;
        m34_identity(t);
        t[0][3] = 0; t[1][3] = -0.02f + ypos; t[2][3] = 0;
        plat_draw(g_menu_floor, t);
    }
    if (car >= 0) {
        m34_rot_y(model, G.t * 0.8f);
        model[1][3] = ypos;
        plat_draw(g_car_preview[car], model);
    }
}

static void draw_list(const char **items, int n, int sel, float y0, float step, float sc) {
    int i;
    for (i = 0; i < n; i++) {
        Col c = (i == sel) ? YELLOW : WHITE;
        if (i == sel) {
            float w = text_width(items[i], sc) + 40;
            plat_rect(SCREEN_W / 2 - w / 2, y0 + i * step - 6, w, 7 * sc + 12, cola(0, 0, 0, 120));
            text_draw(SCREEN_W / 2 - w / 2 + 6, y0 + i * step, ">", sc, YELLOW);
        }
        text_draw_center(SCREEN_W / 2, y0 + i * step, items[i], sc, c);
    }
}

static void draw_minimap(const Track *tr, float cx, float cy, float size, const Race *r) {
    int i;
    float sc = size / (tr->extent * 2.0f);
    for (i = 0; i < tr->n; i += 3) {
        float x = cx + (tr->p[i].x - tr->center.x) * sc, y = cy + (tr->p[i].z - tr->center.z) * sc;
        Col c = tr->tunnel[i] ? col(120, 100, 90) : (tr->bridge[i] ? col(200, 200, 255) : WHITE);
        plat_rect(x - 1.5f, y - 1.5f, 3, 3, c);
    }
    {
        float x = cx + (tr->p[0].x - tr->center.x) * sc, y = cy + (tr->p[0].z - tr->center.z) * sc;
        plat_rect(x - 3, y - 3, 6, 6, col(240, 40, 40));
    }
    if (r) {
        for (i = 0; i < RACERS; i++) {
            const Entity *e = &r->ent[i];
            float x = cx + (e->pos.x - tr->center.x) * sc, y = cy + (e->pos.z - tr->center.z) * sc;
            float s = e->is_player ? 7 : 5;
            plat_rect(x - s / 2 - 1, y - s / 2 - 1, s + 2, s + 2, col(0, 0, 0));
            plat_rect(x - s / 2, y - s / 2, s, s, e->paint);
        }
        for (i = POLICE_BASE; i < ENT_MAX; i++) {
            const Entity *e = &r->ent[i];
            float x = cx + (e->pos.x - tr->center.x) * sc, y = cy + (e->pos.z - tr->center.z) * sc;
            if (!e->active || !e->is_police) continue;
            plat_rect(x - 3, y - 3, 6, 6, (((int)(r->time * 6)) & 1) ? col(255, 50, 50) : col(60, 120, 255));
        }
    }
}

static void stat_bar(float x, float y, const char *label, float v) {
    text_draw(x, y, label, 2, GREY);
    plat_rect(x + 120, y, 200, 14, cola(0, 0, 0, 140));
    plat_rect(x + 120, y, 200 * clampf(v, 0, 1), 14, ORANGE);
}

/* ------------------------------------------------------------ */
/* HUD                                                           */
/* ------------------------------------------------------------ */

static void draw_hud(const Viewport *vp, int p) {
    Race *r = &g_race;
    PlayerView *v = &r->view[p];
    Entity *e = NULL;
    int i;
    float S = (G.nplayers > 1) ? 2.0f : 3.0f;   /* escala de texto */
    float x0 = vp->x, y0 = vp->y, W = vp->w, H = vp->h;
    char b[48];
    for (i = 0; i < RACERS; i++) if (r->ent[i].is_player && r->ent[i].player == p) e = &r->ent[i];
    if (!e) return;

    /* posicion */
    snprintf(b, sizeof(b), "%d", e->place);
    text_draw(x0 + 16, y0 + 12, b, S * 2.2f, YELLOW);
    snprintf(b, sizeof(b), "/%d", RACERS);
    text_draw(x0 + 16 + text_width("0", S * 2.2f) + 4, y0 + 12 + S * 7, b, S, WHITE);

    /* vuelta y tiempo */
    {
        int lap = e->lap < 1 ? 1 : (e->lap > RACE_LAPS ? RACE_LAPS : e->lap);
        snprintf(b, sizeof(b), "VUELTA %d/%d", lap, RACE_LAPS);
        text_draw_right(x0 + W - 16, y0 + 12, b, S, WHITE);
        fmt_time(b, sizeof(b), e->finished ? e->finish_time : r->time);
        text_draw_right(x0 + W - 16, y0 + 12 + S * 9, b, S, WHITE);
        if (v->best_lap > 0) {
            char t[24];
            fmt_time(t, sizeof(t), v->best_lap);
            snprintf(b, sizeof(b), "MEJOR %s", t);
            text_draw_right(x0 + W - 16, y0 + 12 + S * 18, b, S * 0.67f, GREY);
        }
    }

    /* velocimetro */
    {
        float spd = (r->mode == MODE_SIM) ? e->veh.vf : e->veh.speed;
        float rpm = e->veh.rpm01;
        snprintf(b, sizeof(b), "%d", (int)(fabsf(spd) * 3.6f + 0.5f));
        text_draw_right(x0 + W - 16 - text_width("KM/H", S * 0.67f) - 8, y0 + H - 16 - S * 7 * 1.6f, b, S * 1.6f, WHITE);
        text_draw_right(x0 + W - 16, y0 + H - 16 - S * 7 * 0.67f, "KM/H", S * 0.67f, GREY);
        snprintf(b, sizeof(b), "%d", e->veh.gear);
        text_draw_right(x0 + W - 16, y0 + H - 16 - S * 7 * 1.6f, b, S * 0.9f, ORANGE);
        plat_rect(x0 + W - 16 - 150, y0 + H - 16 - S * 7 * 1.6f - 14, 150, 8, cola(0, 0, 0, 140));
        plat_rect(x0 + W - 16 - 150, y0 + H - 16 - S * 7 * 1.6f - 14, 150 * rpm, 8, rpm > 0.9f ? col(255, 60, 40) : col(120, 230, 120));
    }

    /* turbo / nitro */
    if (r->mode != MODE_SIM) {
        float bw = 180 * (S / 3.0f) + 60;
        float bx = x0 + 16, by = y0 + H - 16 - 18;
        text_draw(bx, by - S * 7 - 4, r->mode == MODE_CALLE ? "NITRO" : "TURBO", S * 0.67f, e->veh.boosting ? ORANGE : WHITE);
        plat_rect(bx, by, bw, 14, cola(0, 0, 0, 150));
        plat_rect(bx + 2, by + 2, (bw - 4) * e->veh.boost, 10, e->veh.boosting ? ORANGE : col(70, 170, 255));
        if (r->mode == MODE_CALLE && G.nplayers == 1) {
            float sy = by - S * 7 - 30;
            text_draw(bx, sy - S * 7 * 0.67f - 4, "CAMARA LENTA (Y)", S * 0.5f, v->slowing ? col(255, 230, 90) : GREY);
            plat_rect(bx, sy, bw, 10, cola(0, 0, 0, 150));
            plat_rect(bx + 2, sy + 2, (bw - 4) * v->slow, 6, col(255, 220, 60));
        }
    }
    /* persecucion: nivel de busqueda y medidor de detencion */
    if (r->police && r->state != RS_COUNTDOWN) {
        int k2;
        float sx = x0 + W / 2 - 70;
        text_draw(sx - 6, y0 + 12, "BUSQUEDA", S * 0.5f, col(255, 90, 90));
        for (k2 = 0; k2 < 5; k2++) {
            Col c = k2 < r->heat ? ((((int)(r->time * 4)) & 1) ? col(255, 60, 60) : col(80, 140, 255)) : cola(0, 0, 0, 120);
            plat_rect(sx + 84 + k2 * 14, y0 + 13, 10, 10, c);
        }
        if (v->bust > 0.05f) {
            text_draw_center(x0 + W / 2, y0 + H * 0.62f, "DETENIENDO...", S * 0.8f, col(255, 70, 70));
            plat_rect(x0 + W / 2 - 80, y0 + H * 0.62f + S * 8, 160, 10, cola(0, 0, 0, 150));
            plat_rect(x0 + W / 2 - 78, y0 + H * 0.62f + S * 8 + 2, 156 * clampf(v->bust / 3.0f, 0, 1), 6, col(255, 60, 60));
        }
    }
    if (r->mode == MODE_DERRAPE && e->veh.drifting) {
        text_draw(x0 + 16, y0 + H - 16 - 18 - S * 7 * 2.2f, "DERRAPE", S * 0.8f, col(255, 120, 220));
    }

    /* cuenta atras */
    if (r->state == RS_COUNTDOWN) {
        int sec = (int)ceilf(r->countdown);
        if (sec <= 3 && sec >= 1) {
            snprintf(b, sizeof(b), "%d", sec);
            text_draw_center(x0 + W / 2, y0 + H * 0.3f, b, S * 4, YELLOW);
        }
    }
    /* mensajes */
    if (v->msg_t > 0) {
        float a = clampf(v->msg_t * 2, 0, 1);
        Col c = v->msg_col;
        c.a = (unsigned char)(255 * a);
        text_draw_center(x0 + W / 2, y0 + H * 0.36f, v->msg, S * 1.4f, c);
    }
    if (e->finished) {
        snprintf(b, sizeof(b), "POSICION FINAL: %d", e->place);
        text_draw_center(x0 + W / 2, y0 + H * 0.52f, b, S, WHITE);
    }
    /* minimapa (solo 1 jugador) */
    if (G.nplayers == 1) {
        plat_rect(x0 + W - 150, y0 + H / 2 - 70, 140, 140, cola(0, 0, 0, 90));
        draw_minimap(r->track, x0 + W - 80, y0 + H / 2, 125, r);
    }
}

/* ------------------------------------------------------------ */

static void render_race(void) {
    Race *r = &g_race;
    Track *tr = &g_track;
    Viewport vps[2];
    int p, i, np = G.nplayers;
    if (np == 1) { vps[0].x = 0; vps[0].y = 0; vps[0].w = SCREEN_W; vps[0].h = SCREEN_H; }
    else {
        vps[0].x = 0; vps[0].y = 0; vps[0].w = SCREEN_W; vps[0].h = SCREEN_H / 2;
        vps[1].x = 0; vps[1].y = SCREEN_H / 2; vps[1].w = SCREEN_W; vps[1].h = SCREEN_H / 2;
    }
    for (p = 0; p < np; p++) {
        M34 view;
        float fov;
        V3 eye = r->view[p].eye, fwd = vnorm(vsub(r->view[p].target, r->view[p].eye));
        plat_sky(&vps[p], tr->sky_top, tr->sky_horizon);
        race_camera(r, p, view, &fov);
        if (np > 1) fov *= 0.72f;
        plat_view_begin(&vps[p], fov, 0.5f, tr->zfar, view, tr->fog, tr->fog_start, tr->fog_end);
        plat_draw(tr->world_gpu, NULL);
        for (i = 0; i < tr->nchunks; i++) {
            TrackChunk *c = &tr->chunks[i];
            V3 dv = vsub(c->c, eye);
            float dist = vlen(dv);
            if (!c->gpu) continue;
            if (dist - c->r > fminf(1400.0f, tr->fog_end + 150.0f)) continue;
            if (vdot(dv, fwd) < -c->r) continue;
            plat_draw(c->gpu, NULL);
        }
        for (i = 0; i < ENT_MAX; i++) {
            Entity *e = &r->ent[i];
            M34 m;
            V3 dv;
            if (!e->active || !g_ent_gpu[i]) continue;
            dv = vsub(e->pos, eye);
            if (vlen(dv) > 700.0f || vdot(dv, fwd) < -6.0f) continue;
            /* parpadeo durante la invulnerabilidad */
            if (e->invuln > 0 && ((int)(e->invuln * 12) & 1)) continue;
            race_entity_matrix(r, e, m);
            plat_draw(g_ent_gpu[i], m);
            if (e->is_police || (e->is_player && e->car == CAR_POLICE && !e->crashed))
                plat_draw(g_lightbar[((int)(r->time * 7.0f)) & 1], m);
        }
        for (i = 0; i < WRECK_MAX; i++) {
            const Wreck *w = &r->wreck[i];
            M34 m;
            V3 dv;
            if (!w->active || !g_wreck_gpu[i]) continue;
            dv = vsub(w->pos, eye);
            if (vlen(dv) > 700.0f || vdot(dv, fwd) < -6.0f) continue;
            race_wreck_matrix(w, m);
            plat_draw(g_wreck_gpu[i], m);
        }
        for (i = 0; i < DEBRIS_MAX; i++) {
            const Debris *d = &r->debris[i];
            M34 m;
            if (!d->active || !g_wheel_gpu) continue;
            race_debris_matrix(d, m);
            plat_draw(g_wheel_gpu, m);
        }
    }
    plat_2d_begin();
    for (p = 0; p < np; p++) draw_hud(&vps[p], p);
    if (np > 1) plat_rect(0, SCREEN_H / 2 - 2, SCREEN_W, 4, col(0, 0, 0));
}

static void render_results(void) {
    Race *r = &g_race;
    int place, i;
    char b[64];
    plat_rect(60, 60, SCREEN_W - 120, SCREEN_H - 120, cola(0, 0, 0, 170));
    text_draw_center(SCREEN_W / 2, 80, "RESULTADOS", 4, YELLOW);
    for (place = 1; place <= RACERS; place++) {
        for (i = 0; i < RACERS; i++) {
            Entity *e = &r->ent[i];
            char t[24], who[8];
            if (e->place != place) continue;
            if (e->finished) fmt_time(t, sizeof(t), e->finish_time); else snprintf(t, sizeof(t), "--:--.--");
            if (e->is_player) snprintf(who, sizeof(who), "J%d", e->player + 1); else snprintf(who, sizeof(who), "CPU");
            snprintf(b, sizeof(b), "%d. %-11s %-3s %s", place, CAR_SPECS[e->car].name, who, t);
            text_draw(100, 140 + (place - 1) * 36, b, 3, e->is_player ? YELLOW : WHITE);
        }
    }
    {
        const char *opts[] = {"REVANCHA", "MENU PRINCIPAL"};
        draw_list(opts, 2, G.res_sel, 320, 34, 3);
    }
}

/* ------------------------------------------------------------ */

static void engine_audio(const PadIn pads[2]) {
    int i;
    if (G.st != GS_RACE) { plat_engine(0, 0, 0, 0); plat_engine(1, 0, 0, 0); plat_siren(0); return; }
    plat_siren(g_race.police ? g_race.siren : 0);
    for (i = 0; i < RACERS; i++) {
        Entity *e = &g_race.ent[i];
        if (!e->is_player) continue;
        if (e->crashed) plat_engine(e->player, 1, 0.15f, 0.2f);
        else {
            float rpm = g_race.state == RS_COUNTDOWN ? 0.2f + 0.6f * fmaxf(pads[e->player].trig_r, (pads[e->player].held & BTN_A) ? 1.0f : 0.0f)
                                                    : e->veh.rpm01;
            plat_engine(e->player, 1, rpm, (pads[e->player].held & BTN_A) || pads[e->player].trig_r > 0.2f ? 1.0f : 0.4f);
        }
    }
}

static PadIn g_pads_last[2];

/* ------------------------------------------------------------ */
/* GARAGE: ver cada auto de cerca, con su ficha                  */
/* ------------------------------------------------------------ */

static void garage_build(void) {
    Mesh m;
    const CarSpec *sp = &CAR_SPECS[G.g_car];
    Col paint = G.g_paint == 0 ? sp->paint : CAR_ALT_PAINTS[G.g_paint - 1];
    if (G.g_built_car == G.g_car && G.g_built_paint == G.g_paint && g_garage_mesh) return;
    car_build_mesh(&m, sp->shape, paint, sp->accent);
    plat_mesh_release(g_garage_mesh);
    g_garage_mesh = plat_mesh_upload(&m);
    mesh_free(&m);
    G.g_built_car = G.g_car;
    G.g_built_paint = G.g_paint;
}

static void garage_screen(unsigned n0, unsigned pr0) {
    const CarSpec *sp;
    const CarInfo *ci;
    const PadIn *p = &g_pads_last[0];
    Viewport vp = {0, 0, SCREEN_W, SCREEN_H};
    M34 view, model;
    float d, dt = 1.0f / 60.0f;
    char b[64];
    const char *traccion;

    if (n0 & BTN_LEFT) { G.g_car = (G.g_car + CAR_COUNT - 1) % CAR_COUNT; G.g_paint = 0; plat_sfx(SFX_SELECT); }
    if (n0 & BTN_RIGHT) { G.g_car = (G.g_car + 1) % CAR_COUNT; G.g_paint = 0; plat_sfx(SFX_SELECT); }
    if (pr0 & BTN_X) { G.g_paint = (G.g_paint + 1) % 7; plat_sfx(SFX_SELECT); }
    if (pr0 & BTN_A) G.g_auto ^= 1;
    if (pr0 & BTN_B) { G.st = GS_TITLE; G.sel = 1; plat_sfx(SFX_SELECT); return; }
    /* gatillos L/R giran el auto; palanca arriba/abajo acerca o aleja */
    if (p->trig_l > 0.1f || (p->held & BTN_L)) { G.g_yaw -= 2.2f * dt * fmaxf(p->trig_l, 0.6f); G.g_auto = 0; }
    if (p->trig_r > 0.1f || (p->held & BTN_R)) { G.g_yaw += 2.2f * dt * fmaxf(p->trig_r, 0.6f); G.g_auto = 0; }
    if (G.g_auto) G.g_yaw += 0.45f * dt;
    if (fabsf(p->sy) > 0.25f) G.g_zoom = clampf(G.g_zoom - p->sy * 0.9f * dt, 0.6f, 1.5f);
    if (p->held & BTN_Z) G.g_zoom = 1.0f;
    garage_build();
    sp = &CAR_SPECS[G.g_car];
    ci = &CAR_INFO[G.g_car];

    /* auto en el centro, en un piso de exhibicion, con camara que lo rodea */
    plat_sky(&vp, col(30, 34, 46), col(96, 100, 112));
    d = 7.2f * G.g_zoom;
    m34_lookat(view, v3(sinf(G.g_yaw) * d, 1.2f + 1.1f * G.g_zoom, cosf(G.g_yaw) * d), v3(0, 1, 0), v3(0, 0.25f, 0));
    plat_view_begin(&vp, 42.0f, 0.3f, 100.0f, view, col(96, 100, 112), 60.0f, 90.0f);
    {
        M34 t;
        m34_identity(t);
        t[1][3] = -0.02f;
        plat_draw(g_menu_floor, t);
    }
    m34_identity(model);
    model[1][3] = 0.0f;
    plat_draw(g_garage_mesh, model);

    plat_2d_begin();
    text_draw(16, 14, "GARAGE", 3, YELLOW);
    snprintf(b, sizeof(b), "%d/%d", G.g_car + 1, CAR_COUNT);
    text_draw_right(SCREEN_W - 16, 14, b, 3, WHITE);
    text_draw_center(SCREEN_W / 2, 46, sp->name, 4, WHITE);
    text_draw_center(SCREEN_W / 2, 84, sp->desc, 2, GREY);
    text_draw(20, 200, "<", 5, WHITE);
    text_draw_right(SCREEN_W - 20, 200, ">", 5, WHITE);

    /* ficha */
    plat_rect(0, 296, SCREEN_W, 184, cola(0, 0, 0, 150));
    stat_bar(16, 306, "VELOCIDAD", sp->stat_speed);
    stat_bar(16, 326, "ACELERAC.", sp->stat_accel);
    stat_bar(16, 346, "MANEJO", sp->stat_handling);
    stat_bar(16, 366, "DERRAPE", clampf((sp->drift - 0.85f) / 0.40f, 0.05f, 1));
    traccion = sp->drive == 2 ? "TOTAL" : (sp->drive == 1 ? "DELANTERA" : "TRASERA");
    snprintf(b, sizeof(b), "MOTOR: %s", ci->motor);              text_draw(356, 306, b, 2, WHITE);
    snprintf(b, sizeof(b), "POTENCIA: %d CV", (int)(sp->power / 735.5f + 0.5f)); text_draw(356, 326, b, 2, WHITE);
    snprintf(b, sizeof(b), "PESO: %d KG", (int)sp->mass);       text_draw(356, 346, b, 2, WHITE);
    snprintf(b, sizeof(b), "TRACCION: %s", traccion);           text_draw(356, 366, b, 2, WHITE);
    snprintf(b, sizeof(b), "VEL. MAX: %d KM/H", (int)(sp->vmax * 3.6f + 0.5f)); text_draw(356, 386, b, 2, WHITE);
    text_draw(16, 408, ci->line1, 2, col(230, 230, 210));
    text_draw(16, 428, ci->line2, 2, col(230, 230, 210));
    text_draw(16, 450, ci->credit, 2, ORANGE);
    text_draw_right(SCREEN_W - 12, 470 - 2, "L/R GIRAR  PALANCA ZOOM  X PINTURA  A AUTO-GIRO  B SALIR", 1, GREY);
}

void game_frame(const PadIn pads[2], float dt) {
    g_pads_last[0] = pads[0]; g_pads_last[1] = pads[1];
    unsigned n0 = nav(&pads[0], 0);
    unsigned pr0 = pads[0].pressed;
    G.t += dt;

    plat_frame_begin();

    switch (G.st) {
    case GS_TITLE: {
        const char *it[] = {"CARRERA", "GARAGE"};
        menu_background(((int)(G.t / 4.0f)) % CAR_COUNT, -0.2f);
        plat_2d_begin();
        text_draw_center(SCREEN_W / 2, 70, "TURBO", 8, YELLOW);
        text_draw_center(SCREEN_W / 2, 140, "RUGIDO", 8, ORANGE);
        text_draw_center(SCREEN_W / 2, 220, "CARRERAS ARCADE PARA GAMECUBE", 2, WHITE);
        draw_list(it, 2, G.sel & 1, 330, 40, 3);
        if (n0 & (BTN_UP | BTN_DOWN)) { G.sel = (G.sel & 1) ^ 1; plat_sfx(SFX_SELECT); }
        if (pr0 & (BTN_START | BTN_A)) {
            plat_sfx(SFX_SELECT);
            if ((G.sel & 1) == 0) { G.st = GS_PLAYERS; G.sel = G.nplayers - 1; }
            else { G.st = GS_GARAGE; G.g_yaw = 0.6f; G.g_zoom = 1.0f; G.g_auto = 1; G.g_built_car = -1; }
        }
        break;
    }

    case GS_GARAGE:
        garage_screen(n0, pr0);
        break;

    case GS_PLAYERS: {
        const char *it[] = {"1 JUGADOR", "2 JUGADORES"};
        menu_background(G.cars[0], -0.6f);
        plat_2d_begin();
        text_draw_center(SCREEN_W / 2, 50, "JUGADORES", 4, YELLOW);
        draw_list(it, 2, G.sel, 120, 40, 3);
        if (G.sel == 1) text_draw_center(SCREEN_W / 2, 210, "PANTALLA DIVIDIDA - CONECTA 2 MANDOS", 2, GREY);
        if (n0 & (BTN_UP | BTN_DOWN)) { G.sel ^= 1; plat_sfx(SFX_SELECT); }
        if (pr0 & BTN_A) { G.nplayers = G.sel + 1; G.st = GS_MODE; G.sel = mode_menu_index(G.mode); plat_sfx(SFX_SELECT); }
        if (pr0 & BTN_B) { G.st = GS_TITLE; G.sel = 0; }
        break;
    }

    case GS_MODE: {
        /* SIMULACION y CALLEJERO ya no se ofrecen: quedan las dos fisicas arcade
           (las persecuciones siguen disponibles en las dos) */
        static const int menu_modes[2] = {MODE_DERRAPE, MODE_CHOQUES};
        const char *it[2];
        int k;
        for (k = 0; k < 2; k++) it[k] = mode_name(menu_modes[k]);
        menu_background(G.cars[0], -0.6f);
        plat_2d_begin();
        text_draw_center(SCREEN_W / 2, 50, "FISICA", 4, YELLOW);
        draw_list(it, 2, G.sel, 120, 40, 3);
        text_draw_center(SCREEN_W / 2, 240, mode_desc(menu_modes[G.sel]), 2, GREY);
        if (n0 & (BTN_UP | BTN_DOWN)) { G.sel ^= 1; plat_sfx(SFX_SELECT); }
        if (pr0 & BTN_A) { G.mode = menu_modes[G.sel]; G.st = GS_POLICE; G.sel = G.police; plat_sfx(SFX_SELECT); }
        if (pr0 & BTN_B) { G.st = GS_PLAYERS; G.sel = G.nplayers - 1; }
        break;
    }

    case GS_POLICE: {
        const char *it[] = {"SIN POLICIA", "CON PERSECUCION"};
        menu_background(G.cars[0], -0.6f);
        plat_2d_begin();
        text_draw_center(SCREEN_W / 2, 50, "POLICIA", 4, YELLOW);
        draw_list(it, 2, G.sel, 120, 40, 3);
        if (G.sel == 1) {
            text_draw_center(SCREEN_W / 2, 220, "LAS PATRULLAS TE PERSIGUEN Y EMBISTEN", 2, GREY);
            text_draw_center(SCREEN_W / 2, 244, "SI TE DETIENES CERCA, TE ARRESTAN", 2, GREY);
            text_draw_center(SCREEN_W / 2, 268, "LA BUSQUEDA SUBE CON EL TIEMPO", 2, GREY);
            text_draw_center(SCREEN_W / 2, 292, "CUIDADO CON LOS RETENES", 2, GREY);
        }
        if (n0 & (BTN_UP | BTN_DOWN)) { G.sel ^= 1; plat_sfx(SFX_SELECT); }
        if (pr0 & BTN_A) { G.police = G.sel; G.st = GS_TRACK; G.sel = G.track; plat_sfx(SFX_SELECT); }
        if (pr0 & BTN_B) { G.st = GS_MODE; G.sel = mode_menu_index(G.mode); }
        break;
    }

    case GS_TRACK: {
        const char *it[TRACK_COUNT];
        char b[32];
        int k;
        Viewport vp = {0, 0, SCREEN_W, SCREEN_H};
        for (k = 0; k < TRACK_COUNT; k++) it[k] = track_name(k);
        plat_sky(&vp, g_preview[G.sel].sky_top.r ? g_preview[G.sel].sky_top : col(30, 40, 90), col(30, 40, 90));
        plat_2d_begin();
        text_draw_center(SCREEN_W / 2, 40, "PISTA", 4, YELLOW);
        {
            /* lista con desplazamiento: 9 pistas visibles */
            int vis = 9, first = G.sel - 4;
            if (first > TRACK_COUNT - vis) first = TRACK_COUNT - vis;
            if (first < 0) first = 0;
            draw_list(it + first, TRACK_COUNT - first < vis ? TRACK_COUNT - first : vis, G.sel - first, 82, 23, 2);
            if (first > 0) text_draw_center(SCREEN_W / 2 + 150, 82, "+", 2, GREY);
            if (first + vis < TRACK_COUNT) text_draw_center(SCREEN_W / 2 + 150, 82 + 8 * 23, "+", 2, GREY);
        }
        plat_rect(SCREEN_W / 2 - 85, 290, 170, 156, cola(0, 0, 0, 120));
        draw_minimap(&g_preview[G.sel], SCREEN_W / 2, 368, 145, NULL);
        snprintf(b, sizeof(b), "%d M - %d VUELTAS", (int)g_preview[G.sel].length, RACE_LAPS);
        text_draw_center(SCREEN_W / 2, 454, b, 2, WHITE);
        if (n0 & BTN_UP) { G.sel = (G.sel + TRACK_COUNT - 1) % TRACK_COUNT; plat_sfx(SFX_SELECT); }
        if (n0 & BTN_DOWN) { G.sel = (G.sel + 1) % TRACK_COUNT; plat_sfx(SFX_SELECT); }
        if (pr0 & BTN_A) { G.track = G.sel; G.st = GS_CAR; G.car_player = 0; G.sel = G.cars[0]; plat_sfx(SFX_SELECT); }
        if (pr0 & BTN_B) { G.st = GS_POLICE; G.sel = G.police; }
        break;
    }

    case GS_CAR: {
        int cp = G.car_player;
        const CarSpec *sp = &CAR_SPECS[G.sel];
        unsigned nn = nav(&pads[cp], cp), pp = pads[cp].pressed;
        char b[32];
        if (cp == 0) nn = n0;
        menu_background(G.sel, -0.2f);
        plat_2d_begin();
        snprintf(b, sizeof(b), "AUTO - JUGADOR %d", cp + 1);
        text_draw_center(SCREEN_W / 2, 30, b, 3, YELLOW);
        text_draw_center(SCREEN_W / 2, 70, sp->name, 4, WHITE);
        text_draw_center(SCREEN_W / 2, 108, sp->desc, 2, GREY);
        text_draw(30, 220, "<", 5, WHITE);
        text_draw_right(SCREEN_W - 30, 220, ">", 5, WHITE);
        stat_bar(140, 370, "VELOCIDAD", sp->stat_speed);
        stat_bar(140, 395, "ACELERAC.", sp->stat_accel);
        stat_bar(140, 420, "MANEJO", sp->stat_handling);
        if (nn & BTN_LEFT) { G.sel = (G.sel + CAR_COUNT - 1) % CAR_COUNT; plat_sfx(SFX_SELECT); }
        if (nn & BTN_RIGHT) { G.sel = (G.sel + 1) % CAR_COUNT; plat_sfx(SFX_SELECT); }
        if (pp & BTN_A) {
            G.cars[cp] = G.sel;
            plat_sfx(SFX_SELECT);
            if (cp == 0 && G.nplayers > 1) { G.car_player = 1; G.sel = G.cars[1]; }
            else G.st = GS_LOADING;
        }
        if (pp & BTN_B) {
            if (cp == 1) { G.car_player = 0; G.sel = G.cars[0]; }
            else { G.st = GS_TRACK; G.sel = G.track; }
        }
        break;
    }

    case GS_LOADING: {
        Viewport vp = {0, 0, SCREEN_W, SCREEN_H};
        plat_sky(&vp, col(10, 10, 20), col(30, 30, 50));
        plat_2d_begin();
        text_draw_center(SCREEN_W / 2, 220, "CARGANDO...", 4, WHITE);
        plat_2d_end();
        plat_frame_end();
        load_race();
        G.st = GS_RACE;
        engine_audio(pads);
        return;
    }

    case GS_RACE:
        race_update(&g_race, pads, dt); process_damage();
        render_race();
        if (g_race.state == RS_FINISHED && g_race.finish_wait > 3.5f) { G.st = GS_RESULTS; G.res_sel = 0; }
        if ((pads[0].pressed | pads[1].pressed) & BTN_START) { G.st = GS_PAUSE; G.pause_sel = 0; plat_sfx(SFX_SELECT); }
        break;

    case GS_PAUSE: {
        const char *it[] = {"CONTINUAR", "REINICIAR", "SALIR AL MENU"};
        PadIn none[2];
        memset(none, 0, sizeof(none));
        race_update(&g_race, none, 0);  /* congela, pero mantiene camaras */
        render_race();
        plat_rect(0, 0, SCREEN_W, SCREEN_H, cola(0, 0, 0, 120));
        text_draw_center(SCREEN_W / 2, 140, "PAUSA", 5, YELLOW);
        draw_list(it, 3, G.pause_sel, 220, 40, 3);
        if (n0 & BTN_UP) G.pause_sel = (G.pause_sel + 2) % 3;
        if (n0 & BTN_DOWN) G.pause_sel = (G.pause_sel + 1) % 3;
        if (pr0 & BTN_START) G.st = GS_RACE;
        if (pr0 & BTN_A) {
            if (G.pause_sel == 0) G.st = GS_RACE;
            else if (G.pause_sel == 1) { restart_race(); G.st = GS_RACE; }
            else { unload_race(); G.st = GS_TITLE; }
        }
        break;
    }

    case GS_RESULTS: {
        race_update(&g_race, pads, dt);  /* la IA y el piloto automatico siguen */ process_damage();
        render_race();
        render_results();
        if (n0 & (BTN_UP | BTN_DOWN)) G.res_sel ^= 1;
        if (pr0 & BTN_A) {
            if (G.res_sel == 0) { restart_race(); G.st = GS_RACE; }
            else { unload_race(); G.st = GS_TITLE; }
        }
        break;
    }
    }

    plat_2d_end();
    plat_frame_end();
    engine_audio(pads);
}

/* acceso para las pruebas en PC */
Race *pc_race(void) { return &g_race; }

#ifndef GEKKO
/* solo pruebas en PC: muestra en el menu un auto abollado de frente y sin una rueda */
void game_debug_damage_preview(int car) {
    Mesh m;
    int w;
    car_build_mesh(&m, CAR_SPECS[car].shape, CAR_SPECS[car].paint, CAR_SPECS[car].accent);
    for (w = 0; w < 4; w++) { g_ent_wf[0][w] = g_wheel_first[w]; g_ent_wc[0][w] = g_wheel_count[w]; }
    mesh_clone(&g_ent_def[0], &m);
    deform_mesh(0, vnorm(v3(-0.5f, 0, -1.0f)), 1.0f, 1);
    deform_mesh(0, v3(1, 0, 0), 0.4f, 1);
    plat_mesh_release(g_car_preview[car]);
    g_car_preview[car] = plat_mesh_upload(&g_ent_def[0]);
    mesh_free(&g_ent_def[0]);
    mesh_free(&m);
}
#endif
