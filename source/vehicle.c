/* vehicle.c - fisica de los autos: 2 modos arcade y 1 semi-simulacion */
#include "vehicle.h"

static const char *MODE_NAMES[MODE_COUNT] = { "ARCADE DERRAPE", "ARCADE CHOQUES", "SIMULACION" };
static const char *MODE_DESCS[MODE_COUNT] = {
    "DERRAPES LARGOS Y RAPIDOS",
    "TURBO, TRAFICO Y TAKEDOWNS",
    "PESO, FRENADA Y AGARRE REAL"
};

const char *mode_name(int m) { return (m >= 0 && m < MODE_COUNT) ? MODE_NAMES[m] : "?"; }
const char *mode_desc(int m) { return (m >= 0 && m < MODE_COUNT) ? MODE_DESCS[m] : "?"; }

static V3 dir_of(float a) { return v3(sinf(a), 0, -cosf(a)); }
static V3 right_of(float a) { return v3(cosf(a), 0, sinf(a)); }

V3 veh_forward(const Vehicle *v) { return dir_of(v->yaw); }

float veh_top_speed(const CarSpec *spec, PhysMode mode) {
    if (mode == MODE_SIM) {
        /* equilibrio potencia / resistencia aerodinamica */
        return cbrtf(spec->power / 0.42f) * 0.97f;
    }
    return spec->vmax;
}

void veh_place(Vehicle *v, const Track *tr, float s, float d, float speed) {
    TrackPos tp;
    v->pos = track_point(tr, s, d);
    tp = track_locate(tr, v->pos, -1);
    v->idx = tp.idx; v->s = tp.s; v->d = tp.d; v->tan = tp.tan; v->right = tp.right;
    v->pos.y = tp.h;
    v->yaw = track_heading(tp.tan);
    v->veldir = v->yaw;
    v->speed = speed;
    v->vf = speed; v->vr = 0; v->yawrate = 0; v->steer_cur = 0; v->ax = 0;
    v->drifting = 0;
    v->pitch = asinf(clampf(tp.tan.y, -1, 1));
}

void veh_init(Vehicle *v, const CarSpec *spec, const Track *tr, float s, float d) {
    memset(v, 0, sizeof(Vehicle));
    v->spec = spec;
    v->gear = 1;
    v->gas_off_t = 10.0f;
    veh_place(v, tr, s, d, 0);
}

V3 veh_velocity(const Vehicle *v, PhysMode mode) {
    if (mode == MODE_SIM) return vadd(vscale(dir_of(v->yaw), v->vf), vscale(right_of(v->yaw), v->vr));
    return vscale(dir_of(v->veldir), v->speed);
}

