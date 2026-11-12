/* track.c - generacion procedural de pistas originales */
#include "track.h"

#define RAW_MAX 6000
#define CHUNK_SEGS 24

static V3 raw[RAW_MAX];
static int nraw;

static const char *NAMES[TRACK_COUNT] = { "CIRCUITO OCHO", "FORTALEZA AZUL", "VALLE DEL TUNEL" };

const char *track_name(int id) { return (id >= 0 && id < TRACK_COUNT) ? NAMES[id] : "?"; }

/* ---------- curvas de control ---------- */

static V3 catmull(V3 p0, V3 p1, V3 p2, V3 p3, float t) {
    float t2 = t * t, t3 = t2 * t;
    V3 r;
    r.x = 0.5f * ((2 * p1.x) + (-p0.x + p2.x) * t + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3);
    r.y = 0.5f * ((2 * p1.y) + (-p0.y + p2.y) * t + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3);
    r.z = 0.5f * ((2 * p1.z) + (-p0.z + p2.z) * t + (2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z) * t2 + (-p0.z + 3 * p1.z - 3 * p2.z + p3.z) * t3);
    return r;
}

static void raw_from_controls(const float (*cp)[3], int ncp) {
    int i, k, per = RAW_MAX / ncp - 1;
    nraw = 0;
    for (i = 0; i < ncp; i++) {
        V3 p0 = v3(cp[(i - 1 + ncp) % ncp][0], cp[(i - 1 + ncp) % ncp][2], cp[(i - 1 + ncp) % ncp][1]);
        V3 p1 = v3(cp[i][0], cp[i][2], cp[i][1]);
        V3 p2 = v3(cp[(i + 1) % ncp][0], cp[(i + 1) % ncp][2], cp[(i + 1) % ncp][1]);
        V3 p3 = v3(cp[(i + 2) % ncp][0], cp[(i + 2) % ncp][2], cp[(i + 2) % ncp][1]);
        for (k = 0; k < per; k++) raw[nraw++] = catmull(p0, p1, p2, p3, (float)k / per);
    }
}

/* Ocho: lemniscata de Gerono con un puente en el cruce */
static void raw_ocho(void) {
    int i, N = 4000;
    float A = 330.0f, B = 115.0f, t0 = 1.5f * (float)M_PI - 0.45f;
    nraw = 0;
    for (i = 0; i < N; i++) {
        float t = t0 + 2.0f * (float)M_PI * i / N;
        float x = A * cosf(t);
        float z = B * sinf(2.0f * t);
        float dt = wrap_angle(t - 0.5f * (float)M_PI);
        float y = 11.5f * expf(-(dt / 0.62f) * (dt / 0.62f));
        raw[nraw++] = v3(x, y, z);
    }
}

/* x, z, y */
static const float FORT_CP[][3] = {
    {-60, 0, 0}, {80, -6, 0}, {220, -14, 1}, {330, 30, 2}, {385, 150, 3}, {345, 265, 3},
    {235, 305, 2}, {150, 248, 1}, {62, 268, 1}, {-30, 338, 2}, {-165, 332, 3}, {-262, 238, 3},
    {-262, 118, 2}, {-175, 42, 1}
};

static const float VALLE_CP[][3] = {
    {-40, 0, 0}, {140, 2, 0}, {300, -10, 1}, {440, -70, 5}, {530, -200, 11}, {500, -350, 17},
    {370, -430, 19}, {215, -405, 15}, {95, -318, 9}, {-45, -335, 5}, {-205, -395, 7},
    {-370, -320, 11}, {-440, -170, 8}, {-360, -45, 3}, {-200, 4, 0}
};

static void finalize_centerline(Track *tr) {
    static float cum[RAW_MAX + 1];
    int i, k, n;
    float L;
    cum[0] = 0;
    for (i = 0; i < nraw; i++) {
        V3 a = raw[i], b = raw[(i + 1) % nraw];
        cum[i + 1] = cum[i] + vlen(vsub(b, a));
    }
    L = cum[nraw];
    n = (int)(L / TRACK_SPACING + 0.5f);
    if (n > TRACK_MAX_PTS) n = TRACK_MAX_PTS;
    tr->n = n;
    tr->length = L;
    tr->ds = L / n;
    k = 0;
    for (i = 0; i < n; i++) {
        float s = i * tr->ds;
        while (k < nraw - 1 && cum[k + 1] < s) k++;
        {
            float seg = cum[k + 1] - cum[k];
            float t = seg > 1e-5f ? (s - cum[k]) / seg : 0;
            tr->p[i] = vlerp(raw[k], raw[(k + 1) % nraw], t);
        }
    }
    for (i = 0; i < n; i++) {
        V3 a = tr->p[(i - 1 + n) % n], b = tr->p[(i + 1) % n];
        tr->t[i] = vnorm(vsub(b, a));
        tr->r[i] = vnorm(v3(-tr->t[i].z, 0, tr->t[i].x));
    }
    for (i = 0; i < n; i++) {
        V3 t0 = tr->t[i], t1 = tr->t[(i + 1) % n];
        float sn = vdot(v3(t1.x, 0, t1.z), tr->r[i]);
        tr->curv[i] = asinf(clampf(sn, -1, 1)) / tr->ds;
        (void)t0;
    }
    /* suavizado de curvatura */
    {
        static float tmp[TRACK_MAX_PTS];
        int pass;
        for (pass = 0; pass < 3; pass++) {
            for (i = 0; i < n; i++)
                tmp[i] = (tr->curv[(i - 2 + n) % n] + tr->curv[(i - 1 + n) % n] + tr->curv[i] +
                          tr->curv[(i + 1) % n] + tr->curv[(i + 2) % n]) / 5.0f;
            memcpy(tr->curv, tmp, sizeof(float) * n);
        }
    }
    {
        V3 mn = v3(1e9f, 0, 1e9f), mx = v3(-1e9f, 0, -1e9f);
        for (i = 0; i < n; i++) {
            if (tr->p[i].x < mn.x) mn.x = tr->p[i].x;
            if (tr->p[i].z < mn.z) mn.z = tr->p[i].z;
            if (tr->p[i].x > mx.x) mx.x = tr->p[i].x;
            if (tr->p[i].z > mx.z) mx.z = tr->p[i].z;
        }
        tr->center = v3((mn.x + mx.x) * 0.5f, 0, (mn.z + mx.z) * 0.5f);
        tr->extent = 0.5f * vlen(vsub(mx, mn));
    }
}

