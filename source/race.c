/* race.c - logica de carrera: IA, trafico, colisiones, choques, vueltas, camara */
#include <stdio.h>
#include "race.h"

static void update_places(Race *r);

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

void race_setup(Race *r, Track *tr, PhysMode mode, int nplayers, const int cars[2]) {
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
            int tries = 0;
            do {
                e->car = ai_car % CAR_COUNT;
                ai_car++;
                tries++;
            } while (tries < CAR_COUNT && (e->car == cars[0] || (nplayers > 1 && e->car == cars[1])));
            e->skill = 0.91f + 0.025f * i;
        }
        e->paint = CAR_SPECS[e->car].paint;
        e->accent = CAR_SPECS[e->car].accent;
        if (!is_p && (e->car == cars[0] || (nplayers > 1 && e->car == cars[1])))
            e->paint = CAR_ALT_PAINTS[i % 6];
        if (is_p && nplayers > 1 && e->player == 1 && cars[1] == cars[0])
            e->paint = CAR_ALT_PAINTS[1];

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

    /* trafico solo en modo CHOQUES */
    if (mode == MODE_CHOQUES) {
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
    for (i = 0; i < 2; i++) {
        int k;
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
    return in;
}

CarInput race_autopilot(const Race *r, Entity *e, float dt) {
    const Track *tr = r->track;
    const Vehicle *v = &e->veh;
    CarInput in;
    float spd = (r->mode == MODE_SIM) ? v->vf : v->speed;
    int i = v->idx;
    float st = v->s + 9.0f + fabsf(spd) * 0.45f;
    V3 P = track_point(tr, st, r->line[idx_of(tr, st)] * 0.7f);
    float want = atan2f(P.x - v->pos.x, -(P.z - v->pos.z));
    float ang = wrap_angle(want - v->yaw);
    float vt = r->vlim[(i + 3) % tr->n] * (r->mode == MODE_SIM ? 0.93f : 0.80f);
    in.steer = clampf(ang * 2.6f, -1, 1);
    in.throttle = spd < vt ? 1.0f : 0.0f;
    in.brake = spd > vt + 2.5f ? clampf((spd - vt) * 0.25f, 0, 1) : 0.0f;
    in.boost = 0;
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

static void crash_start(Race *r, Entity *e, V3 vel) {
    float sp;
    if (e->crashed) return;
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
        e->crashed = 0;
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
        if (!e->active || e->is_player || e->crashed) continue;
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
    }
}

static void kin_step(Race *r, Entity *e, float h) {
    const Track *tr = r->track;
    int i = idx_of(tr, e->s);
    V3 tan;
    if (e->is_traffic) {
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
    if (r->mode == MODE_DERRAPE && !e->is_traffic) {
        float dr = clampf(tr->curv[i] * e->v * 0.5f, -0.55f, 0.55f);
        e->yaw += dr;
    }
    e->pitch = asinf(clampf(tan.y, -1, 1));
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
    if (e->is_player && r->mode == MODE_CHOQUES) return 1.7f;
    return 1.0f;
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
    if (r->mode != MODE_CHOQUES) return;
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
                p->veh.boost = clampf(p->veh.boost + 0.12f, 0, 1);
                set_msg(r, p->player, "CASI! +TURBO", col(90, 220, 255), 1.0f);
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

void race_entity_matrix(const Race *r, const Entity *e, M34 out) {
    float cp = cosf(e->pitch), sp = sinf(e->pitch);
    V3 f = v3(sinf(e->yaw) * cp, sp, -cosf(e->yaw) * cp);
    V3 rt = v3(cosf(e->yaw), 0, sinf(e->yaw));
    V3 up = vcross(rt, f);
    m34_from_basis(out, rt, up, vscale(f, -1), e->pos);
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

void race_update(Race *r, const PadIn pads[2], float dt) {
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
    r->time += sim_dt;

    ai_think(r);
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
                if (e->veh.wall_impact > 4.0f && e->bump_cool <= 0) { plat_sfx(SFX_BUMP); e->bump_cool = 0.3f; }
            } else {
                kin_step(r, e, h);
            }
        }
        collisions(r);
        for (i = 0; i < RACERS; i++) {
            Entity *e = &r->ent[i];
            if (e->is_player) { e->s = e->veh.s; }
            update_progress(r, e);
        }
    }
    near_misses(r);
    update_places(r);
    for (p = 0; p < r->nplayers; p++) update_camera(r, p, dt);

    if (r->state == RS_RUNNING && race_all_players_done(r)) {
        r->state = RS_FINISHED;
        r->finish_wait = 0;
    }
    if (r->state == RS_FINISHED) r->finish_wait += dt;
}
