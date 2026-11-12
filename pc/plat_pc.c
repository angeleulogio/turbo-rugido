/* plat_pc.c - plataforma de pruebas en PC: rasterizador por software (solo para verificar) */
#include <stdio.h>
#include "../source/plat.h"

typedef struct { int n; MVtx *v; } PMesh;

static unsigned char fb[SCREEN_H][SCREEN_W][3];
static float zb[SCREEN_H][SCREEN_W];
static Viewport cur_vp;
static M34 cur_view;
static float cur_f, cur_aspect, cur_near, cur_far, fog_s, fog_e;
static Col cur_fog;
int pc_tris_drawn = 0;

void *plat_mesh_upload(const Mesh *m) {
    PMesh *p;
    if (!m || m->n == 0) return NULL;
    p = (PMesh *)malloc(sizeof(PMesh));
    p->n = m->n;
    p->v = (MVtx *)malloc(sizeof(MVtx) * m->n);
    memcpy(p->v, m->v, sizeof(MVtx) * m->n);
    return p;
}
void plat_mesh_release(void *h) { PMesh *p = (PMesh *)h; if (p) { free(p->v); free(p); } }

void plat_frame_begin(void) {
    int x, y;
    for (y = 0; y < SCREEN_H; y++) for (x = 0; x < SCREEN_W; x++) { fb[y][x][0] = fb[y][x][1] = fb[y][x][2] = 0; }
    pc_tris_drawn = 0;
}
void plat_frame_end(void) {}

void plat_sky(const Viewport *vp, Col top, Col hor) {
    int x, y;
    for (y = vp->y; y < vp->y + vp->h; y++) {
        float t = clampf((float)(y - vp->y) / (vp->h * 0.55f), 0, 1);
        for (x = vp->x; x < vp->x + vp->w; x++) {
            fb[y][x][0] = (unsigned char)lerpf(top.r, hor.r, t);
            fb[y][x][1] = (unsigned char)lerpf(top.g, hor.g, t);
            fb[y][x][2] = (unsigned char)lerpf(top.b, hor.b, t);
        }
    }
}

void plat_view_begin(const Viewport *vp, float fovy, float zn, float zf, M34 view, Col fog, float fs, float fe) {
    int x, y;
    cur_vp = *vp;
    memcpy(cur_view, view, sizeof(M34));
    cur_f = 1.0f / tanf(fovy * (float)M_PI / 360.0f);
    cur_aspect = (float)vp->w / vp->h;
    cur_near = zn; cur_far = zf; cur_fog = fog; fog_s = fs; fog_e = fe;
    for (y = vp->y; y < vp->y + vp->h; y++) for (x = vp->x; x < vp->x + vp->w; x++) zb[y][x] = 1e30f;
}

typedef struct { float x, y, z; float r, g, b; } CV;

static void raster(CV a, CV b, CV c) {
    /* a,b,c en espacio de camara; z positiva hacia delante */
    float ax, ay, bx, by, cx, cy;
    int minx, maxx, miny, maxy, x, y;
    float area;
    #define PX(v, sx, sy) { sx = cur_vp.x + ((v.x * cur_f / cur_aspect) / v.z * 0.5f + 0.5f) * cur_vp.w; \
                            sy = cur_vp.y + (0.5f - (v.y * cur_f) / v.z * 0.5f) * cur_vp.h; }
    PX(a, ax, ay); PX(b, bx, by); PX(c, cx, cy);
    area = (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
    if (fabsf(area) < 1e-6f) return;
    minx = (int)floorf(fminf(ax, fminf(bx, cx))); maxx = (int)ceilf(fmaxf(ax, fmaxf(bx, cx)));
    miny = (int)floorf(fminf(ay, fminf(by, cy))); maxy = (int)ceilf(fmaxf(ay, fmaxf(by, cy)));
    if (minx < cur_vp.x) minx = cur_vp.x;
    if (miny < cur_vp.y) miny = cur_vp.y;
    if (maxx > cur_vp.x + cur_vp.w - 1) maxx = cur_vp.x + cur_vp.w - 1;
    if (maxy > cur_vp.y + cur_vp.h - 1) maxy = cur_vp.y + cur_vp.h - 1;
    for (y = miny; y <= maxy; y++) {
        for (x = minx; x <= maxx; x++) {
            float px = x + 0.5f, py = y + 0.5f;
            float w0 = ((bx - px) * (cy - py) - (by - py) * (cx - px)) / area;
            float w1 = ((cx - px) * (ay - py) - (cy - py) * (ax - px)) / area;
            float w2 = 1.0f - w0 - w1;
            float iz, z;
            if (w0 < 0 || w1 < 0 || w2 < 0) continue;
            iz = w0 / a.z + w1 / b.z + w2 / c.z;
            z = 1.0f / iz;
            if (z < zb[y][x]) {
                float fg = clampf((z - fog_s) / (fog_e - fog_s), 0, 1);
                zb[y][x] = z;
                fb[y][x][0] = (unsigned char)lerpf(a.r, cur_fog.r, fg);
                fb[y][x][1] = (unsigned char)lerpf(a.g, cur_fog.g, fg);
                fb[y][x][2] = (unsigned char)lerpf(a.b, cur_fog.b, fg);
            }
        }
    }
}

