/* core.c - implementacion de matematicas y mallas */
#include "core.h"

void m34_identity(M34 m) {
    memset(m, 0, sizeof(M34));
    m[0][0] = m[1][1] = m[2][2] = 1.0f;
}

void m34_from_basis(M34 m, V3 r, V3 u, V3 b, V3 p) {
    m[0][0] = r.x; m[0][1] = u.x; m[0][2] = b.x; m[0][3] = p.x;
    m[1][0] = r.y; m[1][1] = u.y; m[1][2] = b.y; m[1][3] = p.y;
    m[2][0] = r.z; m[2][1] = u.z; m[2][2] = b.z; m[2][3] = p.z;
}

void m34_mul(M34 out, M34 a, M34 b) {
    M34 t;
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            t[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j] + (j == 3 ? a[i][3] : 0.0f);
        }
    }
    memcpy(out, t, sizeof(M34));
}

void m34_lookat(M34 m, V3 eye, V3 up, V3 target) {
    V3 z = vnorm(vsub(eye, target));
    V3 x = vnorm(vcross(up, z));
    V3 y = vcross(z, x);
    m[0][0] = x.x; m[0][1] = x.y; m[0][2] = x.z; m[0][3] = -vdot(x, eye);
    m[1][0] = y.x; m[1][1] = y.y; m[1][2] = y.z; m[1][3] = -vdot(y, eye);
    m[2][0] = z.x; m[2][1] = z.y; m[2][2] = z.z; m[2][3] = -vdot(z, eye);
}

V3 m34_apply(M34 m, V3 p) {
    return v3(m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3],
              m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3],
              m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3]);
}

void m34_rot_x(M34 m, float a) {
    float c = cosf(a), s = sinf(a);
    m34_identity(m);
    m[1][1] = c; m[1][2] = -s; m[2][1] = s; m[2][2] = c;
}
void m34_rot_y(M34 m, float a) {
    float c = cosf(a), s = sinf(a);
    m34_identity(m);
    m[0][0] = c; m[0][2] = s; m[2][0] = -s; m[2][2] = c;
}
void m34_rot_z(M34 m, float a) {
    float c = cosf(a), s = sinf(a);
    m34_identity(m);
    m[0][0] = c; m[0][1] = -s; m[1][0] = s; m[1][1] = c;
}

/* ---------------- mallas ---------------- */

static const V3 LIGHT_DIR = {0.38f, 0.82f, 0.43f};

void mesh_init(Mesh *m) {
    m->v = NULL; m->n = m->cap = 0;
    m->bmin = v3(1e9f, 1e9f, 1e9f);
    m->bmax = v3(-1e9f, -1e9f, -1e9f);
}

void mesh_free(Mesh *m) {
    free(m->v);
    mesh_init(m);
}

static void push(Mesh *m, V3 p, Col c) {
    if (m->n >= m->cap) {
        int nc = m->cap ? m->cap * 2 : 768;
        MVtx *nv = (MVtx *)realloc(m->v, sizeof(MVtx) * nc);
        if (!nv) return;
        m->v = nv; m->cap = nc;
    }
    MVtx *v = &m->v[m->n++];
    v->x = p.x; v->y = p.y; v->z = p.z;
    v->r = c.r; v->g = c.g; v->b = c.b; v->a = c.a;
    if (p.x < m->bmin.x) m->bmin.x = p.x;
    if (p.y < m->bmin.y) m->bmin.y = p.y;
    if (p.z < m->bmin.z) m->bmin.z = p.z;
    if (p.x > m->bmax.x) m->bmax.x = p.x;
    if (p.y > m->bmax.y) m->bmax.y = p.y;
    if (p.z > m->bmax.z) m->bmax.z = p.z;
}

void mesh_tri_flat(Mesh *m, V3 a, V3 b, V3 c, Col cl) {
    push(m, a, cl); push(m, b, cl); push(m, c, cl);
}

void mesh_tri(Mesh *m, V3 a, V3 b, V3 c, Col cl) {
    V3 n = vnorm(vcross(vsub(b, a), vsub(c, a)));
    float d = fabsf(vdot(n, vnorm(LIGHT_DIR)));
    /* caras horizontales mas claras, verticales segun orientacion */
    float s = 0.52f + 0.55f * d;
    mesh_tri_flat(m, a, b, c, col_shade(cl, s));
}