/* ---------- consultas ---------- */

float track_wrap_s(const Track *tr, float s) {
    while (s < 0) s += tr->length;
    while (s >= tr->length) s -= tr->length;
    return s;
}

float track_heading(V3 tan) { return atan2f(tan.x, -tan.z); }

V3 track_point(const Track *tr, float s, float d) {
    float fs, t;
    int i, j;
    V3 c, r;
    s = track_wrap_s(tr, s);
    fs = s / tr->ds;
    i = (int)fs; t = fs - i;
    if (i >= tr->n) i = tr->n - 1;
    j = (i + 1) % tr->n;
    c = vlerp(tr->p[i], tr->p[j], t);
    r = vnorm(vlerp(tr->r[i], tr->r[j], t));
    return vadd(c, vscale(r, d));
}

V3 track_tangent(const Track *tr, float s) {
    float fs, t;
    int i, j;
    s = track_wrap_s(tr, s);
    fs = s / tr->ds;
    i = (int)fs; t = fs - i;
    if (i >= tr->n) i = tr->n - 1;
    j = (i + 1) % tr->n;
    return vnorm(vlerp(tr->t[i], tr->t[j], t));
}

TrackPos track_locate(const Track *tr, V3 pos, int hint) {
    TrackPos tp;
    int i, best = 0, k, a, b, lo, hi;
    float bd = 1e18f;
    if (hint < 0) { lo = 0; hi = tr->n - 1; }
    else { lo = hint - 30; hi = hint + 30; }
    for (k = lo; k <= hi; k++) {
        V3 dv;
        float d2;
        i = (k % tr->n + tr->n) % tr->n;
        dv = vsub(pos, tr->p[i]);
        d2 = dv.x * dv.x + dv.z * dv.z + dv.y * dv.y * 4.0f;
        if (d2 < bd) { bd = d2; best = i; }
    }
    /* proyectar en el segmento vecino adecuado */
    {
        V3 p0 = tr->p[best], p1 = tr->p[(best + 1) % tr->n];
        V3 seg = vsub(p1, p0), rel = vsub(pos, p0);
        float l2 = seg.x * seg.x + seg.z * seg.z;
        float t = l2 > 1e-6f ? (rel.x * seg.x + rel.z * seg.z) / l2 : 0;
        if (t < 0) {
            best = (best - 1 + tr->n) % tr->n;
            p0 = tr->p[best]; p1 = tr->p[(best + 1) % tr->n];
            seg = vsub(p1, p0); rel = vsub(pos, p0);
            l2 = seg.x * seg.x + seg.z * seg.z;
            t = l2 > 1e-6f ? (rel.x * seg.x + rel.z * seg.z) / l2 : 0;
        }
        t = clampf(t, 0, 1);
        a = best; b = (best + 1) % tr->n;
        tp.idx = a;
        tp.s = track_wrap_s(tr, (a + t) * tr->ds);
        {
            V3 c = vlerp(tr->p[a], tr->p[b], t);
            tp.tan = vnorm(vlerp(tr->t[a], tr->t[b], t));
            tp.right = vnorm(v3(-tp.tan.z, 0, tp.tan.x));
            tp.d = (pos.x - c.x) * tp.right.x + (pos.z - c.z) * tp.right.z;
            tp.h = c.y;
        }
    }
    return tp;
}

/* ---------- utilidades de construccion ---------- */

static Mesh *chunk_mesh(Track *tr, int idx) {
    int c = ((idx % tr->n) + tr->n) % tr->n / CHUNK_SEGS;
    if (c >= tr->nchunks) c = tr->nchunks - 1;
    return &tr->chunks[c].mesh;
}

/* punto a distancia lateral d y altura y relativa al centro */
static V3 at(const Track *tr, int i, float d, float dy) {
    V3 p = tr->p[i % tr->n];
    V3 r = tr->r[i % tr->n];
    return v3(p.x + r.x * d, p.y + dy, p.z + r.z * d);
}

/* distancia minima (XZ) desde un punto al centro de la pista */
static float min_dist_xz(const Track *tr, V3 q, int *out_idx) {
    int i, bi = 0;
    float bd = 1e18f;
    for (i = 0; i < tr->n; i += 2) {
        float dx = q.x - tr->p[i].x, dz = q.z - tr->p[i].z;
        float d = dx * dx + dz * dz;
        if (d < bd) { bd = d; bi = i; }
    }
    if (out_idx) *out_idx = bi;
    return sqrtf(bd);
}

