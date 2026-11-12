/* core.h - matematicas, mallas y utilidades compartidas (independiente de plataforma) */
#ifndef CORE_H
#define CORE_H

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct { float x, y, z; } V3;
typedef struct { unsigned char r, g, b, a; } Col;

static inline V3 v3(float x, float y, float z) { V3 v = {x, y, z}; return v; }
static inline V3 vadd(V3 a, V3 b) { return v3(a.x + b.x, a.y + b.y, a.z + b.z); }
static inline V3 vsub(V3 a, V3 b) { return v3(a.x - b.x, a.y - b.y, a.z - b.z); }
static inline V3 vscale(V3 a, float s) { return v3(a.x * s, a.y * s, a.z * s); }
static inline float vdot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline V3 vcross(V3 a, V3 b) {
    return v3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
static inline float vlen(V3 a) { return sqrtf(vdot(a, a)); }
static inline V3 vnorm(V3 a) { float l = vlen(a); return l > 1e-6f ? vscale(a, 1.0f / l) : v3(0, 0, 0); }
static inline V3 vlerp(V3 a, V3 b, float t) { return vadd(a, vscale(vsub(b, a), t)); }

static inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
static inline float lerpf(float a, float b, float t) { return a + (b - a) * t; }
static inline float signf(float v) { return v < 0 ? -1.0f : 1.0f; }
static inline float wrap_angle(float a) {
    while (a > (float)M_PI) a -= 2.0f * (float)M_PI;
    while (a < -(float)M_PI) a += 2.0f * (float)M_PI;
    return a;
}
static inline float approach(float v, float target, float step) {
    if (v < target) { v += step; if (v > target) v = target; }
    else { v -= step; if (v < target) v = target; }
    return v;
}

static inline Col col(int r, int g, int b) { Col c = {(unsigned char)r, (unsigned char)g, (unsigned char)b, 255}; return c; }
static inline Col cola(int r, int g, int b, int a) { Col c = {(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a}; return c; }
static inline Col col_shade(Col c, float s) {
    float r = c.r * s, g = c.g * s, b = c.b * s;
    return cola((int)clampf(r, 0, 255), (int)clampf(g, 0, 255), (int)clampf(b, 0, 255), c.a);
}

/* Matriz 3x4 con la misma convencion que libogc (Mtx = f32[3][4], columnas = ejes) */
typedef float M34[3][4];

void m34_identity(M34 m);
void m34_from_basis(M34 m, V3 right, V3 up, V3 back, V3 pos);
void m34_mul(M34 out, M34 a, M34 b);          /* out = a * b */
void m34_lookat(M34 m, V3 eye, V3 up, V3 target);
V3 m34_apply(M34 m, V3 p);
void m34_rot_x(M34 m, float a);
void m34_rot_y(M34 m, float a);
void m34_rot_z(M34 m, float a);

/* Generador pseudoaleatorio determinista */
typedef struct { unsigned int s; } Rng;
static inline unsigned int rng_u(Rng *r) { r->s = r->s * 1664525u + 1013904223u; return r->s >> 8; }
static inline float rng_f(Rng *r) { return (rng_u(r) & 0xFFFF) / 65535.0f; }
static inline float rng_range(Rng *r, float a, float b) { return a + (b - a) * rng_f(r); }

/* ---------- Mallas: triangulos sueltos con color por vertice (sombreado plano) ---------- */
typedef struct { float x, y, z; unsigned char r, g, b, a; } MVtx;

typedef struct {
    MVtx *v;
    int n, cap;      /* n = numero de vertices (multiplo de 3) */
    V3 bmin, bmax;   /* caja envolvente */
} Mesh;

void mesh_init(Mesh *m);
void mesh_free(Mesh *m);
void mesh_tri(Mesh *m, V3 a, V3 b, V3 c, Col cl);          /* aplica sombreado por luz fija */
void mesh_tri_flat(Mesh *m, V3 a, V3 b, V3 c, Col cl);     /* sin sombreado */
void mesh_quad(Mesh *m, V3 a, V3 b, V3 c, V3 d, Col cl);   /* a-b-c-d en orden */
void mesh_quad_flat(Mesh *m, V3 a, V3 b, V3 c, V3 d, Col cl);
void mesh_box(Mesh *m, V3 mn, V3 mx, Col side, Col top);
/* caja orientada: centro, eje derecho (x), eje adelante (z horizontal), medidas */
void mesh_obox(Mesh *m, V3 base_center, V3 right, V3 fwd, float hw, float hl, float h, Col side, Col top);
void mesh_prism(Mesh *m, V3 base, float radius, float h, int sides, Col side, Col top);
void mesh_cone(Mesh *m, V3 base, float radius, float h, int sides, Col c);
void mesh_append(Mesh *dst, const Mesh *src);

#endif