void veh_set_velocity(Vehicle *v, PhysMode mode, V3 vel) {
    vel.y = 0;
    if (mode == MODE_SIM) {
        v->vf = vdot(vel, dir_of(v->yaw));
        v->vr = vdot(vel, right_of(v->yaw));
    } else {
        float sp = vlen(vel);
        if (sp < 0.01f) { v->speed = 0; return; }
        if (vdot(vel, dir_of(v->yaw)) >= 0) {
            v->speed = sp;
            v->veldir = atan2f(vel.x, -vel.z);
        } else {
            v->speed = -sp;
            v->veldir = atan2f(-vel.x, vel.z);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Arcade: comun a DERRAPE y CHOQUES                                   */
/* ------------------------------------------------------------------ */

static void arcade_step(Vehicle *v, PhysMode mode, const CarInput *in, const Track *tr, float dt) {
    const CarSpec *sp = v->spec;
    int choques = (mode == MODE_CHOQUES);
    float vmax = sp->vmax, acc = sp->accel;
    float spd = v->speed, aspd = fabsf(spd);
    float steer = clampf(in->steer, -1, 1);
    float slip;

    /* turbo (solo CHOQUES) */
    v->boosting = 0;
    if (choques && in->boost && v->boost > 0.01f && in->brake < 0.3f) {
        v->boosting = 1;
        vmax *= 1.24f;
        acc *= 1.8f;
        v->boost -= 0.20f * dt;
        if (v->boost < 0) v->boost = 0;
    }

    /* acelerador / freno */
    if (in->throttle > 0.05f && spd > -0.5f) {
        float k = 1.0f - (spd / vmax) * (spd / vmax);
        if (spd > vmax) spd -= (spd - vmax) * 0.8f * dt;
        else spd += acc * in->throttle * clampf(k, 0.05f, 1.0f) * dt * (v->boosting ? 1.0f : 1.0f);
    }
    if (in->brake > 0.05f) {
        if (spd > 0.5f) spd -= (choques ? 22.0f : 19.0f) * in->brake * dt;
        else if (in->throttle < 0.05f) spd = approach(spd, -9.0f, 7.0f * in->brake * dt);
    }
    /* resistencia */
    spd -= signf(spd) * (0.6f + 0.00035f * spd * spd) * dt;
    if (fabsf(spd) < 0.05f && in->throttle < 0.05f && in->brake < 0.05f) spd = 0;
    aspd = fabsf(spd);

    /* deteccion de inicio de derrape */
    if (in->throttle < 0.2f) v->gas_off_t = 0; else v->gas_off_t += dt;
    if (!v->drifting && aspd > 20.0f && fabsf(steer) > 0.45f && spd > 0) {
        int brake_tap = (in->brake > 0.3f && v->prev_brake <= 0.3f);
        int gas_flick = (!choques && in->throttle > 0.5f && v->prev_throttle <= 0.5f && v->gas_off_t < 0.45f);
        if (brake_tap || gas_flick) {
            v->drifting = 1;
            /* patada inicial hacia el interior de la curva */
            v->yaw += signf(steer) * (choques ? 0.10f : 0.16f) * sp->drift;
        }
    }
    v->prev_throttle = in->throttle;
    v->prev_brake = in->brake;

    if (v->drifting) {
        float Kd = (choques ? 1.25f : 1.55f) * sp->turn * sp->drift;
        float kth = choques ? 2.4f : 1.35f;
        float maxslip = choques ? 0.62f : 0.85f;
        v->yaw += steer * Kd * dt;
        slip = wrap_angle(v->yaw - v->veldir);
        v->veldir += kth * slip * dt;
        slip = wrap_angle(v->yaw - v->veldir);
        if (fabsf(slip) > maxslip) v->veldir = v->yaw - signf(slip) * maxslip;
        /* perdida de velocidad leve */
        spd -= ((choques ? 1.2f : 0.8f) + (choques ? 4.0f : 2.6f) * fabsf(sinf(slip))) * dt;
        if (choques) v->boost = clampf(v->boost + 0.10f * dt, 0, 1);
        /* salida del derrape */
        if ((fabsf(slip) < 0.07f && fabsf(steer) < 0.35f) || spd < 12.0f ||
            (signf(steer) != signf(slip) && fabsf(steer) > 0.3f && fabsf(slip) < 0.12f)) {
            v->drifting = 0;
        }
    } else {
        float agrip = (choques ? 20.0f : 15.0f) * sp->turn;
        float rate = steer * fminf(1.9f * sp->turn, agrip / fmaxf(aspd, 4.0f));
        if (aspd < 4.0f) rate *= aspd / 4.0f;
        if (spd < 0) rate = -rate;
        v->yaw += rate * dt;
        /* asistencia de alineacion (CHOQUES) */
        if (choques && fabsf(steer) < 0.15f && aspd > 10.0f) {
            float diff = wrap_angle(track_heading(v->tan) - v->yaw);
            if (fabsf(diff) < 0.5f) v->yaw += diff * 1.1f * dt;
        }
        /* la velocidad sigue al morro con un leve deslizamiento */
        slip = wrap_angle(v->yaw - v->veldir);
        v->veldir += slip * fminf(1.0f, 11.0f * dt);
    }
    v->yaw = wrap_angle(v->yaw);
    v->veldir = wrap_angle(v->veldir);
    v->speed = spd;
    v->slip = wrap_angle(v->yaw - v->veldir);

    {
        V3 dv = vscale(dir_of(v->veldir), spd * dt);
        v->pos.x += dv.x;
        v->pos.z += dv.z;
    }
    (void)tr;
}

/* ------------------------------------------------------------------ */
/* Semi-simulacion: modelo de bicicleta con transferencia de peso      */
/* ------------------------------------------------------------------ */

static float pacejka(float alpha) {
    return sinf(1.32f * atanf(9.0f * alpha));
}

static void sim_substep(Vehicle *v, const CarInput *in, float dt) {
    const CarSpec *sp = v->spec;
    const float g = 9.81f, L = 2.6f, h = 0.5f;
    float m = sp->mass;
    float b = sp->weight_front * L;   /* CG -> eje trasero */
    float a = L - b;                  /* CG -> eje delantero */
    float Iz = m * a * b * 1.05f;
    float mu = sp->grip;
    float vf = v->vf, vr = v->vr, w = v->yawrate;
    float avf = fabsf(vf);
    float Fzf, Fzr, delta, smax, af, ar, Fyf, Fyr, Fxf = 0, Fxr = 0;
    float drive, brake, Fx, Fy, M;

    /* direccion: menos angulo a mayor velocidad, con suavizado */
    smax = 0.52f / (1.0f + avf / 16.0f);
    v->steer_cur = approach(v->steer_cur, clampf(in->steer, -1, 1) * smax, 2.2f * dt);
    delta = v->steer_cur;

    /* transferencia de peso longitudinal */
    Fzf = m * g * b / L - m * clampf(v->ax, -9.0f, 6.0f) * h / L;
    Fzr = m * g * a / L + m * clampf(v->ax, -9.0f, 6.0f) * h / L;
    Fzf = fmaxf(Fzf, m * g * 0.15f);
    Fzr = fmaxf(Fzr, m * g * 0.15f);

    /* motor (con limite de traccion = control de traccion) */
    drive = 0;
    if (in->throttle > 0.02f && vf > -0.5f) {
        float Fz_drive = sp->drive == 0 ? Fzr : (sp->drive == 1 ? Fzf : Fzf + Fzr);
        drive = in->throttle * fminf(sp->power / fmaxf(avf, 6.0f), mu * Fz_drive * 0.92f);
    }
    brake = 0;
    if (in->brake > 0.02f) {
        if (vf > 0.6f) brake = in->brake * mu * m * g * 0.95f;  /* ABS */
        else if (in->throttle < 0.05f) drive = -in->brake * 4500.0f * (vf > -8.0f ? 1.0f : 0.0f);
    }
    if (sp->drive == 0) Fxr += drive;
    else if (sp->drive == 1) Fxf += drive;
    else { Fxf += drive * 0.4f; Fxr += drive * 0.6f; }
    if (brake > 0) {
        float sgn = signf(vf);
        Fxf -= sgn * brake * 0.62f;
        Fxr -= sgn * brake * 0.38f;
    }

    /* angulos de deriva */
    {
        float den = avf + 0.8f;
        af = atan2f(vr + a * w, den) - delta * signf(vf + 0.001f);
        ar = atan2f(vr - b * w, den);
    }
    /* leve tendencia subviradora: mas agarre atras (estable con mando) */
    Fyf = -mu * 0.98f * Fzf * pacejka(af);
    Fyr = -mu * 1.07f * Fzr * pacejka(ar);
    /* control de traccion: corta motor si la trasera desliza */
    {
        float k = clampf((fabsf(ar) - 0.10f) / 0.10f, 0, 1);
        if (sp->drive != 1 && drive > 0) {
            float cut = 1.0f - 0.8f * k;
            Fxr *= (Fxr > 0) ? cut : 1.0f;
            Fxf *= (Fxf > 0 && sp->drive == 2) ? cut : 1.0f;
        }
    }
    /* circulo de friccion */
    {
        float lf = sqrtf(fmaxf(0.0f, (mu * Fzf) * (mu * Fzf) - Fxf * Fxf));
        float lr = sqrtf(fmaxf(0.0f, (mu * Fzr) * (mu * Fzr) - Fxr * Fxr));
        Fyf = clampf(Fyf, -lf, lf);
        Fyr = clampf(Fyr, -lr, lr);
    }

    Fx = Fxr + Fxf * cosf(delta) - Fyf * sinf(delta);
    Fy = Fyr + Fxf * sinf(delta) + Fyf * cosf(delta);
    M = a * (Fxf * sinf(delta) + Fyf * cosf(delta)) - b * Fyr;

    /* control de estabilidad: frena el giro si la trasera se escapa */
    if (avf > 5.0f) {
        float wmax = mu * g / avf;
        float wk = clampf(vf * tanf(delta) / L, -wmax, wmax);
        float k = clampf((fabsf(ar) - 0.05f) / 0.10f, 0, 1);
        M -= k * (w - wk) * Iz * 5.0f;
    }

    /* resistencias */
    Fx -= 0.42f * vf * fabsf(vf);
    Fx -= signf(vf) * fminf(avf * 400.0f, 0.015f * m * g);

    v->ax = lerpf(v->ax, Fx / m, clampf(dt * 8.0f, 0, 1));
    vf += (Fx / m + vr * w) * dt;
    vr += (Fy / m - vf * w) * dt;
    w += (M / Iz) * dt;

    /* a baja velocidad, mezcla con modelo cinematico estable */
    if (fabsf(vf) < 4.0f) {
        float k = fabsf(vf) / 4.0f;
        float wk = vf * tanf(delta) / L;
        w = lerpf(wk, w, k);
        vr *= lerpf(0.85f, 1.0f, k);
    }
    v->vf = vf; v->vr = vr; v->yawrate = w;
    v->yaw = wrap_angle(v->yaw + w * dt);
    {
        V3 vel = vadd(vscale(dir_of(v->yaw), vf), vscale(right_of(v->yaw), vr));
        v->pos.x += vel.x * dt;
        v->pos.z += vel.z * dt;
    }
    v->slip = atan2f(vr, fabsf(vf) + 0.5f);
}

/* ------------------------------------------------------------------ */

static void update_gear(Vehicle *v, float speed, float top) {
    float r = clampf(fabsf(speed) / top, 0, 1.1f);
    int gear = 1 + (int)(r * 5.0f);
    float base, span;
    if (gear > 6) gear = 6;
    v->gear = gear;
    base = (gear - 1) / 5.0f;
    span = 1.0f / 5.0f;
    v->rpm01 = clampf(0.25f + 0.75f * (r - base) / span * 0.85f, 0.2f, 1.0f);
    if (gear == 6) v->rpm01 = clampf(0.3f + 0.7f * (r - base) / (1.1f - base), 0.2f, 1.0f);
}

void veh_step(Vehicle *v, PhysMode mode, const CarInput *in, const Track *tr, float dt) {
    TrackPos tp;
    float lim;
    v->wall_impact = 0;

    if (mode == MODE_SIM) {
        int k;
        for (k = 0; k < 2; k++) sim_substep(v, in, dt * 0.5f);
    } else {
        arcade_step(v, mode, in, tr, dt);
    }

    /* seguir la pista */
    tp = track_locate(tr, v->pos, v->idx);
    v->idx = tp.idx; v->s = tp.s; v->d = tp.d; v->tan = tp.tan; v->right = tp.right;
    v->pos.y = tp.h;
    v->pitch = asinf(clampf(tp.tan.y, -1, 1));

    /* muros */
    lim = tr->wall - 1.0f;
    if (fabsf(tp.d) > lim) {
        float side = signf(tp.d);
        float over = fabsf(tp.d) - lim;
        V3 vel = veh_velocity(v, mode);
        V3 nrm = vscale(tp.right, side);
        float vn = vdot(vel, nrm);
        v->pos.x -= nrm.x * over;
        v->pos.z -= nrm.z * over;
        v->d = side * lim;
        if (vn > 0) {
            V3 vt;
            float scrape = (mode == MODE_SIM) ? 0.07f : (mode == MODE_CHOQUES ? 0.04f : 0.03f);
            vel = vsub(vel, vscale(nrm, vn * 1.25f));
            vt = vsub(vel, vscale(nrm, vdot(vel, nrm)));
            vel = vadd(vscale(vt, 1.0f - scrape - 0.004f * vn), vscale(nrm, vdot(vel, nrm)));
            veh_set_velocity(v, mode, vel);
            v->wall_impact = vn;
            if (mode != MODE_SIM) {
                float th = track_heading(tp.tan);
                float diff = wrap_angle(th - v->yaw);
                if (fabsf(diff) < 1.2f) v->yaw += diff * 0.35f;
                if (v->drifting && vn > 6.0f) v->drifting = 0;
            } else {
                v->yawrate *= 0.6f;
            }
        }
    }

    update_gear(v, mode == MODE_SIM ? v->vf : v->speed, veh_top_speed(v->spec, mode));
}