static CV lerpcv(CV a, CV b, float t) {
    CV r = {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.r, a.g, a.b};
    return r;
}

void plat_draw(void *h, M34 model) {
    PMesh *p = (PMesh *)h;
    M34 mv;
    int i;
    if (!p) return;
    if (model) m34_mul(mv, cur_view, model); else memcpy(mv, cur_view, sizeof(M34));
    for (i = 0; i + 2 < p->n; i += 3) {
        CV v[3], in[3], out[3];
        int k, ni = 0, no = 0;
        for (k = 0; k < 3; k++) {
            V3 q = m34_apply(mv, v3(p->v[i + k].x, p->v[i + k].y, p->v[i + k].z));
            v[k].x = q.x; v[k].y = q.y; v[k].z = -q.z;
            v[k].r = p->v[i].r; v[k].g = p->v[i].g; v[k].b = p->v[i].b;
            if (v[k].z >= cur_near) in[ni++] = v[k]; else out[no++] = v[k];
        }
        if (ni == 0) continue;
        if (v[0].z > cur_far && v[1].z > cur_far && v[2].z > cur_far) continue;
        pc_tris_drawn++;
        if (ni == 3) raster(v[0], v[1], v[2]);
        else if (ni == 1) {
            CV a = in[0];
            CV b = lerpcv(a, out[0], (a.z - cur_near) / (a.z - out[0].z));
            CV c = lerpcv(a, out[1], (a.z - cur_near) / (a.z - out[1].z));
            raster(a, b, c);
        } else {
            CV a = in[0], b = in[1];
            CV c = lerpcv(a, out[0], (a.z - cur_near) / (a.z - out[0].z));
            CV d = lerpcv(b, out[0], (b.z - cur_near) / (b.z - out[0].z));
            raster(a, b, d);
            raster(a, d, c);
        }
    }
}

void plat_2d_begin(void) {}
void plat_2d_end(void) {}

void plat_rect(float x, float y, float w, float h, Col c) {
    int x0 = (int)floorf(x + 0.5f), y0 = (int)floorf(y + 0.5f), x1 = (int)floorf(x + w + 0.5f), y1 = (int)floorf(y + h + 0.5f);
    int i, j;
    float a = c.a / 255.0f;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > SCREEN_W) x1 = SCREEN_W;
    if (y1 > SCREEN_H) y1 = SCREEN_H;
    for (j = y0; j < y1; j++) for (i = x0; i < x1; i++) {
        fb[j][i][0] = (unsigned char)lerpf(fb[j][i][0], c.r, a);
        fb[j][i][1] = (unsigned char)lerpf(fb[j][i][1], c.g, a);
        fb[j][i][2] = (unsigned char)lerpf(fb[j][i][2], c.b, a);
    }
}

static PadIn script_pads[2];
void pc_set_pads(const PadIn p[2]) { script_pads[0] = p[0]; script_pads[1] = p[1]; }
void plat_read_pads(PadIn pads[2]) { pads[0] = script_pads[0]; pads[1] = script_pads[1]; }

void plat_engine(int player, int on, float rpm01, float load) { (void)player; (void)on; (void)rpm01; (void)load; }
void plat_sfx(int id) { (void)id; }

void pc_save_ppm(const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "P6\n%d %d\n255\n", SCREEN_W, SCREEN_H);
    fwrite(fb, 1, sizeof(fb), f);
    fclose(f);
}
