/* plat_gc.c - capa de plataforma para Nintendo GameCube (libogc / GX / ASND) */
#ifdef GEKKO

#include <gccore.h>
#include <asndlib.h>
#include <malloc.h>
#include <ogc/lwp_watchdog.h>
#include "plat.h"
#include "game.h"

#define FIFO_SIZE (512 * 1024)
#define MAX_RECTS 3000
#define DL_BATCH  30000      /* vertices por GX_Begin (multiplo de 3, < 65535) */

static GXRModeObj *rmode;
static void *xfb[2];
static int fbi;
static float sx_scale = 1.0f, sy_scale = 1.0f;
static Mtx view_mtx;

typedef struct { void *dl; u32 size; } GMesh;

/* ------------------------------------------------------------ */
/* video                                                        */
/* ------------------------------------------------------------ */

static void reset_cb(u32 irq, void *ctx) {
    void (*reload)(void) = (void (*)(void))0x80001800;  /* volver al cargador (Swiss) */
    (void)irq; (void)ctx;
    reload();
}

static void setup_vertex_format(void) {
    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetNumChans(1);
    GX_SetNumTexGens(0);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetNumTevStages(1);
}

static void video_init(void) {
    void *fifo;
    f32 yscale;
    u32 xfbHeight;
    GXColor bg = {0, 0, 0, 255};

    VIDEO_Init();
    PAD_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb[0]);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    fifo = memalign(32, FIFO_SIZE);
    memset(fifo, 0, FIFO_SIZE);
    GX_Init(fifo, FIFO_SIZE);
    GX_SetCopyClear(bg, GX_MAX_Z24);
    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
    yscale = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
    xfbHeight = GX_SetDispCopyYScale(yscale);
    GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
    GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
    GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
    GX_SetPixelFmt(rmode->aa ? GX_PF_RGB565_Z16 : GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    GX_SetCullMode(GX_CULL_NONE);
    GX_CopyDisp(xfb[0], GX_TRUE);
    GX_SetDispCopyGamma(GX_GM_1_0);

    sx_scale = rmode->fbWidth / (float)SCREEN_W;
    sy_scale = rmode->efbHeight / (float)SCREEN_H;

    setup_vertex_format();
    SYS_SetResetCallback(reset_cb);
}

void *plat_mesh_upload(const Mesh *m) {
    GMesh *g;
    u32 cap, len;
    int i, start;
    if (!m || m->n < 3) return NULL;
    cap = (u32)m->n * 16 + (m->n / DL_BATCH + 1) * 8 + 128;
    cap = (cap + 31) & ~31u;
    g = (GMesh *)malloc(sizeof(GMesh));
    if (!g) return NULL;
    g->dl = memalign(32, cap);
    if (!g->dl) { free(g); return NULL; }
    memset(g->dl, 0, cap);
    DCInvalidateRange(g->dl, cap);
    GX_BeginDispList(g->dl, cap);
    for (start = 0; start < m->n; start += DL_BATCH) {
        int cnt = m->n - start;
        if (cnt > DL_BATCH) cnt = DL_BATCH;
        cnt -= cnt % 3;
        GX_Begin(GX_TRIANGLES, GX_VTXFMT0, (u16)cnt);
        for (i = start; i < start + cnt; i++) {
            const MVtx *v = &m->v[i];
            GX_Position3f32(v->x, v->y, v->z);
            GX_Color4u8(v->r, v->g, v->b, v->a);
        }
        GX_End();
    }
    len = GX_EndDispList();
    if (len == 0) { free(g->dl); free(g); return NULL; }
    g->size = len;
    return g;
}

void plat_mesh_release(void *h) {
    GMesh *g = (GMesh *)h;
    if (!g) return;
    GX_DrawDone();   /* asegurar que la GPU ya no la usa */
    free(g->dl);
    free(g);
}

void plat_frame_begin(void) {
    GX_InvVtxCache();
    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
    GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
}

void plat_frame_end(void) {
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);
    GX_CopyDisp(xfb[fbi], GX_TRUE);
    GX_DrawDone();
    VIDEO_SetNextFramebuffer(xfb[fbi]);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    fbi ^= 1;
}

/* ------------------------------------------------------------ */
/* 2D: rectangulos acumulados                                    */
/* ------------------------------------------------------------ */