/* ---------- geometria de la pista ---------- */

static void build_road(Track *tr, Col wallA, Col wallB, Col grass) {
    int i, n = tr->n;
    float hw = tr->hw, W = tr->wall;
    for (i = 0; i < n; i++) {
        int j = (i + 1) % n;
        Mesh *m = chunk_mesh(tr, i);
        int band = (i / 4) & 1;
        Col asphalt = band ? col(84, 84, 92) : col(92, 92, 100);
        Col shoulder = tr->tunnel[i] ? col(70, 66, 62) : grass;
        float y0 = 0.02f;

        /* asfalto */
        if (i == 0) {
            /* linea de meta a cuadros */
            int k, row;
            for (row = 0; row < 2; row++) {
                for (k = 0; k < 10; k++) {
                    float d0 = -hw + 2 * hw * k / 10.0f, d1 = -hw + 2 * hw * (k + 1) / 10.0f;
                    float f0 = row * 0.5f, f1 = f0 + 0.5f;
                    V3 a = vlerp(at(tr, i, d0, y0), at(tr, j, d0, y0), f0);
                    V3 b = vlerp(at(tr, i, d1, y0), at(tr, j, d1, y0), f0);
                    V3 c = vlerp(at(tr, i, d1, y0), at(tr, j, d1, y0), f1);
                    V3 d = vlerp(at(tr, i, d0, y0), at(tr, j, d0, y0), f1);
                    mesh_quad_flat(m, a, b, c, d, ((k + row) & 1) ? col(20, 20, 20) : col(240, 240, 240));
                }
            }
        } else {
            mesh_quad_flat(m, at(tr, i, -hw, y0), at(tr, i, hw, y0), at(tr, j, hw, y0), at(tr, j, -hw, y0), asphalt);
        }
        /* linea central discontinua y lineas de borde */
        if (((i / 3) & 1) == 0 && i > 1)
            mesh_quad_flat(m, at(tr, i, -0.18f, y0 + 0.02f), at(tr, i, 0.18f, y0 + 0.02f), at(tr, j, 0.18f, y0 + 0.02f), at(tr, j, -0.18f, y0 + 0.02f), col(235, 235, 220));
        mesh_quad_flat(m, at(tr, i, -hw + 0.3f, y0 + 0.02f), at(tr, i, -hw + 0.55f, y0 + 0.02f), at(tr, j, -hw + 0.55f, y0 + 0.02f), at(tr, j, -hw + 0.3f, y0 + 0.02f), col(240, 240, 240));
        mesh_quad_flat(m, at(tr, i, hw - 0.55f, y0 + 0.02f), at(tr, i, hw - 0.3f, y0 + 0.02f), at(tr, j, hw - 0.3f, y0 + 0.02f), at(tr, j, hw - 0.55f, y0 + 0.02f), col(240, 240, 240));

        /* pianos (rojo/blanco) y arcen */
        {
            Col kc = tr->tunnel[i] ? col(90, 90, 96) : ((i & 1) ? col(220, 30, 30) : col(245, 245, 245));
            float side;
            for (side = -1; side <= 1; side += 2) {
                float a0 = side * hw, a1 = side * (hw + 1.2f), a2 = side * W;
                mesh_quad_flat(m, at(tr, i, a0, y0 + 0.04f), at(tr, i, a1, y0 + 0.04f), at(tr, j, a1, y0 + 0.04f), at(tr, j, a0, y0 + 0.04f), kc);
                mesh_quad_flat(m, at(tr, i, a1, y0), at(tr, i, a2, y0), at(tr, j, a2, y0), at(tr, j, a1, y0), shoulder);
            }
        }

        /* muros o tunel */
        if (tr->tunnel[i]) {
            float H = 6.5f;
            Col rock = (i & 1) ? col(96, 86, 80) : col(104, 94, 86);
            mesh_quad(m, at(tr, i, -W, 0), at(tr, j, -W, 0), at(tr, j, -W, H), at(tr, i, -W, H), rock);
            mesh_quad(m, at(tr, i, W, 0), at(tr, j, W, 0), at(tr, j, W, H), at(tr, i, W, H), rock);
            mesh_quad_flat(m, at(tr, i, -W, H), at(tr, i, W, H), at(tr, j, W, H), at(tr, j, -W, H), col(58, 54, 52));
            if (i % 5 == 0) {
                mesh_quad_flat(m, at(tr, i, -0.6f, H - 0.05f), at(tr, i, 0.6f, H - 0.05f), at(tr, j, 0.6f, H - 0.05f), at(tr, j, -0.6f, H - 0.05f), col(255, 230, 140));
            }
            /* exterior de la montana */
            {
                float W1 = W + 0.5f, top = 7.2f, W2 = W1 + (tr->p[i].y + top) * 1.7f + 6.0f;
                float W2j = W1 + (tr->p[j].y + top) * 1.7f + 6.0f;
                Col hill = col(96, 126, 66), hill2 = col(104, 134, 72);
                V3 li = at(tr, i, -W1, top), lj = at(tr, j, -W1, top);
                V3 ri = at(tr, i, W1, top), rj = at(tr, j, W1, top);
                V3 gli = at(tr, i, -W2, 0), glj = at(tr, j, -W2j, 0);
                V3 gri = at(tr, i, W2, 0), grj = at(tr, j, W2j, 0);
                gli.y = glj.y = gri.y = grj.y = -0.2f;
                mesh_quad(m, li, ri, rj, lj, hill2);
                mesh_quad(m, gli, li, lj, glj, hill);
                mesh_quad(m, ri, gri, grj, rj, hill);
                /* portales */
                if (!tr->tunnel[(i - 1 + n) % n] || !tr->tunnel[j]) {
                    int k2 = tr->tunnel[(i - 1 + n) % n] ? j : i;
                    Col face = col(122, 112, 100);
                    float W2k = W1 + (tr->p[k2].y + top) * 1.7f + 6.0f;
                    V3 g0 = at(tr, k2, -W2k, 0), g1 = at(tr, k2, W2k, 0);
                    g0.y = g1.y = -0.2f;
                    mesh_quad(m, g0, at(tr, k2, -W, 0), at(tr, k2, -W, H), at(tr, k2, -W1, top), face);
                    mesh_quad(m, at(tr, k2, W, 0), g1, at(tr, k2, W1, top), at(tr, k2, W, H), face);
                    mesh_quad(m, at(tr, k2, -W, H), at(tr, k2, W, H), at(tr, k2, W1, top), at(tr, k2, -W1, top), face);
                }
            }
        } else {
            Col wc = ((i / 4) & 1) ? wallA : wallB;
            float side;
            for (side = -1; side <= 1; side += 2) {
                float a = side * W, b = side * (W + 0.5f), h = 1.1f;
                mesh_quad(m, at(tr, i, a, 0), at(tr, j, a, 0), at(tr, j, a, h), at(tr, i, a, h), wc);
                mesh_quad_flat(m, at(tr, i, a, h), at(tr, j, a, h), at(tr, j, b, h), at(tr, i, b, h), col_shade(wc, 1.1f));
                mesh_quad(m, at(tr, i, b, 0), at(tr, j, b, 0), at(tr, j, b, h), at(tr, i, b, h), col_shade(wc, 0.8f));
            }
        }

        /* puente o terraplen */
        if (tr->bridge[i]) {
            float b = W + 0.5f, under = -0.9f;
            Col concrete = col(150, 146, 140);
            mesh_quad_flat(m, at(tr, i, -b, under), at(tr, i, b, under), at(tr, j, b, under), at(tr, j, -b, under), col(80, 78, 76));
            mesh_quad(m, at(tr, i, -b, under), at(tr, j, -b, under), at(tr, j, -b, 0), at(tr, i, -b, 0), concrete);
            mesh_quad(m, at(tr, i, b, under), at(tr, j, b, under), at(tr, j, b, 0), at(tr, i, b, 0), concrete);
        } else if (!tr->tunnel[i] && (tr->p[i].y > 0.3f || tr->p[j].y > 0.3f)) {
            float side;
            for (side = -1; side <= 1; side += 2) {
                float b = side * (W + 0.5f);
                float ei = side * (W + 0.5f + 4.0f + tr->p[i].y * 2.2f);
                float ej = side * (W + 0.5f + 4.0f + tr->p[j].y * 2.2f);
                V3 gi = at(tr, i, ei, 0), gj = at(tr, j, ej, 0);
                gi.y = gj.y = -0.15f;
                mesh_quad(m, at(tr, i, b, 0), at(tr, j, b, 0), gj, gi, col_shade(grass, 0.95f));
            }
        }
    }

    /* pilares del puente */
    for (i = 0; i < n; i += 8) {
        if (tr->bridge[i] && tr->p[i].y > 3.0f) {
            int k, ok = 1;
            V3 base = v3(tr->p[i].x, 0, tr->p[i].z);
            for (k = 0; k < n; k++) {
                if (!tr->bridge[k]) {
                    float dx = base.x - tr->p[k].x, dz = base.z - tr->p[k].z;
                    if (dx * dx + dz * dz < (tr->wall + 4) * (tr->wall + 4)) { ok = 0; break; }
                }
            }
            if (ok) mesh_obox(chunk_mesh(tr, i), base, tr->r[i], v3(tr->t[i].x, 0, tr->t[i].z), 3.0f, 1.2f, tr->p[i].y - 0.9f, col(160, 156, 150), col(160, 156, 150));
        }
    }
}

