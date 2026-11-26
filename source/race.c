/* race.c - logica de carrera: IA, trafico, colisiones, choques, vueltas, camara */
#include <stdio.h>
#include "race.h"
#include "cars.h"

static void update_places(Race *r);
static unsigned rng_cheap(const Entity *e) { return (unsigned)((e->s * 7.0f) + (e->pos.x * 3.0f)) ^ 0x5bd1u; }

static float wrap_rel(const Track *tr, float ds) {
    float L = tr->length;
    while (ds > L * 0.5f) ds -= L;
    while (ds < -L * 0.5f) ds += L;
    return ds;
}

static int idx_of(const Track *tr, float s) {
    int i = (int)(track_wrap_s(tr, s) / tr->ds);
    return i % tr->n;
}

static void set_msg(Race *r, int p, const char *txt, Col c, float t) {
    PlayerView *v = &r->view[p];
    strncpy(v->msg, txt, sizeof(v->msg) - 1);
    v->msg[sizeof(v->msg) - 1] = 0;
    v->msg_col = c;
    v->msg_t = t;
}

/* ---------------------------------------------------------------- */

static void compute_limits(Race *r) {
    const Track *tr = r->track;
    int i, pass, n = tr->n;
    float A = (r->mode == MODE_SIM) ? 9.8f : 23.0f;
    float D = (r->mode == MODE_SIM) ? 7.5f : 12.0f;
    float vcap = 0;
    for (i = 0; i < CAR_COUNT; i++) {
        float t = veh_top_speed(&CAR_SPECS[i], r->mode);
        if (t > vcap) vcap = t;
    }
    for (i = 0; i < n; i++) {
        float k = fabsf(tr->curv[i]);
        r->vlim[i] = fminf(vcap * 1.1f, sqrtf(A / fmaxf(k, 1e-4f)));
    }
    for (pass = 0; pass < 3; pass++) {
        for (i = n - 1; i >= 0; i--) {
            float nx = r->vlim[(i + 1) % n];
            float lim = sqrtf(nx * nx + 2.0f * D * tr->ds);
            if (r->vlim[i] > lim) r->vlim[i] = lim;
        }
    }
    /* linea de carrera: por dentro en las curvas */
    for (i = 0; i < n; i++) {
        int k;
        float avg = 0;
        for (k = 0; k < 14; k++) avg += tr->curv[(i + k) % n];
        avg /= 14.0f;
        r->line[i] = clampf(avg * (tr->hw * 0.55f / 0.010f), -tr->hw * 0.55f, tr->hw * 0.55f);
    }
}

static int es_auto_de_jugador(const int cars[MAX_PLAYERS], int nplayers, int c) {
    int k;
    for (k = 0; k < nplayers; k++) if (cars[k] == c) return 1;
    return 0;
}

void race_setup(Race *r, Track *tr, PhysMode mode, int nplayers, const int cars[MAX_PLAYERS], int police) {
    int i, slot = 0, ai_car = 0;
    int grid_entity[RACERS];
    memset(r, 0, sizeof(Race));
    r->track = tr;
    r->mode = mode;
    r->nplayers = nplayers;
    r->state = RS_COUNTDOWN;
    r->countdown = 3.6f;
    r->last_beep = 4;
    compute_limits(r);

    /* IA primero (delante en la parrilla), jugadores al fondo */
    for (i = 0; i < RACERS; i++) grid_entity[i] = i;
    for (i = 0; i < RACERS; i++) {
        Entity *e = &r->ent[i];
        int is_p = (i >= RACERS - nplayers);
        float s = tr->length - 14.0f - slot * 9.0f;
        float d = (slot & 1) ? 3.2f : -3.2f;
        e->active = 1;
        e->is_player = is_p;
        e->player = is_p ? i - (RACERS - nplayers) : -1;
        if (is_p) {
            e->car = cars[e->player];
        } else {
            /* elegir un auto de IA distinto a los de los jugadores si es posible */
            /* rivales de la misma clase que el auto del jugador 1 (si alcanzan), distintos a los de los jugadores */
            int tries = 0;
            char clase = CAR_CLASS[cars[0]];
            do {
                e->car = ai_car % (CAR_COUNT - 1);   /* la patrulla no corre como rival */
                ai_car++;
                tries++;
            } while (tries < 3 * CAR_COUNT &&
                     (es_auto_de_jugador(cars, nplayers, e->car) || (tries < 2 * CAR_COUNT && CAR_CLASS[e->car] != clase)));
            e->skill = 0.91f + 0.025f * i;
        }
        e->paint = CAR_SPECS[e->car].paint;
        e->accent = CAR_SPECS[e->car].accent;
        if (!is_p && es_auto_de_jugador(cars, nplayers, e->car))
            e->paint = CAR_ALT_PAINTS[i % 6];
        if (is_p && e->player > 0) {   /* si otro jugador ya tiene ese auto, cambia el color */
            int k;
            for (k = 0; k < e->player; k++) if (cars[k] == cars[e->player]) e->paint = CAR_ALT_PAINTS[e->player % 6];
        }

        if (is_p) {
            veh_init(&e->veh, &CAR_SPECS[e->car], tr, s, d);
            e->pos = e->veh.pos; e->yaw = e->veh.yaw; e->pitch = e->veh.pitch;
            e->s = e->veh.s; e->d = e->veh.d;
        } else {
            e->s = track_wrap_s(tr, s); e->d = d; e->v = 0; e->target_d = d;
            e->pos = track_point(tr, e->s, e->d);
            e->yaw = track_heading(track_tangent(tr, e->s));
        }
        e->prev_s = e->s;
        e->lap = 0;
        e->progress = e->s;
        e->cap_v = 1e9f;
        slot++;
    }
    (void)grid_entity;

    /* trafico en CHOQUES y CALLE */
    if (mode == MODE_CHOQUES || mode == MODE_CALLE) {
        static const Col tcols[] = {{200, 200, 205, 255}, {70, 90, 140, 255}, {150, 40, 40, 255},
                                    {230, 225, 210, 255}, {60, 110, 70, 255}, {120, 120, 128, 255}};
        Rng rg = {31337};
        r->ntraffic = 10;
        for (i = 0; i < r->ntraffic; i++) {
            Entity *e = &r->ent[RACERS + i];
            e->active = 1;
            e->is_traffic = 1;
            e->car = i & 1;  /* 0 sedan, 1 furgoneta */
            e->paint = tcols[i % 6];
            e->lane = (i & 2) ? 3.6f : -3.6f;
            e->s = track_wrap_s(tr, 140.0f + i * tr->length / r->ntraffic + rng_range(&rg, -30, 30));
            e->d = e->lane;
            e->v = rng_range(&rg, 15.0f, 21.0f);
            e->skill = e->v;
            e->pos = track_point(tr, e->s, e->d);
            e->yaw = track_heading(track_tangent(tr, e->s));
        }
    }
    /* patrullas: dos al inicio, mas segun el nivel de busqueda */
    r->police = police;
    r->heat = 1;
    r->heat_t = 0;
    r->roadblock_cd = 70.0f;
    r->timescale = 1.0f;
    if (police) {
        for (i = 0; i < POLICE_MAX; i++) {
            Entity *e = &r->ent[POLICE_BASE + i];
            e->is_police = 1;
            e->car = CAR_POLICE;
            e->paint = col(24, 24, 30);
            e->accent = col(245, 245, 250);
            e->active = (i < 2);
            e->s = track_wrap_s(tr, tr->length - 80.0f - i * 25.0f);
            e->d = (i & 1) ? 3.0f : -3.0f;
            e->target_d = e->d;
            e->v = 0;
            e->pos = track_point(tr, e->s, e->d);
            e->yaw = track_heading(track_tangent(tr, e->s));
        }
        /* camionetas: 2 por jugador, esperan a la busqueda 5 */
        for (i = 0; i < SUV_MAX; i++) {
            Entity *e = &r->ent[SUV_BASE + i];
            e->is_police = 1;
            e->is_suv = 1;
            e->suv_player = i / 2;
            e->car = CAR_POLICE;
            e->paint = col(22, 22, 28);
            e->accent = col(242, 242, 246);
            e->active = 0;
            e->rb_t = (i & 1) ? 6.0f : 0.0f;   /* la segunda llega unos segundos despues */
            e->s = 0; e->d = 0; e->v = 0;
        }
    }
    for (i = 0; i < 2; i++) {
        int k;
        r->view[i].slow = 1.0f;
        r->view[i].fov = 62.0f;
        r->view[i].best_lap = 0;
        for (k = 0; k < TRAFFIC_MAX; k++) r->view[i].prev_rel[k] = 1000.0f;
    }
    update_places(r);
    /* posiciones iniciales de camara */
    for (i = 0; i < RACERS; i++) {
        Entity *e = &r->ent[i];
        if (e->is_player) {
            PlayerView *v = &r->view[e->player];
            v->yaw = e->yaw;
            v->eye = vadd(e->pos, v3(-sinf(e->yaw) * 7, 2.6f, cosf(e->yaw) * 7));
            v->target = e->pos;
        }
    }
}

