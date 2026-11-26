/* musica.c - musica sintetizada dentro del juego (sin archivos de audio)
 *  - menu: pieza original estilo consola de 16 bits (pulso con vibrato y eco, arpegios,
 *          bajo y bateria), 16 compases en bucle
 *  - jingle: fanfarria de 8 bits de 3 segundos al cruzar la meta
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "musica.h"

#define BPM 132.0f
#define COMPASES 16
#define SEMIS (COMPASES * 16)   /* semicorcheas en todo el bucle */

typedef struct { unsigned char nota, dur; } Nota;   /* nota MIDI (0 = silencio), duracion en semicorcheas */

/* melodia: 16 compases (cada uno suma 16 semicorcheas) */
static const Nota MELODIA[] = {
    /* A: C G Am F C G F G */
    {76, 4}, {79, 2}, {84, 2}, {83, 2}, {79, 2}, {76, 4},
    {74, 2}, {79, 2}, {83, 4}, {81, 2}, {79, 2}, {74, 4},
    {72, 2}, {76, 2}, {81, 4}, {79, 2}, {76, 2}, {81, 4},
    {81, 4}, {84, 2}, {81, 2}, {79, 4}, {77, 4},
    {76, 2}, {79, 2}, {84, 4}, {88, 4}, {86, 2}, {84, 2},
    {83, 4}, {86, 2}, {83, 2}, {79, 4}, {74, 4},
    {81, 2}, {84, 2}, {89, 4}, {88, 2}, {84, 2}, {81, 4},
    {79, 4}, {83, 4}, {86, 6}, {0, 2},
    /* B: Am F C G Am F G G */
    {81, 6}, {84, 2}, {88, 4}, {86, 2}, {84, 2},
    {84, 4}, {81, 4}, {77, 4}, {81, 4},
    {79, 6}, {76, 2}, {79, 4}, {84, 4},
    {83, 2}, {84, 2}, {86, 4}, {83, 4}, {79, 4},
    {88, 4}, {86, 2}, {84, 2}, {81, 4}, {76, 4},
    {77, 2}, {81, 2}, {84, 4}, {89, 4}, {88, 4},
    {86, 4}, {83, 4}, {79, 4}, {83, 4},
    {86, 6}, {83, 2}, {79, 4}, {0, 4},
};

/* acorde de cada compas: raiz del bajo y las tres notas del arpegio */
static const unsigned char ACORDES[COMPASES][4] = {
    {48, 60, 64, 67}, {43, 55, 59, 62}, {45, 57, 60, 64}, {41, 53, 57, 60},
    {48, 60, 64, 67}, {43, 55, 59, 62}, {41, 53, 57, 60}, {43, 55, 59, 62},
    {45, 57, 60, 64}, {41, 53, 57, 60}, {48, 60, 64, 67}, {43, 55, 59, 62},
    {45, 57, 60, 64}, {41, 53, 57, 60}, {43, 55, 59, 62}, {43, 55, 59, 62},
};

static float frec(int nota) { return 440.0f * powf(2.0f, (nota - 69) / 12.0f); }

static float pulso(float fase, float ancho) { fase -= floorf(fase); return fase < ancho ? 1.0f : -1.0f; }
static float triangulo(float fase) { fase -= floorf(fase); return fase < 0.5f ? 4.0f * fase - 1.0f : 3.0f - 4.0f * fase; }

static unsigned int ruido_semilla = 22222;
static float ruido(void) {
    ruido_semilla = ruido_semilla * 1103515245u + 12345u;
    return ((ruido_semilla >> 9) & 0xFFFF) / 32768.0f - 1.0f;
}

/* envolvente sencilla: ataque corto, cae a un sostenido y suelta al final de la nota */
static float envolvente(float t, float dur, float ataque, float sost, float suelta) {
    float a = t < ataque ? t / ataque : 1.0f;
    float d = sost + (1.0f - sost) * expf(-(t - ataque) * 6.0f);
    float r = t > dur - suelta ? fmaxf(0.0f, (dur - t) / suelta) : 1.0f;
    return a * (t < ataque ? 1.0f : d) * r;
}

static short a16(float x) {
    x = x > 1.0f ? 1.0f : (x < -1.0f ? -1.0f : x);
    x = x * (1.5f - 0.5f * x * x);   /* saturacion suave */
    return (short)(x * 30000.0f);
}

int musica_menu_muestras(int rate) {
    float semi = 60.0f / BPM / 4.0f;
    return (int)(SEMIS * semi * rate);
}