/* ---------- escenografia ---------- */

static void tree_round(Mesh *m, V3 p, float s) {
    mesh_prism(m, p, 0.35f * s, 2.2f * s, 5, col(110, 76, 46), col(110, 76, 46));
    {
        V3 c = vadd(p, v3(0, 3.4f * s, 0));
        float r = 1.9f * s;
        Col g = col(60, 150, 60);
        V3 top = vadd(c, v3(0, r, 0)), bot = vadd(c, v3(0, -r * 0.8f, 0));
        int k;
        for (k = 0; k < 5; k++) {
            float a0 = 2 * (float)M_PI * k / 5, a1 = 2 * (float)M_PI * (k + 1) / 5;
            V3 e0 = vadd(c, v3(cosf(a0) * r, 0, sinf(a0) * r)), e1 = vadd(c, v3(cosf(a1) * r, 0, sinf(a1) * r));
            mesh_tri(m, e0, e1, top, g);
            mesh_tri(m, e1, e0, bot, col_shade(g, 0.8f));
        }
    }
}

static void tree_pine(Mesh *m, V3 p, float s) {
    mesh_prism(m, p, 0.3f * s, 1.5f * s, 4, col(100, 70, 44), col(100, 70, 44));
    mesh_cone(m, vadd(p, v3(0, 1.4f * s, 0)), 2.2f * s, 3.6f * s, 6, col(38, 108, 54));
    mesh_cone(m, vadd(p, v3(0, 3.6f * s, 0)), 1.6f * s, 3.2f * s, 6, col(46, 122, 60));
}