CarInput race_pad_to_input(const PadIn *p) {
    CarInput in;
    float sx = p->sx;
    if (p->held & BTN_LEFT) sx = -1;
    if (p->held & BTN_RIGHT) sx = 1;
    /* zona muerta y curva de respuesta */
    if (fabsf(sx) < 0.12f) sx = 0;
    else sx = signf(sx) * (fabsf(sx) - 0.12f) / 0.88f;
    in.steer = signf(sx) * (0.55f * fabsf(sx) + 0.45f * sx * sx);
    in.throttle = (p->held & BTN_A) ? 1.0f : 0.0f;
    if (p->trig_r > in.throttle) in.throttle = p->trig_r;
    in.brake = (p->held & BTN_B) ? 1.0f : 0.0f;
    if (p->trig_l > in.brake) in.brake = p->trig_l;
    in.boost = (p->held & BTN_X) ? 1 : 0;
    in.slowmo = (p->held & BTN_Y) ? 1 : 0;
    return in;
}

CarInput race_autopilot(const Race *r, Entity *e, float dt) {
    const Track *tr = r->track;
    const Vehicle *v = &e->veh;
    CarInput in;
    float spd = (r->mode == MODE_SIM) ? v->vf : v->speed;
    int i = v->idx;
    float st = v->s + 9.0f + fabsf(spd) * 0.45f;
    float pd = r->line[idx_of(tr, st)] * 0.7f;
    V3 P;
    int k;
    /* esquivar restos y autos chocados */
    if (r->mode == MODE_CHOQUES) {
        for (k = 0; k < WRECK_MAX + ENT_MAX; k++) {
            float os, od, rel;
            if (k < WRECK_MAX) {
                if (!r->wreck[k].active) continue;
                os = r->wreck[k].s; od = r->wreck[k].d;
            } else {
                const Entity *c = &r->ent[k - WRECK_MAX];
                if (c == e || !c->active || !c->crashed) continue;
                os = c->s; od = c->d;
            }
            rel = wrap_rel(tr, os - v->s);
            if (rel > 0 && rel < 60.0f && fabsf(od - v->d) < 3.4f) {
                float lim = tr->wall - 1.5f;
                pd = (od - 4.0f > -lim && (od + 4.0f > lim || od > 0)) ? od - 4.0f : od + 4.0f;
                st = v->s + fminf(rel, 9.0f + fabsf(spd) * 0.45f);
            }
        }
    }
    P = track_point(tr, st, pd);
    float want = atan2f(P.x - v->pos.x, -(P.z - v->pos.z));
    float ang = wrap_angle(want - v->yaw);
    float vt = r->vlim[(i + 3) % tr->n] * (r->mode == MODE_SIM ? 0.93f : 0.80f);
    in.steer = clampf(ang * 2.6f, -1, 1);
    in.throttle = spd < vt ? 1.0f : 0.0f;
    in.brake = spd > vt + 2.5f ? clampf((spd - vt) * 0.25f, 0, 1) : 0.0f;
    in.boost = 0;
    in.slowmo = 0;
    /* atascado contra el muro: marcha atras unos instantes */
    if (e->reverse_t > 0) {
        e->reverse_t -= dt;
        in.steer = -in.steer;
        in.throttle = 0;
        in.brake = 1;
    } else if (fabsf(spd) < 2.0f && in.throttle > 0) {
        e->stuck_t += dt;
        if (e->stuck_t > 1.2f) { e->reverse_t = 1.3f; e->stuck_t = 0; }
    } else {
        e->stuck_t = 0;
    }
    return in;
}

/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/* Danos, ruedas sueltas y restos (modo CHOQUES)                     */
/* ---------------------------------------------------------------- */

static V3 ent_vel(const Race *r, const Entity *e);
static unsigned g_dmg_seed = 12345u;
static float frand(void) {
    g_dmg_seed = g_dmg_seed * 1664525u + 1013904223u;
    return (float)((g_dmg_seed >> 8) & 0xFFFF) / 65535.0f;
}

/* direccion mundial -> coordenadas locales del auto (x derecha, z hacia atras) */
static V3 to_local(const Entity *e, V3 w) {
    V3 rt = v3(cosf(e->yaw), 0, sinf(e->yaw));
    V3 bk = v3(-sinf(e->yaw), 0, cosf(e->yaw));
    return v3(vdot(w, rt), 0, vdot(w, bk));
}

/* abolladura: wdir apunta desde el centro del auto hacia el golpe */
static void dent(Race *r, Entity *e, V3 wdir, float amt) {
    V3 l;
    float n;
    if (r->mode != MODE_CHOQUES) return;
    l = to_local(e, wdir);
    n = vlen(l);
    if (n < 1e-3f) l = v3(0, 0, -1); else l = vscale(l, 1.0f / n);
    if (e->dmg_dirty && e->dmg_amt >= amt) return;
    e->dmg_dir = l;
    e->dmg_amt = clampf(amt, 0, 1);
    e->dmg_dirty = 1;
}

static void spawn_wheel(Race *r, Entity *e, int bit, V3 vel) {
    int k;
    V3 rt = v3(cosf(e->yaw), 0, sinf(e->yaw));
    V3 bk = v3(-sinf(e->yaw), 0, cosf(e->yaw));
    float sx = (bit & 1) ? 1.0f : -1.0f, sz = (bit & 2) ? 1.0f : -1.0f;
    for (k = 0; k < DEBRIS_MAX; k++) {
        Debris *d = &r->debris[k];
        if (d->active) continue;
        d->active = 1;
        d->t = 0;
        d->s = e->s;
        d->pos = vadd(e->pos, vadd(vscale(rt, 0.9f * sx), vscale(bk, 1.35f * sz)));
        d->pos.y += 0.4f;
        d->vel = vadd(vscale(vel, 0.75f), vscale(rt, sx * (4.0f + 5.0f * frand())));
        d->vel.y = 6.0f + 6.0f * frand();
        d->rot = v3(0, e->yaw, 0);
        d->spin = v3(8.0f + 10.0f * frand(), 2.0f * frand() - 1.0f, 6.0f * frand() - 3.0f);
        return;
    }
}

/* golpe fuerte: abolla el lado del impacto y puede soltar una o dos ruedas */
static void crash_damage(Race *r, Entity *e, V3 vel) {
    V3 dv = vsub(vel, ent_vel(r, e)), dir, l;
    float sp = vlen(vel);
    int bit;
    if (r->mode != MODE_CHOQUES) return;
    dv.y = 0;
    if (vlen(dv) > 3.0f) dir = vscale(vnorm(dv), -1.0f);       /* el golpe viene del lado contrario al empujon */
    else dir = v3(sinf(e->yaw), 0, -cosf(e->yaw));              /* de frente */
    dent(r, e, dir, 0.45f + sp / 55.0f);
    l = to_local(e, dir);
    bit = (fabsf(l.x) < 0.3f ? (frand() < 0.5f) : (l.x > 0)) | ((l.z > 0) ? 2 : 0);
    if (!(e->wheels_lost & (1 << bit))) { e->wheels_lost |= 1 << bit; spawn_wheel(r, e, bit, vel); }
    if (sp > 32.0f) {
        int b2 = bit ^ 1;
        if (!(e->wheels_lost & (1 << b2))) { e->wheels_lost |= 1 << b2; spawn_wheel(r, e, b2, vel); }
    }
}