typedef struct { float x, y, w, h; Col c; } Rect2;
static Rect2 rects[MAX_RECTS];
static int nrects;

static void set_2d_state(const Viewport *clip) {
    Mtx44 ortho;
    Mtx ident;
    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
    if (clip) GX_SetScissor((u32)(clip->x * sx_scale), (u32)(clip->y * sy_scale), (u32)(clip->w * sx_scale), (u32)(clip->h * sy_scale));
    else GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    guOrtho(ortho, 0, SCREEN_H, 0, SCREEN_W, 0, 10);
    GX_LoadProjectionMtx(ortho, GX_ORTHOGRAPHIC);
    guMtxIdentity(ident);
    GX_LoadPosMtxImm(ident, GX_PNMTX0);
    GX_SetCurrentMtx(GX_PNMTX0);
    GX_SetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    {
        GXColor c = {0, 0, 0, 0};
        GX_SetFog(GX_FOG_NONE, 0, 1, 0.1f, 1, c);
    }
}

static void flush_rects(void) {
    int i;
    if (nrects == 0) return;
    GX_Begin(GX_QUADS, GX_VTXFMT0, (u16)(nrects * 4));
    for (i = 0; i < nrects; i++) {
        Rect2 *r = &rects[i];
        GX_Position3f32(r->x, r->y, -1); GX_Color4u8(r->c.r, r->c.g, r->c.b, r->c.a);
        GX_Position3f32(r->x + r->w, r->y, -1); GX_Color4u8(r->c.r, r->c.g, r->c.b, r->c.a);
        GX_Position3f32(r->x + r->w, r->y + r->h, -1); GX_Color4u8(r->c.r, r->c.g, r->c.b, r->c.a);
        GX_Position3f32(r->x, r->y + r->h, -1); GX_Color4u8(r->c.r, r->c.g, r->c.b, r->c.a);
    }
    GX_End();
    nrects = 0;
}

void plat_sky(const Viewport *vp, Col top, Col hor) {
    float y1 = vp->y + vp->h * 0.55f;
    set_2d_state(vp);
    GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
    GX_Position3f32(vp->x, vp->y, -1); GX_Color4u8(top.r, top.g, top.b, 255);
    GX_Position3f32(vp->x + vp->w, vp->y, -1); GX_Color4u8(top.r, top.g, top.b, 255);
    GX_Position3f32(vp->x + vp->w, y1, -1); GX_Color4u8(hor.r, hor.g, hor.b, 255);
    GX_Position3f32(vp->x, y1, -1); GX_Color4u8(hor.r, hor.g, hor.b, 255);
    GX_Position3f32(vp->x, y1, -1); GX_Color4u8(hor.r, hor.g, hor.b, 255);
    GX_Position3f32(vp->x + vp->w, y1, -1); GX_Color4u8(hor.r, hor.g, hor.b, 255);
    GX_Position3f32(vp->x + vp->w, vp->y + vp->h, -1); GX_Color4u8(hor.r, hor.g, hor.b, 255);
    GX_Position3f32(vp->x, vp->y + vp->h, -1); GX_Color4u8(hor.r, hor.g, hor.b, 255);
    GX_End();
}

void plat_2d_begin(void) {
    nrects = 0;
    set_2d_state(NULL);
}

void plat_rect(float x, float y, float w, float h, Col c) {
    if (nrects >= MAX_RECTS) flush_rects();
    rects[nrects].x = x; rects[nrects].y = y; rects[nrects].w = w; rects[nrects].h = h; rects[nrects].c = c;
    nrects++;
}

void plat_2d_end(void) { flush_rects(); }

/* ------------------------------------------------------------ */
/* 3D                                                           */
/* ------------------------------------------------------------ */

void plat_view_begin(const Viewport *vp, float fovy, float zn, float zf, M34 view, Col fog, float fs, float fe) {
    Mtx44 proj;
    GXColor fc = {fog.r, fog.g, fog.b, 255};
    flush_rects();
    GX_SetViewport(vp->x * sx_scale, vp->y * sy_scale, vp->w * sx_scale, vp->h * sy_scale, 0, 1);
    GX_SetScissor((u32)(vp->x * sx_scale), (u32)(vp->y * sy_scale), (u32)(vp->w * sx_scale), (u32)(vp->h * sy_scale));
    guPerspective(proj, fovy, (f32)vp->w / (f32)vp->h, zn, zf);
    GX_LoadProjectionMtx(proj, GX_PERSPECTIVE);
    memcpy(view_mtx, view, sizeof(Mtx));
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GX_SetFog(GX_FOG_LIN, fs, fe, zn, zf, fc);
    GX_SetCurrentMtx(GX_PNMTX0);
}

