/* cars.c - especificaciones y modelado de autos originales (estilo low-poly) */
#include "cars.h"

const CarSpec CAR_SPECS[CAR_COUNT] = {
    /* name, desc, spd, acc, hnd, vmax, accel, turn, drift, mass, power, grip, drive, wf, shape, paint, accent */
    {"FURIA R",   "SUPERDEPORTIVO EN CUNA",  0.85f, 0.70f, 0.65f, 71.0f, 9.0f, 1.00f, 1.00f, 1250, 265000, 1.08f, 0, 0.44f, 0, {214, 32, 36, 255},  {250, 250, 250, 255}},
    {"COMETA GT", "COUPE EQUILIBRADO",       0.72f, 0.82f, 0.85f, 67.0f, 10.0f, 1.10f, 1.05f, 1180, 225000, 1.12f, 0, 0.50f, 1, {28, 96, 214, 255},  {245, 245, 250, 255}},
    {"TORMENTA 4X","COMPACTO DE RALLY",      0.60f, 0.95f, 0.95f, 63.0f, 11.0f, 1.18f, 1.20f, 1100, 205000, 1.16f, 2, 0.58f, 2, {246, 196, 26, 255}, {30, 30, 36, 255}},
    {"BRISA V12", "GRAN TURISMO DE RECTA",   0.98f, 0.55f, 0.50f, 75.0f, 8.0f, 0.92f, 0.90f, 1420, 330000, 1.04f, 0, 0.52f, 3, {120, 48, 168, 255}, {200, 200, 210, 255}},
};

const Col CAR_ALT_PAINTS[6] = {
    {30, 160, 90, 255}, {240, 120, 30, 255}, {230, 230, 235, 255},
    {40, 40, 48, 255},  {60, 190, 220, 255}, {200, 40, 120, 255}
};

typedef struct { float z, ws, yb, ys, yt; } Sec;

static void section_pts(const Sec *s, V3 out[6]) {
    float wb = s->ws * 0.92f;
    out[0] = v3(-wb, s->yb, s->z);
    out[1] = v3(wb, s->yb, s->z);
    out[2] = v3(s->ws, s->ys, s->z);
    out[3] = v3(s->ws * 0.82f, s->yt, s->z);
    out[4] = v3(-s->ws * 0.82f, s->yt, s->z);
    out[5] = v3(-s->ws, s->ys, s->z);
}

static void body(Mesh *m, const Sec *sec, int ns, Col paint, Col dark) {
    int i, k;
    for (i = 0; i < ns - 1; i++) {
        V3 a[6], b[6];
        section_pts(&sec[i], a);
        section_pts(&sec[i + 1], b);
        for (k = 0; k < 6; k++) {
            int k2 = (k + 1) % 6;
            Col c = (k == 0) ? dark : paint;
            mesh_quad(m, a[k], b[k], b[k2], a[k2], c);
        }
    }
    /* tapas delantera y trasera */
    {
        V3 a[6], b[6];
        section_pts(&sec[0], a);
        section_pts(&sec[ns - 1], b);
        for (k = 1; k < 5; k++) {
            mesh_tri(m, a[0], a[k], a[k + 1], paint);
            mesh_tri(m, b[0], b[k + 1], b[k], paint);
        }
    }
}

static void cabin(Mesh *m, float z0, float z1, float ybase, float yroof, float wbase, float wtop,
                  float rake_f, float rake_r, Col paint, Col glass) {
    V3 b0 = v3(-wbase, ybase, z0), b1 = v3(wbase, ybase, z0), b2 = v3(wbase, ybase, z1), b3 = v3(-wbase, ybase, z1);
    V3 t0 = v3(-wtop, yroof, z0 + rake_f), t1 = v3(wtop, yroof, z0 + rake_f);
    V3 t2 = v3(wtop, yroof, z1 - rake_r), t3 = v3(-wtop, yroof, z1 - rake_r);
    mesh_quad(m, b0, b1, t1, t0, glass);          /* parabrisas */
    mesh_quad(m, b2, b3, t3, t2, glass);          /* luneta */
    mesh_quad(m, b1, b2, t2, t1, glass);          /* lateral derecho */
    mesh_quad(m, b3, b0, t0, t3, glass);          /* lateral izquierdo */
    mesh_quad(m, t0, t1, t2, t3, paint);          /* techo */
    /* pilares */
    {
        V3 mid0 = vlerp(b1, b2, 0.5f), mid1 = vlerp(t1, t2, 0.5f);
        V3 n0 = vlerp(b0, b3, 0.5f), n1 = vlerp(t0, t3, 0.5f);
        mesh_quad(m, vadd(mid0, v3(0.01f, 0, -0.07f)), vadd(mid0, v3(0.01f, 0, 0.07f)), vadd(mid1, v3(0.01f, 0, 0.07f)), vadd(mid1, v3(0.01f, 0, -0.07f)), paint);
        mesh_quad(m, vadd(n0, v3(-0.01f, 0, -0.07f)), vadd(n0, v3(-0.01f, 0, 0.07f)), vadd(n1, v3(-0.01f, 0, 0.07f)), vadd(n1, v3(-0.01f, 0, -0.07f)), paint);
    }
}