/* deja los restos en la pista y devuelve el hueco por el que reaparecer */
static void leave_wreck(Race *r, Entity *e, TrackPos *tp) {
    Wreck *w = &r->wreck[r->wreck_next];
    float c = cosf(e->crash_rot.x) * cosf(e->crash_rot.z);
    r->wreck_next = (r->wreck_next + 1) % WRECK_MAX;
    w->active = 1;
    w->src = (int)(e - r->ent);
    w->owner = e->is_player ? e->player : -1;
    w->mesh_pending = 1;
    w->flipped = c < 0;
    w->s = tp->s;
    w->d = tp->d;
    w->t = 0;
    w->hit_cd = 0;
    w->pos = v3(e->pos.x, tp->h, e->pos.z);
    w->yaw = e->yaw + e->crash_rot.y;
    e->dmg_reset = 1;
    e->dmg_dirty = 0;
    e->wheels_lost = 0;
}

static void crash_start(Race *r, Entity *e, V3 vel) {
    float sp;
    if (e->crashed) return;
    crash_damage(r, e, vel);
    e->crashed = 1;
    e->crash_t = 0;
    vel.y = 0;
    sp = vlen(vel);
    e->crash_vel = vscale(vel, 0.8f);
    e->crash_vy = 4.0f + sp * 0.09f;
    e->crash_y = 0;
    e->crash_rot = v3(0, 0, 0);
    e->crash_spin = v3(2.5f + sp * 0.06f, 1.5f + sp * 0.03f, (e->d > 0 ? -1.0f : 1.0f) * (3.0f + sp * 0.08f));
    if (e->is_player) {
        plat_sfx(SFX_CRASH);
        set_msg(r, e->player, "CHOQUE!", col(255, 70, 50), 2.0f);
        r->view[e->player].shake = 1.0f;
        if (r->nplayers == 1) r->slowmo = 1.5f;
    }
}

static void crash_update(Race *r, Entity *e, float h) {
    const Track *tr = r->track;
    TrackPos tp;
    e->crash_t += h;
    e->pos.x += e->crash_vel.x * h;
    e->pos.z += e->crash_vel.z * h;
    e->crash_vel = vscale(e->crash_vel, 1.0f - fminf(1.0f, 1.1f * h));
    e->crash_vy -= 20.0f * h;
    e->crash_y += e->crash_vy * h;
    if (e->crash_y < 0) {
        e->crash_y = 0;
        e->crash_vy = -e->crash_vy * 0.35f;
        e->crash_spin = vscale(e->crash_spin, 0.6f);
        e->crash_vel = vscale(e->crash_vel, 0.75f);
    }
    e->crash_rot = vadd(e->crash_rot, vscale(e->crash_spin, h));
    tp = track_locate(tr, e->pos, idx_of(tr, e->s));
    if (fabsf(tp.d) > tr->wall - 1.0f) {
        V3 nrm = vscale(tp.right, signf(tp.d));
        float vn = vdot(e->crash_vel, nrm);
        float over = fabsf(tp.d) - (tr->wall - 1.0f);
        e->pos = vsub(e->pos, vscale(nrm, over));
        if (vn > 0) e->crash_vel = vsub(e->crash_vel, vscale(nrm, vn * 1.5f));
        tp.d = signf(tp.d) * (tr->wall - 1.0f);
    }
    e->s = tp.s;
    e->d = tp.d;
    e->pos.y = tp.h + e->crash_y;
    e->pitch = 0;

    if (e->crash_t > 2.5f) {
        float d = clampf(tp.d, -(tr->hw - 2.0f), tr->hw - 2.0f);
        if (r->mode == MODE_CHOQUES && !e->busted) {
            /* los restos se quedan; el auto vuelve reparado unos metros adelante, por el otro lado */
            leave_wreck(r, e, &tp);
            e->s = track_wrap_s(tr, e->s + 22.0f);
            d = tp.d > 0 ? -tr->hw * 0.45f : tr->hw * 0.45f;
        }
        e->crashed = 0;
        e->busted = 0;
        e->invuln = 2.0f;
        if (e->is_player) {
            veh_place(&e->veh, tr, e->s, d, 22.0f);
            e->pos = e->veh.pos; e->yaw = e->veh.yaw;
            e->s = e->veh.s; e->d = e->veh.d;
        } else if (e->is_traffic) {
            e->s = track_wrap_s(tr, e->s + 250.0f);
            e->d = e->lane;
            e->v = e->skill;
        } else {
            e->d = d; e->v = 18.0f; e->vd = 0;
        }
    }
}

/* ---------------------------------------------------------------- */

static void ai_think(Race *r) {
    const Track *tr = r->track;
    int i, k;
    float lead = -1e9f;
    for (i = 0; i < RACERS; i++)
        if (r->ent[i].is_player && r->ent[i].progress > lead) lead = r->ent[i].progress;

    for (i = 0; i < ENT_MAX; i++) {
        Entity *e = &r->ent[i];
        float base;
        if (!e->active || e->is_player || e->crashed || e->is_police) continue;
        e->cap_v = 1e9f;
        if (e->is_traffic) {
            base = e->lane;
        } else {
            base = r->line[(idx_of(tr, e->s) + 10) % tr->n];
            /* goma elastica respecto al jugador lider */
            if (lead > -1e8f) {
                float gap = e->progress - lead;
                if (gap < -120.0f) e->skill = fminf(e->skill + 0.0004f, 1.06f);
                else if (gap > 160.0f) e->skill = fmaxf(e->skill - 0.0004f, 0.86f);
            }
        }
        e->target_d = base;
        for (k = 0; k < ENT_MAX; k++) {
            Entity *o = &r->ent[k];
            float rel, dd;
            if (k == i || !o->active || o->crashed) continue;
            if (fabsf(o->pos.y - e->pos.y) > 3.0f) continue;
            rel = wrap_rel(tr, o->s - e->s);
            dd = o->d - e->d;
            if (rel > 0 && rel < 26.0f && fabsf(dd) < 2.9f) {
                float ov = o->is_player ? fabsf(o->veh.speed + o->veh.vf) : o->v;
                if (e->is_traffic) {
                    e->cap_v = fminf(e->cap_v, ov);
                } else {
                    float left = o->d - 3.5f, right = o->d + 3.5f, lim = tr->wall - 1.3f;
                    int lok = left > -lim, rok = right < lim;
                    if (lok && (!rok || fabsf(left - base) < fabsf(right - base))) e->target_d = left;
                    else if (rok) e->target_d = right;
                    else e->cap_v = fminf(e->cap_v, ov - 0.5f);
                }
            }
        }
        /* los rivales esquivan restos y choques (el trafico no: ahi empiezan las carambolas) */
        if (r->mode == MODE_CHOQUES && !e->is_traffic) {
            for (k = 0; k < WRECK_MAX + ENT_MAX; k++) {
                float os, od, oy, rel;
                if (k < WRECK_MAX) {
                    const Wreck *w = &r->wreck[k];
                    if (!w->active) continue;
                    os = w->s; od = w->d; oy = w->pos.y;
                } else {
                    const Entity *c = &r->ent[k - WRECK_MAX];
                    if (c == e || !c->active || !c->crashed) continue;
                    os = c->s; od = c->d; oy = c->pos.y;
                }
                if (fabsf(oy - e->pos.y) > 3.0f) continue;
                rel = wrap_rel(tr, os - e->s);
                if (rel > 0 && rel < 32.0f && fabsf(od - e->d) < 3.0f) {
                    float left = od - 3.8f, right = od + 3.8f, lim = tr->wall - 1.3f;
                    int lok = left > -lim, rok = right < lim;
                    if (lok && (!rok || fabsf(left - e->target_d) < fabsf(right - e->target_d))) e->target_d = left;
                    else if (rok) e->target_d = right;
                    else e->cap_v = fminf(e->cap_v, 5.0f);
                }
            }
        }
    }
}