static void building(Mesh *m, Rng *rg, V3 p, float w, float d, float h, V3 face_dir) {
    static const int pal[][3] = {{220, 205, 170}, {170, 200, 225}, {235, 235, 230}, {205, 120, 90}, {150, 170, 160}, {230, 190, 120}};
    const int *c = pal[rng_u(rg) % 6];
    Col wall = col(c[0], c[1], c[2]);
    V3 f = vnorm(v3(face_dir.x, 0, face_dir.z));
    V3 r = v3(-f.z, 0, f.x);
    float y;
    mesh_obox(m, p, r, f, w * 0.5f, d * 0.5f, h, wall, col(110, 110, 115));
    /* franjas de ventanas en las cuatro caras */
    for (y = 4.0f; y < h - 2.0f; y += 5.0f) {
        Col win = col(60, 80, 110);
        int k;
        for (k = 0; k < 4; k++) {
            V3 nrm = k == 0 ? f : k == 1 ? vscale(f, -1) : k == 2 ? r : vscale(r, -1);
            V3 tg = v3(-nrm.z, 0, nrm.x);
            float half = (k < 2) ? w * 0.5f - 1.5f : d * 0.5f - 1.5f;
            float off = ((k < 2) ? d : w) * 0.5f + 0.06f;
            V3 c0 = vadd(p, vscale(nrm, off));
            V3 a = vadd(vadd(c0, vscale(tg, -half)), v3(0, y, 0));
            V3 b = vadd(vadd(c0, vscale(tg, half)), v3(0, y, 0));
            mesh_quad_flat(m, a, b, vadd(b, v3(0, 1.6f, 0)), vadd(a, v3(0, 1.6f, 0)), win);
        }
    }
}

static int place_ok(const Track *tr, V3 q, float clearance) {
    return min_dist_xz(tr, q, NULL) > tr->wall + clearance;
}

static void scatter_trees(Track *tr, Rng *rg, int count, int pine, float dmin, float dmax) {
    int k, tries = 0;
    for (k = 0; k < count && tries < count * 20; tries++) {
        int i = rng_u(rg) % tr->n;
        float side = (rng_u(rg) & 1) ? 1.0f : -1.0f;
        float dd = rng_range(rg, dmin, dmax);
        V3 q = at(tr, i, side * (tr->wall + dd), 0);
        int ni;
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 5.0f) continue;
        if (pine) tree_pine(chunk_mesh(tr, ni), q, rng_range(rg, 0.9f, 1.6f));
        else tree_round(chunk_mesh(tr, ni), q, rng_range(rg, 0.9f, 1.4f));
        k++;
    }
}

static void lamp_posts(Track *tr, int every) {
    int i;
    for (i = 0; i < tr->n; i += every) {
        float side = ((i / every) & 1) ? 1.0f : -1.0f;
        V3 b = at(tr, i, side * (tr->wall + 1.6f), 0);
        Mesh *m = chunk_mesh(tr, i);
        if (tr->tunnel[i]) continue;
        mesh_obox(m, b, tr->r[i], tr->t[i], 0.18f, 0.18f, 7.0f, col(120, 120, 128), col(120, 120, 128));
        {
            V3 h = vadd(b, vscale(tr->r[i], -side * 1.2f));
            h.y += 7.0f;
            mesh_obox(m, h, tr->r[i], tr->t[i], 1.3f, 0.35f, 0.35f, col(90, 90, 96), col(255, 240, 170));
        }
    }
}

static void scenery_ocho(Track *tr) {
    Rng rg = {1234};
    int k, tries;
    /* tribuna junto a la meta */
    for (k = 0; k < 3; k++) {
        int i = tr->n - 8 + k * 6;
        int s;
        for (s = 0; s < 4; s++) {
            V3 b = at(tr, i, -(tr->wall + 3.0f + s * 2.5f), 0);
            Mesh *m = chunk_mesh(tr, i);
            mesh_obox(m, b, tr->r[i % tr->n], tr->t[i % tr->n], 1.25f, 9.0f, 1.5f + s * 1.6f, col(200, 200, 210), col(170, 40, 40));
            {
                int c;
                for (c = 0; c < 6; c++) {
                    static const int pc[][3] = {{250, 220, 60}, {60, 140, 240}, {240, 90, 90}, {250, 250, 250}, {80, 200, 120}};
                    const int *cc = pc[(c + s + k) % 5];
                    V3 q = vadd(b, vscale(tr->t[i % tr->n], -7.5f + c * 3.0f));
                    q.y += 1.5f + s * 1.6f;
                    mesh_obox(m, q, tr->r[i % tr->n], tr->t[i % tr->n], 0.5f, 0.9f, 0.9f, col(cc[0], cc[1], cc[2]), col(cc[0], cc[1], cc[2]));
                }
            }
        }
    }
    /* edificios alrededor */
    for (k = 0, tries = 0; k < 46 && tries < 900; tries++) {
        int i = rng_u(&rg) % tr->n;
        float side = (rng_u(&rg) & 1) ? 1.0f : -1.0f;
        float w = rng_range(&rg, 18, 34), d = rng_range(&rg, 18, 30), h = rng_range(&rg, 16, 64);
        V3 q = at(tr, i, side * (tr->wall + rng_range(&rg, 40, 140)), 0);
        int ni;
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 12 + (w > d ? w : d) * 0.75f) continue;
        building(chunk_mesh(tr, ni), &rg, q, w, d, h, tr->r[ni]);
        k++;
    }
    scatter_trees(tr, &rg, 90, 0, 6, 34);
    lamp_posts(tr, 14);
}