static void wheel(Mesh *m, float x, float z, float r, float w) {
    int k, S = 8;
    float side = x > 0 ? 1.0f : -1.0f;
    float xi = x - side * w * 0.5f, xo = x + side * w * 0.5f;
    Col tire = col(26, 26, 28), hub = col(180, 180, 188);
    for (k = 0; k < S; k++) {
        float a0 = 2 * (float)M_PI * k / S, a1 = 2 * (float)M_PI * (k + 1) / S;
        V3 p0 = v3(xi, r + sinf(a0) * r, z + cosf(a0) * r), p1 = v3(xi, r + sinf(a1) * r, z + cosf(a1) * r);
        V3 q0 = v3(xo, p0.y, p0.z), q1 = v3(xo, p1.y, p1.z);
        mesh_quad(m, p0, p1, q1, q0, tire);
        mesh_tri(m, v3(xo, r, z), q0, q1, (k & 1) ? hub : col_shade(hub, 0.8f));
    }
}

static void lights(Mesh *m, float zf, float zr, float y, float wx, float yr) {
    Col head = col(255, 246, 200), tail = col(230, 20, 20);
    mesh_quad_flat(m, v3(-wx, y, zf - 0.01f), v3(-wx + 0.38f, y, zf - 0.01f), v3(-wx + 0.38f, y + 0.12f, zf - 0.01f), v3(-wx, y + 0.12f, zf - 0.01f), head);
    mesh_quad_flat(m, v3(wx - 0.38f, y, zf - 0.01f), v3(wx, y, zf - 0.01f), v3(wx, y + 0.12f, zf - 0.01f), v3(wx - 0.38f, y + 0.12f, zf - 0.01f), head);
    mesh_quad_flat(m, v3(-wx, yr, zr + 0.01f), v3(-wx + 0.45f, yr, zr + 0.01f), v3(-wx + 0.45f, yr + 0.14f, zr + 0.01f), v3(-wx, yr + 0.14f, zr + 0.01f), tail);
    mesh_quad_flat(m, v3(wx - 0.45f, yr, zr + 0.01f), v3(wx, yr, zr + 0.01f), v3(wx, yr + 0.14f, zr + 0.01f), v3(wx - 0.45f, yr + 0.14f, zr + 0.01f), tail);
}

static void stripe(Mesh *m, const Sec *sec, int ns, float hw, Col c) {
    int i;
    for (i = 0; i < ns - 1; i++) {
        float y0 = sec[i].yt + 0.012f, y1 = sec[i + 1].yt + 0.012f;
        mesh_quad_flat(m, v3(-hw, y0, sec[i].z), v3(hw, y0, sec[i].z), v3(hw, y1, sec[i + 1].z), v3(-hw, y1, sec[i + 1].z), c);
    }
}