static void kin_step(Race *r, Entity *e, float h) {
    const Track *tr = r->track;
    int i = idx_of(tr, e->s);
    V3 tan;
    if (e->is_police) {
        if (e->roadblock) e->v = approach(e->v, 0, 20.0f * h);
        else if (e->v < e->skill) e->v += 10.0f * h;
        else e->v = fmaxf(e->skill, e->v - 14.0f * h);
        if (e->v < 0) e->v = 0;
    } else if (e->is_traffic) {
        float vt = fminf(e->skill, e->cap_v);
        e->v = approach(e->v, vt, 6.0f * h);
    } else {
        const CarSpec *sp = &CAR_SPECS[e->car];
        float top = veh_top_speed(sp, r->mode) * e->skill;
        float vt = fminf(r->vlim[(i + 2) % tr->n] * e->skill, top);
        float acc = (r->mode == MODE_SIM) ? 5.0f : sp->accel * 0.8f;
        vt = fminf(vt, e->cap_v);
        if (e->v < vt) e->v += acc * (1.0f - 0.6f * e->v / fmaxf(top, 1)) * h;
        else e->v = fmaxf(vt, e->v - 12.0f * h);
        if (e->v < 0) e->v = 0;
    }
    {
        float want = clampf((e->target_d - e->d) * 1.6f, -5.0f, 5.0f);
        e->vd = approach(e->vd, want, 18.0f * h);
    }
    e->d += e->vd * h;
    e->d = clampf(e->d, -(tr->wall - 1.1f), tr->wall - 1.1f);
    e->s = track_wrap_s(tr, e->s + e->v * h);
    e->pos = track_point(tr, e->s, e->d);
    tan = track_tangent(tr, e->s);
    e->yaw = track_heading(tan) + atan2f(e->vd, fmaxf(e->v, 2.0f));
    if (r->mode == MODE_DERRAPE && !e->is_traffic && !e->is_police) {
        float dr = clampf(tr->curv[i] * e->v * 0.5f, -0.55f, 0.55f);
        e->yaw += dr;
    }
    if (e->roadblock) e->yaw = track_heading(tan) + (e->d > 0 ? 1.1f : -1.1f);   /* atravesada en el reten */
    e->pitch = asinf(clampf(tan.y, -1, 1));
}


/* ---------------------------------------------------------------- */
/* Persecucion policial                                              */
/* ---------------------------------------------------------------- */

static Entity *police_target(Race *r, Entity *e) {
    Entity *best = NULL;
    float bd = 1e9f;
    int i;
    for (i = 0; i < RACERS; i++) {
        Entity *p = &r->ent[i];
        float d;
        if (!p->is_player || p->finished) continue;
        d = fabsf(wrap_rel(r->track, p->s - e->s));
        if (d < bd) { bd = d; best = p; }
    }
    return best;
}

static float ent_speed(const Race *r, const Entity *e) {
    if (e->is_player) return fabsf(r->mode == MODE_SIM ? e->veh.vf : e->veh.speed);
    return e->v;
}

static void police_spawn_behind(Race *r, Entity *e, Entity *t, float back) {
    e->active = 1;
    e->crashed = 0;
    e->roadblock = 0;
    e->s = track_wrap_s(r->track, t->s - back);
    e->d = (rng_cheap(e) & 1) ? 3.0f : -3.0f;
    e->v = ent_speed(r, t);
    e->vd = 0;
    e->invuln = 1.5f;
    e->lane = 0;
    e->pos = track_point(r->track, e->s, e->d);
}