static void fortress(Track *tr) {
    Mesh *m;
    int ci;
    V3 c = tr->center;
    float dmin = min_dist_xz(tr, c, &ci);
    float S = clampf(dmin - tr->wall - 18.0f, 25.0f, 62.0f) * 0.75f;
    Col stone = col(158, 156, 166), stone2 = col(136, 134, 146), roof = col(46, 70, 150);
    int k;
    c.y = 0;
    m = chunk_mesh(tr, ci);
    /* murallas */
    mesh_box(m, v3(c.x - S, 0, c.z - S), v3(c.x + S, 9, c.z - S + 2.5f), stone, stone2);
    mesh_box(m, v3(c.x - S, 0, c.z + S - 2.5f), v3(c.x + S, 9, c.z + S), stone, stone2);
    mesh_box(m, v3(c.x - S, 0, c.z - S), v3(c.x - S + 2.5f, 9, c.z + S), stone, stone2);
    mesh_box(m, v3(c.x + S - 2.5f, 0, c.z - S), v3(c.x + S, 9, c.z + S), stone, stone2);
    /* almenas */
    for (k = 0; k < 10; k++) {
        float t = -S + 3 + (2 * S - 6) * k / 9.0f;
        mesh_box(m, v3(c.x + t - 0.9f, 9, c.z - S), v3(c.x + t + 0.9f, 10.6f, c.z - S + 2.5f), stone, stone2);
        mesh_box(m, v3(c.x + t - 0.9f, 9, c.z + S - 2.5f), v3(c.x + t + 0.9f, 10.6f, c.z + S), stone, stone2);
        mesh_box(m, v3(c.x - S, 9, c.z + t - 0.9f), v3(c.x - S + 2.5f, 10.6f, c.z + t + 0.9f), stone, stone2);
        mesh_box(m, v3(c.x + S - 2.5f, 9, c.z + t - 0.9f), v3(c.x + S, 10.6f, c.z + t + 0.9f), stone, stone2);
    }
    /* puerta */
    mesh_box(m, v3(c.x - 4, 0, c.z - S - 0.1f), v3(c.x + 4, 7, c.z - S + 0.2f), col(60, 45, 35), col(60, 45, 35));
    /* torres de esquina */
    for (k = 0; k < 4; k++) {
        V3 b = v3(c.x + ((k & 1) ? S : -S), 0, c.z + ((k & 2) ? S : -S));
        mesh_prism(m, b, 5.0f, 16.0f, 8, stone, stone2);
        mesh_cone(m, vadd(b, v3(0, 16, 0)), 6.2f, 10.0f, 8, roof);
        mesh_tri(m, vadd(b, v3(0, 26, 0)), vadd(b, v3(0, 29, 0)), vadd(b, v3(3, 28, 0)), col(230, 200, 40));
    }
    /* torre del homenaje */
    {
        float K = S * 0.38f;
        mesh_box(m, v3(c.x - K, 0, c.z - K), v3(c.x + K, 26, c.z + K), col(176, 174, 184), stone2);
        for (k = 0; k < 4; k++) {
            V3 b = v3(c.x + ((k & 1) ? K : -K), 0, c.z + ((k & 2) ? K : -K));
            mesh_prism(m, b, 2.8f, 30.0f, 8, stone, stone2);
            mesh_cone(m, vadd(b, v3(0, 30, 0)), 3.6f, 7.0f, 8, roof);
        }
        mesh_cone(m, v3(c.x, 26, c.z), K * 1.05f, 16.0f, 4, roof);
        mesh_tri(m, v3(c.x, 42, c.z), v3(c.x, 46, c.z), v3(c.x + 4, 44.5f, c.z), col(230, 200, 40));
    }
}

static void scenery_fortaleza(Track *tr) {
    Rng rg = {777};
    int k, i;
    fortress(tr);
    /* setos y parterres de flores en el exterior */
    for (i = 0; i < tr->n; i += 10) {
        Mesh *m = chunk_mesh(tr, i);
        float side = ((i / 10) % 3 == 0) ? -1.0f : 1.0f;
        V3 b = at(tr, i, side * (tr->wall + 4.0f), 0);
        V3 q = b; q.y = 0;
        if (min_dist_xz(tr, q, NULL) < tr->wall + 2.5f) continue;
        if ((i / 10) & 1) {
            mesh_obox(m, q, tr->r[i], tr->t[i], 1.0f, 7.0f, 1.6f, col(44, 112, 52), col(56, 130, 60));
        } else {
            static const int fl[][3] = {{235, 60, 70}, {250, 215, 60}, {245, 245, 245}, {170, 90, 210}};
            int f;
            mesh_obox(m, q, tr->r[i], tr->t[i], 1.6f, 7.0f, 0.3f, col(120, 84, 56), col(120, 84, 56));
            for (f = 0; f < 6; f++) {
                const int *cc = fl[(f + i) & 3];
                V3 fp = vadd(q, vscale(tr->t[i], -5.5f + f * 2.2f));
                fp.y = 0.3f;
                mesh_obox(m, fp, tr->r[i], tr->t[i], 1.1f, 0.8f, 0.5f, col(cc[0], cc[1], cc[2]), col(cc[0], cc[1], cc[2]));
            }
        }
    }
    /* fuente cerca de la meta */
    {
        V3 q = at(tr, 6, -(tr->wall + 14.0f), 0);
        Mesh *m = chunk_mesh(tr, 6);
        q.y = 0;
        mesh_prism(m, q, 6.0f, 0.9f, 10, col(200, 196, 190), col(80, 150, 230));
        mesh_prism(m, q, 1.0f, 3.5f, 6, col(200, 196, 190), col(120, 190, 250));
    }
    for (k = 0; k < 3; k++) { (void)k; }
    scatter_trees(tr, &rg, 120, 0, 8, 60);
}

