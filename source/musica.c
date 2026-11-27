/* musica.c - musica sintetizada dentro del juego (sin archivos de audio)
 *  - menu: pieza original synthwave (sierras desafinadas con eco, pads que respiran con el bombo,
 *          arpegio, bajo pulsante y bateria de los 80), 16 compases en bucle
 *  - jingle: fanfarria de 8 bits de 3 segundos al cruzar la meta
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "musica.h"

#define BPM 104.0f
#define COMPASES 16
#define SEMIS (COMPASES * 16)   /* semicorcheas en todo el bucle */

typedef struct { unsigned char nota, dur; } Nota;   /* nota MIDI (0 = silencio), duracion en semicorcheas */

/* tema synthwave en La menor: melodia larga y cantada (cada compas suma 16 semicorcheas) */
static const Nota MELODIA[] = {
    /* A: Am F C G Am F C G */
    {76, 6}, {74, 2}, {72, 4}, {69, 4},
    {72, 6}, {69, 2}, {72, 4}, {77, 4},
    {76, 8}, {79, 4}, {76, 4},
    {74, 12}, {0, 4},
    {76, 6}, {79, 2}, {81, 4}, {79, 4},
    {81, 6}, {84, 2}, {81, 4}, {77, 4},
    {79, 6}, {76, 2}, {79, 4}, {84, 4},
    {83, 12}, {0, 4},
    /* B: F G Am Am F G E E */
    {81, 8}, {84, 4}, {81, 4},
    {83, 8}, {86, 4}, {83, 4},
    {84, 6}, {83, 2}, {81, 4}, {76, 4},
    {81, 16},
    {77, 4}, {81, 4}, {84, 4}, {89, 4},
    {88, 6}, {86, 2}, {83, 4}, {79, 4},
    {80, 8}, {83, 4}, {88, 4},
    {88, 12}, {0, 4},
};

/* acorde de cada compas: raiz del bajo y las tres notas del acorde */
static const unsigned char ACORDES[COMPASES][4] = {
    {45, 57, 60, 64}, {41, 53, 57, 60}, {48, 55, 60, 64}, {43, 55, 59, 62},
    {45, 57, 60, 64}, {41, 53, 57, 60}, {48, 55, 60, 64}, {43, 55, 59, 62},
    {41, 53, 57, 60}, {43, 55, 59, 62}, {45, 57, 60, 64}, {45, 57, 60, 64},
    {41, 53, 57, 60}, {43, 55, 59, 62}, {40, 56, 59, 64}, {40, 56, 59, 64},
};

static float frec(int nota) { return 440.0f * powf(2.0f, (nota - 69) / 12.0f); }

static float pulso(float fase, float ancho) { fase -= floorf(fase); return fase < ancho ? 1.0f : -1.0f; }
static float triangulo(float fase) { fase -= floorf(fase); return fase < 0.5f ? 4.0f * fase - 1.0f : 3.0f - 4.0f * fase; }
static float sierra(float fase) { fase -= floorf(fase); return 2.0f * fase - 1.0f; }

static unsigned int ruido_semilla = 22222;
static float ruido(void) {
    ruido_semilla = ruido_semilla * 1103515245u + 12345u;
    return ((ruido_semilla >> 9) & 0xFFFF) / 32768.0f - 1.0f;
}

/* envolvente sencilla: ataque, cae a un sostenido y suelta al final de la nota */
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

/* filtro pasa bajos de un polo */
static float pb(float *y, float x, float a) { *y += a * (x - *y); return *y; }
static float coef(float fc, int rate) { return 1.0f - expf(-2.0f * (float)M_PI * fc / rate); }

int musica_menu_muestras(int rate) {
    float semi = 60.0f / BPM / 4.0f;
    return (int)(SEMIS * semi * rate);
}