void plat_draw(void *h, M34 model) {
    GMesh *g = (GMesh *)h;
    Mtx mv;
    if (!g) return;
    if (model) guMtxConcat(view_mtx, model, mv);
    else memcpy(mv, view_mtx, sizeof(Mtx));
    GX_LoadPosMtxImm(mv, GX_PNMTX0);
    GX_CallDispList(g->dl, g->size);
}

/* ------------------------------------------------------------ */
/* entrada                                                      */
/* ------------------------------------------------------------ */

static float trig_norm(u8 v) { return clampf((v - 25) / 170.0f, 0, 1); }
static float stick_norm(s8 v) { return clampf(v / 80.0f, -1, 1); }

static unsigned map_buttons(u16 b) {
    unsigned o = 0;
    if (b & PAD_BUTTON_A) o |= BTN_A;
    if (b & PAD_BUTTON_B) o |= BTN_B;
    if (b & PAD_BUTTON_X) o |= BTN_X;
    if (b & PAD_BUTTON_Y) o |= BTN_Y;
    if (b & PAD_TRIGGER_Z) o |= BTN_Z;
    if (b & PAD_BUTTON_START) o |= BTN_START;
    if (b & PAD_BUTTON_UP) o |= BTN_UP;
    if (b & PAD_BUTTON_DOWN) o |= BTN_DOWN;
    if (b & PAD_BUTTON_LEFT) o |= BTN_LEFT;
    if (b & PAD_BUTTON_RIGHT) o |= BTN_RIGHT;
    if (b & PAD_TRIGGER_L) o |= BTN_L;
    if (b & PAD_TRIGGER_R) o |= BTN_R;
    return o;
}

void plat_read_pads(PadIn pads[2]) {
    u32 conn = PAD_ScanPads();
    int i;
    for (i = 0; i < 2; i++) {
        PadIn *p = &pads[i];
        p->connected = (conn >> i) & 1;
        p->held = map_buttons(PAD_ButtonsHeld(i));
        p->pressed = map_buttons(PAD_ButtonsDown(i));
        p->sx = stick_norm(PAD_StickX(i));
        p->sy = stick_norm(PAD_StickY(i));
        p->trig_l = trig_norm(PAD_TriggerL(i));
        p->trig_r = trig_norm(PAD_TriggerR(i));
    }
}

/* ------------------------------------------------------------ */
/* audio (sintetizado, sin muestras externas)                    */
/* ------------------------------------------------------------ */

#define ENG_SAMPLES 2048
#define ENG_CYCLES  8
static s16 *eng_buf;
static int eng_on[2];
static s16 *sfx_buf[SFX_COUNT];
static int sfx_len[SFX_COUNT];
static int sfx_rate[SFX_COUNT];
static int sfx_voice = 2;

static s16 *alloc_snd(int samples, int *bytes) {
    int b = (samples * 2 + 31) & ~31;
    s16 *p = (s16 *)memalign(32, b);
    memset(p, 0, b);
    *bytes = b;
    return p;
}

static void make_sfx(int id, float secs, int rate, int kind, float f0) {
    int n = (int)(secs * rate), i, bytes;
    unsigned seed = 1234 + id * 77;
    float lp = 0, ph = 0;
    s16 *b = alloc_snd(n, &bytes);
    for (i = 0; i < n; i++) {
        float t = (float)i / rate, env, s;
        seed = seed * 1103515245u + 12345u;
        {
            float noise = ((seed >> 16) & 0x7FFF) / 16384.0f - 1.0f;
            switch (kind) {
            case 0: /* tono cuadrado */
                ph += f0 / rate;
                s = (ph - floorf(ph)) < 0.5f ? 0.6f : -0.6f;
                env = 1.0f - t / secs;
                break;
            case 1: /* ruido grave (choque) */
                lp += (noise - lp) * 0.08f;
                s = lp * 3.0f + noise * 0.25f;
                env = expf(-t * 3.5f);
                break;
            case 2: /* golpe corto */
                lp += (noise - lp) * 0.2f;
                s = lp * 2.0f;
                env = expf(-t * 30.0f);
                break;
            default: /* silbido ascendente (turbo) */
                lp += (noise - lp) * (0.05f + 0.4f * t / secs);
                s = lp * 2.0f;
                env = sinf((float)M_PI * t / secs);
                break;
            }
        }
        b[i] = (s16)clampf(s * env * 26000.0f, -32000, 32000);
    }
    DCFlushRange(b, bytes);
    sfx_buf[id] = b;
    sfx_len[id] = bytes;
    sfx_rate[id] = rate;
}