void car_build_mesh(Mesh *m, int shape, Col paint, Col accent) {
    Col dark = col(30, 30, 34), glass = col(36, 52, 80);
    mesh_init(m);
    switch (shape) {
    case 0: { /* cuna superdeportiva */
        Sec s[] = {{-2.25f, 0.86f, 0.22f, 0.42f, 0.50f}, {-1.40f, 0.98f, 0.20f, 0.52f, 0.74f},
                   {0.20f, 1.00f, 0.20f, 0.62f, 0.90f}, {1.60f, 1.00f, 0.22f, 0.66f, 0.98f},
                   {2.22f, 0.94f, 0.30f, 0.66f, 0.94f}};
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.16f, accent);
        cabin(m, -0.75f, 1.05f, 0.88f, 1.24f, 0.80f, 0.62f, 0.75f, 0.35f, paint, glass);
        /* aleron bajo */
        mesh_box(m, v3(-0.9f, 0.98f, 1.95f), v3(0.9f, 1.06f, 2.25f), dark, accent);
        wheel(m, -0.86f, -1.35f, 0.34f, 0.28f); wheel(m, 0.86f, -1.35f, 0.34f, 0.28f);
        wheel(m, -0.88f, 1.30f, 0.36f, 0.32f); wheel(m, 0.88f, 1.30f, 0.36f, 0.32f);
        lights(m, -2.25f, 2.22f, 0.36f, 0.72f, 0.62f);
        break;
    }
    case 1: { /* coupe GT */
        Sec s[] = {{-2.20f, 0.84f, 0.24f, 0.48f, 0.62f}, {-1.20f, 0.95f, 0.22f, 0.60f, 0.86f},
                   {0.40f, 0.96f, 0.22f, 0.66f, 0.92f}, {1.70f, 0.95f, 0.24f, 0.70f, 0.98f},
                   {2.18f, 0.90f, 0.32f, 0.70f, 0.96f}};
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.12f, accent);
        cabin(m, -0.35f, 1.35f, 0.90f, 1.36f, 0.78f, 0.62f, 0.62f, 0.55f, paint, glass);
        mesh_box(m, v3(-0.86f, 1.16f, 1.86f), v3(0.86f, 1.22f, 2.18f), accent, accent);
        mesh_box(m, v3(-0.6f, 0.96f, 1.98f), v3(-0.5f, 1.16f, 2.08f), dark, dark);
        mesh_box(m, v3(0.5f, 0.96f, 1.98f), v3(0.6f, 1.16f, 2.08f), dark, dark);
        wheel(m, -0.84f, -1.30f, 0.34f, 0.28f); wheel(m, 0.84f, -1.30f, 0.34f, 0.28f);
        wheel(m, -0.84f, 1.30f, 0.34f, 0.30f); wheel(m, 0.84f, 1.30f, 0.34f, 0.30f);
        lights(m, -2.20f, 2.18f, 0.42f, 0.70f, 0.66f);
        break;
    }
    case 2: { /* compacto de rally */
        Sec s[] = {{-2.00f, 0.86f, 0.30f, 0.56f, 0.74f}, {-1.20f, 0.92f, 0.28f, 0.68f, 0.92f},
                   {0.20f, 0.92f, 0.28f, 0.72f, 0.96f}, {1.55f, 0.92f, 0.30f, 0.74f, 1.00f},
                   {1.98f, 0.88f, 0.36f, 0.74f, 0.98f}};
        body(m, s, 5, paint, dark);
        stripe(m, s, 5, 0.22f, accent);
        cabin(m, -0.70f, 1.55f, 0.94f, 1.52f, 0.80f, 0.68f, 0.55f, 0.20f, paint, glass);
        /* toma de aire en el techo y gran aleron */
        mesh_box(m, v3(-0.22f, 1.52f, -0.05f), v3(0.22f, 1.64f, 0.45f), accent, accent);
        mesh_box(m, v3(-0.95f, 1.38f, 1.62f), v3(0.95f, 1.46f, 2.02f), accent, accent);
        mesh_box(m, v3(-0.75f, 1.00f, 1.75f), v3(-0.65f, 1.38f, 1.88f), dark, dark);
        mesh_box(m, v3(0.65f, 1.00f, 1.75f), v3(0.75f, 1.38f, 1.88f), dark, dark);
        /* faros auxiliares */
        mesh_box(m, v3(-0.55f, 0.60f, -2.08f), v3(-0.25f, 0.80f, -2.00f), col(255, 250, 210), dark);
        mesh_box(m, v3(0.25f, 0.60f, -2.08f), v3(0.55f, 0.80f, -2.00f), col(255, 250, 210), dark);
        wheel(m, -0.86f, -1.25f, 0.36f, 0.28f); wheel(m, 0.86f, -1.25f, 0.36f, 0.28f);
        wheel(m, -0.86f, 1.22f, 0.36f, 0.28f); wheel(m, 0.86f, 1.22f, 0.36f, 0.28f);
        lights(m, -2.00f, 1.98f, 0.44f, 0.72f, 0.70f);
        break;
    }
    default: { /* gran turismo largo */
        Sec s[] = {{-2.45f, 0.86f, 0.22f, 0.44f, 0.56f}, {-1.30f, 0.97f, 0.20f, 0.58f, 0.80f},
                   {0.60f, 0.99f, 0.20f, 0.62f, 0.86f}, {1.85f, 0.98f, 0.22f, 0.64f, 0.90f},
                   {2.42f, 0.90f, 0.30f, 0.62f, 0.86f}};
        body(m, s, 5, paint, dark);
        cabin(m, 0.05f, 1.70f, 0.84f, 1.24f, 0.80f, 0.60f, 0.70f, 0.70f, paint, glass);
        /* franjas laterales */
        {
            int side;
            for (side = -1; side <= 1; side += 2) {
                float x = side * 1.003f;
                mesh_quad_flat(m, v3(x, 0.50f, -1.3f), v3(x, 0.50f, 1.85f), v3(x, 0.58f, 1.85f), v3(x, 0.58f, -1.3f), accent);
            }
        }
        wheel(m, -0.86f, -1.45f, 0.35f, 0.28f); wheel(m, 0.86f, -1.45f, 0.35f, 0.28f);
        wheel(m, -0.88f, 1.45f, 0.36f, 0.32f); wheel(m, 0.88f, 1.45f, 0.36f, 0.32f);
        lights(m, -2.45f, 2.42f, 0.36f, 0.72f, 0.58f);
        break;
    }
    }
    /* sombra bajo el auto */
    mesh_quad_flat(m, v3(-1.05f, 0.03f, -2.3f), v3(1.05f, 0.03f, -2.3f), v3(1.05f, 0.03f, 2.3f), v3(-1.05f, 0.03f, 2.3f), col(30, 30, 30));
}