static void barn(Mesh *m, V3 p, V3 f) {
    V3 r = v3(-f.z, 0, f.x);
    float w = 9, l = 14, h = 7;
    Col red = col(170, 40, 34), roof = col(70, 60, 60);
    mesh_obox(m, p, r, f, w, l, h, red, red);
    {
        V3 a0 = vadd(vadd(p, vscale(r, -w - 0.8f)), vscale(f, -l - 0.5f)); a0.y = h;
        V3 a1 = vadd(vadd(p, vscale(r, -w - 0.8f)), vscale(f, l + 0.5f)); a1.y = h;
        V3 b0 = vadd(vadd(p, vscale(r, w + 0.8f)), vscale(f, -l - 0.5f)); b0.y = h;
        V3 b1 = vadd(vadd(p, vscale(r, w + 0.8f)), vscale(f, l + 0.5f)); b1.y = h;
        V3 t0 = vadd(p, vscale(f, -l - 0.5f)); t0.y = h + 6;
        V3 t1 = vadd(p, vscale(f, l + 0.5f)); t1.y = h + 6;
        mesh_quad(m, a0, a1, t1, t0, roof);
        mesh_quad(m, b0, t0, t1, b1, roof);
        mesh_tri(m, vadd(a0, v3(0, 0, 0)), b0, t0, red);
        mesh_tri(m, a1, b1, t1, red);
    }
    {
        V3 s = vadd(p, vscale(r, w + 6));
        mesh_prism(m, s, 3.5f, 16, 8, col(190, 190, 196), col(190, 190, 196));
        mesh_cone(m, vadd(s, v3(0, 16, 0)), 3.8f, 3.0f, 8, col(120, 124, 130));
    }
}

static void scenery_valle(Track *tr) {
    Rng rg = {4242};
    int k, tries;
    /* lago en el interior */
    {
        V3 c = tr->center;
        int ci;
        float dmin = min_dist_xz(tr, c, &ci);
        float R = clampf(dmin - tr->wall - 20, 20, 90);
        Mesh *m = chunk_mesh(tr, ci);
        for (k = 0; k < 14; k++) {
            float a0 = 2 * (float)M_PI * k / 14, a1 = 2 * (float)M_PI * (k + 1) / 14;
            float r0 = R * (0.8f + 0.2f * sinf(k * 1.7f)), r1 = R * (0.8f + 0.2f * sinf((k + 1) * 1.7f));
            mesh_tri_flat(m, v3(c.x, 0.08f, c.z), v3(c.x + cosf(a0) * r0, 0.08f, c.z + sinf(a0) * r0),
                          v3(c.x + cosf(a1) * r1, 0.08f, c.z + sinf(a1) * r1), col(60, 120, 200));
        }
    }
    /* granero */
    {
        int i = tr->n / 8;
        V3 q = at(tr, i, tr->wall + 34, 0);
        q.y = 0;
        if (place_ok(tr, q, 26)) barn(chunk_mesh(tr, i), q, tr->t[i]);
    }
    /* colinas lejanas */
    for (k = 0, tries = 0; k < 18 && tries < 400; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        float side = (rng_u(&rg) & 1) ? 1.0f : -1.0f;
        float R = rng_range(&rg, 40, 90);
        V3 q = at(tr, i, side * (tr->wall + R + rng_range(&rg, 30, 120)), 0);
        q.y = -0.5f;
        if (min_dist_xz(tr, q, &ni) < tr->wall + R + 12) continue;
        mesh_cone(chunk_mesh(tr, ni), q, R, rng_range(&rg, 14, 34), 7, k & 1 ? col(92, 140, 66) : col(84, 128, 60));
        k++;
    }
    /* pacas de heno */
    for (k = 0, tries = 0; k < 24 && tries < 300; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 6, 20)), 0);
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 4) continue;
        mesh_prism(chunk_mesh(tr, ni), q, 1.1f, 1.6f, 6, col(220, 190, 90), col(235, 205, 110));
        k++;
    }
    scatter_trees(tr, &rg, 230, 1, 5, 70);
}

/* ---------- suelo y montanas ---------- */