static void police_update(Race *r, float dt) {
    const Track *tr = r->track;
    float vcap = 0;
    int i, k, want;
    if (!r->police || r->state != RS_RUNNING) return;
    for (i = 0; i < CAR_COUNT; i++) vcap = fmaxf(vcap, veh_top_speed(&CAR_SPECS[i], r->mode));

    /* nivel de busqueda: sube con el tiempo */
    r->heat_t += dt;
    r->heat = 1 + (int)(r->heat_t / 45.0f);
    if (r->heat > 5) r->heat = 5;
    vcap *= 0.98f + 0.02f * r->heat;
    want = r->heat + 1;
    if (want > POLICE_MAX) want = POLICE_MAX;

    for (i = 0; i < POLICE_MAX; i++) {
        Entity *e = &r->ent[POLICE_BASE + i];
        Entity *t = police_target(r, e);
        float rel, tv;
        if (!t) { e->skill = 0; continue; }
        if (!e->active) {
            e->rb_t -= dt;
            if (i < want && e->rb_t <= 0) police_spawn_behind(r, e, t, 300.0f + i * 40.0f);
            continue;
        }
        if (e->crashed) continue;
        rel = wrap_rel(tr, t->s - e->s);   /* > 0: el objetivo va delante */
        tv = ent_speed(r, t);
        if (e->roadblock) {
            e->rb_t -= dt;
            if (e->rb_t <= 0 || rel > 150.0f) e->roadblock = 0;
            e->skill = 0;
            continue;
        }
        /* si se quedo muy lejos, reaparece detras del objetivo */
        /* si la dejas muy atras te pierde de vista; vuelve a aparecer despues (antes con mas busqueda) */
        if (rel > 520.0f || rel < -350.0f) { e->active = 0; e->rb_t = 26.0f - 4.0f * r->heat; continue; }
        if (rel > 35.0f) {               /* alcanzar */
            e->skill = fminf(tv + 3.0f + 2.0f * r->heat + rel * 0.02f, vcap);
            e->target_d = r->line[(idx_of(tr, e->s) + 10) % tr->n];
        } else if (rel > -8.0f) {        /* pegarse y embestir; tras el golpe se abre un momento */
            float gap = vlen(v3(e->pos.x - t->pos.x, 0, e->pos.z - t->pos.z));
            if (e->lane > 0) e->lane -= dt;
            else if (gap < 5.5f) e->lane = 1.6f + 0.5f * i - 0.2f * r->heat;
            e->skill = e->lane > 0 ? fmaxf(tv - 4.0f, 0) : fminf(tv + 4.0f, vcap * 1.05f);
            e->target_d = t->d + ((i & 1) ? 1.2f : -1.2f) * (rel < 6.0f ? 1.0f : 0.0f);
        } else {                         /* va delante: frenar para cerrarle el paso */
            e->skill = fmaxf(tv - 6.0f, 0);
            e->target_d = t->d;
        }
        e->target_d = clampf(e->target_d, -(tr->wall - 1.3f), tr->wall - 1.3f);
        /* esquivar trafico y otros autos (pero no al objetivo) */
        for (k = 0; k < ENT_MAX; k++) {
            Entity *o = &r->ent[k];
            float orel;
            if (o == e || o == t || !o->active || o->crashed) continue;
            orel = wrap_rel(tr, o->s - e->s);
            if (orel > 0 && orel < 24.0f && fabsf(o->d - e->d) < 2.9f) {
                e->target_d = (o->d > 0) ? o->d - 3.6f : o->d + 3.6f;
                if (fabsf(e->target_d) > tr->wall - 1.3f) e->skill = fminf(e->skill, ent_speed(r, o));
            }
        }
    }


    /* camionetas (busqueda 5): cada una persigue a su jugador y lo embiste sin descanso */
    for (i = 0; i < SUV_MAX; i++) {
        Entity *e = &r->ent[SUV_BASE + i];
        Entity *t = NULL;
        float rel, tv, gap;
        for (k = 0; k < RACERS; k++) if (r->ent[k].is_player && r->ent[k].player == e->suv_player) t = &r->ent[k];
        if (!t || t->finished || e->suv_player >= r->nplayers) { e->active = 0; e->skill = 0; continue; }
        if (r->heat < 5) {   /* si baja la busqueda se retiran */
            e->active = 0;
            e->rb_t = (i & 1) ? 6.0f : 0.0f;
            continue;
        }
        if (!e->active) {
            if (!r->suv_aviso) {
                for (k = 0; k < r->nplayers; k++) set_msg(r, k, "BUSQUEDA 5: CAMIONETAS!", col(255, 60, 60), 3.0f);
                r->suv_aviso = 1;
            }
            e->rb_t -= dt;
            if (e->rb_t <= 0 && !t->crashed) police_spawn_behind(r, e, t, 240.0f + (i & 1) * 60.0f);
            continue;
        }
        if (e->crashed) continue;
        rel = wrap_rel(tr, t->s - e->s);
        tv = ent_speed(r, t);
        if (rel > 600.0f || rel < -300.0f) { e->active = 0; e->rb_t = 8.0f; continue; }
        gap = vlen(v3(e->pos.x - t->pos.x, 0, e->pos.z - t->pos.z));
        if (e->lane > 0) e->lane -= dt;
        if (rel > 30.0f) {                      /* alcanzar: mas rapidas que las patrullas */
            e->skill = fminf(tv + 9.0f + rel * 0.03f, vcap * 1.08f);
            e->target_d = t->d;
        } else if (rel > -10.0f) {              /* embestir: por detras o de costado, directo al jugador */
            if (gap < 4.5f && e->lane <= 0) e->lane = 0.7f;   /* tras el golpe se separa apenas un momento */
            e->skill = e->lane > 0 ? fmaxf(tv - 2.0f, 0) : fminf(tv + 7.0f, vcap * 1.10f);
            e->target_d = t->d + ((fabsf(rel) < 4.0f) ? 0.0f : ((i & 1) ? 0.8f : -0.8f));
        } else {                                /* se le adelanto: frena para cerrarle el paso */
            e->skill = fmaxf(tv - 9.0f, 0);
            e->target_d = t->d;
        }
        e->target_d = clampf(e->target_d, -(tr->wall - 1.4f), tr->wall - 1.4f);
        for (k = 0; k < ENT_MAX; k++) {   /* esquiva el trafico, no al jugador ni a la otra camioneta */
            Entity *o = &r->ent[k];
            float orel;
            if (o == e || o == t || !o->active || o->crashed || o->is_suv) continue;
            orel = wrap_rel(tr, o->s - e->s);
            if (orel > 0 && orel < 22.0f && fabsf(o->d - e->d) < 3.0f && rel > 30.0f)
                e->target_d = clampf((o->d > 0) ? o->d - 3.8f : o->d + 3.8f, -(tr->wall - 1.4f), tr->wall - 1.4f);
        }
    }

    /* retenes desde el nivel 3 */
    if (r->heat >= 3) {
        r->roadblock_cd -= dt;
        if (r->roadblock_cd <= 0) {
            Entity *t = NULL;
            int n = 0;
            for (i = 0; i < RACERS; i++) if (r->ent[i].is_player && !r->ent[i].finished) { t = &r->ent[i]; break; }
            if (t) {
                /* la patrulla mas cercana sigue persiguiendo; las demas montan el reten */
                int closest = -1;
                float cd = 1e9f;
                for (i = 0; i < POLICE_MAX; i++) {
                    Entity *e = &r->ent[POLICE_BASE + i];
                    float d;
                    if (!e->active || e->crashed || e->roadblock) continue;
                    d = fabsf(wrap_rel(tr, t->s - e->s));
                    if (d < cd) { cd = d; closest = i; }
                }
                for (i = 0; i < POLICE_MAX && n < 2; i++) {
                    Entity *e = &r->ent[POLICE_BASE + i];
                    if (!e->active || e->crashed || e->roadblock || i == closest) continue;
                    e->roadblock = 1;
                    e->rb_t = 30.0f;
                    e->s = track_wrap_s(tr, t->s + 480.0f + n * 5.0f);
                    /* dejan un hueco por un lado */
                    e->d = n == 0 ? -tr->hw * 0.55f : tr->hw * 0.05f;
                    e->target_d = e->d;
                    e->v = 0; e->vd = 0;
                    e->pos = track_point(tr, e->s, e->d);
                    n++;
                }
                if (n > 0) for (i = 0; i < r->nplayers; i++) set_msg(r, i, "RETEN ADELANTE!", col(255, 80, 80), 2.0f);
            }
            r->roadblock_cd = 55.0f;
        }
    }

    /* detencion: patrulla encima y el jugador casi parado */
    r->siren = 0;
    for (i = 0; i < RACERS; i++) {
        Entity *p = &r->ent[i];
        PlayerView *v;
        float nearest = 1e9f;
        if (!p->is_player || p->crashed || p->finished) continue;
        v = &r->view[p->player];
        for (k = POLICE_BASE; k < ENT_MAX; k++) {
            Entity *e = &r->ent[k];
            float d;
            if (!e->active || e->crashed) continue;
            if (fabsf(e->pos.y - p->pos.y) > 3.0f) continue;
            d = vlen(v3(e->pos.x - p->pos.x, 0, e->pos.z - p->pos.z));
            if (d < nearest) nearest = d;
        }
        if (p->player == 0) r->siren = clampf(1.0f - nearest / 260.0f, 0, 1);
        if (nearest < 12.0f && ent_speed(r, p) < 6.0f) v->bust += dt;
        else v->bust = fmaxf(0, v->bust - dt * 0.7f);
        if (v->bust > 3.0f) {
            /* detenido: el auto queda parado unos segundos y la busqueda baja */
            v->bust = 0;
            p->crashed = 1;
            p->crash_t = -1.5f;
            p->busted = 1;
            p->crash_vel = v3(0, 0, 0);
            p->crash_spin = v3(0, 0, 0);
            p->crash_rot = v3(0, 0, 0);
            p->crash_vy = 0;
            p->crash_y = 0;
            set_msg(r, p->player, "DETENIDO!", col(255, 60, 60), 3.5f);
            r->heat_t = 0;
            for (k = 0; k < POLICE_MAX; k++) {
                Entity *e = &r->ent[POLICE_BASE + k];
                if (e->active) police_spawn_behind(r, e, p, 300.0f + k * 40.0f);
            }
            for (k = 0; k < SUV_MAX; k++) {   /* las camionetas se retiran */
                Entity *e = &r->ent[SUV_BASE + k];
                e->active = 0; e->rb_t = 10.0f + (k & 1) * 6.0f;
            }
            r->suv_aviso = 0;
        }
    }
    /* evadido: ninguna patrulla cerca durante un rato */
    if (r->siren <= 0.0f) {
        r->evade_t += dt;
        if (r->evade_t > 15.0f && r->heat_t > 45.0f) {
            r->heat_t -= 45.0f;
            r->evade_t = 0;
            for (i = 0; i < r->nplayers; i++) set_msg(r, i, "EVADIDO! BUSQUEDA -1", col(120, 255, 120), 2.5f);
        }
    } else r->evade_t = 0;
}

/* ---------------------------------------------------------------- */

static V3 ent_vel(const Race *r, const Entity *e) {
    if (e->is_player) return veh_velocity(&e->veh, r->mode);
    {
        V3 t = track_tangent(r->track, e->s);
        V3 rt = vnorm(v3(-t.z, 0, t.x));
        return vadd(vscale(t, e->v), vscale(rt, e->vd));
    }
}

static void ent_add(Race *r, Entity *e, V3 dv, V3 push) {
    if (e->is_player) {
        V3 vel = vadd(veh_velocity(&e->veh, r->mode), dv);
        veh_set_velocity(&e->veh, r->mode, vel);
        e->veh.pos.x += push.x;
        e->veh.pos.z += push.z;
        e->pos = e->veh.pos;
    } else {
        V3 t = track_tangent(r->track, e->s);
        V3 rt = vnorm(v3(-t.z, 0, t.x));
        e->v = fmaxf(0.0f, e->v + vdot(dv, t));
        e->vd += vdot(dv, rt);
        e->d += vdot(push, rt);
        e->s = track_wrap_s(r->track, e->s + vdot(push, t));
    }
}

static float ent_mass(const Race *r, const Entity *e) {
    if (e->is_traffic) return 1.5f;
    if (e->is_suv) return 2.8f;   /* camioneta: mucho mas pesada, empuja fuerte */
    if (e->is_police) return 1.6f;
    if (e->is_player && r->mode == MODE_CHOQUES) return 1.7f;
    return 1.0f;
}


/* choque contra restos o contra un auto que esta chocando: obstaculo fijo */
static Entity *player_ent(Race *r, int p) {
    int i;
    for (i = 0; i < RACERS; i++) if (r->ent[i].is_player && r->ent[i].player == p) return &r->ent[i];
    return NULL;
}

