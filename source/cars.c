/* cars.c - especificaciones y modelado de autos originales (estilo low-poly) */
#include "cars.h"

const CarSpec CAR_SPECS[CAR_COUNT] = {
    /* name, desc, spd, acc, hnd, vmax, accel, turn, drift, mass, power, grip, drive, wf, shape, paint, accent */
    {"FURIA R",   "SUPERDEPORTIVO EN CUNA",  0.85f, 0.70f, 0.65f, 71.0f, 9.0f, 1.00f, 1.00f, 1250, 265000, 1.08f, 0, 0.44f, 0, {214, 32, 36, 255},  {250, 250, 250, 255}},
    {"COMETA GT", "COUPE EQUILIBRADO",       0.72f, 0.82f, 0.85f, 67.0f, 10.0f, 1.10f, 1.05f, 1180, 225000, 1.12f, 0, 0.50f, 1, {28, 96, 214, 255},  {245, 245, 250, 255}},
    {"TORMENTA 4X","COMPACTO DE RALLY",      0.60f, 0.95f, 0.95f, 63.0f, 11.0f, 1.18f, 1.20f, 1100, 205000, 1.16f, 2, 0.58f, 2, {246, 196, 26, 255}, {30, 30, 36, 255}},
    {"BRISA V12", "GRAN TURISMO DE RECTA",   0.98f, 0.55f, 0.50f, 75.0f, 8.0f, 0.92f, 0.90f, 1420, 330000, 1.04f, 0, 0.52f, 3, {120, 48, 168, 255}, {200, 200, 210, 255}},
    /* disenado por el hermano del jugador */
    {"HAYASHI INCEPTION", "SEDAN DEPORTIVO 4 PUERTAS", 0.78f, 0.74f, 0.74f, 68.5f, 9.4f, 1.02f, 1.00f, 1530, 245000, 1.08f, 2, 0.57f, 4, {150, 26, 40, 255}, {200, 202, 210, 255}},
    /* dibujado por el jugador: superdeportivo turco */
    {"YILDIZ GT", "SUPERDEPORTIVO EN CUNA 80S", 0.93f, 0.76f, 0.60f, 74.0f, 9.6f, 0.98f, 1.06f, 1300, 300000, 1.07f, 0, 0.42f, 5, {206, 18, 46, 255}, {245, 245, 245, 255}},
    /* dibujado por el jugador: sedan cuadrado de los 80, traccion trasera y facil de derrapar */
    {"ATLAS 84", "SEDAN CLASICO DE LOS 80", 0.64f, 0.70f, 0.84f, 64.0f, 9.0f, 1.10f, 1.18f, 1250, 200000, 1.10f, 0, 0.52f, 6, {64, 94, 142, 255}, {196, 198, 204, 255}},
    /* dibujado por el jugador: sedan frances con motor V10 de competicion */
    {"LUTECE V10", "SEDAN CON MOTOR V10 DE CARRERAS", 1.00f, 0.92f, 0.55f, 78.0f, 11.0f, 0.95f, 1.02f, 1350, 380000, 1.08f, 0, 0.44f, 7, {28, 58, 150, 255}, {200, 206, 216, 255}},
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
    case 4: { /* sedan deportivo de 4 puertas (dibujo del hermano) */
        Sec s[] = {{-2.48f, 0.88f, 0.26f, 0.52f, 0.66f}, {-1.70f, 0.95f, 0.24f, 0.64f, 0.82f},
                   {0.30f, 0.96f, 0.24f, 0.70f, 0.88f}, {1.85f, 0.95f, 0.26f, 0.72f, 0.94f},
                   {2.45f, 0.90f, 0.34f, 0.72f, 0.92f}};
        Col chrome = col(205, 208, 216), grille = col(28, 28, 32);
        int side;
        body(m, s, 5, paint, dark);
        /* habitaculo largo de 4 puertas con luneta inclinada */
        cabin(m, -0.70f, 1.75f, 0.88f, 1.44f, 0.84f, 0.68f, 0.78f, 0.62f, paint, glass);
        /* parrilla ancha y faros finos */
        mesh_quad_flat(m, v3(-0.62f, 0.34f, -2.49f), v3(0.62f, 0.34f, -2.49f), v3(0.56f, 0.60f, -2.49f), v3(-0.56f, 0.60f, -2.49f), grille);
        mesh_quad_flat(m, v3(-0.60f, 0.46f, -2.50f), v3(0.60f, 0.46f, -2.50f), v3(0.60f, 0.50f, -2.50f), v3(-0.60f, 0.50f, -2.50f), chrome);
        mesh_quad_flat(m, v3(-0.80f, 0.56f, -2.47f), v3(-0.46f, 0.56f, -2.47f), v3(-0.50f, 0.64f, -2.42f), v3(-0.82f, 0.62f, -2.40f), col(255, 246, 210));
        mesh_quad_flat(m, v3(0.46f, 0.56f, -2.47f), v3(0.80f, 0.56f, -2.47f), v3(0.82f, 0.62f, -2.40f), v3(0.50f, 0.64f, -2.42f), col(255, 246, 210));
        /* tomas de aire bajas en la defensa */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.78f, 0.28f, -2.49f), v3(side * 0.48f, 0.28f, -2.49f), v3(side * 0.50f, 0.38f, -2.49f), v3(side * 0.80f, 0.38f, -2.49f), grille);
        /* lineas de puertas, manijas y molduras cromadas */
        for (side = -1; side <= 1; side += 2) {
            float x = side * 0.965f;
            float zs[3] = {-0.62f, 0.52f, 1.62f};
            int k;
            for (k = 0; k < 3; k++)
                mesh_quad_flat(m, v3(x, 0.32f, zs[k] - 0.015f), v3(x, 0.32f, zs[k] + 0.015f), v3(x, 0.86f, zs[k] + 0.015f), v3(x, 0.86f, zs[k] - 0.015f), dark);
            mesh_quad_flat(m, v3(x * 1.002f, 0.70f, -0.25f), v3(x * 1.002f, 0.70f, -0.08f), v3(x * 1.002f, 0.74f, -0.08f), v3(x * 1.002f, 0.74f, -0.25f), chrome);
            mesh_quad_flat(m, v3(x * 1.002f, 0.70f, 0.90f), v3(x * 1.002f, 0.70f, 1.07f), v3(x * 1.002f, 0.74f, 1.07f), v3(x * 1.002f, 0.74f, 0.90f), chrome);
            mesh_quad_flat(m, v3(x * 1.003f, 0.30f, -1.55f), v3(x * 1.003f, 0.30f, 1.75f), v3(x * 1.003f, 0.34f, 1.75f), v3(x * 1.003f, 0.34f, -1.55f), chrome);
        }
        /* pequeno aleron en la tapa del maletero */
        mesh_box(m, v3(-0.78f, 0.92f, 2.22f), v3(0.78f, 0.98f, 2.40f), paint, col_shade(paint, 1.15f));
        wheel(m, -0.88f, -1.52f, 0.35f, 0.28f); wheel(m, 0.88f, -1.52f, 0.35f, 0.28f);
        wheel(m, -0.88f, 1.55f, 0.35f, 0.28f); wheel(m, 0.88f, 1.55f, 0.35f, 0.28f);
        lights(m, -2.60f, 2.45f, 0.30f, 0.74f, 0.66f);
        break;
    }
    case 5: { /* cuna de los 80 (dibujo del jugador) */
        Sec s[] = {{-2.30f, 0.86f, 0.24f, 0.40f, 0.46f}, {-1.45f, 0.96f, 0.22f, 0.52f, 0.66f},
                   {0.10f, 0.99f, 0.22f, 0.62f, 0.82f}, {1.55f, 0.99f, 0.24f, 0.66f, 0.90f},
                   {2.25f, 0.96f, 0.30f, 0.66f, 0.88f}};
        Col black = col(22, 22, 26);
        int side, k;
        body(m, s, 5, paint, dark);
        /* cabina baja y adelantada, con luneta casi plana hacia la cola */
        cabin(m, -0.60f, 1.05f, 0.80f, 1.16f, 0.82f, 0.60f, 0.80f, 0.15f, paint, glass);
        mesh_quad(m, v3(-0.60f, 1.16f, 0.90f), v3(0.60f, 1.16f, 0.90f), v3(0.80f, 0.90f, 2.05f), v3(-0.80f, 0.90f, 2.05f), glass);
        /* persianas sobre la luneta trasera */
        for (k = 0; k < 6; k++) {
            float t0 = 0.08f + k * 0.15f, t1 = t0 + 0.06f;
            V3 a0 = vlerp(v3(-0.66f, 1.165f, 0.90f), v3(-0.82f, 0.905f, 2.05f), t0), b0 = vlerp(v3(0.66f, 1.165f, 0.90f), v3(0.82f, 0.905f, 2.05f), t0);
            V3 a1 = vlerp(v3(-0.66f, 1.165f, 0.90f), v3(-0.82f, 0.905f, 2.05f), t1), b1 = vlerp(v3(0.66f, 1.165f, 0.90f), v3(0.82f, 0.905f, 2.05f), t1);
            a0.y += 0.02f; b0.y += 0.02f; a1.y += 0.02f; b1.y += 0.02f;
            mesh_quad_flat(m, a0, b0, b1, a1, black);
        }
        /* faros escamoteables (cerrados) sobre el frente */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.30f, 0.555f, -1.95f), v3(side * 0.72f, 0.555f, -1.95f), v3(side * 0.72f, 0.60f, -1.65f), v3(side * 0.30f, 0.60f, -1.65f), col_shade(paint, 0.75f));
        /* franja blanca lateral y tomas de aire (branquias) detras de las puertas */
        for (side = -1; side <= 1; side += 2) {
            float x = side * 1.0f;
            mesh_quad_flat(m, v3(x * 1.004f, 0.40f, -2.1f), v3(x * 1.004f, 0.40f, 2.1f), v3(x * 1.004f, 0.46f, 2.1f), v3(x * 1.004f, 0.46f, -2.1f), accent);
            for (k = 0; k < 4; k++) {
                float y = 0.50f + k * 0.07f;
                mesh_quad_flat(m, v3(x * 1.005f, y, 0.85f), v3(x * 1.005f, y, 1.45f), v3(x * 1.005f, y + 0.035f, 1.45f), v3(x * 1.005f, y + 0.035f, 0.85f), black);
            }
            mesh_quad_flat(m, v3(x * 1.004f, 0.28f, -0.60f), v3(x * 1.004f, 0.28f, 0.80f), v3(x * 1.004f, 0.30f, 0.80f), v3(x * 1.004f, 0.30f, -0.60f), black);
        }
        /* trasera: banda de pilotos a lo ancho y panel negro */
        mesh_quad_flat(m, v3(-0.92f, 0.46f, 2.26f), v3(0.92f, 0.46f, 2.26f), v3(0.92f, 0.62f, 2.26f), v3(-0.92f, 0.62f, 2.26f), col(200, 20, 20));
        mesh_quad_flat(m, v3(-0.70f, 0.32f, 2.27f), v3(0.70f, 0.32f, 2.27f), v3(0.70f, 0.44f, 2.27f), v3(-0.70f, 0.44f, 2.27f), black);
        /* faros auxiliares en el paragolpes delantero */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.42f, 0.30f, -2.31f), v3(side * 0.78f, 0.30f, -2.31f), v3(side * 0.78f, 0.38f, -2.31f), v3(side * 0.42f, 0.38f, -2.31f), col(255, 246, 210));
        wheel(m, -0.88f, -1.40f, 0.34f, 0.28f); wheel(m, 0.88f, -1.40f, 0.34f, 0.28f);
        wheel(m, -0.90f, 1.40f, 0.36f, 0.32f); wheel(m, 0.90f, 1.40f, 0.36f, 0.32f);
        break;
    }
    case 6: { /* sedan cuadrado de los 80 (dibujo del jugador) */
        Sec s[] = {{-2.32f, 0.88f, 0.28f, 0.66f, 0.76f}, {-1.75f, 0.92f, 0.26f, 0.72f, 0.84f},
                   {0.30f, 0.93f, 0.26f, 0.74f, 0.86f}, {1.70f, 0.93f, 0.27f, 0.75f, 0.90f},
                   {2.30f, 0.90f, 0.30f, 0.75f, 0.90f}};
        Col black = col(30, 30, 34), chrome = col(196, 198, 206);
        int side, k;
        body(m, s, 5, paint, dark);
        /* habitaculo alto y recto, pilares finos */
        cabin(m, -0.85f, 1.45f, 0.86f, 1.48f, 0.86f, 0.80f, 0.55f, 0.40f, paint, glass);
        for (side = -1; side <= 1; side += 2) {
            float x = side * 0.865f;
            /* pilar entre la puerta trasera y la ventanilla de cuarto */
            mesh_quad_flat(m, v3(x, 0.88f, 1.05f), v3(x, 0.88f, 1.15f), v3(x * 0.95f, 1.46f, 1.02f), v3(x * 0.95f, 1.46f, 0.92f), paint);
            /* moldura lateral y lineas de puertas */
            mesh_quad_flat(m, v3(side * 0.935f, 0.52f, -2.2f), v3(side * 0.935f, 0.52f, 2.2f), v3(side * 0.935f, 0.58f, 2.2f), v3(side * 0.935f, 0.58f, -2.2f), black);
            for (k = 0; k < 3; k++) {
                float z = -0.85f + k * 1.15f;
                mesh_quad_flat(m, v3(side * 0.936f, 0.30f, z - 0.012f), v3(side * 0.936f, 0.30f, z + 0.012f), v3(side * 0.936f, 0.86f, z + 0.012f), v3(side * 0.936f, 0.86f, z - 0.012f), black);
            }
            mesh_quad_flat(m, v3(side * 0.937f, 0.70f, -0.62f), v3(side * 0.937f, 0.70f, -0.45f), v3(side * 0.937f, 0.74f, -0.45f), v3(side * 0.937f, 0.74f, -0.62f), chrome);
            mesh_quad_flat(m, v3(side * 0.937f, 0.70f, 0.52f), v3(side * 0.937f, 0.70f, 0.69f), v3(side * 0.937f, 0.74f, 0.69f), v3(side * 0.937f, 0.74f, 0.52f), chrome);
        }
        /* frente: faros rectangulares y parrilla de barras horizontales */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.42f, 0.48f, -2.33f), v3(side * 0.80f, 0.48f, -2.33f), v3(side * 0.80f, 0.66f, -2.33f), v3(side * 0.42f, 0.66f, -2.33f), col(255, 246, 214));
        mesh_quad_flat(m, v3(-0.40f, 0.48f, -2.33f), v3(0.40f, 0.48f, -2.33f), v3(0.40f, 0.66f, -2.33f), v3(-0.40f, 0.66f, -2.33f), black);
        for (k = 0; k < 3; k++) {
            float y = 0.51f + k * 0.05f;
            mesh_quad_flat(m, v3(-0.38f, y, -2.34f), v3(0.38f, y, -2.34f), v3(0.38f, y + 0.02f, -2.34f), v3(-0.38f, y + 0.02f, -2.34f), chrome);
        }
        /* defensas anchas oscuras */
        mesh_box(m, v3(-0.92f, 0.26f, -2.45f), v3(0.92f, 0.42f, -2.28f), black, black);
        mesh_box(m, v3(-0.92f, 0.28f, 2.26f), v3(0.92f, 0.44f, 2.42f), black, black);
        /* pilotos traseros grandes */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.30f, 0.50f, 2.31f), v3(side * 0.88f, 0.50f, 2.31f), v3(side * 0.88f, 0.72f, 2.31f), v3(side * 0.30f, 0.72f, 2.31f), col(200, 24, 24));
        mesh_quad_flat(m, v3(-0.28f, 0.52f, 2.31f), v3(0.28f, 0.52f, 2.31f), v3(0.28f, 0.70f, 2.31f), v3(-0.28f, 0.70f, 2.31f), black);
        wheel(m, -0.86f, -1.42f, 0.32f, 0.24f); wheel(m, 0.86f, -1.42f, 0.32f, 0.24f);
        wheel(m, -0.86f, 1.42f, 0.32f, 0.24f); wheel(m, 0.86f, 1.42f, 0.32f, 0.24f);
        break;
    }
    case 7: { /* sedan V10 (dibujo del jugador) */
        Sec s[] = {{-2.36f, 0.90f, 0.20f, 0.52f, 0.64f}, {-1.60f, 0.97f, 0.20f, 0.64f, 0.82f},
                   {0.30f, 0.98f, 0.20f, 0.70f, 0.88f}, {1.75f, 0.97f, 0.22f, 0.74f, 0.98f},
                   {2.30f, 0.92f, 0.30f, 0.74f, 0.98f}};
        Col black = col(22, 22, 26);
        int side, k;
        body(m, s, 5, paint, dark);
        /* techo que cae suave hasta el maletero alto */
        cabin(m, -0.75f, 1.80f, 0.86f, 1.40f, 0.84f, 0.66f, 0.85f, 0.85f, paint, glass);
        /* retrovisores */
        for (side = -1; side <= 1; side += 2)
            mesh_box(m, v3(side * 0.86f - (side > 0 ? 0 : 0.22f), 0.92f, -0.62f), v3(side * 0.86f + (side > 0 ? 0.22f : 0), 1.04f, -0.48f), paint, paint);
        /* frente de competicion: parrilla baja ancha, grandes tomas y labio */
        mesh_quad_flat(m, v3(-0.70f, 0.24f, -2.37f), v3(0.70f, 0.24f, -2.37f), v3(0.62f, 0.44f, -2.37f), v3(-0.62f, 0.44f, -2.37f), black);
        for (side = -1; side <= 1; side += 2) {
            mesh_quad_flat(m, v3(side * 0.40f, 0.50f, -2.36f), v3(side * 0.82f, 0.50f, -2.36f), v3(side * 0.84f, 0.58f, -2.30f), v3(side * 0.42f, 0.60f, -2.30f), col(255, 246, 214));
            mesh_quad_flat(m, v3(side * 0.74f, 0.22f, -2.37f), v3(side * 0.90f, 0.22f, -2.37f), v3(side * 0.90f, 0.40f, -2.37f), v3(side * 0.76f, 0.40f, -2.37f), black);
        }
        mesh_box(m, v3(-0.96f, 0.10f, -2.52f), v3(0.96f, 0.16f, -2.26f), black, black);
        /* faldones laterales y franja */
        for (side = -1; side <= 1; side += 2) {
            mesh_box(m, v3(side > 0 ? 0.90f : -1.02f, 0.12f, -1.1f), v3(side > 0 ? 1.02f : -0.90f, 0.24f, 1.1f), black, black);
            mesh_quad_flat(m, v3(side * 1.003f, 0.56f, -2.1f), v3(side * 1.003f, 0.56f, 2.1f), v3(side * 1.003f, 0.60f, 2.1f), v3(side * 1.003f, 0.60f, -2.1f), accent);
            /* lineas de las 4 puertas y manijas */
            for (k = 0; k < 3; k++) {
                float z = -0.72f + k * 1.18f;
                mesh_quad_flat(m, v3(side * 1.004f, 0.26f, z - 0.012f), v3(side * 1.004f, 0.26f, z + 0.012f), v3(side * 1.004f, 0.86f, z + 0.012f), v3(side * 1.004f, 0.86f, z - 0.012f), black);
            }
            mesh_quad_flat(m, v3(side * 1.005f, 0.72f, -0.50f), v3(side * 1.005f, 0.72f, -0.33f), v3(side * 1.005f, 0.76f, -0.33f), v3(side * 1.005f, 0.76f, -0.50f), accent);
            mesh_quad_flat(m, v3(side * 1.005f, 0.72f, 0.68f), v3(side * 1.005f, 0.72f, 0.85f), v3(side * 1.005f, 0.76f, 0.85f), v3(side * 1.005f, 0.76f, 0.68f), accent);
            /* toma de aire del V10 detras de las puertas traseras */
            mesh_quad_flat(m, v3(side * 1.004f, 0.32f, 1.45f), v3(side * 1.004f, 0.32f, 1.85f), v3(side * 1.004f, 0.50f, 1.85f), v3(side * 1.004f, 0.50f, 1.45f), black);
        }
        /* trasera: pilotos, difusor y cuatro escapes */
        for (side = -1; side <= 1; side += 2)
            mesh_quad_flat(m, v3(side * 0.40f, 0.66f, 2.31f), v3(side * 0.88f, 0.66f, 2.31f), v3(side * 0.88f, 0.80f, 2.31f), v3(side * 0.40f, 0.80f, 2.31f), col(210, 24, 24));
        mesh_box(m, v3(-0.86f, 0.12f, 2.20f), v3(0.86f, 0.30f, 2.42f), black, black);
        for (k = 0; k < 4; k++) {
            float x = -0.54f + k * 0.36f;
            mesh_prism(m, v3(x, 0.18f, 2.43f), 0.06f, 0.01f, 6, col(160, 160, 166), col(40, 40, 44));
            mesh_quad_flat(m, v3(x - 0.06f, 0.16f, 2.43f), v3(x + 0.06f, 0.16f, 2.43f), v3(x + 0.06f, 0.26f, 2.43f), v3(x - 0.06f, 0.26f, 2.43f), col(60, 60, 64));
        }
        /* pequeno aleron en el borde del maletero */
        mesh_box(m, v3(-0.82f, 0.98f, 2.12f), v3(0.82f, 1.06f, 2.32f), paint, accent);
        wheel(m, -0.90f, -1.48f, 0.36f, 0.30f); wheel(m, 0.90f, -1.48f, 0.36f, 0.30f);
        wheel(m, -0.92f, 1.50f, 0.37f, 0.34f); wheel(m, 0.92f, 1.50f, 0.37f, 0.34f);
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