void musica_menu_render(short *out, int n, int rate) {
    float semi = 60.0f / BPM / 4.0f;
    int eco = (int)(3 * semi * rate);   /* eco a 3 semicorcheas */
    float *lead = (float *)calloc(n, sizeof(float));
    int i, k, pos;
    float fase;
    if (!lead) { memset(out, 0, n * sizeof(short)); return; }
    ruido_semilla = 22222;
    /* melodia (pulso 50% con vibrato) */
    pos = 0;
    fase = 0;
    for (k = 0; k < (int)(sizeof(MELODIA) / sizeof(MELODIA[0])); k++) {
        float dur = MELODIA[k].dur * semi;
        int len = (int)(dur * rate), j;
        if (MELODIA[k].nota) {
            float f = frec(MELODIA[k].nota);
            for (j = 0; j < len && pos + j < n; j++) {
                float t = (float)j / rate;
                float vib = 1.0f + 0.006f * sinf(2 * (float)M_PI * 5.5f * t) * fminf(1.0f, t * 4.0f);
                fase += f * vib / rate;
                lead[pos + j] = pulso(fase, 0.5f) * envolvente(t, dur, 0.008f, 0.7f, 0.03f) * 0.20f;
            }
        }
        pos += len;
    }
    for (i = 0; i < n; i++) {
        float t = (float)i / rate;
        int s = (int)(t / semi);
        float ts = t - s * semi;
        int compas = (s / 16) % COMPASES, paso = s % 16;
        const unsigned char *ac = ACORDES[compas];
        float x = 0;
        /* eco de la melodia (dos rebotes) */
        x += lead[i];
        if (i >= eco) x += lead[i - eco] * 0.35f;
        if (i >= 2 * eco) x += lead[i - 2 * eco] * 0.15f;
        /* bajo: triangulo en corcheas, octava alterna */
        {
            int nb = ac[0] + ((paso / 2) & 1 ? 12 : 0);
            float tb = t - (s & ~1) * semi;
            x += triangulo(t * frec(nb)) * envolvente(tb, 2 * semi, 0.004f, 0.8f, 0.02f) * 0.30f;
        }
        /* arpegio: pulso 25%, semicorcheas sobre las notas del acorde */
        {
            int na = ac[1 + (paso % 3)] + 12;
            x += pulso(t * frec(na), 0.25f) * envolvente(ts, semi, 0.002f, 0.4f, 0.01f) * 0.07f;
        }
        /* bateria */
        if (paso == 0 || paso == 8 || paso == 10) {   /* bombo */
            float tb = ts + (paso == 10 ? 0 : 0);
            if (tb < 0.14f) x += sinf(2 * (float)M_PI * (45.0f + 90.0f * expf(-tb * 30.0f)) * tb) * expf(-tb * 18.0f) * 0.45f;
        }
        if (paso == 4 || paso == 12) {                /* caja */
            if (ts < 0.15f) x += (ruido() * 0.7f + 0.3f * sinf(2 * (float)M_PI * 190.0f * ts)) * expf(-ts * 22.0f) * 0.28f;
        }
        if ((paso & 1) == 0 && ts < 0.035f)            /* platillo cerrado */
            x += ruido() * expf(-ts * 120.0f) * ((paso & 2) ? 0.06f : 0.10f);
        /* acorde suave de fondo (pad) */
        x += (triangulo(t * frec(ac[1])) + triangulo(t * frec(ac[2])) + triangulo(t * frec(ac[3]))) * 0.025f;
        out[i] = a16(x * 0.85f);
    }
    /* suavizar la union del bucle */
    for (i = 0; i < 64 && i < n; i++) out[n - 1 - i] = (short)(out[n - 1 - i] * (i / 64.0f));
    free(lead);
}

/* ---- jingle de meta: 8 bits puros (cuadrada, triangulo y ruido) ---- */

typedef struct { unsigned char nota; float dur; } NotaS;
static const NotaS JINGLE_LEAD[] = {
    {67, 0.10f}, {72, 0.10f}, {76, 0.10f}, {79, 0.10f}, {84, 0.30f}, {0, 0.05f},
    {79, 0.10f}, {84, 0.10f}, {88, 0.25f}, {0, 0.05f},
    {86, 0.12f}, {88, 0.12f}, {86, 0.12f}, {91, 1.00f},
};
static const NotaS JINGLE_BAJO[] = {
    {48, 0.40f}, {55, 0.30f}, {53, 0.30f}, {55, 0.36f}, {48, 1.20f},
};

#define JINGLE_SEG 3.0f

int musica_jingle_muestras(int rate) { return (int)(JINGLE_SEG * rate); }

static void pista_jingle(float *buf, int n, int rate, const NotaS *ns, int cuantas, int tipo, float vol) {
    int k, pos = 0, j;
    for (k = 0; k < cuantas; k++) {
        int len = (int)(ns[k].dur * rate);
        if (ns[k].nota) {
            float f = frec(ns[k].nota);
            for (j = 0; j < len && pos + j < n; j++) {
                float t = (float)j / rate, w;
                float ultima = (k == cuantas - 1) ? 1.0f : 0.0f;
                if (tipo == 0) w = pulso(t * f * (1.0f + ultima * 0.004f * sinf(2 * (float)M_PI * 6.0f * t)), 0.5f);
                else w = triangulo(t * f);
                buf[pos + j] += w * vol * (ultima ? expf(-t * 1.4f) : envolvente(t, ns[k].dur, 0.003f, 0.85f, 0.015f));
            }
        }
        pos += len;
    }
}

void musica_jingle_render(short *out, int n, int rate) {
    float *mix = (float *)calloc(n, sizeof(float));
    int i;
    if (!mix) { memset(out, 0, n * sizeof(short)); return; }
    ruido_semilla = 777;
    pista_jingle(mix, n, rate, JINGLE_LEAD, sizeof(JINGLE_LEAD) / sizeof(JINGLE_LEAD[0]), 0, 0.30f);
    pista_jingle(mix, n, rate, JINGLE_BAJO, sizeof(JINGLE_BAJO) / sizeof(JINGLE_BAJO[0]), 1, 0.38f);
    for (i = 0; i < n; i++) {
        float t = (float)i / rate, x = mix[i];
        /* golpes de ruido en los tiempos fuertes */
        float tg = fmodf(t, 0.4f);
        if (t < 1.6f && tg < 0.05f) x += ruido() * expf(-tg * 70.0f) * 0.18f;
        /* cuantizar a 4 bits por canal para el sonido de 8 bits */
        x = floorf(x * 12.0f) / 12.0f;
        if (t > JINGLE_SEG - 0.25f) x *= (JINGLE_SEG - t) / 0.25f;
        out[i] = a16(x);
    }
    free(mix);
}