static void obstacle_hit(Race *r, Entity *e, V3 opos, float oyaw, int owner, float *cd) {
    V3 fo = v3(sinf(oyaw), 0, -cosf(oyaw)), fe = v3(sinf(e->yaw), 0, -cosf(e->yaw));
    V3 best_n = v3(0, 0, 0), ve, dv = v3(0, 0, 0);
    float best_pen = 0, vrel, closing;
    int ca, cb;
    Entity *own;
    if (fabsf(e->pos.y - opos.y) > 3.0f) return;
    if (fabsf(e->pos.x - opos.x) > 7.0f || fabsf(e->pos.z - opos.z) > 7.0f) return;
    for (ca = -1; ca <= 1; ca += 2)
        for (cb = -1; cb <= 1; cb += 2) {
            V3 po = vadd(opos, vscale(fo, 1.2f * ca));
            V3 pe = vadd(e->pos, vscale(fe, 1.2f * cb));
            V3 dl = v3(pe.x - po.x, 0, pe.z - po.z);
            float dist = vlen(dl), pen = 2.1f - dist;
            if (pen > best_pen) { best_pen = pen; best_n = dist > 1e-4f ? vscale(dl, 1.0f / dist) : v3(1, 0, 0); }
        }
    if (best_pen <= 0) return;
    ve = ent_vel(r, e);
    vrel = vdot(ve, best_n);
    closing = -vrel;
    if (vrel < 0) dv = vscale(best_n, -vrel * 1.25f);
    ent_add(r, e, dv, vscale(best_n, best_pen));
    if (closing < 4.0f) return;
    dent(r, e, vscale(best_n, -1.0f), closing / 50.0f);
    if (e->is_player && e->bump_cool <= 0) { plat_sfx(SFX_BUMP); e->bump_cool = 0.3f; }
    /* premio: alguien choco con tus restos */
    own = owner >= 0 ? player_ent(r, owner) : NULL;
    if (own && own != e && *cd <= 0 && closing > 6.0f) {
        int wrecked = e->invuln <= 0 && closing > (e->is_player ? 20.0f : 12.0f);
        own->veh.boost = clampf(own->veh.boost + (wrecked ? 0.45f : 0.25f), 0, 1);
        set_msg(r, owner, wrecked ? "CARAMBOLA! +NITRO" : "RESTOS: +NITRO", col(255, 170, 40), 2.0f);
        plat_sfx(SFX_BOOST);
        *cd = 1.2f;
    }
    if (e->invuln <= 0 && closing > (e->is_player ? 20.0f : 12.0f))
        crash_start(r, e, vadd(ve, vscale(best_n, closing * 0.3f)));
}

static void obstacles(Race *r) {
    int i, k;
    if (r->mode != MODE_CHOQUES) return;
    for (i = 0; i < ENT_MAX; i++) {
        Entity *e = &r->ent[i];
        if (!e->active || e->crashed) continue;
        for (k = 0; k < WRECK_MAX; k++) {
            Wreck *w = &r->wreck[k];
            if (w->active) obstacle_hit(r, e, w->pos, w->yaw, w->owner, &w->hit_cd);
        }
        for (k = 0; k < ENT_MAX; k++) {
            Entity *c = &r->ent[k];
            if (k == i || !c->active || !c->crashed || c->busted || c->crash_t < 0.6f) continue;
            obstacle_hit(r, e, c->pos, c->yaw + c->crash_rot.y, c->is_player ? c->player : -1, &c->hit_cd);
        }
    }
}

static void debris_update(Race *r, float dt) {
    const Track *tr = r->track;
    int k;
    for (k = 0; k < DEBRIS_MAX; k++) {
        Debris *d = &r->debris[k];
        TrackPos tp;
        if (!d->active) continue;
        d->t += dt;
        if (d->t > 8.0f) { d->active = 0; continue; }
        d->vel.y -= 20.0f * dt;
        d->pos = vadd(d->pos, vscale(d->vel, dt));
        d->rot = vadd(d->rot, vscale(d->spin, dt));
        tp = track_locate(tr, d->pos, idx_of(tr, d->s));
        d->s = tp.s;
        if (fabsf(tp.d) > tr->wall - 0.4f) {
            V3 nrm = vscale(tp.right, signf(tp.d));
            float vn = vdot(d->vel, nrm);
            d->pos = vsub(d->pos, vscale(nrm, fabsf(tp.d) - (tr->wall - 0.4f)));
            if (vn > 0) d->vel = vsub(d->vel, vscale(nrm, vn * 1.6f));
        }
        if (d->pos.y < tp.h + 0.35f) {
            d->pos.y = tp.h + 0.35f;
            if (d->vel.y < 0) d->vel.y = -d->vel.y * 0.45f;
            d->vel.x *= 0.8f; d->vel.z *= 0.8f;
            d->spin = vscale(d->spin, 0.8f);
        }
    }
}

static void takedown(Race *r, Entity *attacker, Entity *victim, V3 vvel) {
    crash_start(r, victim, vvel);
    if (attacker->is_player) {
        attacker->veh.boost = clampf(attacker->veh.boost + 0.45f, 0, 1);
        set_msg(r, attacker->player, "TAKEDOWN!", col(255, 210, 40), 2.0f);
        plat_sfx(SFX_BOOST);
    }
}