static void audio_init(void) {
    int i, bytes;
    ASND_Init();
    ASND_Pause(0);
    /* motor: 8 ciclos con explosiones ligeramente distintas */
    eng_buf = alloc_snd(ENG_SAMPLES, &bytes);
    for (i = 0; i < ENG_SAMPLES; i++) {
        int per = ENG_SAMPLES / ENG_CYCLES, cyc = i / per;
        float ph = (float)(i % per) / per;
        static const float amp[ENG_CYCLES] = {1.0f, 0.82f, 0.95f, 0.74f, 0.98f, 0.8f, 0.9f, 0.77f};
        float saw = 1.0f - 2.0f * ph;
        float pulse = ph < 0.18f ? 1.0f : -0.25f;
        float s = (0.55f * saw + 0.45f * pulse) * amp[cyc];
        s += 0.25f * sinf(2 * (float)M_PI * ph * 2.0f);
        eng_buf[i] = (s16)clampf(s * 9000.0f, -32000, 32000);
    }
    DCFlushRange(eng_buf, bytes);
    make_sfx(SFX_BEEP, 0.18f, 32000, 0, 880.0f);
    make_sfx(SFX_GO, 0.45f, 32000, 0, 1760.0f);
    make_sfx(SFX_CRASH, 1.0f, 32000, 1, 0);
    make_sfx(SFX_BUMP, 0.12f, 32000, 2, 0);
    make_sfx(SFX_BOOST, 0.6f, 32000, 3, 0);
    make_sfx(SFX_SELECT, 0.06f, 32000, 0, 1320.0f);
}

void plat_engine(int player, int on, float rpm01, float load) {
    int voice = player;  /* voces 0 y 1 */
    if (!eng_buf || player < 0 || player > 1) return;
    if (!on) {
        if (eng_on[player]) { ASND_StopVoice(voice); eng_on[player] = 0; }
        return;
    }
    {
        float freq = 32.0f + rpm01 * 230.0f;               /* frecuencia fundamental */
        s32 pitch = (s32)(freq * (ENG_SAMPLES / ENG_CYCLES));
        s32 vol = (s32)(70 + 90 * clampf(load, 0, 1));
        if (pitch > 140000) pitch = 140000;
        if (!eng_on[player] || ASND_StatusVoice(voice) == SND_UNUSED) {
            ASND_SetInfiniteVoice(voice, VOICE_MONO_16BIT, pitch, 0, eng_buf, ENG_SAMPLES * 2, vol, vol);
            eng_on[player] = 1;
        } else {
            ASND_ChangePitchVoice(voice, pitch);
            ASND_ChangeVolumeVoice(voice, vol, vol);
        }
    }
}

void plat_sfx(int id) {
    if (id < 0 || id >= SFX_COUNT || !sfx_buf[id]) return;
    ASND_SetVoice(sfx_voice, VOICE_MONO_16BIT, sfx_rate[id], 0, sfx_buf[id], sfx_len[id], 200, 200, NULL);
    sfx_voice++;
    if (sfx_voice >= 8) sfx_voice = 2;
}

/* ------------------------------------------------------------ */

int main(int argc, char **argv) {
    u64 last;
    (void)argc; (void)argv;
    video_init();
    audio_init();
    game_init();
    last = gettime();
    while (1) {
        PadIn pads[2];
        u64 now;
        float dt;
        plat_read_pads(pads);
        now = gettime();
        dt = diff_usec(last, now) / 1000000.0f;
        last = now;
        dt = clampf(dt, 1.0f / 120.0f, 1.0f / 20.0f);
        game_frame(pads, dt);
    }
    return 0;
}

#endif /* GEKKO */