void mesh_quad(Mesh *m, V3 a, V3 b, V3 c, V3 d, Col cl) {
    V3 n = vnorm(vcross(vsub(b, a), vsub(c, a)));
    if (vlen(n) < 0.5f) n = vnorm(vcross(vsub(c, a), vsub(d, a)));
    float s = 0.52f + 0.55f * fabsf(vdot(n, vnorm(LIGHT_DIR)));
    Col sc = col_shade(cl, s);
    mesh_tri_flat(m, a, b, c, sc);
    mesh_tri_flat(m, a, c, d, sc);
}

void mesh_quad_flat(Mesh *m, V3 a, V3 b, V3 c, V3 d, Col cl) {
    mesh_tri_flat(m, a, b, c, cl);
    mesh_tri_flat(m, a, c, d, cl);
}

void mesh_box(Mesh *m, V3 a, V3 b, Col side, Col top) {
    V3 p000 = v3(a.x, a.y, a.z), p100 = v3(b.x, a.y, a.z), p010 = v3(a.x, b.y, a.z), p110 = v3(b.x, b.y, a.z);
    V3 p001 = v3(a.x, a.y, b.z), p101 = v3(b.x, a.y, b.z), p011 = v3(a.x, b.y, b.z), p111 = v3(b.x, b.y, b.z);
    mesh_quad(m, p000, p100, p110, p010, side);  /* -z */
    mesh_quad(m, p101, p001, p011, p111, side);  /* +z */
    mesh_quad(m, p001, p000, p010, p011, side);  /* -x */
    mesh_quad(m, p100, p101, p111, p110, side);  /* +x */
    mesh_quad(m, p010, p110, p111, p011, top);   /* arriba */
}

void mesh_obox(Mesh *m, V3 c, V3 r, V3 f, float hw, float hl, float h, Col side, Col top) {
    V3 up = v3(0, h, 0);
    V3 R = vscale(r, hw), F = vscale(f, hl);
    V3 b0 = vsub(vsub(c, R), F), b1 = vsub(vadd(c, R), F), b2 = vadd(vadd(c, R), F), b3 = vadd(vsub(c, R), F);
    V3 t0 = vadd(b0, up), t1 = vadd(b1, up), t2 = vadd(b2, up), t3 = vadd(b3, up);
    mesh_quad(m, b0, b1, t1, t0, side);
    mesh_quad(m, b1, b2, t2, t1, side);
    mesh_quad(m, b2, b3, t3, t2, side);
    mesh_quad(m, b3, b0, t0, t3, side);
    mesh_quad(m, t0, t1, t2, t3, top);
}

void mesh_prism(Mesh *m, V3 base, float radius, float h, int sides, Col side, Col top) {
    int i;
    V3 ctop = v3(base.x, base.y + h, base.z);
    for (i = 0; i < sides; i++) {
        float a0 = 2.0f * (float)M_PI * i / sides, a1 = 2.0f * (float)M_PI * (i + 1) / sides;
        V3 p0 = v3(base.x + cosf(a0) * radius, base.y, base.z + sinf(a0) * radius);
        V3 p1 = v3(base.x + cosf(a1) * radius, base.y, base.z + sinf(a1) * radius);
        V3 q0 = vadd(p0, v3(0, h, 0)), q1 = vadd(p1, v3(0, h, 0));
        mesh_quad(m, p0, p1, q1, q0, side);
        mesh_tri(m, ctop, q0, q1, top);
    }
}

void mesh_cone(Mesh *m, V3 base, float radius, float h, int sides, Col c) {
    int i;
    V3 tip = v3(base.x, base.y + h, base.z);
    for (i = 0; i < sides; i++) {
        float a0 = 2.0f * (float)M_PI * i / sides, a1 = 2.0f * (float)M_PI * (i + 1) / sides;
        V3 p0 = v3(base.x + cosf(a0) * radius, base.y, base.z + sinf(a0) * radius);
        V3 p1 = v3(base.x + cosf(a1) * radius, base.y, base.z + sinf(a1) * radius);
        mesh_tri(m, p0, p1, tip, c);
    }
}

void mesh_append(Mesh *dst, const Mesh *src) {
    int i;
    for (i = 0; i < src->n; i++) {
        const MVtx *v = &src->v[i];
        Col c = cola(v->r, v->g, v->b, v->a);
        push(dst, v3(v->x, v->y, v->z), c);
    }
}