static void collisions(Race *r) {
    int i, j;
    for (i = 0; i < ENT_MAX; i++) {
        Entity *a = &r->ent[i];
        if (!a->active || a->crashed) continue;
        for (j = i + 1; j < ENT_MAX; j++) {
            Entity *b = &r->ent[j];
            V3 fa, fb, best_n = v3(0, 0, 0);
            float best_pen = 0, ra, rb;
            int ca, cb;
            if (!b->active || b->crashed) continue;
            if (a->is_traffic && b->is_traffic) continue;
            if (fabsf(a->pos.y - b->pos.y) > 3.0f) continue;
            if (fabsf(a->pos.x - b->pos.x) > 7.0f || fabsf(a->pos.z - b->pos.z) > 7.0f) continue;
            fa = v3(sinf(a->yaw), 0, -cosf(a->yaw));
            fb = v3(sinf(b->yaw), 0, -cosf(b->yaw));
            ra = a->is_traffic ? 1.15f : 1.05f;
            rb = b->is_traffic ? 1.15f : 1.05f;
            for (ca = -1; ca <= 1; ca += 2) {
                for (cb = -1; cb <= 1; cb += 2) {
                    V3 pa = vadd(a->pos, vscale(fa, 1.2f * ca));
                    V3 pb = vadd(b->pos, vscale(fb, 1.2f * cb));
                    V3 dlt = v3(pb.x - pa.x, 0, pb.z - pa.z);
                    float dist = vlen(dlt);
                    float pen = ra + rb - dist;
                    if (pen > best_pen) {
                        best_pen = pen;
                        best_n = dist > 1e-4f ? vscale(dlt, 1.0f / dist) : v3(1, 0, 0);
                    }
                }
            }
            if (best_pen <= 0) continue;
            {
                float ma = ent_mass(r, a), mb = ent_mass(r, b);
                float wa = mb / (ma + mb), wb = ma / (ma + mb);
                V3 va = ent_vel(r, a), vb = ent_vel(r, b);
                float vrel = vdot(vsub(vb, va), best_n);
                V3 dva = v3(0, 0, 0), dvb = v3(0, 0, 0);
                float closing = -vrel;
                if (vrel < 0) {
                    float jimp = -(1.0f + 0.35f) * vrel / (1.0f / ma + 1.0f / mb);
                    dva = vscale(best_n, -jimp / ma);
                    dvb = vscale(best_n, jimp / mb);
                }
                ent_add(r, a, dva, vscale(best_n, -best_pen * wa));
                ent_add(r, b, dvb, vscale(best_n, best_pen * wb));
                if (closing > 6.0f) {
                    dent(r, a, best_n, closing / 45.0f);
                    dent(r, b, vscale(best_n, -1.0f), closing / 45.0f);
                }

                if ((a->is_suv && b->is_player) || (b->is_suv && a->is_player)) {
                    /* embestida de camioneta: empujon extra hacia afuera del golpe */
                    Entity *pl = a->is_player ? a : b;
                    V3 n = a->is_player ? vscale(best_n, -1.0f) : best_n;   /* de la camioneta hacia el jugador */
                    if (closing > 5.0f && pl->bump_cool <= 0) {
                        ent_add(r, pl, vscale(n, 0.45f * closing), v3(0, 0, 0));
                        set_msg(r, pl->player, "EMBESTIDA!", col(255, 90, 60), 1.0f);
                    }
                }
                if (r->mode != MODE_CHOQUES && (a->is_police || b->is_police) && (a->is_player || b->is_player)) {
                    Entity *pol = a->is_police ? a : b, *pl = a->is_player ? a : b;
                    if (closing > (pol->is_suv ? 26.0f : 16.0f) && pol->invuln <= 0) {
                        crash_start(r, pol, vscale(ent_vel(r, pl), 0.8f));
                        set_msg(r, pl->player, "PATRULLA FUERA!", col(90, 200, 255), 2.0f);
                    }
                }
                if (r->mode == MODE_CHOQUES) {
                    Entity *pl = a->is_player ? a : (b->is_player ? b : NULL);
                    Entity *ot = pl == a ? b : a;
                    V3 dvo = pl == a ? dvb : dva;
                    if (pl && ot->is_traffic) {
                        if (closing > 11.0f && pl->invuln <= 0) {
                            crash_start(r, pl, pl == a ? va : vb);
                            crash_start(r, ot, vscale(pl == a ? va : vb, 0.6f));
                        }
                    } else if (pl && ot->is_player) {
                        float da = vlen(dva), db = vlen(dvb);
                        Entity *victim = da > db ? a : b;
                        Entity *att = victim == a ? b : a;
                        if (fmaxf(da, db) > 6.0f && victim->invuln <= 0)
                            takedown(r, att, victim, ent_vel(r, victim));
                    } else if (pl) {
                        float lat = vlen(dvo);
                        if (ot->invuln <= 0 && (lat > 4.5f || (fabsf(ot->d) > r->track->wall - 2.2f && closing > 2.5f)))
                            takedown(r, pl, ot, vadd(ent_vel(r, ot), vscale(dvo, 2.0f)));
                    } else if ((a->is_traffic || b->is_traffic) && closing > 14.0f) {
                        Entity *rc = a->is_traffic ? b : a;
                        if (rc->invuln <= 0) crash_start(r, rc, ent_vel(r, rc));
                    }
                }
                if (closing > 3.0f) {
                    if ((a->is_player && a->bump_cool <= 0) || (b->is_player && b->bump_cool <= 0)) {
                        plat_sfx(SFX_BUMP);
                        a->bump_cool = b->bump_cool = 0.3f;
                    }
                }
            }
        }
    }
}

/* ---------------------------------------------------------------- */

static void update_progress(Race *r, Entity *e) {
    const Track *tr = r->track;
    float L = tr->length;
    if (e->prev_s > L * 0.75f && e->s < L * 0.25f) {
        e->lap++;
        if (e->is_player) {
            PlayerView *v = &r->view[e->player];
            if (e->lap >= 2 && e->lap <= RACE_LAPS + 1) {
                float lt = r->time - v->lap_start;
                if (v->best_lap <= 0 || lt < v->best_lap) v->best_lap = lt;
            }
            v->lap_start = r->time;
            if (e->lap == RACE_LAPS) set_msg(r, e->player, "VUELTA FINAL", col(255, 230, 60), 2.0f);
            else if (e->lap > 1 && e->lap < RACE_LAPS) {
                char b[32];
                snprintf(b, sizeof(b), "VUELTA %d", e->lap);
                set_msg(r, e->player, b, col(255, 255, 255), 1.6f);
            }
        }
    } else if (e->prev_s < L * 0.25f && e->s > L * 0.75f) {
        e->lap--;
    }
    e->prev_s = e->s;
    e->progress = e->lap * L + e->s;
    if (!e->finished && e->lap > RACE_LAPS) {
        e->finished = 1;
        e->finish_time = r->time;
        e->place = ++r->finish_order;
        if (e->is_player) set_msg(r, e->player, "META!", col(120, 255, 120), 4.0f);
    }
}

static void update_places(Race *r) {
    int i, j;
    for (i = 0; i < RACERS; i++) {
        Entity *e = &r->ent[i];
        int place = 1;
        if (e->finished) continue;
        for (j = 0; j < RACERS; j++) {
            Entity *o = &r->ent[j];
            if (j == i) continue;
            if (o->finished || o->progress > e->progress) place++;
        }
        e->place = place;
    }
}

int race_all_players_done(const Race *r) {
    int i;
    for (i = 0; i < RACERS; i++)
        if (r->ent[i].is_player && !r->ent[i].finished) return 0;
    return 1;
}

static void near_misses(Race *r) {
    int i, k;
    if (r->mode != MODE_CHOQUES && r->mode != MODE_CALLE) return;
    for (i = 0; i < RACERS; i++) {
        Entity *p = &r->ent[i];
        PlayerView *v;
        if (!p->is_player) continue;
        v = &r->view[p->player];
        for (k = 0; k < r->ntraffic; k++) {
            Entity *t = &r->ent[RACERS + k];
            float rel = wrap_rel(r->track, t->s - p->s);
            float prev = v->prev_rel[k];
            if (!p->crashed && !t->crashed && prev > 0 && rel <= 0 && prev < 30.0f &&
                fabsf(t->d - p->d) < 3.7f && p->veh.speed > 22.0f && fabsf(t->pos.y - p->pos.y) < 3) {
                p->veh.boost = clampf(p->veh.boost + (r->mode == MODE_CALLE ? 0.08f : 0.12f), 0, 1);
                v->slow = clampf(v->slow + 0.10f, 0, 1);
                set_msg(r, p->player, r->mode == MODE_CALLE ? "CASI! +NITRO" : "CASI! +TURBO", col(90, 220, 255), 1.0f);
            }
            v->prev_rel[k] = rel;
        }
    }
}

static void update_camera(Race *r, int p, float dt) {
    PlayerView *v = &r->view[p];
    Entity *e = NULL;
    int i;
    for (i = 0; i < RACERS; i++) if (r->ent[i].is_player && r->ent[i].player == p) e = &r->ent[i];
    if (!e) return;
    if (e->crashed) {
        float a = e->crash_t * 0.9f + e->yaw + 1.2f;
        v->eye = vadd(e->pos, v3(cosf(a) * 10.0f, 4.5f, sinf(a) * 10.0f));
        v->target = e->pos;
        v->fov = lerpf(v->fov, 55.0f, fminf(1, 3 * dt));
    } else {
        const Vehicle *vh = &e->veh;
        float dist = v->far_cam ? 9.5f : 6.8f, hgt = v->far_cam ? 3.3f : 2.5f;
        float spd = (r->mode == MODE_SIM) ? vh->vf : vh->speed;
        float top = veh_top_speed(vh->spec, r->mode);
        float desired;
        V3 dir;
        if (r->mode == MODE_SIM) desired = vh->yaw + vh->slip * 0.55f;
        else desired = vh->yaw - wrap_angle(vh->yaw - vh->veldir) * 0.55f;
        v->yaw += wrap_angle(desired - v->yaw) * fminf(1.0f, 7.0f * dt);
        dir = v3(sinf(v->yaw), 0, -cosf(v->yaw));
        {
            V3 want_eye = vadd(vsub(e->pos, vscale(dir, dist)), v3(0, hgt, 0));
            v->eye = vlerp(v->eye, want_eye, fminf(1.0f, 12.0f * dt));
            v->eye.y = fmaxf(v->eye.y, e->pos.y + 1.4f);
        }
        v->target = vadd(vadd(e->pos, vscale(dir, 4.0f)), v3(0, 1.1f, 0));
        {
            float want_fov = 60.0f + clampf(fabsf(spd) / top, 0, 1.3f) * 9.0f + (vh->boosting ? 10.0f : 0.0f);
            v->fov = lerpf(v->fov, want_fov, fminf(1, 3 * dt));
        }
    }
    if (v->shake > 0) v->shake = fmaxf(0, v->shake - dt * 1.5f);
    if (v->msg_t > 0) v->msg_t -= dt;
}