void traffic_build_mesh(Mesh *m, int kind, Col paint) {
    Col dark = col(34, 34, 38), glass = col(40, 56, 80);
    mesh_init(m);
    if (kind == 0) { /* sedan */
        mesh_box(m, v3(-0.9f, 0.25f, -2.2f), v3(0.9f, 0.85f, 2.2f), paint, paint);
        cabin(m, -0.9f, 1.2f, 0.85f, 1.42f, 0.84f, 0.74f, 0.45f, 0.35f, paint, glass);
        wheel(m, -0.86f, -1.35f, 0.32f, 0.24f); wheel(m, 0.86f, -1.35f, 0.32f, 0.24f);
        wheel(m, -0.86f, 1.35f, 0.32f, 0.24f); wheel(m, 0.86f, 1.35f, 0.32f, 0.24f);
        lights(m, -2.2f, 2.2f, 0.55f, 0.78f, 0.6f);
    } else { /* furgoneta */
        mesh_box(m, v3(-1.0f, 0.3f, -2.6f), v3(1.0f, 2.3f, 2.6f), paint, col(220, 220, 225));
        mesh_quad_flat(m, v3(-0.9f, 1.3f, -2.61f), v3(0.9f, 1.3f, -2.61f), v3(0.9f, 2.1f, -2.61f), v3(-0.9f, 2.1f, -2.61f), glass);
        mesh_box(m, v3(-1.0f, 0.3f, -2.62f), v3(1.0f, 0.55f, -2.55f), dark, dark);
        wheel(m, -0.95f, -1.7f, 0.36f, 0.26f); wheel(m, 0.95f, -1.7f, 0.36f, 0.26f);
        wheel(m, -0.95f, 1.7f, 0.36f, 0.26f); wheel(m, 0.95f, 1.7f, 0.36f, 0.26f);
        lights(m, -2.6f, 2.6f, 0.65f, 0.85f, 0.7f);
    }
    mesh_quad_flat(m, v3(-1.05f, 0.03f, -2.6f), v3(1.05f, 0.03f, -2.6f), v3(1.05f, 0.03f, 2.6f), v3(-1.05f, 0.03f, 2.6f), col(30, 30, 30));
}