static void build_world(Track *tr, Col g1, Col g2, Col mnt) {
    Mesh *m = &tr->world;
    int x, z, k;
    float T = 100.0f, R = tr->extent + 450.0f;
    int N = (int)(R / T) + 1;
    mesh_init(m);
    for (x = -N; x < N; x++) {
        for (z = -N; z < N; z++) {
            float x0 = tr->center.x + x * T, z0 = tr->center.z + z * T;
            Col c = ((x + z) & 1) ? g1 : g2;
            mesh_quad_flat(m, v3(x0, -0.1f, z0), v3(x0 + T, -0.1f, z0), v3(x0 + T, -0.1f, z0 + T), v3(x0, -0.1f, z0 + T), c);
        }
    }
    /* anillo de montanas */
    {
        Rng rg = {99};
        float MR = R + 180.0f;
        for (k = 0; k < 30; k++) {
            float a0 = 2 * (float)M_PI * k / 30 - 0.05f, a1 = 2 * (float)M_PI * (k + 1) / 30 + 0.05f;
            float am = (a0 + a1) * 0.5f + rng_range(&rg, -0.05f, 0.05f);
            float h = rng_range(&rg, 110, 260);
            V3 p0 = v3(tr->center.x + cosf(a0) * MR, -1, tr->center.z + sinf(a0) * MR);
            V3 p1 = v3(tr->center.x + cosf(a1) * MR, -1, tr->center.z + sinf(a1) * MR);
            V3 top = v3(tr->center.x + cosf(am) * (MR + 60), h, tr->center.z + sinf(am) * (MR + 60));
            Col c = col_shade(mnt, rng_range(&rg, 0.85f, 1.1f));
            mesh_tri_flat(m, p0, p1, top, c);
            if (h > 180) {
                V3 s0 = vlerp(top, p0, 0.22f), s1 = vlerp(top, p1, 0.22f);
                mesh_tri_flat(m, s0, s1, top, col(240, 240, 250));
            }
        }
    }
}

/* ---------- construccion principal ---------- */

void track_build_centerline(Track *tr, int id) {
    int i;
    memset(tr, 0, sizeof(Track));
    tr->id = id;
    tr->name = track_name(id);
    switch (id) {
    case TRACK_OCHO: raw_ocho(); tr->hw = 7.5f; break;
    case TRACK_FORTALEZA: raw_from_controls(FORT_CP, sizeof(FORT_CP) / sizeof(FORT_CP[0])); tr->hw = 7.0f; break;
    default: raw_from_controls(VALLE_CP, sizeof(VALLE_CP) / sizeof(VALLE_CP[0])); tr->hw = 7.5f; break;
    }
    tr->wall = tr->hw + 1.7f;
    finalize_centerline(tr);

    for (i = 0; i < tr->n; i++) {
        tr->bridge[i] = (id == TRACK_OCHO && tr->p[i].y > 1.2f);
        tr->tunnel[i] = 0;
    }
    if (id == TRACK_VALLE) {
        V3 tc = v3(440, 0, -400);  /* zona del tunel (cima de la colina) */
        for (i = 0; i < tr->n; i++) {
            float dx = tr->p[i].x - tc.x, dz = tr->p[i].z - tc.z;
            if (dx * dx + dz * dz < 105.0f * 105.0f) tr->tunnel[i] = 1;
        }
    }
}

void track_build(Track *tr, int id) {
    int i;
    Col grass;
    track_build_centerline(tr, id);

    tr->nchunks = (tr->n + CHUNK_SEGS - 1) / CHUNK_SEGS;
    if (tr->nchunks > TRACK_MAX_CHUNKS) tr->nchunks = TRACK_MAX_CHUNKS;
    for (i = 0; i < tr->nchunks; i++) mesh_init(&tr->chunks[i].mesh);

    switch (id) {
    case TRACK_OCHO:
        grass = col(92, 150, 74);
        tr->sky_top = col(40, 92, 196); tr->sky_horizon = col(168, 206, 240); tr->fog = tr->sky_horizon;
        build_road(tr, col(40, 90, 200), col(235, 235, 240), grass);
        scenery_ocho(tr);
        build_world(tr, col(92, 150, 74), col(86, 142, 70), col(110, 120, 170));
        break;
    case TRACK_FORTALEZA:
        grass = col(100, 172, 82);
        tr->sky_top = col(70, 134, 224); tr->sky_horizon = col(200, 226, 250); tr->fog = tr->sky_horizon;
        build_road(tr, col(170, 166, 156), col(150, 146, 138), grass);
        scenery_fortaleza(tr);
        build_world(tr, col(100, 172, 82), col(94, 164, 78), col(120, 140, 180));
        break;
    default:
        grass = col(112, 160, 70);
        tr->sky_top = col(70, 84, 170); tr->sky_horizon = col(244, 184, 128); tr->fog = col(236, 190, 146);
        build_road(tr, col(240, 200, 40), col(40, 40, 44), grass);
        scenery_valle(tr);
        build_world(tr, col(112, 160, 70), col(104, 152, 66), col(130, 100, 130));
        break;
    }

    for (i = 0; i < tr->nchunks; i++) {
        Mesh *m = &tr->chunks[i].mesh;
        V3 mn = m->bmin, mx = m->bmax;
        tr->chunks[i].c = vscale(vadd(mn, mx), 0.5f);
        tr->chunks[i].r = 0.5f * vlen(vsub(mx, mn));
        tr->chunks[i].gpu = NULL;
    }
    tr->world_gpu = NULL;
}

void track_free_meshes(Track *tr) {
    int i;
    for (i = 0; i < tr->nchunks; i++) mesh_free(&tr->chunks[i].mesh);
    mesh_free(&tr->world);
}