void race_camera(Race *r, int p, M34 view, float *fov) {
    PlayerView *v = &r->view[p];
    V3 eye = v->eye;
    if (v->shake > 0) {
        eye.x += sinf(r->time * 53.0f) * v->shake * 0.3f;
        eye.y += sinf(r->time * 41.0f) * v->shake * 0.25f;
    }
    m34_lookat(view, eye, v3(0, 1, 0), v->target);
    *fov = v->fov;
}

void race_wreck_matrix(const Wreck *w, M34 out) {
    V3 f = v3(sinf(w->yaw), 0, -cosf(w->yaw));
    V3 rt = v3(cosf(w->yaw), 0, sinf(w->yaw));
    m34_from_basis(out, rt, v3(0, 1, 0), vscale(f, -1), w->pos);
    if (w->flipped) {
        M34 rz;
        m34_rot_z(rz, (float)M_PI);
        rz[1][3] = 1.40f;
        m34_mul(out, out, rz);
    }
}

void race_debris_matrix(const Debris *d, M34 out) {
    M34 rx, ry, rz;
    m34_rot_y(ry, d->rot.y);
    m34_rot_x(rx, d->rot.x);
    m34_rot_z(rz, d->rot.z);
    m34_mul(out, ry, rx);
    m34_mul(out, out, rz);
    out[0][3] = d->pos.x; out[1][3] = d->pos.y; out[2][3] = d->pos.z;
}

void race_entity_matrix(const Race *r, const Entity *e, M34 out) {
    float cp = cosf(e->pitch), sp = sinf(e->pitch);
    V3 f = v3(sinf(e->yaw) * cp, sp, -cosf(e->yaw) * cp);
    V3 rt = v3(cosf(e->yaw), 0, sinf(e->yaw));
    V3 up = vcross(rt, f);
    m34_from_basis(out, rt, up, vscale(f, -1), e->pos);
    {   /* peralte: el auto se inclina con el camino (segun hacia donde mire) */
        float b = track_bank(r->track, e->s);
        if (fabsf(b) > 1e-4f && !e->crashed) {
            V3 tan = track_tangent(r->track, e->s);
            float dir = f.x * tan.x + f.z * tan.z;
            M34 rz;
            m34_rot_z(rz, atanf(b) * clampf(dir, -1, 1));
            m34_mul(out, out, rz);
        }
    }
    if (e->crashed) {
        M34 rx, ry, rz, t;
        m34_rot_x(rx, e->crash_rot.x);
        m34_rot_y(ry, e->crash_rot.y);
        m34_rot_z(rz, e->crash_rot.z);
        m34_mul(t, ry, rx);
        m34_mul(t, t, rz);
        t[1][3] = 0.6f;
        m34_mul(out, out, t);
    } else if (e->is_player && r->mode != MODE_SIM) {
        /* leve balanceo al derrapar */
        M34 rz;
        m34_rot_z(rz, -clampf(e->veh.slip, -0.6f, 0.6f) * 0.08f);
        m34_mul(out, out, rz);
    }
}

/* ---------------------------------------------------------------- */

void race_update(Race *r, const PadIn pads[MAX_PLAYERS], float dt) {
    int i, p, steps, k;
    float sim_dt, h;

    for (p = 0; p < r->nplayers; p++) {
        if (pads[p].pressed & BTN_Z) r->view[p].far_cam = !r->view[p].far_cam;
    }

    if (r->state == RS_COUNTDOWN) {
        int sec;
        r->countdown -= dt;
        sec = (int)ceilf(r->countdown);
        if (sec < r->last_beep && sec >= 1 && sec <= 3) { plat_sfx(SFX_BEEP); r->last_beep = sec; }
        if (r->countdown <= 0) {
            r->state = RS_RUNNING;
            plat_sfx(SFX_GO);
            for (p = 0; p < r->nplayers; p++) set_msg(r, p, "YA!", col(120, 255, 120), 1.0f);
        }
        for (p = 0; p < r->nplayers; p++) update_camera(r, p, dt);
        return;
    }

    sim_dt = dt;
    if (r->slowmo > 0) { r->slowmo -= dt; sim_dt = dt * 0.35f; }
    /* CALLE: camara lenta a voluntad (boton Y), solo con 1 jugador */
    r->timescale = 1.0f;
    if (r->mode == MODE_CALLE && r->nplayers == 1 && r->state == RS_RUNNING) {
        PlayerView *v = &r->view[0];
        Entity *pl = NULL;
        for (i = 0; i < RACERS; i++) if (r->ent[i].is_player) pl = &r->ent[i];
        v->slowing = (pads[0].held & BTN_Y) && v->slow > 0.02f && pl && !pl->crashed;
        if (v->slowing) {
            v->slow = fmaxf(0, v->slow - 0.33f * dt);
            r->timescale = 0.4f;
            sim_dt *= 0.4f;
        } else if (pl && fabsf(pl->veh.speed) > pl->veh.spec->vmax * 0.7f) {
            v->slow = clampf(v->slow + 0.03f * dt, 0, 1);
        }
    }
    r->time += sim_dt;

    ai_think(r);
    police_update(r, sim_dt);
    steps = (int)ceilf(sim_dt / (1.0f / 120.0f));
    if (steps < 1) steps = 1;
    if (steps > 8) steps = 8;
    h = sim_dt / steps;

    for (k = 0; k < steps; k++) {
        for (i = 0; i < ENT_MAX; i++) {
            Entity *e = &r->ent[i];
            if (!e->active) continue;
            if (e->invuln > 0) e->invuln -= h;
            if (e->bump_cool > 0) e->bump_cool -= h;
            if (e->crashed) { crash_update(r, e, h); continue; }
            if (e->is_player) {
                CarInput in = e->finished ? race_autopilot(r, e, h) : race_pad_to_input(&pads[e->player]);
                veh_step(&e->veh, r->mode, &in, r->track, h);
                e->pos = e->veh.pos; e->yaw = e->veh.yaw; e->pitch = e->veh.pitch;
                e->s = e->veh.s; e->d = e->veh.d;
                if (r->mode == MODE_CHOQUES) {
                    float top = e->veh.spec->vmax;
                    if (e->veh.speed > top * 0.9f) e->veh.boost = clampf(e->veh.boost + 0.02f * h, 0, 1);
                    if (e->veh.wall_impact > 12.0f && e->invuln <= 0)
                        crash_start(r, e, vadd(veh_velocity(&e->veh, r->mode), vscale(e->veh.right, signf(e->d) * e->veh.wall_impact)));
                }
                if (e->veh.wall_impact > 4.0f && e->bump_cool <= 0) {
                    plat_sfx(SFX_BUMP); e->bump_cool = 0.3f;
                    dent(r, e, vscale(e->veh.right, signf(e->d)), e->veh.wall_impact / 40.0f);
                }
            } else {
                kin_step(r, e, h);
            }
        }
        collisions(r);
        obstacles(r);
        for (i = 0; i < RACERS; i++) {
            Entity *e = &r->ent[i];
            if (e->is_player) { e->s = e->veh.s; }
            update_progress(r, e);
        }
    }
    debris_update(r, sim_dt);
    for (i = 0; i < WRECK_MAX; i++) {
        Wreck *w = &r->wreck[i];
        if (!w->active) continue;
        w->t += sim_dt;
        if (w->hit_cd > 0) w->hit_cd -= sim_dt;
        if (w->t > 45.0f) w->active = 0;
    }
    for (i = 0; i < ENT_MAX; i++) if (r->ent[i].hit_cd > 0) r->ent[i].hit_cd -= sim_dt;
    near_misses(r);
    update_places(r);
    for (p = 0; p < r->nplayers; p++) update_camera(r, p, dt);

    if (r->state == RS_RUNNING && race_all_players_done(r)) {
        r->state = RS_FINISHED;
        r->finish_wait = 0;
    }
    if (r->state == RS_FINISHED) r->finish_wait += dt;
}