void musica_menu_render(short *out, int n, int rate) {
    float semi = 60.0f / BPM / 4.0f, beat = 4 * semi;
    float *lead = (float *)calloc(n, sizeof(float));
    int i, k, pos;
    if (!lead) { memset(out, 0, n * sizeof(short)); return; }
    ruido_semilla = 22222;
    /* melodia: dos sierras desafinadas + octava baja suave, filtrada, con vibrato que entra despacio */
    pos = 0;
    {
        float f1 = 0, f2 = 0, f3 = 0, y = 0, a = coef(3600.0f, rate);
        for (k = 0; k < (int)(sizeof(MELODIA) / sizeof(MELODIA[0])); k++) {
            float dur = MELODIA[k].dur * semi;
            int len = (int)(dur * rate), j;
            if (MELODIA[k].nota) {
                float f = frec(MELODIA[k].nota);
                for (j = 0; j < len && pos + j < n; j++) {
                    float t = (float)j / rate;
                    float vib = 1.0f + 0.005f * sinf(2 * (float)M_PI * 5.0f * t) * fminf(1.0f, fmaxf(0.0f, t - 0.25f) * 3.0f);
                    float x;
                    f1 += f * vib * 1.004f / rate;
                    f2 += f * vib * 0.996f / rate;
                    f3 += f * vib * 0.5f / rate;
                    x = sierra(f1) + sierra(f2) + 0.5f * pulso(f3, 0.5f);
                    lead[pos + j] = pb(&y, x, a) * envolvente(t, dur, 0.02f, 0.85f, 0.06f) * 0.17f;
                }
            }
            pos += len;
        }
    }
    {
        int eco1 = (int)(3 * semi * rate), eco2 = (int)(6 * semi * rate), eco3 = (int)(0.043f * rate), eco4 = (int)(0.071f * rate);
        float ph_pad[3] = {0, 0, 0}, ph_pad2[3] = {0, 0, 0}, ph_arp = 0, ph_bajo = 0, ph_bajo2 = 0;
        float y_pad = 0, y_arp = 0, y_bajo = 0, y_hat = 0;
        float a_pad = coef(900.0f, rate), a_bajo = coef(420.0f, rate);
        int s_prev = -1;
        float fpad[3] = {0, 0, 0}, fbajo = 0, farp = 0;
        for (i = 0; i < n; i++) {
            float t = (float)i / rate;
            int s = (int)(t / semi);
            float ts = t - s * semi, tb = fmodf(t, beat);
            int compas = (s / 16) % COMPASES, paso = s % 16;
            const unsigned char *ac = ACORDES[compas];
            float x = 0, bombeo = 0.3f + 0.7f * fminf(1.0f, tb / (0.6f * beat));   /* los pads "respiran" con el bombo */
            if (s != s_prev) {   /* cambia la semicorchea: recalcular frecuencias */
                static const int arp[4] = {1, 2, 3, 2};
                int q;
                for (q = 0; q < 3; q++) fpad[q] = frec(ac[1 + q]);
                fbajo = frec(ac[0]);
                farp = frec(ac[arp[paso % 4]] + 12 + ((paso / 4) % 2 ? 12 : 0));
                s_prev = s;
            }
            /* melodia con eco y un poco de ambiente */
            x += lead[i];
            if (i >= eco1) x += lead[i - eco1] * 0.38f;
            if (i >= eco2) x += lead[i - eco2] * 0.18f;
            if (i >= eco3) x += lead[i - eco3] * 0.22f;
            if (i >= eco4) x += lead[i - eco4] * 0.16f;
            /* pads: acorde de sierras desafinadas muy filtradas */
            {
                float p = 0;
                int q;
                for (q = 0; q < 3; q++) {
                    ph_pad[q] += fpad[q] * 1.003f / rate;
                    ph_pad2[q] += fpad[q] * 0.997f / rate;
                    p += sierra(ph_pad[q]) + sierra(ph_pad2[q]);
                }
                x += pb(&y_pad, p, a_pad) * 0.075f * bombeo;
            }
            /* arpegio de sierra en semicorcheas, con brillo que se apaga en cada nota */
            ph_arp += farp / rate;
            x += pb(&y_arp, sierra(ph_arp), coef(600.0f + 3000.0f * expf(-ts * 25.0f), rate)) * expf(-ts * 9.0f) * 0.14f * bombeo;
            /* bajo pulsante en corcheas: sierra + octava, filtrado */
            {
                float te = fmodf(t, 2 * semi);
                ph_bajo += fbajo / rate;
                ph_bajo2 += fbajo * 0.5f / rate;
                x += pb(&y_bajo, sierra(ph_bajo) + 0.6f * pulso(ph_bajo2, 0.5f), a_bajo) * envolvente(te, 2 * semi, 0.004f, 0.7f, 0.03f) * 0.20f * (0.5f + 0.5f * bombeo);
            }
            /* bateria: bombo en cada tiempo, caja con cola de reverberacion cortada, platillo en contratiempo */
            if (tb < 0.18f) x += sinf(2 * (float)M_PI * (48.0f * tb + 110.0f / 35.0f * (1.0f - expf(-tb * 35.0f)))) * expf(-tb * 16.0f) * 0.36f;
            if (paso == 4 || paso == 12) {
                if (ts < 0.04f) x += (ruido() * 0.6f + 0.4f * sinf(2 * (float)M_PI * 200.0f * ts)) * 0.30f;
            }
            {
                int ps = (paso >= 12) ? paso - 12 : (paso >= 4 ? paso - 4 : -1);
                float tc = ps >= 0 ? ps * semi + ts : -1;
                if (tc >= 0.04f && tc < 0.30f) x += ruido() * 0.20f * (1.0f - (tc - 0.04f) / 0.26f);   /* cola cortada */
            }
            if ((paso & 3) == 2 && ts < 0.09f) x += (ruido() - pb(&y_hat, ruido(), 0.5f)) * expf(-ts * 40.0f) * 0.16f;
            out[i] = a16(x * 0.85f);
        }
    }
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
