/* track.c - generacion procedural de pistas originales */
#include "track.h"

#define RAW_MAX 6000
#define CHUNK_SEGS 24

static V3 raw[RAW_MAX];
static int nraw;

static const char *NAMES[TRACK_COUNT] = { "CIRCUITO OCHO", "FORTALEZA AZUL", "VALLE DEL TUNEL", "BURGOS", "MONTE FUJI", "CASTELLOLI", "SUPEROVALO", "COSTA ALTA", "PARQUE TUCAN",
    "DESIERTO SONORA", "SELVA MAYA", "LOS VOLCANES", "CAMPOS DE AGAVE", "BAJA CALIFORNIA", "CUMBRES NEVADAS", "PARQUE FUNDIDORA", "VILLA OLIMPICA", "PARQUE ACUATICO", "EL SEMINARIO", "ISLA DEL SOL", "ESPINAZO", "CAMPOS DEL TREN", "BOSQUE DENSO" };

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

/* Burgos: trazado dibujado a mano por el jugador (x, z, altura) */
static const float BURGOS_CP[][3] = {
    {-32, -175, 2},
    {-41, -265, 4},
    {-67, -330, 6},
    {-75, -365, 7},
    {-45, -385, 7},
    {64, -377, 7},
    {167, -343, 6},
    {212, -272, 5},
    {221, -156, 4},
    {229, -40, 3},
    {251, 56, 2},
    {328, 121, 1},
    {405, 179, 0},
    {431, 243, 0},
    {392, 297, 0},
    {283, 311, 0},
    {129, 300, 0},
    {-39, 262, 0},
    {-139, 238, 0},
    {-178, 198, 0},
    {-213, 170, 0},
    {-296, 137, 0},
    {-380, 114, 0},
    {-424, 104, 0},
    {-428, 78, 0},
    {-402, 64, 0},
    {-354, 60, 0},
    {-238, 9, 1},
    {-129, -53, 1},
    {-58, -111, 1}
};

/* Monte Fuji: trazado dibujado a mano por el jugador (x, z, altura) */
static const float FUJI_CP[][3] = {
    {-321, -162, 11},
    {-47, -164, 8},
    {227, -167, 5},
    {447, -169, 3},
    {529, -166, 2},
    {565, -136, 2},
    {545, -81, 2},
    {480, -10, 1},
    {348, 83, 0},
    {194, 154, 0},
    {52, 187, 0},
    {-58, 185, 0},
    {-107, 149, 1},
    {-91, 100, 1},
    {-25, 75, 2},
    {96, 67, 2},
    {194, 50, 3},
    {236, 6, 3},
    {222, -65, 4},
    {151, -98, 4},
    {41, -100, 4},
    {-69, -76, 5},
    {-157, -21, 6},
    {-222, 56, 7},
    {-299, 138, 8},
    {-376, 182, 9},
    {-464, 160, 10},
    {-541, 89, 11},
    {-560, -10, 12},
    {-527, -103, 12},
    {-464, -151, 12}
};

/* Castelloli: trazado dibujado a mano por el jugador (x, z, altura) */
static const float CAST_CP[][3] = {
    {-153, -70, 4},
    {-153, -231, 9},
    {-153, -407, 14},
    {-133, -508, 16},
    {-52, -555, 16},
    {43, -548, 15},
    {70, -501, 14},
    {43, -407, 11},
    {43, -326, 9},
    {97, -258, 9},
    {107, -177, 8},
    {56, -110, 6},
    {-25, -63, 5},
    {-85, -16, 4},
    {-99, 38, 3},
    {-58, 99, 2},
    {29, 126, 1},
    {164, 108, 1},
    {272, 92, 1},
    {359, 133, 0},
    {396, 207, 0},
    {373, 281, 0},
    {292, 319, 0},
    {177, 321, 0},
    {117, 313, 0},
    {70, 340, 0},
    {9, 332, 0},
    {-52, 308, 0},
    {-160, 297, 1},
    {-267, 288, 1},
    {-305, 240, 2},
    {-315, 160, 2},
    {-294, 108, 3},
    {-234, 62, 3},
    {-180, 5, 4}
};

/* Superovalo con infield y Costa Alta: dibujados a mano por el hermano del jugador */
static const float OVALO_CP[][3] = {
    {-167, -235, 0},
    {-164, -116, 0},
    {-172, 23, 0},
    {-182, 143, 0},
    {-170, 243, 0},
    {-137, 307, 0},
    {-102, 340, 0},
    {-57, 302, 0},
    {-20, 292, 0},
    {-0, 330, 0},
    {-8, 384, 0},
    {23, 392, 0},
    {77, 352, 0},
    {119, 284, 0},
    {135, 203, 0},
    {127, 139, 0},
    {67, 137, 0},
    {18, 131, 0},
    {16, 43, 0},
    {24, -42, 0},
    {87, -54, 0},
    {165, -66, 0},
    {171, -156, 0},
    {172, -285, 0},
    {169, -395, 0},
    {142, -455, 0},
    {77, -482, 0},
    {-12, -486, 0},
    {-92, -474, 0},
    {-144, -435, 0},
    {-164, -365, 0}
};

static const float COSTA_CP[][3] = {
    {-394, 136, 2},
    {-402, -39, 3},
    {-388, -174, 5},
    {-330, -258, 7},
    {-222, -326, 10},
    {-87, -380, 13},
    {68, -418, 16},
    {203, -431, 18},
    {312, -423, 19},
    {374, -393, 20},
    {390, -342, 20},
    {363, -301, 21},
    {306, -291, 21},
    {224, -301, 22},
    {136, -301, 22},
    {71, -283, 23},
    {49, -201, 24},
    {41, -91, 25},
    {129, -80, 26},
    {209, -66, 26},
    {220, 48, 26},
    {214, 158, 25},
    {122, 169, 24},
    {14, 177, 23},
    {4, 285, 18},
    {1, 420, 11},
    {-23, 544, 6},
    {-80, 602, 4},
    {-175, 623, 3},
    {-269, 609, 2},
    {-326, 558, 2},
    {-369, 460, 2},
    {-385, 312, 2}
};

/* Parque Tucan (Monterrey): trazado tomado de un mapa de OpenStreetMap y agrandado a escala */
static const float TUCAN_CP[][3] = {
    {115, -230, 3.7},
    {160, -267, 4.9},
    {208, -287, 6.0},
    {282, -295, 7.5},
    {370, -290, 9.4},
    {413, -274, 10.0},
    {413, -249, 9.7},
    {381, -231, 8.7},
    {329, -220, 7.5},
    {272, -204, 6.1},
    {216, -156, 4.6},
    {183, -91, 3.4},
    {154, -27, 2.4},
    {92, 31, 1.4},
    {-5, 63, 1.2},
    {-72, 88, 1.9},
    {-108, 132, 2.7},
    {-131, 193, 3.6},
    {-156, 248, 4.6},
    {-219, 304, 6.4},
    {-306, 337, 8.7},
    {-367, 350, 10.0},
    {-401, 327, 10.0},
    {-411, 276, 10.0},
    {-390, 222, 8.8},
    {-345, 175, 7.2},
    {-283, 163, 5.9},
    {-218, 158, 4.6},
    {-175, 121, 3.6},
    {-111, 70, 2.2},
    {-39, -2, 1.1},
    {7, -77, 1.3},
    {57, -156, 2.3},
    {87, -200, 3.0}
};
/* conversion de pixeles del mapa a metros del juego */
#define TUC_X(px) (((px) - 478.00f) * 1.6372f)
#define TUC_Z(py) (((py) - 282.32f) * 1.6372f)

/* cinco trazados dibujados a mano por el jugador, ambientados en Mexico */
static const float SONORA_CP[][3] = {
    {-44, 291, 0.0},
    {-144, 175, 1.0},
    {-253, 41, 2.0},
    {-353, -89, 3.0},
    {-433, -208, 4.0},
    {-461, -278, 4.0},
    {-413, -292, 4.0},
    {-357, -224, 4.0},
    {-293, -168, 3.0},
    {-184, -158, 3.0},
    {-44, -198, 4.0},
    {126, -240, 5.0},
    {242, -196, 5.0},
    {306, -99, 4.0},
    {282, -53, 4.0},
    {176, -59, 4.0},
    {56, -77, 3.0},
    {18, -53, 3.0},
    {62, -21, 3.0},
    {182, 3, 2.0},
    {286, 75, 1.0},
    {342, 195, 0.0},
    {358, 295, 0.0},
    {306, 339, 0.0},
    {186, 331, 0.0},
    {70, 347, 0.0},
    {-18, 321, 0.0}
};
static const float MAYA_CP[][3] = {
    {-234, -367, 3.0},
    {-110, -375, 3.0},
    {30, -344, 4.0},
    {146, -313, 4.0},
    {173, -220, 3.0},
    {224, -127, 2.0},
    {247, -2, 2.0},
    {224, 75, 1.0},
    {201, 169, 1.0},
    {209, 262, 0.0},
    {154, 355, 0.0},
    {92, 402, 0.0},
    {22, 456, 0.0},
    {-63, 371, 0.0},
    {-172, 262, 1.0},
    {-273, 200, 1.0},
    {-250, 145, 1.0},
    {-110, 138, 2.0},
    {45, 114, 2.0},
    {146, 75, 2.0},
    {123, -33, 3.0},
    {100, -142, 3.0},
    {61, -189, 4.0},
    {-63, -173, 4.0},
    {-172, -127, 4.0},
    {-242, -95, 4.0},
    {-258, -204, 4.0},
    {-250, -313, 3.0}
};
static const float VOLCAN_CP[][3] = {
    {-27, 141, 2.0},
    {-48, 17, 5.0},
    {-70, -132, 9.0},
    {-93, -297, 13.0},
    {-98, -430, 16.0},
    {-56, -487, 17.0},
    {43, -496, 18.0},
    {150, -443, 19.0},
    {216, -388, 19.0},
    {200, -344, 18.0},
    {117, -339, 17.0},
    {30, -377, 16.0},
    {-20, -377, 15.0},
    {-43, -281, 13.0},
    {-27, -173, 11.0},
    {59, -119, 10.0},
    {122, -41, 8.0},
    {159, 99, 6.0},
    {205, 248, 4.0},
    {238, 389, 2.0},
    {238, 496, 1.0},
    {175, 533, 0.0},
    {43, 516, 0.0},
    {-114, 521, 0.0},
    {-192, 480, 0.0},
    {-271, 380, 0.0},
    {-367, 273, 0.0},
    {-321, 228, 1.0},
    {-189, 209, 1.0},
    {-60, 192, 1.0}
};
static const float AGAVE_CP[][3] = {
    {-120, 70, 2.0},
    {-314, 67, 2.0},
    {-464, 67, 2.0},
    {-546, 37, 3.0},
    {-551, -53, 4.0},
    {-516, -135, 5.0},
    {-389, -225, 6.0},
    {-284, -180, 5.0},
    {-187, -143, 5.0},
    {-105, -173, 6.0},
    {-15, -218, 7.0},
    {83, -270, 8.0},
    {120, -390, 9.0},
    {153, -430, 9.0},
    {183, -382, 9.0},
    {198, -278, 8.0},
    {240, -188, 7.0},
    {344, -83, 5.0},
    {442, 22, 4.0},
    {457, 127, 3.0},
    {389, 201, 2.0},
    {285, 261, 1.0},
    {240, 351, 0.0},
    {135, 359, 0.0},
    {15, 321, 0.0},
    {-75, 276, 0.0},
    {-82, 201, 0.0},
    {-15, 179, 1.0},
    {127, 162, 1.0},
    {162, 135, 1.0},
    {135, 115, 1.0},
    {15, 109, 1.0},
    {-60, 91, 2.0}
};
static const float BAJA_CP[][3] = {
    {-385, -717, 30.0},
    {-329, -650, 30.0},
    {-329, -448, 28.0},
    {-261, -403, 26.0},
    {-138, -358, 24.0},
    {-37, -223, 22.0},
    {64, -133, 20.0},
    {71, -43, 18.0},
    {53, 13, 16.0},
    {98, 92, 14.0},
    {143, 204, 11.0},
    {177, 339, 8.0},
    {300, 372, 6.0},
    {502, 357, 4.0},
    {619, 402, 3.0},
    {597, 469, 3.0},
    {502, 456, 3.0},
    {435, 451, 3.0},
    {300, 429, 4.0},
    {188, 395, 6.0},
    {53, 294, 9.0},
    {-104, 253, 12.0},
    {-228, 215, 14.0},
    {-295, 69, 17.0},
    {-356, -66, 20.0},
    {-363, -223, 23.0},
    {-407, -358, 26.0},
    {-441, -515, 28.0},
    {-430, -672, 30.0}
};

/* dibujado por el jugador: recta plana abajo, lado sur mas alto y cordillera nevada al sur */
static const float CUMBRES_CP[][3] = {
    {-222, -218, 0.0},
    {-27, -218, 0.0},
    {183, -218, 0.0},
    {392, -218, 0.0},
    {587, -218, 0.0},
    {662, -191, 0.2},
    {635, -146, 1.6},
    {594, -111, 2.7},
    {527, -56, 4.7},
    {377, -56, 4.7},
    {257, -63, 4.4},
    {201, 4, 6.8},
    {145, 104, 10.7},
    {96, 161, 12.9},
    {-12, 188, 14.0},
    {-72, 251, 16.5},
    {-124, 326, 19.5},
    {-162, 374, 21.5},
    {-237, 383, 21.9},
    {-327, 344, 20.3},
    {-414, 269, 17.2},
    {-449, 191, 14.1},
    {-446, 116, 11.1},
    {-409, 64, 9.1},
    {-327, 12, 7.1},
    {-192, -18, 6.0},
    {-42, -39, 5.2},
    {25, -54, 4.7},
    {60, -81, 3.7},
    {36, -114, 2.6},
    {-72, -126, 2.2},
    {-222, -135, 1.9},
    {-327, -141, 1.7},
    {-364, -165, 0.9},
    {-330, -201, 0.0}
};

/* Parque Fundidora (Monterrey): antiguo autodromo, trazado tomado de una foto satelital */
static const float FUND_CP[][3] = {
    {-131, -310, 1.0},
    {-16, -304, 1.0},
    {178, -293, 1.0},
    {374, -277, 1.0},
    {467, -240, 1.0},
    {497, -190, 1.0},
    {497, -118, 1.0},
    {489, -79, 1.0},
    {407, -64, 1.0},
    {277, -73, 1.0},
    {139, -79, 1.0},
    {73, -64, 1.0},
    {21, -31, 1.0},
    {11, 14, 1.0},
    {48, 76, 1.0},
    {114, 195, 1.0},
    {155, 284, 1.0},
    {217, 335, 1.0},
    {238, 354, 1.0},
    {203, 387, 1.0},
    {-20, 389, 1.0},
    {-210, 348, 1.0},
    {-404, 336, 1.0},
    {-478, 303, 1.0},
    {-461, 272, 1.0},
    {-385, 241, 1.0},
    {-362, 212, 1.0},
    {-352, 135, 1.0},
    {-329, -66, 1.0},
    {-307, -254, 1.0},
    {-296, -351, 1.0},
    {-269, -388, 1.0},
    {-201, -372, 1.0},
    {-185, -330, 1.0}
};
#define FUN_X(px) (((px) - 307.29f) * 1.9419f)
#define FUN_Z(py) (((py) - 229.74f) * 1.9419f)

/* dibujado por el jugador, inspirado en un parque deportivo de San Bernabe */
static const float VILLA_CP[][3] = {
    {-60, -627, 1.0},
    {65, -552, 1.0},
    {190, -444, 1.0},
    {337, -341, 1.0},
    {431, -268, 1.0},
    {463, -199, 1.0},
    {432, -143, 1.0},
    {390, -95, 1.0},
    {273, -100, 1.0},
    {196, -110, 1.0},
    {65, -95, 1.0},
    {-46, -81, 1.0},
    {-60, -12, 1.0},
    {-56, 91, 1.0},
    {-46, 188, 1.0},
    {-4, 208, 1.0},
    {65, 224, 1.0},
    {80, 265, 1.0},
    {79, 404, 1.0},
    {68, 542, 1.0},
    {52, 563, 1.0},
    {-4, 571, 1.0},
    {-108, 570, 1.0},
    {-212, 573, 1.0},
    {-236, 546, 1.0},
    {-240, 473, 1.0},
    {-243, 265, 1.0},
    {-241, 120, 1.0},
    {-191, 78, 1.0},
    {-116, 36, 1.0},
    {-105, -26, 1.0},
    {-112, -88, 1.0},
    {-160, -136, 1.0},
    {-185, -199, 1.0},
    {-174, -268, 1.0},
    {-157, -359, 1.0},
    {-172, -455, 1.0},
    {-149, -528, 1.0},
    {-108, -592, 1.0}
};

/* dibujado por el jugador: parque acuatico en la falda de un cerro (norte alto, sur bajo) */
static const float AZTLAN_CP[][3] = {
    {-375, -336, 13.54},
    {-328, -259, 12.68},
    {-282, -166, 11.24},
    {-243, -88, 9.75},
    {-236, 28, 7.30},
    {-219, 185, 4.00},
    {-181, 301, 1.92},
    {-134, 394, 0.72},
    {-64, 488, 0.07},
    {-9, 527, 0.00},
    {37, 519, 0.00},
    {131, 473, 0.13},
    {247, 410, 0.56},
    {341, 364, 1.06},
    {355, 301, 1.92},
    {302, 208, 3.55},
    {278, 107, 5.61},
    {286, -10, 8.12},
    {310, -126, 10.51},
    {300, -242, 12.46},
    {270, -313, 13.32},
    {170, -336, 13.54},
    {68, -337, 13.54},
    {-26, -391, 13.89},
    {-96, -442, 14.00},
    {-219, -441, 14.00},
    {-313, -434, 14.00},
    {-371, -383, 13.85}
};
#define AZT_ZMIN (-442.3f)
#define AZT_ZMAX (527.0f)
#define AZT_H (14.0f)

/* calles alrededor del seminario (Monterrey), tomadas de una foto satelital y agrandadas a escala */
static const float SEMI_CP[][3] = {
    {-332, -213, 1.3},
    {-226, -215, 1.3},
    {-116, -220, 1.3},
    {23, -246, 1.2},
    {146, -274, 1.2},
    {264, -309, 1.1},
    {395, -332, 1.0},
    {484, -287, 1.1},
    {531, -248, 1.2},
    {531, -172, 1.4},
    {496, -98, 1.6},
    {417, -20, 1.9},
    {273, 64, 2.1},
    {143, 140, 2.3},
    {29, 209, 2.5},
    {-78, 286, 2.7},
    {-167, 341, 2.9},
    {-195, 374, 2.9},
    {-231, 385, 3.0},
    {-267, 364, 2.9},
    {-292, 341, 2.9},
    {-358, 258, 2.6},
    {-371, 133, 2.3},
    {-371, 18, 2.0},
    {-371, -103, 1.6},
    {-356, -177, 1.4}
};
#define SEM_X(px) (((px) - 290.19f) * 1.6490f)
#define SEM_Z(py) (((py) - 319.27f) * 1.6490f)

/* dibujado por el jugador: circuito en una isla con hoteles de playa */
static const float ISLA_CP[][3] = {
    {486, 195, 1.5},
    {402, 310, 2.9},
    {288, 421, 3.8},
    {179, 527, 3.9},
    {79, 618, 3.2},
    {-37, 614, 1.9},
    {-103, 560, 0.4},
    {-128, 444, -0.7},
    {-145, 368, -1.0},
    {-202, 309, -0.4},
    {-294, 202, 0.8},
    {-311, 104, 2.3},
    {-319, 20, 3.5},
    {-357, -87, 4.0},
    {-369, -163, 3.6},
    {-336, -220, 2.5},
    {-286, -270, 1.1},
    {-205, -271, -0.2},
    {-101, -295, -1.0},
    {-77, -353, -0.8},
    {-88, -436, 0.2},
    {-112, -517, 1.6},
    {-70, -560, 3.0},
    {-26, -523, 3.9},
    {38, -436, 3.9},
    {121, -303, 3.1},
    {187, -195, 1.8},
    {171, -162, 0.3},
    {204, -87, -0.7},
    {237, -4, -1.0},
    {327, 22, -0.4},
    {403, 46, 0.9},
    {443, 120, 2.4}
};

/* dibujado por el jugador: cima de una cresta de montana con barrancos a ambos lados */
static const float ESPINAZO_CP[][3] = {
    {-173, -467, 150.0},
    {-217, -436, 157.7},
    {-255, -303, 153.5},
    {-168, -178, 153.0},
    {-104, -46, 155.9},
    {-93, 113, 147.6},
    {-122, 229, 141.1},
    {-253, 384, 146.8},
    {-271, 493, 148.2},
    {-184, 603, 146.2},
    {-130, 649, 154.5},
    {-39, 655, 159.5},
    {44, 649, 152.2},
    {157, 595, 150.2},
    {182, 462, 151.8},
    {163, 392, 143.8},
    {63, 243, 140.7},
    {24, 129, 149.4},
    {13, -54, 151.6},
    {15, -203, 150.0},
    {48, -303, 157.1},
    {152, -340, 158.3},
    {269, -381, 148.5},
    {307, -475, 146.6},
    {269, -511, 148.7},
    {198, -507, 142.7},
    {129, -456, 143.3},
    {30, -445, 153.7},
    {-54, -489, 154.9}
};

/* dibujado por el jugador: campo de cultivo con una via de tren junto a la recta superior */
static const float GRANJA_CP[][3] = {
    {-627, 216, 1.0},
    {-625, 89, 1.7},
    {-623, -29, 2.3},
    {-613, -78, 2.5},
    {-466, -112, 2.4},
    {-210, -161, 1.9},
    {-83, -195, 1.2},
    {45, -235, 0.5},
    {155, -292, -0.1},
    {257, -356, -0.5},
    {403, -442, -0.4},
    {544, -420, -0.1},
    {572, -343, 0.6},
    {530, -208, 1.3},
    {577, -139, 2.0},
    {556, -43, 2.4},
    {486, 12, 2.5},
    {341, 37, 2.2},
    {258, 91, 1.6},
    {234, 174, 0.9},
    {208, 286, 0.2},
    {129, 294, -0.3},
    {11, 296, -0.5},
    {-57, 238, -0.3},
    {-213, 224, 0.2},
    {-284, 231, 0.9},
    {-381, 292, 1.6},
    {-509, 291, 2.2},
    {-614, 282, 2.5}
};
/* extremos de la recta superior (via del tren) */
#define GRJ_A_X (-613.5f)
#define GRJ_A_Z (-78.2f)
#define GRJ_B_X (402.7f)
#define GRJ_B_Z (-442.1f)

/* dibujado por el jugador: camino entre un bosque muy denso */
static const float BOSQUE_CP[][3] = {
    {133, 232, 3.0},
    {76, 310, 5.8},
    {18, 386, 6.1},
    {-34, 434, 4.9},
    {-90, 400, 4.6},
    {-150, 324, 5.7},
    {-217, 205, 5.8},
    {-157, 137, 3.5},
    {-60, 83, 0.4},
    {45, 74, -0.5},
    {114, 62, 0.7},
    {151, 9, 1.6},
    {137, -50, 0.8},
    {105, -83, 0.3},
    {1, -44, 2.1},
    {-105, -15, 5.3},
    {-226, -45, 6.8},
    {-375, -62, 5.8},
    {-502, -74, 4.4},
    {-561, -142, 4.7},
    {-553, -196, 5.4},
    {-499, -247, 4.2},
    {-451, -268, 1.1},
    {-361, -252, -0.9},
    {-255, -269, -0.3},
    {-150, -260, 1.4},
    {-105, -250, 1.7},
    {-29, -276, 1.0},
    {46, -284, 1.7},
    {84, -247, 4.4},
    {137, -182, 6.9},
    {196, -150, 6.7},
    {305, -154, 4.9},
    {372, -129, 3.9},
    {396, -81, 4.5},
    {381, -7, 4.4},
    {366, 32, 2.1},
    {373, 99, -0.7},
    {386, 174, -1.1},
    {361, 208, 0.7},
    {302, 214, 2.2},
    {226, 191, 2.0},
    {174, 194, 1.8}
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
            Col kc = tr->tunnel[i] ? col(90, 90, 96) : ((i & 1) ? tr->curb_a : tr->curb_b);
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
        } else if (!tr->terrain && !tr->tunnel[i] && (tr->p[i].y > 0.3f || tr->p[j].y > 0.3f)) {
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

/* ---------- Burgos: catedral gotica, casco antiguo, rio, castillo y meseta ---------- */

/* punto local: c + r*x + arriba*y + f*z */
static V3 lp(V3 c, V3 r, V3 f, float x, float y, float z) {
    return v3(c.x + r.x * x + f.x * z, c.y + y, c.z + r.z * x + f.z * z);
}

/* tejado a dos aguas sobre un rectangulo (cumbrera a lo largo de f) */
static void gable_roof(Mesh *m, V3 c, V3 r, V3 f, float hw, float hl, float y0, float rise, Col roof, Col wall) {
    V3 a0 = lp(c, r, f, -hw - 0.4f, y0, -hl - 0.4f), a1 = lp(c, r, f, -hw - 0.4f, y0, hl + 0.4f);
    V3 b0 = lp(c, r, f, hw + 0.4f, y0, -hl - 0.4f), b1 = lp(c, r, f, hw + 0.4f, y0, hl + 0.4f);
    V3 t0 = lp(c, r, f, 0, y0 + rise, -hl - 0.4f), t1 = lp(c, r, f, 0, y0 + rise, hl + 0.4f);
    mesh_quad(m, a0, a1, t1, t0, roof);
    mesh_quad(m, b0, t0, t1, b1, col_shade(roof, 0.85f));
    mesh_tri(m, lp(c, r, f, -hw, y0, -hl), lp(c, r, f, hw, y0, -hl), lp(c, r, f, 0, y0 + rise, -hl), wall);
    mesh_tri(m, lp(c, r, f, -hw, y0, hl), lp(c, r, f, hw, y0, hl), lp(c, r, f, 0, y0 + rise, hl), wall);
}

/* aguja gotica calada: prisma octogonal fino + pinaculos */
static void spire(Mesh *m, V3 base, float r, float h, Col stone) {
    int k;
    mesh_cone(m, base, r, h, 8, stone);
    for (k = 0; k < 4; k++) {
        float a = (float)M_PI * 0.25f + k * (float)M_PI * 0.5f;
        mesh_cone(m, v3(base.x + cosf(a) * r * 1.1f, base.y, base.z + sinf(a) * r * 1.1f), r * 0.22f, h * 0.22f, 4, stone);
    }
}

static void cathedral(Mesh *m, V3 c, V3 f) {
    V3 r = v3(-f.z, 0, f.x);
    Col stone = col(228, 218, 192), stone2 = col(206, 194, 166), slate = col(96, 102, 118), dark = col(52, 46, 46);
    float NL = 34, NW = 9, NH = 21;   /* nave: medio largo, medio ancho, alto */
    int k;
    /* nave central y naves laterales */
    mesh_obox(m, c, r, f, NW, NL, NH, stone, stone2);
    gable_roof(m, c, r, f, NW, NL, NH, 8, slate, stone);
    mesh_obox(m, lp(c, r, f, -NW - 5, 0, 0), r, f, 5, NL - 2, 12, stone2, slate);
    mesh_obox(m, lp(c, r, f, NW + 5, 0, 0), r, f, 5, NL - 2, 12, stone2, slate);
    /* contrafuertes y pinaculos a lo largo de la nave */
    for (k = -3; k <= 3; k++) {
        float z = k * 9.0f;
        int s;
        for (s = -1; s <= 1; s += 2) {
            mesh_obox(m, lp(c, r, f, s * (NW + 10.5f), 0, z), r, f, 0.9f, 1.2f, 15, stone, stone);
            mesh_cone(m, lp(c, r, f, s * (NW + 10.5f), 15, z), 1.0f, 4.5f, 4, stone);
        }
    }
    /* crucero */
    mesh_obox(m, lp(c, r, f, 0, 0, -6), f, r, 7, 24, NH, stone, stone2);
    gable_roof(m, lp(c, r, f, 0, 0, -6), f, r, 7, 24, NH, 7, slate, stone);
    /* cimborrio octogonal sobre el crucero */
    {
        V3 cb = lp(c, r, f, 0, NH + 5, -6);
        mesh_prism(m, cb, 8.0f, 14.0f, 8, stone, stone2);
        mesh_cone(m, vadd(cb, v3(0, 14, 0)), 8.5f, 7.0f, 8, slate);
        for (k = 0; k < 8; k++) {
            float a = 2 * (float)M_PI * k / 8;
            mesh_cone(m, v3(cb.x + cosf(a) * 8.3f, cb.y + 14, cb.z + sinf(a) * 8.3f), 0.8f, 6.0f, 4, stone);
        }
    }
    /* cabecera (abside poligonal) */
    {
        V3 ap = lp(c, r, f, 0, 0, -NL);
        mesh_prism(m, ap, 9.0f, 18.0f, 8, stone, stone2);
        mesh_cone(m, vadd(ap, v3(0, 18, 0)), 9.5f, 6.0f, 8, slate);
    }
    /* fachada: dos torres con agujas caladas */
    {
        int s;
        for (s = -1; s <= 1; s += 2) {
            V3 tb = lp(c, r, f, s * 7.0f, 0, NL + 4);
            mesh_obox(m, tb, r, f, 5.0f, 5.0f, 44, stone, stone2);
            /* balaustrada */
            mesh_obox(m, vadd(tb, v3(0, 44, 0)), r, f, 5.6f, 5.6f, 1.6f, stone2, stone2);
            spire(m, vadd(tb, v3(0, 45.6f, 0)), 4.2f, 30.0f, stone);
            /* ventanales */
            mesh_quad_flat(m, lp(c, r, f, s * 7.0f - 1.6f, 26, NL + 9.06f), lp(c, r, f, s * 7.0f + 1.6f, 26, NL + 9.06f),
                           lp(c, r, f, s * 7.0f + 1.6f, 38, NL + 9.06f), lp(c, r, f, s * 7.0f - 1.6f, 38, NL + 9.06f), dark);
        }
        /* cuerpo central de la fachada, rosetón y portada */
        mesh_obox(m, lp(c, r, f, 0, 0, NL + 4), r, f, 2.2f, 5.0f, 30, stone, stone2);
        {
            V3 rc = lp(c, r, f, 0, 20, NL + 9.06f);
            for (k = 0; k < 12; k++) {
                float a0 = 2 * (float)M_PI * k / 12, a1 = 2 * (float)M_PI * (k + 1) / 12;
                mesh_tri_flat(m, rc, vadd(rc, vadd(vscale(r, cosf(a0) * 3.2f), v3(0, sinf(a0) * 3.2f, 0))),
                              vadd(rc, vadd(vscale(r, cosf(a1) * 3.2f), v3(0, sinf(a1) * 3.2f, 0))), (k & 1) ? col(60, 70, 120) : col(150, 60, 60));
            }
        }
        mesh_quad_flat(m, lp(c, r, f, -2.4f, 0, NL + 9.08f), lp(c, r, f, 2.4f, 0, NL + 9.08f),
                       lp(c, r, f, 2.4f, 9, NL + 9.08f), lp(c, r, f, -2.4f, 9, NL + 9.08f), dark);
        mesh_tri_flat(m, lp(c, r, f, -2.4f, 9, NL + 9.08f), lp(c, r, f, 2.4f, 9, NL + 9.08f), lp(c, r, f, 0, 12, NL + 9.08f), dark);
    }
    /* plaza empedrada delante */
    mesh_quad_flat(m, lp(c, r, f, -22, 0.05f, NL + 9), lp(c, r, f, 22, 0.05f, NL + 9),
                   lp(c, r, f, 22, 0.05f, NL + 30), lp(c, r, f, -22, 0.05f, NL + 30), col(186, 176, 156));
}

/* casa castellana: muros claros y tejado de teja */
static void casa(Mesh *m, Rng *rg, V3 c, V3 f) {
    static const int walls[][3] = {{236, 228, 206}, {222, 206, 174}, {240, 236, 226}, {214, 190, 150}, {228, 214, 190}};
    const int *w = walls[rng_u(rg) % 5];
    V3 r = v3(-f.z, 0, f.x);
    float hw = rng_range(rg, 4, 7), hl = rng_range(rg, 5, 9), h = rng_range(rg, 7, 13);
    Col wall = col(w[0], w[1], w[2]);
    Col roof = col_shade(col(178, 88, 56), rng_range(rg, 0.85f, 1.1f));
    float y;
    mesh_obox(m, c, r, f, hw, hl, h, wall, wall);
    gable_roof(m, c, r, f, hw, hl, h, rng_range(rg, 2.5f, 4.0f), roof, wall);
    /* hileras de balcones / ventanas en la fachada larga */
    for (y = 2.5f; y < h - 1.5f; y += 3.2f) {
        int s;
        for (s = -1; s <= 1; s += 2)
            mesh_quad_flat(m, lp(c, r, f, s * (hw + 0.05f), y, -hl + 1.5f), lp(c, r, f, s * (hw + 0.05f), y, hl - 1.5f),
                           lp(c, r, f, s * (hw + 0.05f), y + 1.3f, hl - 1.5f), lp(c, r, f, s * (hw + 0.05f), y + 1.3f, -hl + 1.5f), col(70, 64, 60));
    }
}

/* gran puerta-arco de la muralla, con torreones */
static void arco_puerta(Mesh *m, V3 c, V3 f) {
    V3 r = v3(-f.z, 0, f.x);
    Col stone = col(222, 210, 180), stone2 = col(200, 188, 158), dark = col(46, 40, 40);
    int k;
    mesh_obox(m, c, r, f, 14, 4, 24, stone, stone2);
    for (k = -3; k <= 3; k++) {
        if (k == 0) continue;
        mesh_prism(m, lp(c, r, f, k * 4.2f, 24, 3.2f), 1.5f, 6.0f, 6, stone, stone2);
        mesh_cone(m, lp(c, r, f, k * 4.2f, 30, 3.2f), 1.7f, 2.0f, 6, stone2);
    }
    mesh_obox(m, lp(c, r, f, 0, 24, 0), r, f, 4, 4, 9, stone, stone2);
    for (k = -1; k <= 1; k += 2) {
        mesh_quad_flat(m, lp(c, r, f, -4, 0, k * 4.06f), lp(c, r, f, 4, 0, k * 4.06f), lp(c, r, f, 4, 10, k * 4.06f), lp(c, r, f, -4, 10, k * 4.06f), dark);
        mesh_tri_flat(m, lp(c, r, f, -4, 10, k * 4.06f), lp(c, r, f, 4, 10, k * 4.06f), lp(c, r, f, 0, 13, k * 4.06f), dark);
    }
    /* lienzos de muralla a los lados */
    mesh_obox(m, lp(c, r, f, -32, 0, 0), r, f, 18, 2.2f, 11, stone2, stone2);
    mesh_obox(m, lp(c, r, f, 32, 0, 0), r, f, 18, 2.2f, 11, stone2, stone2);
}

static void alamo(Mesh *m, V3 p, float s) {
    Col g = col(92, 150, 64);
    V3 c = vadd(p, v3(0, 6.5f * s, 0));
    V3 top = vadd(p, v3(0, 13.0f * s, 0)), bot = vadd(p, v3(0, 2.0f * s, 0));
    int k;
    mesh_prism(m, p, 0.3f * s, 2.5f * s, 4, col(150, 140, 120), col(150, 140, 120));
    for (k = 0; k < 5; k++) {
        float a0 = 2 * (float)M_PI * k / 5, a1 = 2 * (float)M_PI * (k + 1) / 5, R = 1.7f * s;
        V3 e0 = vadd(c, v3(cosf(a0) * R, 0, sinf(a0) * R)), e1 = vadd(c, v3(cosf(a1) * R, 0, sinf(a1) * R));
        mesh_tri(m, e0, e1, top, g);
        mesh_tri(m, e1, e0, bot, col_shade(g, 0.8f));
    }
}

static void aerogenerador(Mesh *m, V3 p, float yaw, float rot) {
    V3 f = v3(sinf(yaw), 0, -cosf(yaw)), r = v3(-f.z, 0, f.x);
    Col white = col(240, 240, 244);
    V3 hub = vadd(p, v3(0, 62, 0));
    int k;
    mesh_prism(m, p, 1.6f, 62, 6, white, white);
    mesh_obox(m, vsub(hub, v3(0, 1.5f, 0)), r, f, 1.6f, 4.0f, 3.0f, white, white);
    hub = vadd(hub, vscale(f, 4.2f));
    for (k = 0; k < 3; k++) {
        float a = rot + k * 2.0944f;
        V3 dir = vadd(vscale(r, cosf(a)), v3(0, sinf(a), 0));
        V3 side = vadd(vscale(r, -sinf(a)), v3(0, cosf(a), 0));
        V3 tip = vadd(hub, vscale(dir, 28));
        mesh_tri(m, vadd(hub, vscale(side, 1.2f)), vsub(hub, vscale(side, 1.2f)), tip, white);
    }
}

static void scenery_burgos(Track *tr) {
    Rng rg = {1221};
    int k, tries, ci;
    V3 c = tr->center;
    /* catedral en el interior del circuito, mirando a la recta de meta */
    {
        V3 tgt = tr->p[0];
        float dmin = min_dist_xz(tr, c, &ci);
        V3 f = vnorm(v3(tgt.x - c.x, 0, tgt.z - c.z));
        (void)dmin;
        c.y = 0;
        cathedral(chunk_mesh(tr, ci), c, f);
    }
    /* casco antiguo: casas alrededor de la catedral (dentro) y fuera del circuito */
    for (k = 0, tries = 0; k < 70 && tries < 2000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        float side = (rng_u(&rg) & 1) ? 1.0f : -1.0f;
        V3 q = at(tr, i, side * (tr->wall + rng_range(&rg, 14, 70)), 0);
        float dc;
        q.y = 0;
        dc = sqrtf((q.x - tr->center.x) * (q.x - tr->center.x) + (q.z - tr->center.z) * (q.z - tr->center.z));
        if (dc < 75.0f) continue;                      /* no tapar la catedral */
        if (min_dist_xz(tr, q, &ni) < tr->wall + 13.0f) continue;
        casa(chunk_mesh(tr, ni), &rg, q, tr->t[ni]);
        k++;
    }
    /* arco de entrada a la ciudad junto a la meta */
    {
        int i = 18 % tr->n;
        V3 q = at(tr, i, -(tr->wall + 26.0f), 0);
        q.y = 0;
        if (place_ok(tr, q, 22)) arco_puerta(chunk_mesh(tr, i), q, tr->r[i]);
    }
    /* rio con alamos al sur del circuito */
    {
        float zr = 0, xmin = 1e9f, xmax = -1e9f, x;
        int i;
        Mesh *m;
        for (i = 0; i < tr->n; i++) {
            if (tr->p[i].z > zr) zr = tr->p[i].z;
            if (tr->p[i].x < xmin) xmin = tr->p[i].x;
            if (tr->p[i].x > xmax) xmax = tr->p[i].x;
        }
        zr += 70.0f;
        min_dist_xz(tr, v3((xmin + xmax) * 0.5f, 0, zr), &ci);
        m = chunk_mesh(tr, ci);
        for (x = xmin - 300; x < xmax + 300; x += 40) {
            float z0 = zr + 10 * sinf(x * 0.01f), z1 = zr + 10 * sinf((x + 40) * 0.01f);
            mesh_quad_flat(m, v3(x, 0.06f, z0 - 11), v3(x + 40, 0.06f, z1 - 11), v3(x + 40, 0.06f, z1 + 11), v3(x, 0.06f, z0 + 11), col(72, 124, 168));
            mesh_quad_flat(m, v3(x, 0.04f, z0 - 15), v3(x + 40, 0.04f, z1 - 15), v3(x + 40, 0.04f, z1 - 11), v3(x, 0.04f, z0 - 11), col(150, 140, 110));
            mesh_quad_flat(m, v3(x, 0.04f, z0 + 11), v3(x + 40, 0.04f, z1 + 11), v3(x + 40, 0.04f, z1 + 15), v3(x, 0.04f, z0 + 15), col(150, 140, 110));
        }
        for (x = xmin - 280; x < xmax + 280; x += 17) {
            float z0 = zr + 10 * sinf(x * 0.01f);
            int s;
            for (s = -1; s <= 1; s += 2) {
                V3 q = v3(x + rng_range(&rg, -3, 3), 0, z0 + s * 19);
                int ni;
                if (min_dist_xz(tr, q, &ni) < tr->wall + 4) continue;
                alamo(chunk_mesh(tr, ni), q, rng_range(&rg, 0.9f, 1.3f));
            }
        }
        /* puente de piedra sobre el rio */
        {
            float xb = (xmin + xmax) * 0.5f + 120, zb = zr + 10 * sinf(xb * 0.01f);
            for (k = 0; k < 3; k++) {
                mesh_box(m, v3(xb - 5, 0, zb - 16 + k * 11), v3(xb + 5, 2.6f, zb - 13 + k * 11), col(200, 186, 150), col(200, 186, 150));
            }
            mesh_box(m, v3(xb - 5, 2.6f, zb - 18), v3(xb + 5, 3.6f, zb + 18), col(200, 186, 150), col(184, 170, 136));
        }
    }
    /* cerro del castillo al noroeste */
    {
        V3 q = v3(tr->center.x - tr->extent * 0.85f, -1, tr->center.z - tr->extent * 0.9f);
        int ni;
        if (min_dist_xz(tr, q, &ni) > tr->wall + 75) {
            Mesh *m = chunk_mesh(tr, ni);
            Col st = col(196, 180, 146);
            mesh_cone(m, q, 70, 32, 9, col(140, 150, 84));
            q.y = 28;
            mesh_box(m, v3(q.x - 16, q.y, q.z - 12), v3(q.x + 16, q.y + 9, q.z - 9), st, st);
            mesh_box(m, v3(q.x - 16, q.y, q.z + 9), v3(q.x + 16, q.y + 7, q.z + 12), st, st);
            mesh_box(m, v3(q.x - 16, q.y, q.z - 12), v3(q.x - 13, q.y + 8, q.z + 12), st, st);
            mesh_prism(m, v3(q.x + 13, q.y, q.z - 9), 4.0f, 14, 6, st, st);
            mesh_prism(m, v3(q.x - 13, q.y, q.z + 9), 3.5f, 11, 6, st, st);
        }
    }
    /* aerogeneradores en los paramos lejanos */
    for (k = 0; k < 9; k++) {
        float a = 2.6f + k * 0.17f;
        float R = tr->extent + 280.0f + (k & 1) * 60.0f;
        V3 q = v3(tr->center.x + cosf(a) * R, 0, tr->center.z + sinf(a) * R);
        int ni;
        min_dist_xz(tr, q, &ni);
        aerogenerador(chunk_mesh(tr, ni), q, a + 1.4f, k * 0.7f);
    }
    /* encinas sueltas en los campos */
    scatter_trees(tr, &rg, 70, 0, 10, 90);
}

/* ---------- Monte Fuji: autodromo con gradas, boxes, bosque y el volcan al fondo ---------- */


/* ---------- colocacion segura: nada de escenografia sobre la pista ---------- */

static const Track *g_cur;   /* pista que se esta construyendo */
static void grada_raw(Mesh *m, Rng *rg, V3 c, V3 r, V3 f, float hl);
static void boxes_raw(Mesh *m, V3 c, V3 r, V3 f, float hl);

/* revisa toda la superficie de un objeto rectangular (x a lo largo de r, z a lo largo de f) */
static int footprint_ok(const Track *tr, V3 c, V3 r, V3 f, float x0, float x1, float hl, float margin) {
    float x, z;
    for (x = x0; x <= x1 + 0.01f; x += 3.0f)
        for (z = -hl; z <= hl + 0.01f; z += 3.0f) {
            V3 q = v3(c.x + r.x * x + f.x * z, 0, c.z + r.z * x + f.z * z);
            if (min_dist_xz(tr, q, NULL) < tr->wall + margin) return 0;
        }
    return 1;
}

/* gradas: si en una curva invaden la pista, se acortan; si no caben, no se ponen */
static void grada(Mesh *m, Rng *rg, V3 c, V3 r, V3 f, float hl) {
    r = vnorm(v3(r.x, 0, r.z)); f = vnorm(v3(f.x, 0, f.z));
    for (; hl >= 12.0f; hl -= 6.0f)
        if (footprint_ok(g_cur, c, r, f, -15.0f, 1.2f, hl, 1.2f)) { grada_raw(m, rg, c, r, f, hl); return; }
}

static void boxes_y_torre(Mesh *m, V3 c, V3 r, V3 f, float hl) {
    r = vnorm(v3(r.x, 0, r.z)); f = vnorm(v3(f.x, 0, f.z));
    for (; hl >= 20.0f; hl -= 8.0f)
        if (footprint_ok(g_cur, c, r, f, -9.5f, 20.0f, hl, 1.0f)) { boxes_raw(m, c, r, f, hl); return; }
}

static void grada_raw(Mesh *m, Rng *rg, V3 c, V3 r, V3 f, float hl) {
    /* r apunta hacia la pista; las gradas suben alejandose de ella */
    int s, k;
    Col conc = col(200, 200, 206), seat = col(48, 92, 170);
    for (s = 0; s < 6; s++) {
        V3 b = lp(c, r, f, -s * 2.2f, 0, 0);
        mesh_obox(m, b, r, f, 1.1f, hl, 1.0f + s * 1.5f, conc, seat);
        for (k = 0; k < 10; k++) {
            static const int pc[][3] = {{250, 220, 60}, {240, 240, 245}, {230, 80, 80}, {60, 150, 240}, {90, 200, 120}, {250, 150, 60}};
            const int *cc = pc[rng_u(rg) % 6];
            V3 q = lp(c, r, f, -s * 2.2f, 1.0f + s * 1.5f, -hl + 1.5f + k * (2 * hl - 3) / 9.0f);
            mesh_obox(m, q, r, f, 0.45f, 0.9f, 0.8f, col(cc[0], cc[1], cc[2]), col(cc[0], cc[1], cc[2]));
        }
    }
    /* tejado sobre pilares */
    for (k = -1; k <= 1; k += 2) mesh_obox(m, lp(c, r, f, -13.5f, 0, k * (hl - 1)), r, f, 0.35f, 0.35f, 14.0f, conc, conc);
    {
        V3 a = lp(c, r, f, 1.5f, 13.0f, -hl), b = lp(c, r, f, 1.5f, 13.0f, hl);
        V3 cc = lp(c, r, f, -14.0f, 14.5f, hl), d = lp(c, r, f, -14.0f, 14.5f, -hl);
        mesh_quad(m, a, b, cc, d, col(230, 232, 236));
    }
}

static void boxes_raw(Mesh *m, V3 c, V3 r, V3 f, float hl) {
    /* r apunta hacia la pista */
    Col wall = col(236, 236, 240), dark = col(46, 52, 62), glass = col(60, 110, 160);
    int k;
    mesh_obox(m, c, r, f, 9.0f, hl, 9.0f, wall, col(180, 184, 190));
    for (k = 0; k < (int)(hl / 6.0f) * 2 - 1; k++) {
        float z = -hl + 4.0f + k * 6.0f;
        mesh_quad_flat(m, lp(c, r, f, 9.06f, 0, z), lp(c, r, f, 9.06f, 0, z + 4.4f), lp(c, r, f, 9.06f, 4.2f, z + 4.4f), lp(c, r, f, 9.06f, 4.2f, z), dark);
    }
    mesh_quad_flat(m, lp(c, r, f, 9.07f, 6.0f, -hl + 1), lp(c, r, f, 9.07f, 6.0f, hl - 1), lp(c, r, f, 9.07f, 8.0f, hl - 1), lp(c, r, f, 9.07f, 8.0f, -hl + 1), glass);
    /* calle de boxes */
    mesh_quad_flat(m, lp(c, r, f, 9.0f, 0.03f, -hl), lp(c, r, f, 9.0f, 0.03f, hl), lp(c, r, f, 20.0f, 0.03f, hl), lp(c, r, f, 20.0f, 0.03f, -hl), col(100, 100, 108));
    /* torre de control */
    {
        V3 t = lp(c, r, f, -2.0f, 0, -hl * 0.35f);
        mesh_obox(m, t, r, f, 5.0f, 5.0f, 30.0f, wall, wall);
        mesh_obox(m, vadd(t, v3(0, 30.0f, 0)), r, f, 7.0f, 7.0f, 6.0f, glass, col(200, 40, 40));
        mesh_obox(m, vadd(t, v3(0, 36.0f, 0)), r, f, 7.5f, 7.5f, 1.0f, col(200, 40, 40), col(220, 220, 226));
    }
}

static void pasarela(Mesh *m, V3 c, V3 r, V3 f, float half) {
    Col red = col(196, 44, 44), grey = col(170, 170, 176);
    int s;
    mesh_obox(m, vadd(c, v3(0, 7.5f, 0)), r, f, half, 2.0f, 2.2f, red, grey);
    for (s = -1; s <= 1; s += 2) mesh_obox(m, lp(c, r, f, s * (half - 1.5f), 0, 0), r, f, 1.2f, 1.6f, 7.5f, grey, grey);
}

static void add_fuji(Track *tr) {
    Mesh *m = &tr->world;
    V3 base = v3(tr->center.x + tr->extent + 1500.0f, -2, tr->center.z - 700.0f);
    float R = 1500.0f, H = 900.0f;
    int k, S = 18;
    Col rock = col(92, 104, 140), snow = col(244, 246, 252);
    for (k = 0; k < S; k++) {
        float a0 = 2 * (float)M_PI * k / S, a1 = 2 * (float)M_PI * (k + 1) / S;
        float rr = 0.32f + 0.05f * ((k * 7) % 3);     /* borde de nieve irregular */
        V3 p0 = v3(base.x + cosf(a0) * R, base.y, base.z + sinf(a0) * R);
        V3 p1 = v3(base.x + cosf(a1) * R, base.y, base.z + sinf(a1) * R);
        /* la cima es truncada, como un crater */
        V3 q0 = v3(base.x + cosf(a0) * R * 0.09f, H, base.z + sinf(a0) * R * 0.09f);
        V3 q1 = v3(base.x + cosf(a1) * R * 0.09f, H, base.z + sinf(a1) * R * 0.09f);
        V3 s0 = vlerp(q0, p0, rr), s1 = vlerp(q1, p1, rr + 0.04f * ((k & 1) ? 1 : -1));
        Col rk = col_shade(rock, 0.9f + 0.15f * ((k & 1) ? 1.0f : 0.0f));
        mesh_quad_flat(m, s0, s1, p1, p0, rk);
        mesh_quad_flat(m, q0, q1, s1, s0, (k & 1) ? snow : col(226, 230, 240));
        mesh_tri_flat(m, q0, q1, v3(base.x, H - 15, base.z), col(210, 214, 224));
    }
}

static void scenery_fuji(Track *tr) {
    Rng rg = {3776};
    int k, tries;
    /* gradas en el exterior de la recta principal y boxes en el interior */
    {
        int i0 = 6;
        V3 rr = tr->r[i0];
        V3 ff = tr->t[i0];
        V3 out, in;
        float side_out = (rr.x * (tr->p[i0].x - tr->center.x) + rr.z * (tr->p[i0].z - tr->center.z)) > 0 ? 1.0f : -1.0f;
        ff = vnorm(v3(ff.x, 0, ff.z));
        out = at(tr, i0 + 20, side_out * (tr->wall + 4.0f), 0); out.y = tr->p[i0 + 20].y;
        in = at(tr, i0 + 14, -side_out * (tr->wall + 22.0f), 0); in.y = tr->p[i0 + 14].y;
        grada(chunk_mesh(tr, i0 + 20), &rg, out, vscale(rr, -side_out), ff, 55.0f);
        grada(chunk_mesh(tr, i0 + 55), &rg, at(tr, i0 + 55, side_out * (tr->wall + 4.0f), tr->p[i0 + 55].y - tr->p[i0 + 55].y), vscale(rr, -side_out), vnorm(v3(tr->t[i0 + 55].x, 0, tr->t[i0 + 55].z)), 50.0f);
        boxes_y_torre(chunk_mesh(tr, i0 + 14), in, vscale(rr, side_out), ff, 70.0f);
        /* pasarela sobre la recta */
        {
            int ib = 140;
            V3 pc = tr->p[ib];
            pasarela(chunk_mesh(tr, ib), pc, vnorm(v3(tr->r[ib].x, 0, tr->r[ib].z)), vnorm(v3(tr->t[ib].x, 0, tr->t[ib].z)), tr->wall + 3.0f);
        }
    }
    /* colinas boscosas en el interior y alrededor */
    for (k = 0, tries = 0; k < 16 && tries < 500; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        float side = (rng_u(&rg) & 1) ? 1.0f : -1.0f;
        float R = rng_range(&rg, 35, 80);
        V3 q = at(tr, i, side * (tr->wall + R + rng_range(&rg, 15, 90)), 0);
        q.y = -0.5f;
        if (min_dist_xz(tr, q, &ni) < tr->wall + R + 10) continue;
        mesh_cone(chunk_mesh(tr, ni), q, R, rng_range(&rg, 12, 30), 8, (k & 1) ? col(58, 118, 62) : col(66, 128, 66));
        k++;
    }
    /* bosque de cedros y arces */
    scatter_trees(tr, &rg, 260, 1, 8, 110);
    scatter_trees(tr, &rg, 90, 0, 8, 80);
}

/* ---------- Castelloli: monte bajo mediterraneo, terrazas y las agujas de Montserrat ---------- */

static void pino_pinonero(Mesh *m, V3 p, float s) {
    Col g = col(70, 118, 56);
    V3 c = vadd(p, v3(0, 6.5f * s, 0));
    int k;
    mesh_prism(m, p, 0.3f * s, 6.0f * s, 5, col(120, 86, 60), col(120, 86, 60));
    for (k = 0; k < 7; k++) {
        float a0 = 2 * (float)M_PI * k / 7, a1 = 2 * (float)M_PI * (k + 1) / 7, R = 3.6f * s;
        V3 e0 = vadd(c, v3(cosf(a0) * R, 0, sinf(a0) * R)), e1 = vadd(c, v3(cosf(a1) * R, 0, sinf(a1) * R));
        mesh_tri(m, e0, e1, vadd(c, v3(0, 1.8f * s, 0)), g);
        mesh_tri(m, e1, e0, vadd(c, v3(0, -0.9f * s, 0)), col_shade(g, 0.75f));
    }
}

static void matorral(Mesh *m, V3 p, float s) {
    mesh_cone(m, p, 1.6f * s, 1.4f * s, 5, col(110, 124, 70));
}

static void add_montserrat(Track *tr) {
    Mesh *m = &tr->world;
    Rng rg = {1025};
    float dirx = -0.55f, dirz = -0.83f;    /* hacia el noroeste */
    float D = tr->extent + 1150.0f;
    V3 c = v3(tr->center.x + dirx * D, -2, tr->center.z + dirz * D);
    V3 side = v3(-dirz, 0, dirx);
    int k;
    /* zocalo del macizo */
    mesh_cone(m, c, 900.0f, 160.0f, 10, col(120, 120, 92));
    /* agujas redondeadas de roca */
    for (k = 0; k < 34; k++) {
        float t = rng_range(&rg, -1, 1);
        float h = 160.0f + (1.0f - fabsf(t)) * rng_range(&rg, 150, 330);
        float R = rng_range(&rg, 22, 46);
        V3 b = vadd(c, vadd(vscale(side, t * 700.0f), v3(dirx * rng_range(&rg, -160, 160), 80, dirz * rng_range(&rg, -160, 160))));
        Col rock = col_shade(col(196, 164, 144), rng_range(&rg, 0.85f, 1.08f));
        mesh_prism(m, b, R, h, 7, rock, rock);
        mesh_cone(m, vadd(b, v3(0, h, 0)), R, R * 0.9f, 7, col_shade(rock, 1.05f));
    }
}

static void scenery_castelloli(Track *tr) {
    Rng rg = {808};
    int k, tries;
    /* paddock con boxes y una grada en la ladera junto a la recta de meta */
    {
        int i0 = 10;
        V3 rr = tr->r[i0], ff = vnorm(v3(tr->t[i0].x, 0, tr->t[i0].z));
        float side_out = (rr.x * (tr->p[i0].x - tr->center.x) + rr.z * (tr->p[i0].z - tr->center.z)) > 0 ? 1.0f : -1.0f;
        V3 in = at(tr, i0 + 8, -side_out * (tr->wall + 22.0f), 0);
        V3 out = at(tr, i0 + 18, side_out * (tr->wall + 4.0f), 0);
        boxes_y_torre(chunk_mesh(tr, i0 + 8), in, vscale(rr, side_out), ff, 45.0f);
        grada(chunk_mesh(tr, i0 + 18), &rg, out, vscale(rr, -side_out), ff, 35.0f);
    }
    /* masias (casas de campo de piedra) */
    for (k = 0, tries = 0; k < 7 && tries < 400; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 30, 90)), 0);
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 16) continue;
        casa(chunk_mesh(tr, ni), &rg, q, tr->t[ni]);
        k++;
    }
    /* colinas en terrazas */
    for (k = 0, tries = 0; k < 14 && tries < 500; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        float R = rng_range(&rg, 45, 95), H = rng_range(&rg, 14, 34);
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + R + rng_range(&rg, 20, 120)), 0);
        int s;
        q.y = -0.5f;
        if (min_dist_xz(tr, q, &ni) < tr->wall + R + 10) continue;
        for (s = 0; s < 3; s++) {
            float rr = R * (1.0f - s * 0.3f);
            mesh_prism(chunk_mesh(tr, ni), vadd(q, v3(0, s * H / 3, 0)), rr, H / 3, 9, col(170, 140, 96), (s & 1) ? col(150, 150, 80) : col(132, 140, 72));
        }
        k++;
    }
    /* pinos y matorral */
    for (k = 0, tries = 0; k < 160 && tries < 3000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 6, 120)), 0);
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 5) continue;
        if (k % 3 == 0) pino_pinonero(chunk_mesh(tr, ni), q, rng_range(&rg, 0.9f, 1.3f));
        else matorral(chunk_mesh(tr, ni), q, rng_range(&rg, 0.8f, 1.6f));
        k++;
    }
}

/* ---------- Superovalo: estadio con gradas gigantes, torres de luz e infield ---------- */

static void torre_luz(Mesh *m, V3 p) {
    Col steel = col(150, 150, 160);
    int k;
    mesh_prism(m, p, 0.7f, 34.0f, 4, steel, steel);
    mesh_box(m, v3(p.x - 4, 34, p.z - 0.6f), v3(p.x + 4, 38, p.z + 0.6f), steel, steel);
    for (k = 0; k < 4; k++)
        mesh_box(m, v3(p.x - 3.6f + k * 2.0f, 34.6f, p.z - 0.7f), v3(p.x - 2.2f + k * 2.0f, 37.4f, p.z - 0.62f), col(255, 250, 210), col(255, 250, 210));
}

static void scenery_ovalo(Track *tr) {
    Rng rg = {2025};
    int i, k, tries;
    /* gradas por todo el exterior del ovalo (no en la zona del infield) */
    for (i = 4; i < tr->n; i += 34) {
        V3 rr = tr->r[i], ff = vnorm(v3(tr->t[i].x, 0, tr->t[i].z));
        V3 rel = vsub(tr->p[i], tr->center);
        float side_out = (rr.x * rel.x + rr.z * rel.z) > 0 ? 1.0f : -1.0f;
        V3 q = at(tr, i, side_out * (tr->wall + 4.0f), 0);
        V3 probe = at(tr, i, side_out * (tr->wall + 20.0f), 0);
        probe.y = 0;
        if (min_dist_xz(tr, probe, NULL) < tr->wall + 12.0f) continue;   /* hay pista detras: es el infield */
        grada(chunk_mesh(tr, i), &rg, q, vscale(rr, -side_out), ff, 48.0f);
        if ((i / 34) % 2 == 0) torre_luz(chunk_mesh(tr, i), at(tr, i, side_out * (tr->wall + 22.0f), 0));
    }
    /* boxes en el interior de la recta principal */
    {
        int i0 = 8;
        V3 rr = tr->r[i0], ff = vnorm(v3(tr->t[i0].x, 0, tr->t[i0].z));
        V3 rel = vsub(tr->p[i0], tr->center);
        float side_in = (rr.x * rel.x + rr.z * rel.z) > 0 ? -1.0f : 1.0f;
        boxes_y_torre(chunk_mesh(tr, i0 + 20), at(tr, i0 + 20, side_in * (tr->wall + 22.0f), 0), vscale(rr, -side_in), ff, 60.0f);
    }
    /* estacionamiento con autos de aficionados fuera del estadio */
    for (k = 0, tries = 0; k < 120 && tries < 3000; tries++) {
        int ii = rng_u(&rg) % tr->n, ni;
        V3 rr = tr->r[ii];
        V3 rel = vsub(tr->p[ii], tr->center);
        float side_out = (rr.x * rel.x + rr.z * rel.z) > 0 ? 1.0f : -1.0f;
        V3 q = at(tr, ii, side_out * (tr->wall + rng_range(&rg, 45, 110)), 0);
        static const int pc[][3] = {{200, 40, 40}, {40, 80, 180}, {230, 230, 235}, {30, 30, 34}, {220, 190, 40}, {90, 140, 90}};
        const int *cc = pc[rng_u(&rg) % 6];
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 30) continue;
        mesh_obox(chunk_mesh(tr, ni), q, rr, tr->t[ii], 0.9f, 2.1f, 1.3f, col(cc[0], cc[1], cc[2]), col_shade(col(cc[0], cc[1], cc[2]), 0.7f));
        k++;
    }
    scatter_trees(tr, &rg, 40, 0, 120, 200);
}

/* ---------- Costa Alta: ciudad costera montanosa ---------- */

static void palmera(Mesh *m, V3 p, float s) {
    Col trunk = col(150, 118, 80), leaf = col(56, 140, 64);
    V3 top = vadd(p, v3(0.8f * s, 8.0f * s, 0));
    int k;
    mesh_quad(m, vadd(p, v3(-0.3f * s, 0, 0)), vadd(p, v3(0.3f * s, 0, 0)), vadd(top, v3(0.2f * s, 0, 0)), vadd(top, v3(-0.2f * s, 0, 0)), trunk);
    mesh_quad(m, vadd(p, v3(0, 0, -0.3f * s)), vadd(p, v3(0, 0, 0.3f * s)), vadd(top, v3(0, 0, 0.2f * s)), vadd(top, v3(0, 0, -0.2f * s)), trunk);
    for (k = 0; k < 6; k++) {
        float a = k * 1.047f;
        V3 d = v3(cosf(a), 0, sinf(a));
        V3 side = v3(-d.z, 0, d.x);
        V3 mid = vadd(top, vadd(vscale(d, 2.6f * s), v3(0, 0.4f * s, 0)));
        V3 tip = vadd(top, vadd(vscale(d, 4.6f * s), v3(0, -1.4f * s, 0)));
        mesh_tri(m, top, vadd(mid, vscale(side, 0.9f * s)), tip, leaf);
        mesh_tri(m, top, vsub(mid, vscale(side, 0.9f * s)), tip, col_shade(leaf, 0.85f));
    }
}

static void casa_costera(Mesh *m, Rng *rg, V3 c, V3 f, float base) {
    static const int pal[][3] = {{240, 150, 160}, {250, 210, 90}, {120, 190, 230}, {90, 200, 180}, {245, 245, 240}, {240, 160, 80}, {180, 140, 210}};
    const int *w = pal[rng_u(rg) % 7];
    V3 r = v3(-f.z, 0, f.x);
    float hw = rng_range(rg, 3.5f, 6), hl = rng_range(rg, 4, 7), h = rng_range(rg, 6, 11);
    Col wall = col(w[0], w[1], w[2]);
    if (base > 0.3f) mesh_obox(m, c, r, f, hw + 0.6f, hl + 0.6f, base, col(160, 150, 130), col(160, 150, 130));
    c.y += base;
    mesh_obox(m, c, r, f, hw, hl, h, wall, wall);
    if (rng_u(rg) & 1) gable_roof(m, c, r, f, hw, hl, h, 2.5f, col(186, 92, 60), wall);
    else mesh_obox(m, vadd(c, v3(0, h, 0)), r, f, hw + 0.2f, hl + 0.2f, 0.6f, col(235, 235, 230), col(235, 235, 230));
    {
        int s;
        for (s = -1; s <= 1; s += 2)
            mesh_quad_flat(m, lp(c, r, f, s * (hw + 0.05f), h * 0.45f, -hl + 1.2f), lp(c, r, f, s * (hw + 0.05f), h * 0.45f, hl - 1.2f),
                           lp(c, r, f, s * (hw + 0.05f), h * 0.45f + 1.4f, hl - 1.2f), lp(c, r, f, s * (hw + 0.05f), h * 0.45f + 1.4f, -hl + 1.2f), col(60, 70, 90));
    }
}

static void barco(Mesh *m, V3 p, float yaw, Col c) {
    V3 f = v3(sinf(yaw), 0, -cosf(yaw)), r = v3(-f.z, 0, f.x);
    mesh_obox(m, p, r, f, 1.6f, 4.5f, 1.2f, c, col(220, 210, 180));
    mesh_obox(m, vadd(p, v3(0, 1.2f, 0)), r, f, 1.0f, 1.5f, 1.2f, col(240, 240, 240), col(240, 240, 240));
    mesh_prism(m, vadd(lp(p, r, f, 0, 0, -1.0f), v3(0, 1.2f, 0)), 0.1f, 7.0f, 3, col(200, 200, 200), col(200, 200, 200));
    mesh_tri(m, lp(p, r, f, 0, 2.0f, -1.0f), lp(p, r, f, 0, 8.0f, -1.0f), lp(p, r, f, 0, 2.0f, 2.5f), col(250, 250, 250));
}

static void scenery_costa(Track *tr) {
    Rng rg = {1966};
    int k, tries, i;
    float zmin = 1e9f, zmax = -1e9f, z;
    for (i = 0; i < tr->n; i++) { if (tr->p[i].z < zmin) zmin = tr->p[i].z; if (tr->p[i].z > zmax) zmax = tr->p[i].z; }
    /* mar, playa y paseo de palmeras al oeste */
    for (z = zmin - 500; z < zmax + 500; z += 40) {
        float coast = 1e9f;
        int ci = 0;
        Mesh *m;
        for (i = 0; i < tr->n; i++) {
            if (fabsf(tr->p[i].z - (z + 20)) < 60 && tr->p[i].x < coast) { coast = tr->p[i].x; ci = i; }
        }
        if (coast > 1e8f) {   /* fuera del alcance de la pista: usar la costa mas cercana */
            float best = 1e9f;
            for (i = 0; i < tr->n; i++) if (tr->p[i].x < best) { best = tr->p[i].x; ci = i; }
            coast = best;
        }
        coast -= tr->wall + 30.0f;
        m = chunk_mesh(tr, ci);
        mesh_quad_flat(m, v3(coast, 0.05f, z), v3(coast, 0.05f, z + 40), v3(coast - 30, 0.05f, z + 40), v3(coast - 30, 0.05f, z), col(232, 214, 160));
        mesh_quad_flat(m, v3(coast - 30, 0.1f, z), v3(coast - 30, 0.1f, z + 40), v3(coast - 1800, 0.1f, z + 40), v3(coast - 1800, 0.1f, z), col(40, 120, 190));
        mesh_quad_flat(m, v3(coast - 30, 0.12f, z), v3(coast - 30, 0.12f, z + 40), v3(coast - 36, 0.12f, z + 40), v3(coast - 36, 0.12f, z), col(220, 236, 245));
        if (((int)(z / 40)) % 1 == 0) {
            V3 q = v3(coast + 4, 0, z + 20);
            int ni;
            if (min_dist_xz(tr, q, &ni) > tr->wall + 4) palmera(chunk_mesh(tr, ni), q, rng_range(&rg, 0.9f, 1.25f));
        }
        if (rng_f(&rg) < 0.18f) barco(m, v3(coast - rng_range(&rg, 60, 300), 0.1f, z + 20), rng_range(&rg, 0, 6.28f),
                                      ((rng_u(&rg) & 1) ? col(240, 240, 240) : col(200, 60, 50)));
        if (z > zmin - 60 && z < zmin - 20) {
            /* faro en la punta norte */
            V3 fq = v3(coast - 8, 0, z);
            mesh_prism(m, fq, 3.0f, 26, 8, col(245, 245, 245), col(245, 245, 245));
            mesh_prism(m, vadd(fq, v3(0, 8, 0)), 3.05f, 4, 8, col(200, 40, 40), col(200, 40, 40));
            mesh_prism(m, vadd(fq, v3(0, 18, 0)), 3.05f, 4, 8, col(200, 40, 40), col(200, 40, 40));
            mesh_prism(m, vadd(fq, v3(0, 26, 0)), 2.2f, 3, 8, col(255, 240, 150), col(60, 60, 64));
            mesh_cone(m, vadd(fq, v3(0, 29, 0)), 2.6f, 2.5f, 8, col(200, 40, 40));
        }
    }
    /* casas de colores escalonadas en las laderas */
    for (k = 0, tries = 0; k < 150 && tries < 5000; tries++) {
        int ii = rng_u(&rg) % tr->n, ni;
        float side = (rng_u(&rg) & 1) ? 1.0f : -1.0f;
        float dd = tr->wall + rng_range(&rg, 10, 75);
        V3 q = at(tr, ii, side * dd, 0);
        float h = tr->p[ii].y;
        float base;
        if (h < 3.0f && rng_f(&rg) < 0.6f) continue;
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 11) continue;
        /* al alejarse de la pista la ladera sube hacia la montana (este) o baja hacia el mar (oeste) */
        base = fmaxf(0.0f, h + (q.x - tr->p[ii].x) * 0.12f);
        casa_costera(chunk_mesh(tr, ni), &rg, q, tr->t[ni], base);
        k++;
    }
    scatter_trees(tr, &rg, 80, 1, 20, 120);
}

static void add_montes_costa(Track *tr) {
    Mesh *m = &tr->world;
    Rng rg = {77};
    int k;
    float xmax = -1e9f;
    for (k = 0; k < tr->n; k++) if (tr->p[k].x > xmax) xmax = tr->p[k].x;
    for (k = 0; k < 9; k++) {
        float z = tr->center.z - tr->extent + k * tr->extent * 0.25f;
        V3 b = v3(xmax + rng_range(&rg, 200, 420), -1, z);
        float R = rng_range(&rg, 160, 280), H = rng_range(&rg, 140, 260);
        mesh_cone(m, b, R, H, 9, (k & 1) ? col(70, 120, 70) : col(84, 132, 76));
        mesh_cone(m, vadd(b, v3(0, H * 0.7f, 0)), R * 0.3f, H * 0.3f, 9, col(150, 140, 120));
    }
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

/* ---------- Parque Tucan: parque hundido entre colonias, con el cerro de roca detras ---------- */

static int inside_loop(const Track *tr, float x, float z) {
    int i, j, c = 0;
    for (i = 0, j = tr->n - 1; i < tr->n; j = i, i += 1) {
        V3 a = tr->p[i], b = tr->p[j];
        if (((a.z > z) != (b.z > z)) && (x < (b.x - a.x) * (z - a.z) / (b.z - a.z + 1e-9f) + a.x)) c = !c;
    }
    return c;
}

static float smooth01(float t) { t = clampf(t, 0, 1); return t * t * (3 - 2 * t); }

/* altura del terreno: el parque es un valle; afuera sube hasta el nivel de la calle */
static float slope_h(float z) {
    return AZT_H * smooth01((AZT_ZMAX - z) / (AZT_ZMAX - AZT_ZMIN));
}

static float terrain_h(const Track *tr, float x, float z) {
    int ni;
    float d = min_dist_xz(tr, v3(x, 0, z), &ni);
    float ty = tr->p[ni].y - 0.15f;
    if (tr->terrain == 2) return lerpf(ty, slope_h(z) - 0.15f, smooth01((d - tr->wall - 4.0f) / 26.0f));
    if (tr->terrain == 3) {
        if (inside_loop(tr, x, z)) return ty - 0.15f;
        return lerpf(ty, tr->street_y, smooth01((d - tr->wall - 1.0f) / 90.0f));
    }
    if (inside_loop(tr, x, z)) return ty - 0.03f * fmaxf(0.0f, d - tr->wall) - 0.2f * smooth01((d - tr->wall) / 30.0f);
    return lerpf(ty, tr->street_y, smooth01((d - tr->wall - 0.6f) / 40.0f));
}

static Col terrain_col(const Track *tr, float h, int alt) {
    float t;
    if (tr->terrain == 3) {   /* roca en los barrancos, bosque en el valle, pasto en la cima */
        if (h > 140.0f) return alt ? col(118, 150, 80) : col(110, 142, 76);
        if (h > 30.0f) return alt ? col(150, 128, 104) : col(138, 118, 96);
        return alt ? col(64, 112, 62) : col(58, 104, 58);
    }
    if (tr->terrain == 2) {   /* ladera de pasto con zonas de tierra en lo alto */
        if (h > AZT_H * 0.7f) return alt ? col(132, 160, 84) : col(124, 152, 80);
        return alt ? col(106, 164, 80) : col(98, 156, 76);
    }
    t = clampf(h / tr->street_y, 0, 1);
    Col grass = alt ? col(104, 156, 72) : col(96, 148, 68);
    Col slope = alt ? col(128, 140, 84) : col(120, 134, 80);
    if (t > 0.97f) return alt ? col(176, 170, 156) : col(164, 160, 148);   /* calles y lotes */
    return t < 0.35f ? grass : slope;
}

static void build_world_terrain(Track *tr) {
    Mesh *m = &tr->world;
    float T = 100.0f, R = tr->extent + 450.0f, S = tr->street_y;
    int N = (int)(R / T) + 1, x, z;
    for (x = -N; x < N; x++) {
        for (z = -N; z < N; z++) {
            float x0 = tr->center.x + x * T, z0 = tr->center.z + z * T;
            float dc;
            int ci;
            dc = min_dist_xz(tr, v3(x0 + T * 0.5f, 0, z0 + T * 0.5f), &ci);
            if ((tr->terrain == 1 || tr->terrain == 3) && dc > tr->wall + (tr->terrain == 3 ? 100.0f : 40.0f) + T * 0.75f && !inside_loop(tr, x0 + T * 0.5f, z0 + T * 0.5f)) {
                Col c = tr->terrain == 3 ? (((x + z) & 1) ? col(64, 112, 62) : col(58, 104, 58)) : (((x + z) & 1) ? col(176, 170, 156) : col(164, 160, 148));
                mesh_quad_flat(m, v3(x0, S, z0), v3(x0 + T, S, z0), v3(x0 + T, S, z0 + T), v3(x0, S, z0 + T), c);
            } else {
                /* celda con desnivel: malla de alturas de 10 m */
                const int K = (tr->terrain == 2 && dc > tr->wall + 60.0f + T * 0.75f && !inside_loop(tr, x0 + T * 0.5f, z0 + T * 0.5f)) ? 2 : 10;
                float hs[11][11];
                int a, b;
                float st = T / K;
                for (a = 0; a <= K; a++) for (b = 0; b <= K; b++) hs[a][b] = terrain_h(tr, x0 + a * st, z0 + b * st);
                for (a = 0; a < K; a++) {
                    for (b = 0; b < K; b++) {
                        float px = x0 + a * st, pz = z0 + b * st;
                        V3 p00 = v3(px, hs[a][b], pz), p10 = v3(px + st, hs[a + 1][b], pz);
                        V3 p11 = v3(px + st, hs[a + 1][b + 1], pz + st), p01 = v3(px, hs[a][b + 1], pz + st);
                        float hm = (hs[a][b] + hs[a + 1][b] + hs[a + 1][b + 1] + hs[a][b + 1]) * 0.25f;
                        int ni;
                        min_dist_xz(tr, v3(px + st * 0.5f, 0, pz + st * 0.5f), &ni);
                        mesh_quad(chunk_mesh(tr, ni), p00, p10, p11, p01, terrain_col(tr, hm, (a + b) & 1));
                    }
                }
            }
        }
    }
}

static void buganvilia(Mesh *m, V3 p, float s) {
    mesh_cone(m, p, 1.5f * s, 1.8f * s, 6, col(196, 40, 150));
    mesh_cone(m, vadd(p, v3(0.6f * s, 0, 0.4f * s)), 1.0f * s, 1.3f * s, 5, col(70, 130, 60));
}

static void add_cerros_mty(Track *tr) {
    Mesh *m = &tr->world;
    Rng rg = {1596};
    int k;
    /* cerro de roca al sur, muy cerca: ladera verde abajo y paredes grises arriba */
    {
        float zc = tr->center.z + tr->extent + 520.0f;
        float xs = tr->center.x - 1300.0f, xe = tr->center.x + 1300.0f;
        int S = 18;
        for (k = 0; k < S; k++) {
            float x0 = lerpf(xs, xe, (float)k / S), x1 = lerpf(xs, xe, (float)(k + 1) / S);
            float h0 = 520 + 260 * sinf(k * 0.9f) * sinf(k * 0.31f) + rng_range(&rg, -40, 60);
            float h1 = 520 + 260 * sinf((k + 1) * 0.9f) * sinf((k + 1) * 0.31f) + rng_range(&rg, -40, 60);
            V3 b0 = v3(x0, tr->street_y - 1, zc - 250), b1 = v3(x1, tr->street_y - 1, zc - 250);
            V3 m0 = v3(x0, tr->street_y + h0 * 0.35f, zc - 120), m1 = v3(x1, tr->street_y + h1 * 0.35f, zc - 120);
            V3 t0 = v3(x0, tr->street_y + h0, zc + 40), t1 = v3(x1, tr->street_y + h1, zc + 40);
            mesh_quad_flat(m, b0, b1, m1, m0, (k & 1) ? col(92, 112, 70) : col(100, 120, 74));
            mesh_quad_flat(m, m0, m1, t1, t0, (k & 1) ? col(138, 136, 132) : col(150, 148, 142));
            mesh_tri_flat(m, t0, t1, v3((x0 + x1) * 0.5f, tr->street_y + (h0 + h1) * 0.5f + rng_range(&rg, 20, 90), zc + 60), col(162, 160, 154));
        }
    }
    /* cerro de dos jorobas a lo lejos, al este */
    {
        float xc = tr->center.x + tr->extent + 1500.0f, zc = tr->center.z + 600.0f;
        static const float prof[] = {0, 160, 330, 420, 300, 250, 380, 460, 330, 150, 0};
        for (k = 0; k < 10; k++) {
            float z0 = zc - 900 + k * 180, z1 = z0 + 180;
            V3 a0 = v3(xc - 300, tr->street_y - 1, z0), a1 = v3(xc - 300, tr->street_y - 1, z1);
            V3 c0 = v3(xc, tr->street_y + prof[k], z0), c1 = v3(xc, tr->street_y + prof[k + 1], z1);
            mesh_quad_flat(m, a0, a1, c1, c0, (k & 1) ? col(112, 120, 128) : col(120, 128, 136));
        }
    }
}

static void scenery_tucan(Track *tr) {
    Rng rg = {64350};
    int k, tries;
    float S = tr->street_y;
    /* estanques y cancha de futbol en la parte ancha (suroeste) */
    {
        V3 e1 = v3(TUC_X(302), 0, TUC_Z(424)), e2 = v3(TUC_X(278), 0, TUC_Z(457));
        V3 cf = v3(TUC_X(342), 0, TUC_Z(410));
        int ni;
        float h;
        min_dist_xz(tr, e1, &ni);
        h = terrain_h(tr, e1.x, e1.z) + 0.08f;
        e1.y = h; mesh_prism(chunk_mesh(tr, ni), e1, 14.0f, 0.05f, 10, col(70, 140, 200), col(70, 140, 200));
        e2.y = terrain_h(tr, e2.x, e2.z) + 0.08f; mesh_prism(chunk_mesh(tr, ni), e2, 11.0f, 0.05f, 10, col(70, 140, 200), col(70, 140, 200));
        cf.y = terrain_h(tr, cf.x, cf.z) + 0.1f;
        min_dist_xz(tr, cf, &ni);
        mesh_quad_flat(chunk_mesh(tr, ni), vadd(cf, v3(-22, 0, -14)), vadd(cf, v3(22, 0, -14)), vadd(cf, v3(22, 0, 14)), vadd(cf, v3(-22, 0, 14)), col(70, 160, 80));
        mesh_quad_flat(chunk_mesh(tr, ni), vadd(cf, v3(-0.3f, 0.02f, -14)), vadd(cf, v3(0.3f, 0.02f, -14)), vadd(cf, v3(0.3f, 0.02f, 14)), vadd(cf, v3(-0.3f, 0.02f, 14)), col(240, 240, 240));
        for (k = -1; k <= 1; k += 2) mesh_box(chunk_mesh(tr, ni), vadd(cf, v3(k * 22 - 0.2f, 0, -3)), vadd(cf, v3(k * 22 + 0.2f, 2.4f, 3)), col(245, 245, 245), col(245, 245, 245));
    }
    /* juegos infantiles y kiosco en la parte central */
    {
        V3 j = v3(TUC_X(530), 0, TUC_Z(205));
        int ni;
        min_dist_xz(tr, j, &ni);
        j.y = terrain_h(tr, j.x, j.z);
        mesh_box(chunk_mesh(tr, ni), vadd(j, v3(-3, 0, -3)), vadd(j, v3(3, 0.15f, 3)), col(220, 190, 120), col(220, 190, 120));
        mesh_prism(chunk_mesh(tr, ni), vadd(j, v3(-2, 0, 0)), 0.2f, 3.0f, 4, col(230, 60, 50), col(230, 60, 50));
        mesh_prism(chunk_mesh(tr, ni), vadd(j, v3(2, 0, 0)), 0.2f, 3.0f, 4, col(230, 60, 50), col(230, 60, 50));
        mesh_box(chunk_mesh(tr, ni), vadd(j, v3(-2.2f, 3.0f, -0.2f)), vadd(j, v3(2.2f, 3.3f, 0.2f)), col(40, 110, 200), col(40, 110, 200));
        j = v3(TUC_X(580), 0, TUC_Z(168));
        min_dist_xz(tr, j, &ni);
        j.y = terrain_h(tr, j.x, j.z);
        mesh_prism(chunk_mesh(tr, ni), j, 6.0f, 3.2f, 8, col(230, 225, 210), col(200, 70, 50));
        mesh_cone(chunk_mesh(tr, ni), vadd(j, v3(0, 3.2f, 0)), 7.0f, 2.5f, 8, col(200, 70, 50));
    }
    /* buganvilias y arboles en las laderas */
    for (k = 0, tries = 0; k < 220 && tries < 5000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        float side = (rng_u(&rg) & 1) ? 1.0f : -1.0f;
        V3 q = at(tr, i, side * (tr->wall + rng_range(&rg, 2.5f, 40)), 0);
        float d = min_dist_xz(tr, q, &ni);
        if (d < tr->wall + 2.0f) continue;
        q.y = terrain_h(tr, q.x, q.z) - 0.1f;
        if (q.y > S - 0.5f) continue;
        if (k % 4 == 0) buganvilia(chunk_mesh(tr, ni), q, rng_range(&rg, 0.8f, 1.3f));
        else tree_round(chunk_mesh(tr, ni), q, rng_range(&rg, 0.9f, 1.5f));
        k++;
    }
    /* colonias a nivel de calle: casas de colores y un edificio de estacionamiento */
    for (k = 0, tries = 0; k < 170 && tries < 6000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        float side = (rng_u(&rg) & 1) ? 1.0f : -1.0f;
        V3 q = at(tr, i, side * (tr->wall + rng_range(&rg, 50, 160)), 0);
        float d = min_dist_xz(tr, q, &ni);
        if (d < tr->wall + 46.0f || inside_loop(tr, q.x, q.z)) continue;
        if (terrain_h(tr, q.x, q.z) < S - 0.3f) continue;
        q.y = 0;
        {
            /* las casas se alinean con la reticula de calles (diagonal del mapa) */
            V3 f = vnorm(v3(0.83f, 0, -0.55f));
            casa_costera(chunk_mesh(tr, ni), &rg, v3(q.x, S, q.z), f, 0);
        }
        k++;
    }
    {
        V3 q = v3(TUC_X(470), 0, TUC_Z(180));
        int ni;
        float d = min_dist_xz(tr, q, &ni);
        int tries2 = 0;
        while ((d < tr->wall + 50 || terrain_h(tr, q.x, q.z) < S - 0.3f) && tries2 < 40) {
            q.x -= 12; q.z -= 8; d = min_dist_xz(tr, q, &ni); tries2++;
        }
        q.y = S;
        mesh_obox(chunk_mesh(tr, ni), q, v3(0.83f, 0, -0.55f), v3(0.55f, 0, 0.83f), 40, 18, 12, col(220, 218, 210), col(150, 150, 154));
        for (k = 1; k <= 3; k++)
            mesh_obox(chunk_mesh(tr, ni), vadd(q, v3(0, k * 3.6f - 1.4f, 0)), v3(0.83f, 0, -0.55f), v3(0.55f, 0, 0.83f), 40.05f, 18.05f, 0.9f, col(60, 60, 66), col(60, 60, 66));
    }
}

/* ---------- Paisajes de Mexico ---------- */

static void saguaro(Mesh *m, V3 p, float s) {
    Col g = col(70, 128, 68);
    float h = 7.0f * s;
    mesh_prism(m, p, 0.45f * s, h, 6, g, col_shade(g, 1.1f));
    mesh_cone(m, vadd(p, v3(0, h, 0)), 0.45f * s, 0.5f * s, 6, g);
    /* brazos */
    mesh_obox(m, vadd(p, v3(0.9f * s, 2.6f * s, 0)), v3(1, 0, 0), v3(0, 0, 1), 0.6f * s, 0.32f * s, 0.6f * s, g, g);
    mesh_prism(m, vadd(p, v3(1.35f * s, 2.6f * s, 0)), 0.32f * s, 2.6f * s, 6, g, g);
    mesh_obox(m, vadd(p, v3(-0.8f * s, 3.6f * s, 0)), v3(1, 0, 0), v3(0, 0, 1), 0.5f * s, 0.3f * s, 0.55f * s, g, g);
    mesh_prism(m, vadd(p, v3(-1.2f * s, 3.6f * s, 0)), 0.3f * s, 2.0f * s, 6, g, g);
}

static void meseta_roja(Mesh *m, V3 p, float R, float H, Rng *rg) {
    int k, S = 7;
    Col c1 = col(184, 92, 56), c2 = col(206, 126, 80), top = col(196, 150, 100);
    V3 pts[7];
    for (k = 0; k < S; k++) {
        float a = 2 * (float)M_PI * k / S, r = R * rng_range(rg, 0.75f, 1.15f);
        pts[k] = v3(p.x + cosf(a) * r, p.y, p.z + sinf(a) * r);
    }
    for (k = 0; k < S; k++) {
        V3 a = pts[k], b = pts[(k + 1) % S];
        V3 am = vadd(a, v3(0, H * 0.55f, 0)), bm = vadd(b, v3(0, H * 0.55f, 0));
        V3 at2 = vlerp(vadd(a, v3(0, H, 0)), vadd(p, v3(0, H, 0)), 0.12f), bt = vlerp(vadd(b, v3(0, H, 0)), vadd(p, v3(0, H, 0)), 0.12f);
        mesh_quad(m, a, b, bm, am, c1);
        mesh_quad(m, am, bm, bt, at2, c2);
        mesh_tri(m, at2, bt, vadd(p, v3(0, H, 0)), top);
    }
}

static void scenery_sonora(Track *tr) {
    Rng rg = {1950};
    int k, tries;
    for (k = 0, tries = 0; k < 150 && tries < 4000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 5, 120)), 0);
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 4) continue;
        if (k % 5 == 4) mesh_cone(chunk_mesh(tr, ni), q, rng_range(&rg, 1.0f, 2.5f), rng_range(&rg, 0.8f, 1.8f), 5, col(150, 120, 96));
        else if (k % 7 == 3) mesh_prism(chunk_mesh(tr, ni), q, 0.6f, 0.9f, 7, col(90, 130, 70), col(220, 90, 120));
        else saguaro(chunk_mesh(tr, ni), q, rng_range(&rg, 0.8f, 1.4f));
        k++;
    }
    for (k = 0, tries = 0; k < 12 && tries < 600; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        float R = rng_range(&rg, 40, 110);
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + R + rng_range(&rg, 40, 260)), 0);
        q.y = -0.5f;
        if (min_dist_xz(tr, q, &ni) < tr->wall + R * 1.2f + 15) continue;
        meseta_roja(chunk_mesh(tr, ni), q, R, rng_range(&rg, 30, 75), &rg);
        k++;
    }
}

/* piramide escalonada original de estilo maya */
static void piramide(Mesh *m, V3 c, V3 f) {
    V3 r = v3(-f.z, 0, f.x);
    Col st = col(196, 186, 160), st2 = col(176, 166, 140), dark = col(60, 50, 40);
    int k, T = 8;
    float base = 34, step = 3.6f;
    for (k = 0; k < T; k++) {
        float hw = base - k * 3.4f;
        mesh_obox(m, vadd(c, v3(0, k * step, 0)), r, f, hw, hw, step, k & 1 ? st : st2, st);
    }
    /* escalinata central */
    {
        V3 a = lp(c, r, f, -4.5f, 0, base + 0.2f), b = lp(c, r, f, 4.5f, 0, base + 0.2f);
        V3 tb = lp(c, r, f, 4.5f, T * step, base - (T - 1) * 3.4f + 0.2f), ta = lp(c, r, f, -4.5f, T * step, base - (T - 1) * 3.4f + 0.2f);
        mesh_quad(m, a, b, tb, ta, col(210, 200, 176));
    }
    /* templo superior */
    {
        V3 t = vadd(c, v3(0, T * step, 0));
        mesh_obox(m, t, r, f, 7, 6, 7, st, col(150, 140, 120));
        mesh_quad_flat(m, lp(t, r, f, -1.8f, 0, 6.05f), lp(t, r, f, 1.8f, 0, 6.05f), lp(t, r, f, 1.8f, 4, 6.05f), lp(t, r, f, -1.8f, 4, 6.05f), dark);
        mesh_obox(m, vadd(t, v3(0, 7, 0)), r, f, 5, 1.2f, 4, st2, st2);
    }
}

static void cenote(Mesh *m, V3 c, float R) {
    int k, S = 12;
    for (k = 0; k < S; k++) {
        float a0 = 2 * (float)M_PI * k / S, a1 = 2 * (float)M_PI * (k + 1) / S;
        V3 o0 = v3(c.x + cosf(a0) * R, 0.05f, c.z + sinf(a0) * R), o1 = v3(c.x + cosf(a1) * R, 0.05f, c.z + sinf(a1) * R);
        V3 i0 = v3(c.x + cosf(a0) * R * 0.78f, -3.0f, c.z + sinf(a0) * R * 0.78f), i1 = v3(c.x + cosf(a1) * R * 0.78f, -3.0f, c.z + sinf(a1) * R * 0.78f);
        mesh_quad(m, o0, o1, i1, i0, col(150, 140, 110));
        mesh_tri_flat(m, v3(c.x, -3.0f, c.z), i0, i1, col(40, 150, 170));
    }
}

static void scenery_maya(Track *tr) {
    Rng rg = {1519};
    int k, tries, ci;
    {
        V3 c = tr->center;
        float d = min_dist_xz(tr, c, &ci);
        c.y = 0;
        if (d > tr->wall + 45) piramide(chunk_mesh(tr, ci), c, vnorm(vsub(tr->p[0], c)));
        else {
            /* fuera del circuito si no cabe dentro */
            V3 q = at(tr, tr->n / 3, tr->wall + 70, 0); q.y = 0;
            min_dist_xz(tr, q, &ci);
            piramide(chunk_mesh(tr, ci), q, tr->r[tr->n / 3]);
        }
    }
    for (k = 0, tries = 0; k < 2 && tries < 300; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + 40), 0);
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 30) continue;
        cenote(chunk_mesh(tr, ni), q, 16);
        k++;
    }
    /* selva muy densa */
    for (k = 0, tries = 0; k < 420 && tries < 8000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 3, 90)), 0);
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 3) continue;
        if (k % 6 == 0) { if (min_dist_xz(tr, q, NULL) > tr->wall + 7) palmera(chunk_mesh(tr, ni), q, rng_range(&rg, 0.9f, 1.3f)); }
        else tree_round(chunk_mesh(tr, ni), q, rng_range(&rg, 1.2f, 2.2f));
        k++;
    }
}

static void add_volcanes(Track *tr) {
    Mesh *m = &tr->world;
    float D = tr->extent + 1700.0f;
    /* volcan activo con fumarola */
    {
        V3 b = v3(tr->center.x + D * 0.6f, -2, tr->center.z - D * 0.8f);
        float R = 1300, H = 1050;
        int k, S = 16;
        for (k = 0; k < S; k++) {
            float a0 = 2 * (float)M_PI * k / S, a1 = 2 * (float)M_PI * (k + 1) / S;
            V3 p0 = v3(b.x + cosf(a0) * R, b.y, b.z + sinf(a0) * R), p1 = v3(b.x + cosf(a1) * R, b.y, b.z + sinf(a1) * R);
            V3 q0 = v3(b.x + cosf(a0) * R * 0.1f, H, b.z + sinf(a0) * R * 0.1f), q1 = v3(b.x + cosf(a1) * R * 0.1f, H, b.z + sinf(a1) * R * 0.1f);
            V3 s0 = vlerp(q0, p0, 0.3f), s1 = vlerp(q1, p1, 0.3f);
            mesh_quad_flat(m, s0, s1, p1, p0, (k & 1) ? col(96, 92, 100) : col(104, 100, 108));
            mesh_quad_flat(m, q0, q1, s1, s0, (k & 1) ? col(240, 242, 248) : col(226, 230, 238));
        }
        for (k = 0; k < 5; k++) {
            V3 pf = v3(b.x + k * 60.0f, H + 60 + k * 110.0f, b.z + k * 30.0f);
            mesh_cone(m, vadd(pf, v3(0, -45, 0)), 90 + k * 25.0f, 90, 7, col(206, 206, 210));
            mesh_cone(m, vadd(pf, v3(0, 45, 0)), 90 + k * 25.0f, -90, 7, col(196, 196, 202));
        }
    }
    /* volcan dormido: cresta alargada con varias cumbres nevadas */
    {
        float xc = tr->center.x - D * 0.4f, zc = tr->center.z - D * 0.95f;
        static const float prof[] = {0, 380, 620, 700, 640, 760, 820, 700, 520, 300, 0};
        int k;
        for (k = 0; k < 10; k++) {
            float x0 = xc - 1400 + k * 280, x1 = x0 + 280;
            V3 a0 = v3(x0, -2, zc + 500), a1 = v3(x1, -2, zc + 500);
            V3 c0 = v3(x0, prof[k], zc), c1 = v3(x1, prof[k + 1], zc);
            V3 s0 = vlerp(c0, a0, 0.25f), s1 = vlerp(c1, a1, 0.25f);
            mesh_quad_flat(m, a0, a1, s1, s0, (k & 1) ? col(88, 96, 104) : col(96, 104, 112));
            mesh_quad_flat(m, s0, s1, c1, c0, (k & 1) ? col(238, 240, 246) : col(224, 228, 236));
        }
    }
}

static void scenery_volcanes(Track *tr) {
    Rng rg = {1802};
    int k, tries;
    scatter_trees(tr, &rg, 330, 1, 4, 110);
    for (k = 0, tries = 0; k < 6 && tries < 300; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 30, 90)), 0);
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 18) continue;
        casa(chunk_mesh(tr, ni), &rg, q, tr->t[ni]);
        k++;
    }
}

static void agave(Mesh *m, V3 p, float s) {
    Col c = col(86, 146, 150);
    int k;
    for (k = 0; k < 6; k++) {
        float a = k * 1.047f;
        V3 d = v3(cosf(a), 0, sinf(a)), side = v3(-d.z, 0, d.x);
        V3 tip = vadd(p, vadd(vscale(d, 1.1f * s), v3(0, 1.3f * s, 0)));
        mesh_tri(m, vadd(p, vscale(side, 0.22f * s)), vsub(p, vscale(side, 0.22f * s)), tip, k & 1 ? c : col_shade(c, 0.85f));
    }
}

static void hacienda(Mesh *m, V3 c, V3 f) {
    V3 r = v3(-f.z, 0, f.x);
    Col white = col(240, 236, 226), roof = col(180, 86, 56), dark = col(80, 60, 50);
    int k;
    mesh_obox(m, c, r, f, 22, 9, 6, white, white);
    gable_roof(m, c, r, f, 9, 22, 6, 3, roof, white);
    for (k = -4; k <= 4; k++)
        mesh_quad_flat(m, lp(c, r, f, k * 4.2f - 1.2f, 0, 9.06f), lp(c, r, f, k * 4.2f + 1.2f, 0, 9.06f), lp(c, r, f, k * 4.2f + 1.2f, 3.6f, 9.06f), lp(c, r, f, k * 4.2f - 1.2f, 3.6f, 9.06f), dark);
    /* chimenea de la destileria */
    mesh_prism(m, lp(c, r, f, 26, 0, -4), 1.6f, 18, 8, col(170, 90, 70), col(170, 90, 70));
    /* capilla con campanario */
    mesh_obox(m, lp(c, r, f, -30, 0, 0), r, f, 6, 10, 9, white, white);
    gable_roof(m, lp(c, r, f, -30, 0, 0), r, f, 6, 10, 9, 3, roof, white);
    mesh_obox(m, lp(c, r, f, -30, 0, 11), r, f, 2.5f, 2.5f, 16, white, white);
    mesh_cone(m, lp(c, r, f, -30, 16, 11), 3.0f, 3.5f, 4, roof);
}

static void scenery_agave(Track *tr) {
    Rng rg = {1974};
    int k, tries;
    /* campos de agave en hileras */
    for (k = 0, tries = 0; k < 22 && tries < 2000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        float side = (rng_u(&rg) & 1) ? 1.0f : -1.0f;
        V3 q = at(tr, i, side * (tr->wall + rng_range(&rg, 30, 90)), 0);
        V3 f = vnorm(v3(tr->t[i].x, 0, tr->t[i].z)), r = v3(-f.z, 0, f.x);
        int a, b, ok = 1;
        q.y = 0;
        for (a = 0; a < 3 && ok; a++) for (b = 0; b < 3 && ok; b++) {
            V3 pr = lp(q, r, f, (a - 1) * 15.0f, 0, (b - 1) * 13.0f);
            if (min_dist_xz(tr, pr, NULL) < tr->wall + 4) ok = 0;
        }
        if (!ok) continue;
        min_dist_xz(tr, q, &ni);
        for (a = 0; a < 8; a++) for (b = 0; b < 7; b++)
            agave(chunk_mesh(tr, ni), lp(q, r, f, (a - 3.5f) * 4.2f, 0, (b - 3) * 4.4f), rng_range(&rg, 1.5f, 1.9f));
        k++;
    }
    for (k = 0, tries = 0; k < 2 && tries < 300; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + 70), 0);
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 50) continue;
        hacienda(chunk_mesh(tr, ni), q, tr->r[i]);
        k++;
    }
    scatter_trees(tr, &rg, 60, 0, 10, 120);
}

static void add_volcan_tequila(Track *tr) {
    Mesh *m = &tr->world;
    V3 b = v3(tr->center.x - tr->extent - 1000.0f, -2, tr->center.z + 400.0f);
    mesh_cone(m, b, 1100, 520, 12, col(92, 110, 120));
    mesh_cone(m, vadd(b, v3(0, 440, 0)), 200, 120, 12, col(80, 92, 100));
}

static void cardon(Mesh *m, V3 p, float s) {
    Col g = col(96, 136, 80);
    int k;
    mesh_prism(m, p, 0.5f * s, 9.0f * s, 6, g, g);
    for (k = 0; k < 4; k++) {
        float a = k * 1.57f + 0.4f;
        V3 b = vadd(p, v3(cosf(a) * 0.9f * s, 1.6f * s + k * 0.5f * s, sinf(a) * 0.9f * s));
        mesh_prism(m, b, 0.35f * s, (2.4f + (k % 3) * 0.9f) * s, 6, g, g);
    }
}

static void scenery_baja(Track *tr) {
    Rng rg = {1952};
    int k, tries, i;
    /* eje largo de la peninsula y lado del mar (suroeste) */
    V3 ax = vnorm(vsub(tr->p[tr->n / 2], tr->p[0]));
    V3 nrm = v3(ax.z, 0, -ax.x);
    float off = -1e9f, smin = 1e9f, smax = -1e9f;
    if (nrm.x > 0) nrm = vscale(nrm, -1);
    for (i = 0; i < tr->n; i++) {
        V3 d = vsub(tr->p[i], tr->center);
        float o = vdot(d, nrm), s = vdot(d, ax);
        if (o > off) off = o;
        if (s < smin) smin = s;
        if (s > smax) smax = s;
    }
    off += tr->wall + 55.0f;
    /* mar, playa y acantilados (en tiras a lo largo del eje) */
    {
        float s;
        for (s = smin - 600; s < smax + 600; s += 60) {
            V3 a = vadd(tr->center, vadd(vscale(ax, s), vscale(nrm, off)));
            V3 b = vadd(tr->center, vadd(vscale(ax, s + 60), vscale(nrm, off)));
            V3 a2 = vadd(a, vscale(nrm, 25)), b2 = vadd(b, vscale(nrm, 25));
            V3 a3 = vadd(a, vscale(nrm, 2500)), b3 = vadd(b, vscale(nrm, 2500));
            int ni;
            min_dist_xz(tr, a, &ni);
            a.y = b.y = 0.05f; a2.y = b2.y = 0.05f; a3.y = b3.y = 0.1f;
            mesh_quad_flat(chunk_mesh(tr, ni), a, b, b2, a2, col(228, 210, 160));
            a2.y = b2.y = 0.1f;
            mesh_quad_flat(chunk_mesh(tr, ni), a2, b2, b3, a3, col(30, 110, 170));
        }
    }
    /* arco de roca en el mar, junto a la punta sur */
    {
        V3 tip = tr->p[tr->n / 2];
        V3 q = vadd(tip, vadd(vscale(nrm, off - vdot(vsub(tip, tr->center), nrm) + 70), vscale(ax, 60)));
        int ni;
        Col rock = col(176, 150, 120);
        min_dist_xz(tr, q, &ni);
        q.y = 0;
        mesh_prism(chunk_mesh(tr, ni), vadd(q, vscale(ax, -14)), 7, 34, 6, rock, rock);
        mesh_prism(chunk_mesh(tr, ni), vadd(q, vscale(ax, 14)), 7, 30, 6, rock, rock);
        mesh_obox(chunk_mesh(tr, ni), vadd(q, v3(0, 24, 0)), nrm, ax, 6, 20, 14, rock, col(160, 136, 110));
        mesh_cone(chunk_mesh(tr, ni), vadd(q, vadd(vscale(ax, 34), v3(0, 0, 0))), 9, 40, 6, rock);
    }
    /* cardones, matorral y rocas en tierra */
    for (k = 0, tries = 0; k < 170 && tries < 5000; tries++) {
        int ii = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, ii, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 6, 120)), 0);
        V3 d = vsub(q, tr->center);
        q.y = 0;
        if (vdot(d, nrm) > off - 8) continue;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 4 + tr->p[ni].y * 2.2f) continue;
        if (k % 3) cardon(chunk_mesh(tr, ni), q, rng_range(&rg, 0.8f, 1.4f));
        else mesh_cone(chunk_mesh(tr, ni), q, rng_range(&rg, 1.5f, 3.5f), rng_range(&rg, 1, 2.5f), 5, col(160, 130, 100));
        k++;
    }
    /* sierra al noreste */
    for (k = 0; k < 8; k++) {
        float s = smin + (smax - smin) * k / 7.0f;
        V3 b = vadd(tr->center, vadd(vscale(ax, s), vscale(nrm, -(off + 250))));
        int ni;
        min_dist_xz(tr, b, &ni);
        b.y = -1;
        mesh_cone(chunk_mesh(tr, ni), b, 180, 120 + (k % 3) * 60.0f, 8, (k & 1) ? col(150, 120, 96) : col(140, 112, 90));
    }
}

/* ---------- Cumbres Nevadas: recta baja en el valle y cordillera al sur ---------- */

static void chalet(Mesh *m, V3 c, V3 f) {
    V3 r = v3(-f.z, 0, f.x);
    Col wood = col(150, 100, 64), stone = col(170, 166, 160), roof = col(80, 70, 66);
    mesh_obox(m, c, r, f, 6, 8, 2.5f, stone, stone);
    mesh_obox(m, vadd(c, v3(0, 2.5f, 0)), r, f, 6, 8, 4, wood, wood);
    gable_roof(m, vadd(c, v3(0, 0, 0)), f, r, 8, 6, 6.5f, 4.5f, roof, wood);
    /* nieve en el tejado */
    gable_roof(m, vadd(c, v3(0, 0.25f, 0)), f, r, 7.2f, 5.6f, 6.5f, 4.4f, col(240, 242, 248), wood);
}

static void add_cordillera_sur(Track *tr) {
    Mesh *m = &tr->world;
    Rng rg = {4810};
    float zc = tr->center.z + tr->extent + 900.0f;
    float xs = tr->center.x - 2600.0f, xe = tr->center.x + 2600.0f;
    int k, S = 22;
    for (k = 0; k < S; k++) {
        float x0 = lerpf(xs, xe, (float)k / S), x1 = lerpf(xs, xe, (float)(k + 1) / S), xm = (x0 + x1) * 0.5f;
        float h = rng_range(&rg, 650, 1150) * (0.7f + 0.3f * sinf(k * 0.6f));
        float zz = zc + rng_range(&rg, -150, 250);
        V3 a = v3(x0 - 120, -2, zz - 600), b = v3(x1 + 120, -2, zz - 600);
        V3 a2 = v3(x0 - 120, -2, zz + 500), b2 = v3(x1 + 120, -2, zz + 500);
        V3 top = v3(xm + rng_range(&rg, -60, 60), h, zz);
        V3 sa = vlerp(top, a, 0.42f), sb = vlerp(top, b, 0.42f);
        Col rock = (k & 1) ? col(96, 104, 120) : col(106, 112, 128);
        /* cara norte (la que se ve desde el circuito): roca abajo, nieve arriba */
        mesh_quad_flat(m, a, b, sb, sa, rock);
        mesh_tri_flat(m, sa, sb, top, (k & 1) ? col(244, 246, 252) : col(230, 234, 244));
        mesh_tri_flat(m, b, b2, top, col_shade(rock, 0.85f));
        mesh_tri_flat(m, a2, a, top, col_shade(rock, 0.9f));
    }
    /* estribaciones boscosas entre el circuito y la cordillera */
    for (k = 0; k < 14; k++) {
        V3 b = v3(lerpf(xs + 400, xe - 400, k / 13.0f) + rng_range(&rg, -80, 80), -1, zc - 650 + rng_range(&rg, -60, 60));
        mesh_cone(m, b, rng_range(&rg, 220, 340), rng_range(&rg, 160, 300), 8, (k & 1) ? col(46, 96, 62) : col(54, 104, 66));
    }
}

static void scenery_cumbres(Track *tr) {
    Rng rg = {2212};
    int k, tries;
    /* gradas y boxes a lo largo de la recta principal (el punto mas bajo) */
    {
        int i0 = 14;
        V3 rr = tr->r[i0], ff = vnorm(v3(tr->t[i0].x, 0, tr->t[i0].z));
        /* la recta va de oeste a este; el exterior (norte) queda a la izquierda */
        float side_out = rr.z < 0 ? 1.0f : -1.0f;
        grada(chunk_mesh(tr, i0 + 30), &rg, at(tr, i0 + 30, side_out * (tr->wall + 4.0f), 0), vscale(rr, -side_out), ff, 60.0f);
        grada(chunk_mesh(tr, i0 + 70), &rg, at(tr, i0 + 70, side_out * (tr->wall + 4.0f), 0), vscale(rr, -side_out), ff, 50.0f);
        boxes_y_torre(chunk_mesh(tr, i0 + 50), at(tr, i0 + 50, -side_out * (tr->wall + 22.0f), 0), vscale(rr, side_out), ff, 60.0f);
    }
    /* chalets en la zona alta del sur */
    for (k = 0, tries = 0; k < 9 && tries < 600; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 40, 110)), 0);
        if (tr->p[i].y < 8.0f) continue;
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 16 + tr->p[ni].y * 2.2f) continue;
        chalet(chunk_mesh(tr, ni), q, tr->t[ni]);
        k++;
    }
    /* bosque de pinos, mas denso en el lado sur */
    for (k = 0, tries = 0; k < 340 && tries < 8000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 4, 120)), 0);
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 4 + tr->p[ni].y * 2.2f) continue;
        if (tr->p[ni].y < 3.0f && (rng_u(&rg) % 3)) continue;
        tree_pine(chunk_mesh(tr, ni), q, rng_range(&rg, 1.0f, 1.8f));
        k++;
    }
    /* manchas de nieve en la parte alta */
    for (k = 0, tries = 0; k < 40 && tries < 2000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 10, 100)), 0);
        float R = rng_range(&rg, 4, 12);
        int s;
        if (tr->p[i].y < 12.0f) continue;
        q.y = 0.06f;
        if (min_dist_xz(tr, q, &ni) < tr->wall + R + 4 + tr->p[ni].y * 2.2f) continue;
        for (s = 0; s < 7; s++) {
            float a0 = 2 * (float)M_PI * s / 7, a1 = 2 * (float)M_PI * (s + 1) / 7;
            mesh_tri_flat(chunk_mesh(tr, ni), q, v3(q.x + cosf(a0) * R, 0.06f, q.z + sinf(a0) * R), v3(q.x + cosf(a1) * R, 0.06f, q.z + sinf(a1) * R), col(236, 240, 246));
        }
        k++;
    }
}

/* ---------- Parque Fundidora: patrimonio industrial, lago, canal y el cerro de dos jorobas ---------- */

static void alto_horno(Mesh *m, V3 c) {
    Col rust = col(132, 72, 48), rust2 = col(110, 60, 42), steel = col(90, 90, 96);
    int k;
    /* cuerpo del horno */
    mesh_prism(m, c, 9.0f, 34.0f, 10, rust, rust2);
    mesh_prism(m, vadd(c, v3(0, 34, 0)), 6.0f, 14.0f, 10, rust2, rust2);
    mesh_prism(m, vadd(c, v3(0, 48, 0)), 3.0f, 22.0f, 8, steel, steel);
    /* estructura de vigas alrededor */
    for (k = 0; k < 4; k++) {
        float a = k * 1.5708f + 0.785f;
        V3 p = v3(c.x + cosf(a) * 14, c.y, c.z + sinf(a) * 14);
        mesh_obox(m, p, v3(1, 0, 0), v3(0, 0, 1), 0.6f, 0.6f, 52, steel, steel);
    }
    mesh_box(m, v3(c.x - 15, c.y + 30, c.z - 15), v3(c.x + 15, c.y + 31.2f, c.z + 15), steel, steel);
    mesh_box(m, v3(c.x - 15, c.y + 50, c.z - 15), v3(c.x + 15, c.y + 51.2f, c.z + 15), steel, steel);
    /* estufas (torres cilindricas) en fila */
    for (k = 0; k < 4; k++) {
        V3 s = v3(c.x + 26 + k * 11.0f, c.y, c.z + 6);
        mesh_prism(m, s, 4.6f, 38, 10, col(160, 150, 140), col(130, 120, 110));
        mesh_cone(m, vadd(s, v3(0, 38, 0)), 4.6f, 4.0f, 10, col(130, 120, 110));
    }
    /* tuberia que une las estufas con el horno */
    mesh_box(m, v3(c.x + 8, c.y + 26, c.z + 4.6f), v3(c.x + 66, c.y + 29, c.z + 7.4f), rust2, rust2);
    /* rampa de carga inclinada */
    {
        V3 a = v3(c.x - 6, c.y + 46, c.z - 6), b = v3(c.x - 70, c.y, c.z - 40);
        V3 side = vscale(vnorm(vcross(vsub(b, a), v3(0, 1, 0))), 1.6f);
        mesh_quad(m, vadd(a, side), vsub(a, side), vsub(b, side), vadd(b, side), steel);
    }
}

static void chimenea(Mesh *m, V3 p, float h) {
    mesh_prism(m, p, 3.0f, h, 8, col(150, 70, 52), col(150, 70, 52));
    mesh_prism(m, vadd(p, v3(0, h - 3, 0)), 3.4f, 3.0f, 8, col(120, 56, 42), col(60, 50, 46));
}

/* nave industrial de ladrillo con techo en dientes de sierra */
static void nave(Mesh *m, V3 c, V3 f, float hw, float hl) {
    V3 r = v3(-f.z, 0, f.x);
    Col brick = col(170, 82, 60), dark = col(70, 60, 56), roof = col(120, 120, 126);
    int k, n = (int)(hl / 6.0f);
    mesh_obox(m, c, r, f, hw, hl, 11, brick, brick);
    for (k = 0; k < n; k++) {
        float z0 = -hl + k * (2 * hl / n), z1 = z0 + 2 * hl / n;
        V3 a0 = lp(c, r, f, -hw, 11, z0), a1 = lp(c, r, f, hw, 11, z0);
        V3 b0 = lp(c, r, f, -hw, 15, z1), b1 = lp(c, r, f, hw, 15, z1);
        V3 c0 = lp(c, r, f, -hw, 11, z1), c1 = lp(c, r, f, hw, 11, z1);
        mesh_quad(m, a0, a1, b1, b0, roof);
        mesh_quad_flat(m, c0, c1, b1, b0, col(150, 170, 190));
    }
    for (k = 0; k < n; k++) {
        float z = -hl + 3 + k * (2 * hl / n);
        int s;
        for (s = -1; s <= 1; s += 2)
            mesh_quad_flat(m, lp(c, r, f, s * (hw + 0.05f), 3, z - 1.2f), lp(c, r, f, s * (hw + 0.05f), 3, z + 1.2f),
                           lp(c, r, f, s * (hw + 0.05f), 8, z + 1.2f), lp(c, r, f, s * (hw + 0.05f), 8, z - 1.2f), dark);
    }
}

static void torre_ciudad(Mesh *m, Rng *rg, V3 p) {
    static const int pal[][3] = {{180, 196, 210}, {120, 140, 160}, {210, 210, 214}, {90, 110, 130}, {200, 190, 170}};
    const int *c = pal[rng_u(rg) % 5];
    float w = rng_range(rg, 14, 26), h = rng_range(rg, 40, 140);
    float y;
    mesh_box(m, v3(p.x - w, 0, p.z - w), v3(p.x + w, h, p.z + w), col(c[0], c[1], c[2]), col(110, 110, 116));
    for (y = 6; y < h - 3; y += 7)
        mesh_box(m, v3(p.x - w - 0.1f, y, p.z - w - 0.1f), v3(p.x + w + 0.1f, y + 1.6f, p.z + w + 0.1f), col(60, 80, 110), col(60, 80, 110));
}

static Mesh *near_chunk(Track *tr, V3 q) { int ni; min_dist_xz(tr, q, &ni); return chunk_mesh(tr, ni); }

static V3 fund_spot(Track *tr, float px, float py, float clear) {
    /* posicion del mapa, desplazada si queda demasiado cerca de la pista */
    V3 q = v3(FUN_X(px), 0, FUN_Z(py));
    int k = 0;
    while (min_dist_xz(tr, q, NULL) < tr->wall + clear && k < 40) {
        V3 d = vnorm(v3(q.x - tr->center.x, 0, q.z - tr->center.z));
        q = vsub(q, vscale(d, 6.0f));
        k++;
    }
    return q;
}

static void scenery_fundidora(Track *tr) {
    Rng rg = {1900};
    int k, tries;
    /* alto horno y chimeneas */
    {
        V3 h = fund_spot(tr, 300, 345, 70);
        alto_horno(near_chunk(tr, h), h);
        chimenea(near_chunk(tr, h), vadd(h, v3(-35, 0, 30)), 55);
    }
    {
        V3 q;
        q = fund_spot(tr, 215, 160, 40); nave(near_chunk(tr, q), q, vnorm(v3(0.8f, 0, 0.6f)), 14, 46);
        chimenea(near_chunk(tr, q), vadd(q, v3(30, 0, -40)), 48);
        q = fund_spot(tr, 240, 285, 30); nave(near_chunk(tr, q), q, v3(1, 0, 0), 12, 26);
        q = fund_spot(tr, 285, 300, 30); nave(near_chunk(tr, q), q, v3(1, 0, 0), 12, 24);
        q = fund_spot(tr, 430, 355, 30); nave(near_chunk(tr, q), q, v3(1, 0, 0), 12, 30);
        chimenea(near_chunk(tr, q), vadd(q, v3(0, 0, -30)), 44);
    }
    /* lago */
    {
        V3 c = fund_spot(tr, 450, 130, 40);
        Mesh *m = near_chunk(tr, c);
        float R = 60;
        for (k = 0; k < 14; k++) {
            float a0 = 2 * (float)M_PI * k / 14, a1 = 2 * (float)M_PI * (k + 1) / 14;
            float r0 = R * (1.0f + 0.25f * cosf(a0 * 2)), r1 = R * (1.0f + 0.25f * cosf(a1 * 2));
            mesh_tri_flat(m, v3(c.x, 0.08f, c.z), v3(c.x + cosf(a0) * r0 * 1.4f, 0.08f, c.z + sinf(a0) * r0 * 0.7f),
                          v3(c.x + cosf(a1) * r1 * 1.4f, 0.08f, c.z + sinf(a1) * r1 * 0.7f), col(70, 130, 170));
        }
    }
    /* auditorio moderno (fuera del circuito, al este) */
    {
        V3 a = fund_spot(tr, 440, 280, 40);
        Mesh *m = near_chunk(tr, a);
        mesh_prism(m, a, 34, 16, 12, col(220, 222, 226), col(200, 204, 210));
        mesh_prism(m, vadd(a, v3(0, 16, 0)), 26, 5, 12, col(180, 190, 200), col(200, 204, 210));
    }
    /* canal al sur */
    {
        float z = 0;
        int i;
        for (i = 0; i < tr->n; i++) if (tr->p[i].z > z) z = tr->p[i].z;
        z += tr->wall + 30;
        for (k = 0; k < 30; k++) {
            float x0 = tr->center.x - 900 + k * 60;
            Mesh *m = near_chunk(tr, v3(x0, 0, z));
            mesh_quad_flat(m, v3(x0, 0.06f, z), v3(x0 + 60, 0.06f, z), v3(x0 + 60, 0.06f, z + 14), v3(x0, 0.06f, z + 14), col(60, 120, 150));
            mesh_quad_flat(m, v3(x0, 0.05f, z - 4), v3(x0 + 60, 0.05f, z - 4), v3(x0 + 60, 0.05f, z), v3(x0, 0.05f, z), col(186, 176, 160));
            mesh_quad_flat(m, v3(x0, 0.05f, z + 14), v3(x0 + 60, 0.05f, z + 14), v3(x0 + 60, 0.05f, z + 18), v3(x0, 0.05f, z + 18), col(186, 176, 160));
        }
    }
    /* grandes recintos de exposiciones al oeste y torres de la ciudad alrededor */
    {
        V3 q = fund_spot(tr, 40, 120, 40);
        mesh_box(near_chunk(tr, q), v3(q.x - 50, 0, q.z - 70), v3(q.x + 50, 16, q.z + 70), col(200, 200, 204), col(170, 172, 178));
        q = fund_spot(tr, 40, 270, 40);
        mesh_box(near_chunk(tr, q), v3(q.x - 50, 0, q.z - 60), v3(q.x + 50, 14, q.z + 60), col(206, 206, 210), col(172, 174, 180));
    }
    for (k = 0, tries = 0; k < 26 && tries < 600; tries++) {
        float a = rng_range(&rg, 0, 6.28f), R = tr->extent + rng_range(&rg, 140, 420);
        V3 q = v3(tr->center.x + cosf(a) * R, 0, tr->center.z + sinf(a) * R);
        if (sinf(a) > 0.3f) continue;   /* al sur queda el canal y el cerro */
        if (min_dist_xz(tr, q, NULL) < tr->wall + 60) continue;
        torre_ciudad(near_chunk(tr, q), &rg, q);
        k++;
    }
    /* mucho arbol en el parque */
    scatter_trees(tr, &rg, 420, 0, 4, 90);
    lamp_posts(tr, 18);
}

static void add_silla(Track *tr) {
    Mesh *m = &tr->world;
    /* cerro de dos jorobas al sureste, muy presente */
    float xc = tr->center.x + 1500.0f, zc = tr->center.z + 1300.0f;
    static const float prof[] = {0, 260, 520, 760, 820, 640, 560, 700, 880, 760, 480, 200, 0};
    int k;
    V3 axis = vnorm(v3(-0.6f, 0, 1.0f)), nrm = v3(-axis.z, 0, axis.x);
    for (k = 0; k < 12; k++) {
        float s0 = -1500 + k * 250.0f, s1 = s0 + 250;
        V3 c0 = v3(xc + axis.x * s0, prof[k], zc + axis.z * s0), c1 = v3(xc + axis.x * s1, prof[k + 1], zc + axis.z * s1);
        V3 a0 = v3(c0.x + nrm.x * 700, -2, c0.z + nrm.z * 700), a1 = v3(c1.x + nrm.x * 700, -2, c1.z + nrm.z * 700);
        V3 b0 = v3(c0.x - nrm.x * 700, -2, c0.z - nrm.z * 700), b1 = v3(c1.x - nrm.x * 700, -2, c1.z - nrm.z * 700);
        V3 m0 = vlerp(c0, a0, 0.4f), m1 = vlerp(c1, a1, 0.4f);
        mesh_quad_flat(m, a0, a1, m1, m0, (k & 1) ? col(84, 110, 74) : col(92, 118, 80));
        mesh_quad_flat(m, m0, m1, c1, c0, (k & 1) ? col(126, 126, 124) : col(136, 134, 130));
        mesh_quad_flat(m, b0, b1, c1, c0, col(112, 114, 112));
    }
}

/* ---------- Villa Olimpica: estadio, alberca, velodromo, residencias y canchas ---------- */

/* busca un lugar libre para un objeto de medio ancho hw y medio largo hl, con margen a la pista */
static int villa_spot(Track *tr, Rng *rg, V3 *out, V3 *f, float hw, float hl, float margin, int inside) {
    int tries;
    for (tries = 0; tries < 400; tries++) {
        int i = rng_u(rg) % tr->n;
        float side = (rng_u(rg) & 1) ? 1.0f : -1.0f;
        V3 q = at(tr, i, side * (tr->wall + margin + fmaxf(hw, hl) + rng_range(rg, 0, 120)), 0);
        V3 ff = vnorm(v3(tr->t[i].x, 0, tr->t[i].z)), rr = v3(-ff.z, 0, ff.x);
        q.y = 0;
        if (inside >= 0 && inside_loop(tr, q.x, q.z) != inside) continue;
        if (!footprint_ok(tr, q, rr, ff, -hw, hw, hl, margin)) continue;
        *out = q; *f = ff;
        return 1;
    }
    return 0;
}

static void estadio(Mesh *m, V3 c) {
    int k, S = 16;
    float Rx = 70, Rz = 52;
    Col conc = col(214, 214, 220), seat1 = col(220, 70, 60), seat2 = col(60, 110, 200);
    /* campo y pista de atletismo */
    for (k = 0; k < S; k++) {
        float a0 = 2 * (float)M_PI * k / S, a1 = 2 * (float)M_PI * (k + 1) / S;
        V3 o0 = v3(c.x + cosf(a0) * Rx * 0.78f, 0.06f, c.z + sinf(a0) * Rz * 0.78f), o1 = v3(c.x + cosf(a1) * Rx * 0.78f, 0.06f, c.z + sinf(a1) * Rz * 0.78f);
        V3 i0 = v3(c.x + cosf(a0) * Rx * 0.62f, 0.07f, c.z + sinf(a0) * Rz * 0.62f), i1 = v3(c.x + cosf(a1) * Rx * 0.62f, 0.07f, c.z + sinf(a1) * Rz * 0.62f);
        mesh_quad_flat(m, o0, o1, i1, i0, col(196, 80, 60));
        mesh_tri_flat(m, v3(c.x, 0.08f, c.z), i0, i1, col(70, 160, 80));
    }
    /* anillo de gradas en tres niveles */
    for (k = 0; k < S; k++) {
        float a0 = 2 * (float)M_PI * k / S, a1 = 2 * (float)M_PI * (k + 1) / S;
        int t;
        for (t = 0; t < 3; t++) {
            float r0 = 0.80f + t * 0.08f, r1 = r0 + 0.08f, y0 = 1.5f + t * 6.0f, y1 = y0 + 6.0f;
            V3 a = v3(c.x + cosf(a0) * Rx * r0, y0, c.z + sinf(a0) * Rz * r0), b = v3(c.x + cosf(a1) * Rx * r0, y0, c.z + sinf(a1) * Rz * r0);
            V3 a2 = v3(c.x + cosf(a0) * Rx * r1, y1, c.z + sinf(a0) * Rz * r1), b2 = v3(c.x + cosf(a1) * Rx * r1, y1, c.z + sinf(a1) * Rz * r1);
            mesh_quad(m, a, b, b2, a2, (k + t) & 1 ? seat1 : seat2);
        }
        {
            V3 a = v3(c.x + cosf(a0) * Rx * 1.04f, 0, c.z + sinf(a0) * Rz * 1.04f), b = v3(c.x + cosf(a1) * Rx * 1.04f, 0, c.z + sinf(a1) * Rz * 1.04f);
            mesh_quad(m, a, b, vadd(b, v3(0, 21, 0)), vadd(a, v3(0, 21, 0)), conc);
            /* cubierta */
            mesh_quad(m, vadd(a, v3(0, 21, 0)), vadd(b, v3(0, 21, 0)),
                      v3(c.x + cosf(a1) * Rx * 0.86f, 24, c.z + sinf(a1) * Rz * 0.86f), v3(c.x + cosf(a0) * Rx * 0.86f, 24, c.z + sinf(a0) * Rz * 0.86f), col(240, 240, 244));
        }
    }
    /* pebetero */
    {
        V3 t = v3(c.x + Rx * 1.15f, 0, c.z);
        mesh_prism(m, t, 2.0f, 30, 6, col(200, 200, 206), col(200, 200, 206));
        mesh_cone(m, vadd(t, v3(0, 34, 0)), 4.5f, -4.0f, 8, col(220, 180, 60));
        mesh_cone(m, vadd(t, v3(0, 34, 0)), 3.0f, 6.0f, 6, col(255, 140, 30));
        mesh_cone(m, vadd(t, v3(0, 34, 0)), 1.6f, 9.0f, 5, col(255, 220, 80));
    }
}

static void alberca_olimpica(Mesh *m, V3 c, V3 f) {
    V3 r = v3(-f.z, 0, f.x);
    int k, S = 8;
    Col glass = col(120, 170, 210), wall = col(236, 238, 242);
    mesh_obox(m, c, r, f, 30, 45, 9, glass, wall);
    /* techo curvo */
    for (k = 0; k < S; k++) {
        float a0 = (float)M_PI * k / S, a1 = (float)M_PI * (k + 1) / S;
        V3 p0 = lp(c, r, f, -cosf(a0) * 30, 9 + sinf(a0) * 9, -45), p1 = lp(c, r, f, -cosf(a1) * 30, 9 + sinf(a1) * 9, -45);
        V3 q0 = lp(c, r, f, -cosf(a0) * 30, 9 + sinf(a0) * 9, 45), q1 = lp(c, r, f, -cosf(a1) * 30, 9 + sinf(a1) * 9, 45);
        mesh_quad(m, p0, p1, q1, q0, wall);
        mesh_tri(m, lp(c, r, f, -cosf(a0) * 30, 9, -45), p0, p1, glass);
        mesh_tri(m, lp(c, r, f, -cosf(a0) * 30, 9, 45), q0, q1, glass);
    }
}

static void velodromo(Mesh *m, V3 c) {
    int k, S = 14;
    for (k = 0; k < S; k++) {
        float a0 = 2 * (float)M_PI * k / S, a1 = 2 * (float)M_PI * (k + 1) / S;
        V3 a = v3(c.x + cosf(a0) * 45, 0, c.z + sinf(a0) * 30), b = v3(c.x + cosf(a1) * 45, 0, c.z + sinf(a1) * 30);
        V3 a2 = v3(c.x + cosf(a0) * 38, 14, c.z + sinf(a0) * 24), b2 = v3(c.x + cosf(a1) * 38, 14, c.z + sinf(a1) * 24);
        mesh_quad(m, a, b, b2, a2, col(200, 206, 214));
        mesh_tri(m, a2, b2, v3(c.x, 18, c.z), col(170, 180, 196));
    }
}

static void residencia(Mesh *m, Rng *rg, V3 c, V3 f) {
    static const int pal[][3] = {{230, 230, 236}, {240, 220, 180}, {200, 220, 236}, {236, 200, 196}};
    const int *p = pal[rng_u(rg) % 4];
    V3 r = v3(-f.z, 0, f.x);
    float h = rng_range(rg, 24, 46), y;
    Col wall = col(p[0], p[1], p[2]);
    mesh_obox(m, c, r, f, 8, 22, h, wall, col(150, 150, 156));
    for (y = 3; y < h - 2; y += 3.2f) {
        int s;
        for (s = -1; s <= 1; s += 2)
            mesh_quad_flat(m, lp(c, r, f, s * 8.05f, y, -20.5f), lp(c, r, f, s * 8.05f, y, 20.5f), lp(c, r, f, s * 8.05f, y + 1.4f, 20.5f), lp(c, r, f, s * 8.05f, y + 1.4f, -20.5f), col(70, 96, 130));
    }
}

static void cancha(Mesh *m, V3 c, V3 f, float hw, float hl, Col piso, Col linea) {
    V3 r = v3(-f.z, 0, f.x);
    mesh_quad_flat(m, lp(c, r, f, -hw, 0.06f, -hl), lp(c, r, f, hw, 0.06f, -hl), lp(c, r, f, hw, 0.06f, hl), lp(c, r, f, -hw, 0.06f, hl), piso);
    mesh_quad_flat(m, lp(c, r, f, -hw, 0.08f, -0.2f), lp(c, r, f, hw, 0.08f, -0.2f), lp(c, r, f, hw, 0.08f, 0.2f), lp(c, r, f, -hw, 0.08f, 0.2f), linea);
}

static void banderas(Mesh *m, V3 c, V3 f, int n) {
    static const int pal[][3] = {{220, 40, 50}, {40, 90, 200}, {250, 210, 40}, {40, 160, 80}, {250, 250, 250}, {240, 130, 40}};
    V3 r = v3(-f.z, 0, f.x);
    int k;
    for (k = 0; k < n; k++) {
        V3 p = lp(c, r, f, 0, 0, (k - n * 0.5f) * 5.0f);
        const int *cc = pal[k % 6];
        mesh_prism(m, p, 0.12f, 10, 4, col(220, 220, 226), col(220, 220, 226));
        mesh_quad_flat(m, vadd(p, v3(0, 7.5f, 0)), vadd(p, vadd(vscale(r, 3.0f), v3(0, 7.5f, 0))), vadd(p, vadd(vscale(r, 3.0f), v3(0, 9.6f, 0))), vadd(p, v3(0, 9.6f, 0)), col(cc[0], cc[1], cc[2]));
    }
}

static void scenery_villa(Track *tr) {
    Rng rg = {2028};
    V3 q, f;
    int k;
    /* estadio olimpico dentro del circuito si cabe, si no afuera */
    if (villa_spot(tr, &rg, &q, &f, 80, 60, 12, 1) || villa_spot(tr, &rg, &q, &f, 80, 60, 12, -1))
        estadio(near_chunk(tr, q), q);
    if (villa_spot(tr, &rg, &q, &f, 32, 47, 12, -1)) alberca_olimpica(near_chunk(tr, q), q, f);
    if (villa_spot(tr, &rg, &q, &f, 47, 47, 12, -1)) velodromo(near_chunk(tr, q), q);
    for (k = 0; k < 12; k++)
        if (villa_spot(tr, &rg, &q, &f, 10, 24, 14, 0)) residencia(near_chunk(tr, q), &rg, q, f);
    for (k = 0; k < 8; k++) {
        if (!villa_spot(tr, &rg, &q, &f, 20, 32, 6, -1)) continue;
        if (k & 1) cancha(near_chunk(tr, q), q, f, 20, 32, col(70, 160, 80), col(240, 240, 240));
        else cancha(near_chunk(tr, q), q, f, 11, 18, col(60, 110, 170), col(240, 240, 240));
    }
    for (k = 0; k < 3; k++)
        if (villa_spot(tr, &rg, &q, &f, 4, 26, 6, -1)) banderas(near_chunk(tr, q), q, f, 10);
    /* gradas junto a la recta de salida y boxes */
    {
        int i0 = 12;
        V3 rr = tr->r[i0], ff = vnorm(v3(tr->t[i0].x, 0, tr->t[i0].z));
        V3 rel = vsub(tr->p[i0], tr->center);
        float side_out = (rr.x * rel.x + rr.z * rel.z) > 0 ? 1.0f : -1.0f;
        grada(chunk_mesh(tr, i0 + 20), &rg, at(tr, i0 + 20, side_out * (tr->wall + 4.0f), 0), vscale(rr, -side_out), ff, 50.0f);
        boxes_y_torre(chunk_mesh(tr, i0 + 30), at(tr, i0 + 30, -side_out * (tr->wall + 22.0f), 0), vscale(rr, side_out), ff, 50.0f);
    }
    scatter_trees(tr, &rg, 220, 0, 5, 80);
    lamp_posts(tr, 16);
}

/* ---------- Parque acuatico en ladera: albercas en terrazas, toboganes y rio lento ---------- */

/* altura minima y maxima del terreno bajo un rectangulo */
static void ground_range(const Track *tr, V3 c, V3 r, V3 f, float hw, float hl, float *hmin, float *hmax) {
    float x, z;
    *hmin = 1e9f; *hmax = -1e9f;
    for (x = -hw; x <= hw + 0.01f; x += hw) for (z = -hl; z <= hl + 0.01f; z += hl) {
        float h = terrain_h(tr, c.x + r.x * x + f.x * z, c.z + r.z * x + f.z * z);
        if (h < *hmin) *hmin = h;
        if (h > *hmax) *hmax = h;
    }
}

/* alberca nivelada sobre una terraza (la base absorbe la pendiente) */
static void alberca(Track *tr, Mesh *m, V3 c, V3 f, float hw, float hl, Col agua) {
    V3 r = v3(-f.z, 0, f.x);
    float hmin, hmax, top;
    int k;
    ground_range(tr, c, r, f, hw + 3, hl + 3, &hmin, &hmax);
    top = hmax + 0.4f;
    c.y = hmin - 0.5f;
    mesh_obox(m, c, r, f, hw + 3, hl + 3, top - c.y, col(206, 196, 176), col(232, 226, 210));
    c.y = top + 0.02f;
    mesh_quad_flat(m, lp(c, r, f, -hw, 0, -hl), lp(c, r, f, hw, 0, -hl), lp(c, r, f, hw, 0, hl), lp(c, r, f, -hw, 0, hl), agua);
    for (k = 1; k < (int)(hw / 2.5f); k++) {
        float x = -hw + k * 5.0f;
        if (x >= hw) break;
        mesh_quad_flat(m, lp(c, r, f, x - 0.1f, 0.01f, -hl + 1), lp(c, r, f, x + 0.1f, 0.01f, -hl + 1), lp(c, r, f, x + 0.1f, 0.01f, hl - 1), lp(c, r, f, x - 0.1f, 0.01f, hl - 1), col_shade(agua, 1.15f));
    }
    /* camastros y sombrillas en la orilla */
    for (k = 0; k < 4; k++) {
        V3 p = lp(c, r, f, hw + 1.6f, -0.02f, -hl + 2 + k * (2 * hl - 4) / 3.0f);
        mesh_obox(m, p, r, f, 0.4f, 1.0f, 0.4f, col(250, 250, 250), col(250, 250, 250));
        mesh_prism(m, vadd(p, v3(0.8f, 0, 0)), 0.06f, 2.4f, 4, col(200, 200, 200), col(200, 200, 200));
        mesh_cone(m, vadd(p, v3(0.8f, 2.4f, 0)), 1.6f, 0.6f, 8, (k & 1) ? col(240, 90, 60) : col(60, 150, 230));
    }
}

/* torre de toboganes con tubos en espiral hasta una alberca de llegada */
static void toboganes(Track *tr, Mesh *m, V3 c) {
    static const int pal[][3] = {{250, 200, 30}, {40, 120, 230}, {230, 60, 60}};
    float base = terrain_h(tr, c.x, c.z);
    int t, k;
    V3 b = v3(c.x, base, c.z);
    mesh_obox(m, b, v3(1, 0, 0), v3(0, 0, 1), 3.5f, 3.5f, 20, col(230, 230, 236), col(120, 160, 200));
    mesh_obox(m, vadd(b, v3(0, 20, 0)), v3(1, 0, 0), v3(0, 0, 1), 4.5f, 4.5f, 0.6f, col(200, 200, 206), col(200, 200, 206));
    mesh_cone(m, vadd(b, v3(0, 23, 0)), 5.5f, 3.0f, 8, col(240, 120, 40));
    for (k = 0; k < 4; k++) mesh_obox(m, vadd(b, v3((k & 1) ? 4.2f : -4.2f, 20.6f, (k & 2) ? 4.2f : -4.2f)), v3(1, 0, 0), v3(0, 0, 1), 0.2f, 0.2f, 2.4f, col(200, 200, 206), col(200, 200, 206));
    for (t = 0; t < 3; t++) {
        const int *cc = pal[t];
        Col cl = col(cc[0], cc[1], cc[2]);
        float R = 7.0f + t * 4.0f, a = t * 2.1f;
        V3 prev = v3(b.x + cosf(a) * R, base + 19.5f - t * 2, b.z + sinf(a) * R);
        for (k = 1; k <= 16; k++) {
            float aa = a + k * 0.55f, yy = base + 19.5f - t * 2 - k * (17.0f - t * 2) / 16.0f;
            V3 p = v3(b.x + cosf(aa) * (R + k * 0.6f), yy, b.z + sinf(aa) * (R + k * 0.6f));
            V3 d = vsub(p, prev), side = vscale(vnorm(vcross(d, v3(0, 1, 0))), 0.9f);
            mesh_quad(m, vadd(prev, side), vsub(prev, side), vsub(p, side), vadd(p, side), cl);
            mesh_quad(m, vadd(prev, side), vadd(p, side), vadd(vadd(p, side), v3(0, 0.7f, 0)), vadd(vadd(prev, side), v3(0, 0.7f, 0)), col_shade(cl, 0.8f));
            mesh_quad(m, vsub(prev, side), vsub(p, side), vadd(vsub(p, side), v3(0, 0.7f, 0)), vadd(vsub(prev, side), v3(0, 0.7f, 0)), col_shade(cl, 0.8f));
            if (k % 4 == 0) mesh_prism(m, v3(p.x, terrain_h(tr, p.x, p.z), p.z), 0.25f, p.y - terrain_h(tr, p.x, p.z), 4, col(210, 210, 214), col(210, 210, 214));
            prev = p;
        }
    }
}

static void palapa(Track *tr, Mesh *m, V3 p) {
    p.y = terrain_h(tr, p.x, p.z);
    mesh_prism(m, p, 0.25f, 3.2f, 5, col(130, 96, 60), col(130, 96, 60));
    mesh_cone(m, vadd(p, v3(0, 3.0f, 0)), 3.4f, 1.8f, 8, col(206, 176, 110));
}

/* rio lento: anillo de agua alrededor de una isla con palmeras */
static void rio_lento(Track *tr, Mesh *m, V3 c, float R) {
    int k, S = 16;
    for (k = 0; k < S; k++) {
        float a0 = 2 * (float)M_PI * k / S, a1 = 2 * (float)M_PI * (k + 1) / S;
        V3 o0 = v3(c.x + cosf(a0) * R, 0, c.z + sinf(a0) * R * 0.7f), o1 = v3(c.x + cosf(a1) * R, 0, c.z + sinf(a1) * R * 0.7f);
        V3 i0 = v3(c.x + cosf(a0) * (R - 7), 0, c.z + sinf(a0) * (R - 7) * 0.7f), i1 = v3(c.x + cosf(a1) * (R - 7), 0, c.z + sinf(a1) * (R - 7) * 0.7f);
        o0.y = terrain_h(tr, o0.x, o0.z) + 0.12f; o1.y = terrain_h(tr, o1.x, o1.z) + 0.12f;
        i0.y = terrain_h(tr, i0.x, i0.z) + 0.12f; i1.y = terrain_h(tr, i1.x, i1.z) + 0.12f;
        mesh_quad_flat(m, o0, o1, i1, i0, col(60, 170, 220));
    }
    for (k = 0; k < 5; k++) {
        V3 q = v3(c.x + cosf(k * 1.25f) * (R - 14) * 0.6f, 0, c.z + sinf(k * 1.25f) * (R - 14) * 0.4f);
        q.y = terrain_h(tr, q.x, q.z);
        palmera(m, q, 1.0f);
    }
}

static void scenery_aztlan(Track *tr) {
    Rng rg = {1325};
    V3 q, f;
    int k;
    /* gran alberca de olas y rio lento dentro del circuito */
    if (villa_spot(tr, &rg, &q, &f, 26, 40, 12, 1)) alberca(tr, near_chunk(tr, q), q, f, 22, 36, col(40, 150, 220));
    if (villa_spot(tr, &rg, &q, &f, 50, 40, 10, 1)) rio_lento(tr, near_chunk(tr, q), q, 46);
    /* torres de toboganes */
    for (k = 0; k < 3; k++)
        if (villa_spot(tr, &rg, &q, &f, 26, 26, 12, k == 0 ? 1 : -1)) {
            toboganes(tr, near_chunk(tr, q), q);
            alberca(tr, near_chunk(tr, q), vadd(q, vscale(f, 20)), f, 6, 6, col(50, 180, 230));
        }
    /* albercas en terrazas por todo el parque */
    for (k = 0; k < 10; k++)
        if (villa_spot(tr, &rg, &q, &f, 14, 20, 8, -1)) alberca(tr, near_chunk(tr, q), q, f, rng_range(&rg, 7, 11), rng_range(&rg, 10, 16), (k & 1) ? col(60, 170, 230) : col(80, 200, 220));
    /* palapas y palmeras */
    for (k = 0; k < 40; k++)
        if (villa_spot(tr, &rg, &q, &f, 3.5f, 3.5f, 4, -1)) palapa(tr, near_chunk(tr, q), q);
    for (k = 0; k < 90; k++)
        if (villa_spot(tr, &rg, &q, &f, 3, 3, 4, -1)) { q.y = terrain_h(tr, q.x, q.z); palmera(near_chunk(tr, q), q, rng_range(&rg, 0.9f, 1.3f)); }
    /* gradas en la recta del lado izquierdo (la bajada) */
    {
        int i0 = 20;
        V3 rr = tr->r[i0], ff = vnorm(v3(tr->t[i0].x, 0, tr->t[i0].z));
        V3 rel = vsub(tr->p[i0], tr->center);
        float side_out = (rr.x * rel.x + rr.z * rel.z) > 0 ? 1.0f : -1.0f;
        V3 g = at(tr, i0 + 25, side_out * (tr->wall + 4.0f), 0);
        g.y = terrain_h(tr, g.x, g.z);
        grada(chunk_mesh(tr, i0 + 25), &rg, g, vscale(rr, -side_out), ff, 40.0f);
    }
}

/* ---------- El Seminario: claustros de tejado rojo, capilla, universidad y colonia ---------- */

/* pabellon de muros claros con tejado rojo, solo si no toca la pista */
static void pabellon(Track *tr, V3 c, V3 f, float hw, float hl, float h, Col wall, Col roof) {
    V3 r = v3(-f.z, 0, f.x);
    Mesh *m;
    float y;
    if (!footprint_ok(tr, c, r, f, -hw, hw, hl, 6.0f)) return;
    m = near_chunk(tr, c);
    mesh_obox(m, c, r, f, hw, hl, h, wall, wall);
    gable_roof(m, c, r, f, hw, hl, h, hw * 0.55f, roof, wall);
    for (y = 2.2f; y < h - 1.0f; y += 3.4f) {
        int s;
        for (s = -1; s <= 1; s += 2)
            mesh_quad_flat(m, lp(c, r, f, s * (hw + 0.05f), y, -hl + 1.5f), lp(c, r, f, s * (hw + 0.05f), y, hl - 1.5f),
                           lp(c, r, f, s * (hw + 0.05f), y + 1.3f, hl - 1.5f), lp(c, r, f, s * (hw + 0.05f), y + 1.3f, -hl + 1.5f), col(70, 60, 56));
    }
}

static void capilla(Track *tr, V3 c, V3 f) {
    V3 r = v3(-f.z, 0, f.x);
    Mesh *m;
    Col wall = col(236, 226, 206), roof = col(170, 70, 50);
    if (!footprint_ok(tr, c, r, f, -12, 12, 26, 6.0f)) return;
    m = near_chunk(tr, c);
    mesh_obox(m, c, r, f, 9, 22, 13, wall, wall);
    gable_roof(m, c, r, f, 9, 22, 13, 6, roof, wall);
    /* campanario */
    mesh_obox(m, lp(c, r, f, 0, 0, 24), r, f, 3.5f, 3.5f, 26, wall, wall);
    mesh_obox(m, lp(c, r, f, 0, 26, 24), r, f, 3.0f, 3.0f, 5, col(220, 210, 190), col(220, 210, 190));
    mesh_cone(m, lp(c, r, f, 0, 31, 24), 3.4f, 5.0f, 4, roof);
    /* cruz sencilla en lo alto */
    mesh_obox(m, lp(c, r, f, 0, 36, 24), r, f, 0.15f, 0.15f, 3.0f, col(240, 240, 240), col(240, 240, 240));
    mesh_obox(m, lp(c, r, f, 0, 37.6f, 24), r, f, 1.0f, 0.15f, 0.3f, col(240, 240, 240), col(240, 240, 240));
    /* portada */
    mesh_quad_flat(m, lp(c, r, f, -2.5f, 0, 25.6f + 3.6f), lp(c, r, f, 2.5f, 0, 25.6f + 3.6f), lp(c, r, f, 2.5f, 6, 25.6f + 3.6f), lp(c, r, f, -2.5f, 6, 25.6f + 3.6f), col(80, 56, 44));
}

static void edificio_plano(Track *tr, V3 c, V3 f, float hw, float hl, float h, Col wall, Col top) {
    V3 r = v3(-f.z, 0, f.x);
    Mesh *m;
    float y;
    if (!footprint_ok(tr, c, r, f, -hw, hw, hl, 6.0f)) return;
    m = near_chunk(tr, c);
    mesh_obox(m, c, r, f, hw, hl, h, wall, top);
    for (y = 2.5f; y < h - 1.0f; y += 3.6f) {
        int s;
        for (s = -1; s <= 1; s += 2)
            mesh_quad_flat(m, lp(c, r, f, s * (hw + 0.05f), y, -hl + 1), lp(c, r, f, s * (hw + 0.05f), y, hl - 1),
                           lp(c, r, f, s * (hw + 0.05f), y + 1.5f, hl - 1), lp(c, r, f, s * (hw + 0.05f), y + 1.5f, -hl + 1), col(80, 110, 140));
    }
}

static void scenery_seminario(Track *tr) {
    Rng rg = {1950};
    Col cream = col(236, 226, 206), tile = col(170, 64, 48);
    V3 ns = vnorm(v3(SEM_X(185) - SEM_X(185), 0, SEM_Z(460) - SEM_Z(200)));   /* eje norte-sur */
    V3 ew = v3(1, 0, 0);
    int k;
    /* claustros: espina norte-sur y alas este-oeste */
    pabellon(tr, v3(SEM_X(185), 0, SEM_Z(330)), ns, 6, SEM_Z(460) - SEM_Z(330) - 4, 9, cream, tile);
    {
        static const float wy[] = {218, 272, 338, 398, 452};
        for (k = 0; k < 5; k++) {
            pabellon(tr, v3(SEM_X(145), 0, SEM_Z(wy[k])), ew, 6, SEM_X(180) - SEM_X(145) - 2, 8, cream, tile);
            if (k != 2) pabellon(tr, v3(SEM_X(222), 0, SEM_Z(wy[k])), ew, 6, SEM_X(262) - SEM_X(222) - 2, 8, cream, tile);
        }
    }
    capilla(tr, v3(SEM_X(240), 0, SEM_Z(325)), vnorm(v3(1, 0, -0.3f)));
    /* glorieta con fuente frente al seminario */
    {
        V3 g = v3(SEM_X(196), 0, SEM_Z(508));
        if (footprint_ok(tr, g, v3(1, 0, 0), v3(0, 0, 1), -12, 12, 12, 3.0f)) {
            Mesh *m = near_chunk(tr, g);
            mesh_prism(m, g, 11, 0.4f, 12, col(200, 196, 186), col(90, 160, 80));
            mesh_prism(m, g, 4, 0.9f, 10, col(210, 206, 196), col(80, 150, 220));
            mesh_prism(m, g, 0.6f, 3.0f, 6, col(220, 216, 206), col(220, 216, 206));
        }
    }
    /* edificios de la universidad */
    edificio_plano(tr, v3(SEM_X(505), 0, SEM_Z(148)), ew, SEM_X(550) - SEM_X(505), 10, 10, col(232, 232, 236), col(170, 172, 178));
    edificio_plano(tr, v3(SEM_X(400), 0, SEM_Z(252)), ew, SEM_X(438) - SEM_X(400), 12, 12, col(224, 224, 228), col(160, 162, 168));
    edificio_plano(tr, v3(SEM_X(510), 0, SEM_Z(250)), ew, SEM_X(555) - SEM_X(510), 10, 9, col(230, 226, 220), col(160, 162, 168));
    edificio_plano(tr, v3(SEM_X(455), 0, SEM_Z(297)), ew, SEM_X(498) - SEM_X(455), 9, 8, col(226, 226, 230), col(40, 60, 110));
    edificio_plano(tr, v3(SEM_X(345), 0, SEM_Z(298)), ns, 8, 16, 10, col(220, 220, 226), col(160, 162, 168));
    /* estacionamiento con autos */
    {
        V3 c = v3(SEM_X(300), 0, SEM_Z(215));
        if (footprint_ok(tr, c, v3(1, 0, 0), v3(0, 0, 1), -40, 40, 30, 4.0f)) {
            Mesh *m = near_chunk(tr, c);
            int a, b;
            mesh_quad_flat(m, vadd(c, v3(-40, 0.05f, -30)), vadd(c, v3(40, 0.05f, -30)), vadd(c, v3(40, 0.05f, 30)), vadd(c, v3(-40, 0.05f, 30)), col(110, 110, 116));
            for (a = 0; a < 12; a++) for (b = 0; b < 4; b++) {
                static const int pc[][3] = {{200, 40, 40}, {40, 80, 180}, {230, 230, 235}, {30, 30, 34}, {150, 150, 156}};
                const int *cc = pc[rng_u(&rg) % 5];
                if (rng_f(&rg) < 0.3f) continue;
                mesh_obox(m, vadd(c, v3(-36 + a * 6.5f, 0, -22 + b * 14.0f)), v3(1, 0, 0), v3(0, 0, 1), 0.9f, 2.1f, 1.3f, col(cc[0], cc[1], cc[2]), col_shade(col(cc[0], cc[1], cc[2]), 0.7f));
            }
        }
    }
    /* colonia de casas pequenas al sur y al este */
    for (k = 0; k < 180; k++) {
        V3 q, f;
        if (!villa_spot(tr, &rg, &q, &f, 6, 7, 8, 0)) continue;
        if (q.z < SEM_Z(250) && q.x < SEM_X(500)) continue;   /* al norte hay naves, no casas */
        casa_costera(near_chunk(tr, q), &rg, q, vnorm(v3(0.84f, 0, -0.54f)), 0);
    }
    /* naves industriales al norte */
    for (k = 0; k < 10; k++) {
        V3 q = v3(SEM_X(60 + k * 60), 0, SEM_Z(40 + (k % 3) * 20));
        edificio_plano(tr, q, v3(0, 0, 1), 18, 24, 10, col(200, 200, 204), col(176, 178, 182));
    }
    /* autopista al sur, en diagonal */
    {
        V3 a = v3(SEM_X(100), 0.04f, SEM_Z(790)), b = v3(SEM_X(760), 0.04f, SEM_Z(440));
        V3 d = vnorm(vsub(b, a)), n = v3(-d.z, 0, d.x);
        float L = vlen(vsub(b, a)), s;
        for (s = 0; s < L; s += 40) {
            V3 p0 = vadd(a, vscale(d, s)), p1 = vadd(a, vscale(d, fminf(s + 40, L)));
            V3 mid = vscale(vadd(p0, p1), 0.5f);
            Mesh *m;
            if (min_dist_xz(tr, mid, NULL) < tr->wall + 30) continue;
            m = near_chunk(tr, mid);
            mesh_quad_flat(m, vadd(p0, vscale(n, -16)), vadd(p1, vscale(n, -16)), vadd(p1, vscale(n, 16)), vadd(p0, vscale(n, 16)), col(96, 96, 102));
            mesh_quad_flat(m, vadd(p0, vadd(vscale(n, -0.4f), v3(0, 0.02f, 0))), vadd(p1, vadd(vscale(n, -0.4f), v3(0, 0.02f, 0))),
                           vadd(p1, vadd(vscale(n, 0.4f), v3(0, 0.02f, 0))), vadd(p0, vadd(vscale(n, 0.4f), v3(0, 0.02f, 0))), col(230, 200, 60));
        }
    }
    scatter_trees(tr, &rg, 160, 0, 4, 60);
    lamp_posts(tr, 15);
}

/* ---------- Isla del Sol: mar alrededor, playa, hoteles y palmeras ---------- */

#define ISLA_SEG 64
static float isla_r[ISLA_SEG];   /* radio de la costa por direccion */

static void isla_costa(Track *tr) {
    int k, i;
    for (k = 0; k < ISLA_SEG; k++) isla_r[k] = 0;
    for (i = 0; i < tr->n; i++) {
        float dx = tr->p[i].x - tr->center.x, dz = tr->p[i].z - tr->center.z;
        float a = atan2f(dz, dx), d = sqrtf(dx * dx + dz * dz);
        int s = ((int)floorf((a + (float)M_PI) / (2 * (float)M_PI) * ISLA_SEG)) % ISLA_SEG;
        int t;
        for (t = -2; t <= 2; t++) {
            int u = (s + t + ISLA_SEG) % ISLA_SEG;
            if (d > isla_r[u]) isla_r[u] = d;
        }
    }
    /* suavizar y dejar espacio para hoteles y playa */
    for (k = 0; k < 3; k++) {
        float tmp[ISLA_SEG];
        for (i = 0; i < ISLA_SEG; i++) tmp[i] = (isla_r[(i + ISLA_SEG - 1) % ISLA_SEG] + 2 * isla_r[i] + isla_r[(i + 1) % ISLA_SEG]) * 0.25f;
        memcpy(isla_r, tmp, sizeof(tmp));
    }
    for (k = 0; k < ISLA_SEG; k++) isla_r[k] += 150.0f + 25.0f * sinf(k * 0.9f);
}

static V3 isla_pt(Track *tr, int k, float extra, float y) {
    float a = -(float)M_PI + 2 * (float)M_PI * (k % ISLA_SEG) / ISLA_SEG;
    float r = isla_r[k % ISLA_SEG] + extra;
    return v3(tr->center.x + cosf(a) * r, y, tr->center.z + sinf(a) * r);
}

static void add_mar_isla(Track *tr) {
    Mesh *m = &tr->world;
    int k;
    for (k = 0; k < ISLA_SEG; k++) {
        V3 b0 = isla_pt(tr, k, -45, 0.06f), b1 = isla_pt(tr, k + 1, -45, 0.06f);
        V3 c0 = isla_pt(tr, k, 0, 0.06f), c1 = isla_pt(tr, k + 1, 0, 0.06f);
        V3 s0 = isla_pt(tr, k, 0, 0.12f), s1 = isla_pt(tr, k + 1, 0, 0.12f);
        V3 f0 = isla_pt(tr, k, 8, 0.13f), f1 = isla_pt(tr, k + 1, 8, 0.13f);
        V3 t0 = isla_pt(tr, k, 70, 0.12f), t1 = isla_pt(tr, k + 1, 70, 0.12f);
        V3 o0 = isla_pt(tr, k, 3200, 0.12f), o1 = isla_pt(tr, k + 1, 3200, 0.12f);
        mesh_quad_flat(m, b0, b1, c1, c0, col(240, 226, 180));                 /* arena */
        mesh_quad_flat(m, s0, s1, f1, f0, col(240, 248, 250));                 /* espuma */
        mesh_quad_flat(m, f0, f1, t1, t0, col(60, 200, 210));                  /* agua turquesa */
        mesh_quad_flat(m, t0, t1, o1, o0, col(30, 110, 190));                  /* mar profundo */
    }
}

static void hotel_playa(Mesh *m, Rng *rg, V3 c, V3 f) {
    static const int pal[][3] = {{250, 250, 250}, {250, 230, 200}, {210, 236, 240}, {250, 214, 200}};
    const int *p = pal[rng_u(rg) % 4];
    V3 r = v3(-f.z, 0, f.x);
    float h = rng_range(rg, 30, 70), y;
    Col wall = col(p[0], p[1], p[2]);
    int k;
    /* torre de habitaciones con balcones hacia el mar (f apunta al mar) */
    mesh_obox(m, c, r, f, 26, 8, h, wall, col(180, 186, 190));
    for (y = 4; y < h - 2; y += 3.2f) {
        mesh_obox(m, vadd(lp(c, r, f, 0, 0, 8.6f), v3(0, y, 0)), r, f, 25, 0.7f, 0.25f, col(240, 240, 244), col(240, 240, 244));
        mesh_quad_flat(m, lp(c, r, f, -24, y + 0.3f, 8.05f), lp(c, r, f, 24, y + 0.3f, 8.05f), lp(c, r, f, 24, y + 2.4f, 8.05f), lp(c, r, f, -24, y + 2.4f, 8.05f), col(70, 130, 170));
        /* ventanas tambien en la fachada trasera y en los costados */
        mesh_quad_flat(m, lp(c, r, f, -24, y + 0.6f, -8.05f), lp(c, r, f, 24, y + 0.6f, -8.05f), lp(c, r, f, 24, y + 2.0f, -8.05f), lp(c, r, f, -24, y + 2.0f, -8.05f), col(80, 120, 150));
        mesh_quad_flat(m, lp(c, r, f, 26.05f, y + 0.6f, -6), lp(c, r, f, 26.05f, y + 0.6f, 6), lp(c, r, f, 26.05f, y + 2.0f, 6), lp(c, r, f, 26.05f, y + 2.0f, -6), col(80, 120, 150));
        mesh_quad_flat(m, lp(c, r, f, -26.05f, y + 0.6f, -6), lp(c, r, f, -26.05f, y + 0.6f, 6), lp(c, r, f, -26.05f, y + 2.0f, 6), lp(c, r, f, -26.05f, y + 2.0f, -6), col(80, 120, 150));
    }
    /* alberca y palapas frente al hotel */
    mesh_quad_flat(m, lp(c, r, f, -16, 0.1f, 14), lp(c, r, f, 16, 0.1f, 14), lp(c, r, f, 16, 0.1f, 30), lp(c, r, f, -16, 0.1f, 30), col(232, 226, 210));
    mesh_quad_flat(m, lp(c, r, f, -12, 0.14f, 17), lp(c, r, f, 12, 0.14f, 17), lp(c, r, f, 12, 0.14f, 27), lp(c, r, f, -12, 0.14f, 27), col(60, 190, 230));
    for (k = 0; k < 3; k++) {
        V3 q = lp(c, r, f, -14 + k * 14, 0, 34);
        mesh_prism(m, q, 0.25f, 3.0f, 5, col(130, 96, 60), col(130, 96, 60));
        mesh_cone(m, vadd(q, v3(0, 2.8f, 0)), 3.0f, 1.6f, 8, col(206, 176, 110));
    }
}

static void lancha(Mesh *m, V3 p, float yaw, Col c) {
    V3 f = v3(sinf(yaw), 0, -cosf(yaw)), r = v3(-f.z, 0, f.x);
    mesh_obox(m, p, r, f, 1.2f, 3.2f, 0.9f, c, col(240, 240, 240));
    mesh_obox(m, vadd(lp(p, r, f, 0, 0, -0.6f), v3(0, 0.9f, 0)), r, f, 0.8f, 0.9f, 0.6f, col(60, 80, 110), col(240, 240, 240));
}

static void scenery_isla(Track *tr) {
    Rng rg = {777};
    int k, tries;
    isla_costa(tr);
    /* hoteles mirando al mar, entre la pista y la playa */
    for (k = 0, tries = 0; k < 14 && tries < 400; tries++) {
        int s = rng_u(&rg) % ISLA_SEG;
        V3 pos = isla_pt(tr, s, -95, 0), out = vnorm(vsub(isla_pt(tr, s, 0, 0), tr->center));
        V3 r;
        out.y = 0; out = vnorm(out); r = v3(-out.z, 0, out.x);
        if (!footprint_ok(tr, pos, r, out, -27, 27, 9, 12.0f)) continue;
        if (!footprint_ok(tr, vadd(pos, vscale(out, 24)), r, out, -17, 17, 12, 8.0f)) continue;
        hotel_playa(near_chunk(tr, pos), &rg, pos, out);
        k++;
    }
    /* sombrillas y camastros en la arena */
    for (k = 0; k < 90; k++) {
        int s = rng_u(&rg) % ISLA_SEG;
        V3 q = isla_pt(tr, s, -rng_range(&rg, 8, 38), 0);
        Mesh *m;
        if (min_dist_xz(tr, q, NULL) < tr->wall + 6) continue;
        m = near_chunk(tr, q);
        mesh_prism(m, q, 0.06f, 2.4f, 4, col(220, 220, 220), col(220, 220, 220));
        mesh_cone(m, vadd(q, v3(0, 2.4f, 0)), 1.7f, 0.6f, 8, (k % 3 == 0) ? col(240, 80, 70) : (k % 3 == 1) ? col(250, 210, 50) : col(60, 150, 230));
        mesh_obox(m, vadd(q, v3(1.2f, 0, 0)), v3(1, 0, 0), v3(0, 0, 1), 0.4f, 1.0f, 0.35f, col(250, 250, 250), col(250, 250, 250));
    }
    /* palmeras por toda la isla */
    for (k = 0, tries = 0; k < 220 && tries < 6000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 7, 140)), 0);
        float dx, dz, a, r;
        int s;
        q.y = 0;
        dx = q.x - tr->center.x; dz = q.z - tr->center.z;
        a = atan2f(dz, dx); r = sqrtf(dx * dx + dz * dz);
        s = ((int)floorf((a + (float)M_PI) / (2 * (float)M_PI) * ISLA_SEG)) % ISLA_SEG;
        if (r > isla_r[s] - 6) continue;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 7) continue;
        palmera(chunk_mesh(tr, ni), q, rng_range(&rg, 0.9f, 1.4f));
        k++;
    }
    /* lanchas y un muelle */
    for (k = 0; k < 12; k++) {
        int s = rng_u(&rg) % ISLA_SEG;
        V3 q = isla_pt(tr, s, rng_range(&rg, 40, 260), 0.12f);
        lancha(near_chunk(tr, q), q, rng_range(&rg, 0, 6.28f), (k & 1) ? col(250, 250, 250) : col(230, 60, 50));
    }
    {
        int s = 10;
        V3 a = isla_pt(tr, s, -10, 0.9f), b = isla_pt(tr, s, 90, 0.9f);
        V3 d = vnorm(vsub(b, a)), n = v3(-d.z, 0, d.x);
        mesh_quad(near_chunk(tr, a), vadd(a, vscale(n, -2)), vadd(a, vscale(n, 2)), vadd(b, vscale(n, 2)), vadd(b, vscale(n, -2)), col(150, 110, 70));
    }
    /* gradas en la recta de salida */
    {
        int i0 = 15;
        V3 rr = tr->r[i0], ff = vnorm(v3(tr->t[i0].x, 0, tr->t[i0].z));
        V3 rel = vsub(tr->p[i0], tr->center);
        float side_in = (rr.x * rel.x + rr.z * rel.z) > 0 ? -1.0f : 1.0f;
        grada(chunk_mesh(tr, i0 + 20), &rg, at(tr, i0 + 20, side_in * (tr->wall + 4.0f), 0), vscale(rr, -side_in), ff, 40.0f);
    }
}

/* ---------- Espinazo: cresta de montana, valle con nubes y un festival de carreras en la cima ---------- */

static float plateau_y(Track *tr, V3 q) { int ni; min_dist_xz(tr, q, &ni); return tr->p[ni].y - 0.15f; }

static void escenario(Mesh *m, V3 c, V3 f) {
    V3 r = v3(-f.z, 0, f.x);
    Col truss = col(60, 60, 66), black = col(24, 24, 28);
    static const int lc[][3] = {{255, 60, 160}, {60, 200, 255}, {255, 210, 40}, {120, 255, 120}};
    int k;
    mesh_obox(m, c, r, f, 16, 7, 1.6f, black, col(40, 40, 46));
    for (k = 0; k < 4; k++) mesh_obox(m, lp(c, r, f, (k & 1) ? 15 : -15, 0, (k & 2) ? 6 : -6), r, f, 0.5f, 0.5f, 14, truss, truss);
    mesh_obox(m, vadd(c, v3(0, 14, 0)), r, f, 16.5f, 7.5f, 1.2f, truss, col(230, 60, 140));
    /* pantalla y luces de colores */
    mesh_quad_flat(m, lp(c, r, f, -8, 4, -6.9f), lp(c, r, f, 8, 4, -6.9f), lp(c, r, f, 8, 12, -6.9f), lp(c, r, f, -8, 12, -6.9f), col(40, 90, 200));
    for (k = 0; k < 8; k++) {
        const int *cc = lc[k & 3];
        mesh_obox(m, lp(c, r, f, -14 + k * 4, 13.2f, 6.8f), r, f, 0.7f, 0.5f, 0.8f, col(cc[0], cc[1], cc[2]), col(cc[0], cc[1], cc[2]));
    }
    /* torres de bocinas */
    for (k = -1; k <= 1; k += 2) mesh_obox(m, lp(c, r, f, k * 20, 0, 4), r, f, 2.5f, 2.0f, 9, black, black);
}

static void carpa(Mesh *m, V3 p, Col c) {
    mesh_obox(m, p, v3(1, 0, 0), v3(0, 0, 1), 4, 4, 3, col(240, 240, 240), col(240, 240, 240));
    mesh_cone(m, vadd(p, v3(0, 3, 0)), 6.0f, 4.0f, 4, c);
}

static void rueda_fortuna(Mesh *m, V3 c, V3 f) {
    V3 r = v3(-f.z, 0, f.x);
    float R = 18, hub = 21;
    int k, S = 16;
    static const int gc[][3] = {{240, 60, 60}, {60, 140, 240}, {250, 210, 40}, {80, 200, 120}};
    for (k = -1; k <= 1; k += 2) {
        V3 base0 = lp(c, r, f, -9, 0, k * 2.0f), base1 = lp(c, r, f, 9, 0, k * 2.0f), top = lp(c, r, f, 0, hub, k * 2.0f);
        V3 s0 = vscale(f, 0.4f);
        mesh_quad(m, vsub(base0, s0), vadd(base0, s0), vadd(top, s0), vsub(top, s0), col(220, 220, 228));
        mesh_quad(m, vsub(base1, s0), vadd(base1, s0), vadd(top, s0), vsub(top, s0), col(220, 220, 228));
    }
    for (k = 0; k < S; k++) {
        float a0 = 2 * (float)M_PI * k / S, a1 = 2 * (float)M_PI * (k + 1) / S;
        V3 p0 = lp(c, r, f, cosf(a0) * R, hub + sinf(a0) * R, 0), p1 = lp(c, r, f, cosf(a1) * R, hub + sinf(a1) * R, 0);
        V3 s0 = vscale(f, 1.2f);
        const int *cc = gc[k & 3];
        mesh_quad(m, vsub(p0, s0), vadd(p0, s0), vadd(p1, s0), vsub(p1, s0), col(240, 240, 248));
        mesh_quad_flat(m, lp(c, r, f, 0, hub, 0), p0, vadd(p0, v3(0, 0.3f, 0)), lp(c, r, f, 0, hub + 0.3f, 0), col(200, 200, 210));
        mesh_obox(m, vsub(p0, v3(0, 2.4f, 0)), r, f, 1.1f, 1.1f, 2.0f, col(cc[0], cc[1], cc[2]), col(cc[0], cc[1], cc[2]));
    }
}

/* linea de banderines entre dos postes (paralela a la pista, nunca encima) */
static void banderines(Mesh *m, V3 a, V3 b) {
    static const int pc[][3] = {{240, 60, 60}, {250, 210, 40}, {60, 140, 240}, {80, 200, 120}, {240, 120, 200}};
    V3 d = vsub(b, a);
    int k, n = (int)(vlen(d) / 1.6f);
    mesh_prism(m, a, 0.12f, 6, 4, col(200, 200, 206), col(200, 200, 206));
    mesh_prism(m, b, 0.12f, 6, 4, col(200, 200, 206), col(200, 200, 206));
    for (k = 0; k < n; k++) {
        float t0 = (float)k / n, t1 = (k + 0.8f) / n;
        float sag0 = 1.2f * sinf((float)M_PI * t0), sag1 = 1.2f * sinf((float)M_PI * t1);
        V3 p0 = vadd(vlerp(a, b, t0), v3(0, 6 - sag0, 0)), p1 = vadd(vlerp(a, b, t1), v3(0, 6 - sag1, 0));
        const int *cc = pc[k % 5];
        mesh_tri_flat(m, p0, p1, vadd(vlerp(p0, p1, 0.5f), v3(0, -1.0f, 0)), col(cc[0], cc[1], cc[2]));
    }
}

static void add_sierra_y_nubes(Track *tr) {
    Mesh *m = &tr->world;
    Rng rg = {1958};
    int k;
    /* sierras alrededor, mas altas a lo lejos */
    for (k = 0; k < 40; k++) {
        float a = 2 * (float)M_PI * k / 40 + rng_range(&rg, -0.05f, 0.05f);
        float R = tr->extent + rng_range(&rg, 500, 1400);
        V3 b = v3(tr->center.x + cosf(a) * R, -2, tr->center.z + sinf(a) * R);
        float H = rng_range(&rg, 120, 340);
        mesh_cone(m, b, rng_range(&rg, 220, 420), H, 7, (k & 1) ? col(62, 100, 66) : col(74, 110, 72));
        if (H > 260) mesh_cone(m, vadd(b, v3(0, H * 0.75f, 0)), 60, H * 0.25f, 7, col(126, 116, 104));
    }
    /* mar de nubes en el valle */
    for (k = 0; k < 46; k++) {
        float a = rng_range(&rg, 0, 6.28f), R = tr->extent + rng_range(&rg, 140, 900);
        V3 c = v3(tr->center.x + cosf(a) * R, rng_range(&rg, 70, 110), tr->center.z + sinf(a) * R);
        float W = rng_range(&rg, 50, 120);
        mesh_cone(m, c, W, 14, 8, col(244, 246, 250));
        mesh_cone(m, c, W * 0.9f, -6, 8, col(226, 230, 238));
    }
}

static void scenery_espinazo(Track *tr) {
    Rng rg = {2025};
    V3 q, f;
    int k;
    /* festival en la meseta del centro */
    if (villa_spot(tr, &rg, &q, &f, 24, 12, 10, 1)) { q.y = plateau_y(tr, q); escenario(near_chunk(tr, q), q, f); }
    if (villa_spot(tr, &rg, &q, &f, 14, 14, 10, 1)) { q.y = plateau_y(tr, q); rueda_fortuna(near_chunk(tr, q), q, f); }
    for (k = 0; k < 22; k++) {
        static const int tc[][3] = {{240, 70, 70}, {60, 150, 240}, {250, 200, 40}, {150, 80, 220}, {60, 190, 120}, {250, 130, 40}};
        const int *cc = tc[k % 6];
        if (!villa_spot(tr, &rg, &q, &f, 7, 7, 6, 1)) continue;
        q.y = plateau_y(tr, q);
        carpa(near_chunk(tr, q), q, col(cc[0], cc[1], cc[2]));
    }
    /* autos de exhibicion en el festival */
    for (k = 0; k < 14; k++) {
        static const int pc[][3] = {{220, 40, 40}, {40, 90, 200}, {250, 200, 40}, {240, 240, 244}, {30, 30, 34}};
        const int *cc = pc[k % 5];
        if (!villa_spot(tr, &rg, &q, &f, 3, 4, 6, 1)) continue;
        q.y = plateau_y(tr, q);
        mesh_obox(near_chunk(tr, q), q, v3(-f.z, 0, f.x), f, 0.95f, 2.2f, 1.2f, col(cc[0], cc[1], cc[2]), col_shade(col(cc[0], cc[1], cc[2]), 0.7f));
    }
    /* banderines y torres de luces de colores junto a la pista (del lado del festival) */
    for (k = 0; k < tr->n; k += 22) {
        V3 rel = vsub(tr->p[k], tr->center);
        float side_in;
        int k2 = (k + 10) % tr->n;
        V3 a, b;
        /* lado interior: el que mira hacia la meseta */
        side_in = inside_loop(tr, at(tr, k, tr->wall + 6, 0).x, at(tr, k, tr->wall + 6, 0).z) ? 1.0f : -1.0f;
        a = at(tr, k, side_in * (tr->wall + 4.0f), 0);
        b = at(tr, k2, side_in * (tr->wall + 4.0f), 0);
        a.y = tr->p[k].y; b.y = tr->p[k2].y;
        if (min_dist_xz(tr, a, NULL) < tr->wall + 2 || min_dist_xz(tr, b, NULL) < tr->wall + 2) continue;
        banderines(chunk_mesh(tr, k), a, b);
        (void)rel;
    }
    /* gradas junto a la recta de salida, del lado del festival */
    {
        int i0 = 30;
        V3 rr = tr->r[i0], ff = vnorm(v3(tr->t[i0].x, 0, tr->t[i0].z));
        float side_in = inside_loop(tr, at(tr, i0, tr->wall + 6, 0).x, at(tr, i0, tr->wall + 6, 0).z) ? 1.0f : -1.0f;
        V3 g = at(tr, i0 + 15, side_in * (tr->wall + 4.0f), 0);
        g.y = tr->p[i0 + 15].y;
        grada(chunk_mesh(tr, i0 + 15), &rg, g, vscale(rr, -side_in), ff, 35.0f);
    }
    /* pinos en las laderas y el valle */
    for (k = 0; k < 260; k++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 p = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 25, 220)), 0);
        if (inside_loop(tr, p.x, p.z)) continue;
        if (min_dist_xz(tr, p, &ni) < tr->wall + 20) continue;
        p.y = terrain_h(tr, p.x, p.z);
        tree_pine(chunk_mesh(tr, ni), p, rng_range(&rg, 1.4f, 2.4f));
    }
}

/* ---------- Campos del Tren: cultivos, granjas, arboles variados y via ferrea ---------- */

static void campo(Track *tr, Rng *rg, V3 c, V3 f, float hw, float hl) {
    static const int tipos[][3] = {{214, 186, 90}, {110, 160, 70}, {150, 110, 74}, {180, 200, 80}, {90, 140, 60}};
    const int *t = tipos[rng_u(rg) % 5];
    V3 r = v3(-f.z, 0, f.x);
    Mesh *m = near_chunk(tr, c);
    int k, rows = (int)(hw / 2.5f);
    Col base = col(t[0], t[1], t[2]);
    mesh_quad_flat(m, lp(c, r, f, -hw, 0.04f, -hl), lp(c, r, f, hw, 0.04f, -hl), lp(c, r, f, hw, 0.04f, hl), lp(c, r, f, -hw, 0.04f, hl), base);
    for (k = 0; k < rows; k++) {
        float x = -hw + (k + 0.5f) * (2 * hw / rows);
        mesh_quad_flat(m, lp(c, r, f, x - 0.5f, 0.07f, -hl + 1), lp(c, r, f, x + 0.5f, 0.07f, -hl + 1),
                       lp(c, r, f, x + 0.5f, 0.07f, hl - 1), lp(c, r, f, x - 0.5f, 0.07f, hl - 1), col_shade(base, 0.82f));
    }
    /* cerca de postes en dos lados */
    for (k = 0; k <= 8; k++) {
        float z = -hl + k * hl / 4.0f;
        mesh_obox(m, lp(c, r, f, -hw - 1, 0, z), r, f, 0.12f, 0.12f, 1.3f, col(120, 90, 60), col(120, 90, 60));
        mesh_obox(m, lp(c, r, f, hw + 1, 0, z), r, f, 0.12f, 0.12f, 1.3f, col(120, 90, 60), col(120, 90, 60));
    }
}

static void molino_agua(Mesh *m, V3 p) {
    Col steel = col(170, 170, 176);
    int k;
    mesh_cone(m, p, 2.5f, 13, 4, steel);
    for (k = 0; k < 8; k++) {
        float a = k * 0.785f;
        V3 hub = vadd(p, v3(0, 12.5f, 1.2f));
        mesh_tri(m, hub, vadd(hub, v3(cosf(a) * 2.6f, sinf(a) * 2.6f, 0)), vadd(hub, v3(cosf(a + 0.3f) * 2.6f, sinf(a + 0.3f) * 2.6f, 0)), col(220, 220, 226));
    }
}

static void vaca(Mesh *m, V3 p, float yaw) {
    V3 f = v3(sinf(yaw), 0, -cosf(yaw)), r = v3(-f.z, 0, f.x);
    mesh_obox(m, vadd(p, v3(0, 0.7f, 0)), r, f, 0.45f, 0.9f, 0.7f, col(245, 245, 245), col(40, 40, 40));
    mesh_obox(m, vadd(lp(p, r, f, 0, 0, -1.1f), v3(0, 1.0f, 0)), r, f, 0.25f, 0.3f, 0.4f, col(40, 40, 40), col(40, 40, 40));
    mesh_obox(m, p, r, f, 0.35f, 0.7f, 0.7f, col(60, 50, 44), col(60, 50, 44));
}

static void via_tren(Track *tr) {
    V3 a = v3(GRJ_A_X, 0, GRJ_A_Z), b = v3(GRJ_B_X, 0, GRJ_B_Z);
    V3 d = vnorm(vsub(b, a)), n = v3(d.z, 0, -d.x);
    float L, s, off = tr->wall + 30.0f;
    /* lado exterior: el que se aleja del centro del circuito */
    if (vdot(n, vsub(vscale(vadd(a, b), 0.5f), tr->center)) < 0) n = vscale(n, -1);
    a = vsub(a, vscale(d, 500)); b = vadd(b, vscale(d, 500));
    L = vlen(vsub(b, a));
    for (s = 0; s < L; s += 20) {
        V3 p0 = vadd(vadd(a, vscale(d, s)), vscale(n, off)), p1 = vadd(vadd(a, vscale(d, s + 20)), vscale(n, off));
        V3 mid = vscale(vadd(p0, p1), 0.5f);
        Mesh *m;
        float t;
        if (min_dist_xz(tr, mid, NULL) < tr->wall + 18) continue;
        m = near_chunk(tr, mid);
        mesh_quad(m, vadd(p0, vadd(vscale(n, -3.0f), v3(0, 0.3f, 0))), vadd(p1, vadd(vscale(n, -3.0f), v3(0, 0.3f, 0))),
                  vadd(p1, vadd(vscale(n, 3.0f), v3(0, 0.3f, 0))), vadd(p0, vadd(vscale(n, 3.0f), v3(0, 0.3f, 0))), col(140, 136, 128));
        for (t = 0; t < 20; t += 2.0f) {
            V3 q = vadd(p0, vscale(d, t));
            mesh_quad_flat(m, vadd(q, vadd(vscale(n, -1.4f), v3(0, 0.36f, 0))), vadd(q, vadd(vscale(n, 1.4f), v3(0, 0.36f, 0))),
                           vadd(q, vadd(vadd(vscale(n, 1.4f), vscale(d, 0.5f)), v3(0, 0.36f, 0))), vadd(q, vadd(vadd(vscale(n, -1.4f), vscale(d, 0.5f)), v3(0, 0.36f, 0))), col(110, 80, 56));
        }
        {
            int k;
            for (k = -1; k <= 1; k += 2) {
                V3 o = vscale(n, k * 0.75f);
                mesh_quad(m, vadd(vadd(p0, o), v3(0, 0.38f, 0)), vadd(vadd(p1, o), v3(0, 0.38f, 0)), vadd(vadd(p1, o), v3(0, 0.55f, 0)), vadd(vadd(p0, o), v3(0, 0.55f, 0)), col(90, 90, 96));
            }
        }
    }
    /* tren de carga detenido sobre la via */
    {
        static const int cc[][3] = {{180, 50, 40}, {60, 90, 150}, {200, 160, 50}, {80, 120, 80}, {140, 140, 146}};
        float s0 = L * 0.35f;
        int k;
        for (k = 0; k < 7; k++) {
            V3 p = vadd(vadd(a, vscale(d, s0 + k * 15.5f)), vscale(n, off));
            Mesh *m = near_chunk(tr, p);
            p.y = 0.55f;
            if (k == 0) {
                mesh_obox(m, p, n, d, 1.6f, 7.0f, 3.8f, col(240, 200, 40), col(60, 60, 64));
                mesh_obox(m, vadd(p, v3(0, 3.8f, 0)), n, d, 1.4f, 2.2f, 0.8f, col(60, 60, 64), col(60, 60, 64));
            } else {
                const int *c = cc[k % 5];
                mesh_obox(m, p, n, d, 1.6f, 7.0f, 3.4f, col(c[0], c[1], c[2]), col_shade(col(c[0], c[1], c[2]), 0.8f));
            }
        }
    }
}

static void scenery_granja(Track *tr) {
    Rng rg = {1880};
    V3 q, f;
    int k;
    via_tren(tr);
    for (k = 0; k < 26; k++)
        if (villa_spot(tr, &rg, &q, &f, 24, 34, 6, -1)) campo(tr, &rg, q, f, rng_range(&rg, 16, 22), rng_range(&rg, 24, 32));
    for (k = 0; k < 4; k++)
        if (villa_spot(tr, &rg, &q, &f, 26, 22, 12, -1)) {
            barn(near_chunk(tr, q), q, f);
            casa(near_chunk(tr, q), &rg, vadd(q, vscale(v3(-f.z, 0, f.x), 32)), f);
        }
    for (k = 0; k < 6; k++)
        if (villa_spot(tr, &rg, &q, &f, 4, 4, 6, -1)) molino_agua(near_chunk(tr, q), q);
    for (k = 0; k < 40; k++)
        if (villa_spot(tr, &rg, &q, &f, 2, 2, 5, -1)) mesh_prism(near_chunk(tr, q), q, 1.1f, 1.6f, 6, col(220, 190, 90), col(235, 205, 110));
    for (k = 0; k < 30; k++)
        if (villa_spot(tr, &rg, &q, &f, 2, 2, 6, -1)) vaca(near_chunk(tr, q), q, rng_range(&rg, 0, 6.28f));
    /* arboles de varios tipos: redondos, pinos y alamos en hileras */
    for (k = 0; k < 260; k++) {
        if (!villa_spot(tr, &rg, &q, &f, 3, 3, 4, -1)) continue;
        switch (k % 3) {
        case 0: tree_round(near_chunk(tr, q), q, rng_range(&rg, 0.9f, 1.6f)); break;
        case 1: tree_pine(near_chunk(tr, q), q, rng_range(&rg, 1.0f, 1.6f)); break;
        default: alamo(near_chunk(tr, q), q, rng_range(&rg, 0.9f, 1.3f)); break;
        }
    }
}

/* ---------- Bosque Denso ---------- */

static void tronco_caido(Mesh *m, V3 p, float yaw) {
    V3 f = v3(sinf(yaw), 0, -cosf(yaw)), r = v3(-f.z, 0, f.x);
    mesh_obox(m, p, r, f, 0.5f, 4.0f, 1.0f, col(110, 80, 54), col(130, 96, 64));
}

static void hongo(Mesh *m, V3 p) {
    mesh_prism(m, p, 0.15f, 0.5f, 5, col(240, 236, 220), col(240, 236, 220));
    mesh_cone(m, vadd(p, v3(0, 0.5f, 0)), 0.45f, 0.35f, 6, col(210, 50, 40));
}

static void scenery_bosque(Track *tr) {
    Rng rg = {2112};
    int k, tries;
    /* muchisimos arboles de varias especies, pegados a la pista */
    for (k = 0, tries = 0; k < 1400 && tries < 30000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        float dd = (k % 10 < 7) ? rng_range(&rg, 3, 40) : rng_range(&rg, 40, 160);
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + dd), 0);
        float clear;
        q.y = 0;
        clear = min_dist_xz(tr, q, &ni);
        if (clear < tr->wall + 3) continue;
        switch (rng_u(&rg) % 5) {
        case 0: case 1: tree_pine(chunk_mesh(tr, ni), q, rng_range(&rg, 1.3f, 2.6f)); break;
        case 2: tree_round(chunk_mesh(tr, ni), q, rng_range(&rg, 1.2f, 2.0f)); break;
        case 3: if (clear > tr->wall + 5) alamo(chunk_mesh(tr, ni), q, rng_range(&rg, 1.0f, 1.5f)); break;
        default: if (clear > tr->wall + 6) tree_round(chunk_mesh(tr, ni), q, rng_range(&rg, 2.0f, 2.8f)); break;
        }
        k++;
    }
    /* sotobosque: arbustos, troncos caidos, rocas y hongos */
    for (k = 0, tries = 0; k < 220 && tries < 6000; tries++) {
        int i = rng_u(&rg) % tr->n, ni;
        V3 q = at(tr, i, ((rng_u(&rg) & 1) ? 1 : -1) * (tr->wall + rng_range(&rg, 2, 60)), 0);
        q.y = 0;
        if (min_dist_xz(tr, q, &ni) < tr->wall + 2) continue;
        switch (k % 4) {
        case 0: mesh_cone(chunk_mesh(tr, ni), q, rng_range(&rg, 1.0f, 2.0f), rng_range(&rg, 0.8f, 1.6f), 6, col(50, 100, 50)); break;
        case 1: if (min_dist_xz(tr, q, NULL) > tr->wall + 6) tronco_caido(chunk_mesh(tr, ni), q, rng_range(&rg, 0, 6.28f)); break;
        case 2: mesh_cone(chunk_mesh(tr, ni), q, rng_range(&rg, 1.0f, 2.2f), rng_range(&rg, 0.8f, 1.6f), 5, col(120, 116, 110)); break;
        default: hongo(chunk_mesh(tr, ni), q); break;
        }
        k++;
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
    case TRACK_BURGOS: raw_from_controls(BURGOS_CP, sizeof(BURGOS_CP) / sizeof(BURGOS_CP[0])); tr->hw = 7.5f; break;
    case TRACK_FUJI: raw_from_controls(FUJI_CP, sizeof(FUJI_CP) / sizeof(FUJI_CP[0])); tr->hw = 8.0f; break;
    case TRACK_CASTELLOLI: raw_from_controls(CAST_CP, sizeof(CAST_CP) / sizeof(CAST_CP[0])); tr->hw = 7.5f; break;
    case TRACK_OVALO: raw_from_controls(OVALO_CP, sizeof(OVALO_CP) / sizeof(OVALO_CP[0])); tr->hw = 9.0f; break;
    case TRACK_COSTA: raw_from_controls(COSTA_CP, sizeof(COSTA_CP) / sizeof(COSTA_CP[0])); tr->hw = 7.0f; break;
    case TRACK_TUCAN: raw_from_controls(TUCAN_CP, sizeof(TUCAN_CP) / sizeof(TUCAN_CP[0])); tr->hw = 6.8f; break;
    case TRACK_SONORA: raw_from_controls(SONORA_CP, sizeof(SONORA_CP) / sizeof(SONORA_CP[0])); tr->hw = 7.5f; break;
    case TRACK_MAYA: raw_from_controls(MAYA_CP, sizeof(MAYA_CP) / sizeof(MAYA_CP[0])); tr->hw = 7.0f; break;
    case TRACK_VOLCANES: raw_from_controls(VOLCAN_CP, sizeof(VOLCAN_CP) / sizeof(VOLCAN_CP[0])); tr->hw = 7.0f; break;
    case TRACK_AGAVE: raw_from_controls(AGAVE_CP, sizeof(AGAVE_CP) / sizeof(AGAVE_CP[0])); tr->hw = 7.5f; break;
    case TRACK_BAJA: raw_from_controls(BAJA_CP, sizeof(BAJA_CP) / sizeof(BAJA_CP[0])); tr->hw = 7.0f; break;
    case TRACK_CUMBRES: raw_from_controls(CUMBRES_CP, sizeof(CUMBRES_CP) / sizeof(CUMBRES_CP[0])); tr->hw = 7.5f; break;
    case TRACK_FUNDIDORA: raw_from_controls(FUND_CP, sizeof(FUND_CP) / sizeof(FUND_CP[0])); tr->hw = 7.5f; break;
    case TRACK_VILLA: raw_from_controls(VILLA_CP, sizeof(VILLA_CP) / sizeof(VILLA_CP[0])); tr->hw = 7.5f; break;
    case TRACK_AZTLAN: raw_from_controls(AZTLAN_CP, sizeof(AZTLAN_CP) / sizeof(AZTLAN_CP[0])); tr->hw = 7.5f; break;
    case TRACK_SEMINARIO: raw_from_controls(SEMI_CP, sizeof(SEMI_CP) / sizeof(SEMI_CP[0])); tr->hw = 7.0f; break;
    case TRACK_ISLA: raw_from_controls(ISLA_CP, sizeof(ISLA_CP) / sizeof(ISLA_CP[0])); tr->hw = 7.5f; break;
    case TRACK_ESPINAZO: raw_from_controls(ESPINAZO_CP, sizeof(ESPINAZO_CP) / sizeof(ESPINAZO_CP[0])); tr->hw = 7.0f; break;
    case TRACK_GRANJA: raw_from_controls(GRANJA_CP, sizeof(GRANJA_CP) / sizeof(GRANJA_CP[0])); tr->hw = 7.5f; break;
    case TRACK_BOSQUE: raw_from_controls(BOSQUE_CP, sizeof(BOSQUE_CP) / sizeof(BOSQUE_CP[0])); tr->hw = 7.0f; break;
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
    g_cur = tr;
    tr->fog_start = 420.0f; tr->fog_end = 1900.0f; tr->zfar = 2600.0f;
    tr->curb_a = col(220, 30, 30); tr->curb_b = col(245, 245, 245);
    tr->terrain = 0; tr->street_y = 0;

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
    case TRACK_FUJI:
        grass = col(96, 156, 76);
        tr->sky_top = col(46, 104, 210); tr->sky_horizon = col(196, 220, 244); tr->fog = col(206, 224, 242);
        build_road(tr, col(40, 140, 84), col(236, 236, 240), grass);
        scenery_fuji(tr);
        build_world(tr, col(96, 156, 76), col(88, 146, 70), col(80, 112, 96));
        add_fuji(tr);
        tr->fog_start = 900.0f; tr->fog_end = 4200.0f; tr->zfar = 4800.0f;
        break;
    case TRACK_CASTELLOLI:
        grass = col(150, 146, 84);
        tr->sky_top = col(56, 118, 214); tr->sky_horizon = col(214, 226, 238); tr->fog = col(214, 222, 230);
        build_road(tr, col(50, 90, 176), col(238, 200, 44), grass);
        scenery_castelloli(tr);
        build_world(tr, col(176, 160, 100), col(140, 146, 80), col(150, 140, 120));
        add_montserrat(tr);
        tr->fog_start = 800.0f; tr->fog_end = 3600.0f; tr->zfar = 4200.0f;
        break;
    case TRACK_OVALO:
        grass = col(84, 150, 70);
        tr->sky_top = col(36, 46, 120); tr->sky_horizon = col(250, 150, 92); tr->fog = col(236, 160, 120);
        build_road(tr, col(240, 240, 244), col(216, 216, 222), grass);
        scenery_ovalo(tr);
        build_world(tr, col(84, 150, 70), col(78, 140, 66), col(110, 90, 120));
        break;
    case TRACK_COSTA:
        grass = col(110, 156, 76);
        tr->sky_top = col(40, 118, 222); tr->sky_horizon = col(200, 230, 250); tr->fog = col(206, 230, 248);
        build_road(tr, col(178, 162, 140), col(236, 236, 232), grass);
        scenery_costa(tr);
        build_world(tr, col(110, 156, 76), col(102, 148, 70), col(90, 120, 110));
        add_montes_costa(tr);
        tr->fog_start = 700.0f; tr->fog_end = 3000.0f; tr->zfar = 3600.0f;
        break;
    case TRACK_SONORA:
        grass = col(214, 180, 120);
        tr->sky_top = col(60, 120, 214); tr->sky_horizon = col(244, 216, 172); tr->fog = col(240, 214, 176);
        build_road(tr, col(190, 120, 80), col(232, 212, 170), grass);
        scenery_sonora(tr);
        build_world(tr, col(216, 182, 122), col(204, 168, 110), col(176, 96, 64));
        break;
    case TRACK_MAYA:
        grass = col(70, 128, 58);
        tr->sky_top = col(80, 150, 214); tr->sky_horizon = col(206, 230, 222); tr->fog = col(196, 222, 210);
        build_road(tr, col(186, 176, 150), col(150, 142, 118), grass);
        scenery_maya(tr);
        build_world(tr, col(66, 122, 56), col(60, 112, 52), col(60, 100, 70));
        tr->fog_start = 300.0f; tr->fog_end = 1400.0f;
        break;
    case TRACK_VOLCANES:
        grass = col(98, 140, 70);
        tr->sky_top = col(40, 100, 210); tr->sky_horizon = col(200, 224, 246); tr->fog = col(210, 226, 244);
        build_road(tr, col(230, 230, 236), col(60, 120, 70), grass);
        scenery_volcanes(tr);
        build_world(tr, col(110, 146, 74), col(150, 132, 90), col(80, 100, 90));
        add_volcanes(tr);
        tr->fog_start = 900.0f; tr->fog_end = 4400.0f; tr->zfar = 5000.0f;
        break;
    case TRACK_AGAVE:
        grass = col(170, 104, 72);
        tr->sky_top = col(56, 116, 210); tr->sky_horizon = col(236, 220, 190); tr->fog = col(232, 216, 190);
        build_road(tr, col(240, 236, 226), col(180, 86, 56), grass);
        scenery_agave(tr);
        build_world(tr, col(176, 108, 74), col(150, 120, 80), col(110, 110, 120));
        add_volcan_tequila(tr);
        tr->fog_start = 700.0f; tr->fog_end = 3200.0f; tr->zfar = 3800.0f;
        break;
    case TRACK_BAJA:
        grass = col(196, 170, 120);
        tr->sky_top = col(30, 110, 220); tr->sky_horizon = col(200, 228, 248); tr->fog = col(206, 228, 246);
        build_road(tr, col(200, 190, 170), col(236, 236, 232), grass);
        scenery_baja(tr);
        build_world(tr, col(200, 172, 122), col(186, 160, 112), col(140, 112, 96));
        tr->fog_start = 700.0f; tr->fog_end = 3000.0f; tr->zfar = 3600.0f;
        break;
    case TRACK_GRANJA:
        grass = col(118, 162, 74);
        tr->sky_top = col(60, 130, 220); tr->sky_horizon = col(220, 230, 236); tr->fog = col(222, 230, 234);
        build_road(tr, col(236, 236, 240), col(60, 130, 70), grass);
        scenery_granja(tr);
        build_world(tr, col(118, 162, 74), col(130, 168, 80), col(100, 120, 110));
        tr->fog_start = 700.0f; tr->fog_end = 3200.0f; tr->zfar = 3800.0f;
        break;
    case TRACK_BOSQUE:
        grass = col(60, 104, 52);
        tr->sky_top = col(70, 130, 200); tr->sky_horizon = col(196, 216, 210); tr->fog = col(150, 176, 160);
        build_road(tr, col(110, 90, 66), col(92, 76, 56), grass);
        scenery_bosque(tr);
        build_world(tr, col(56, 98, 50), col(52, 92, 48), col(46, 80, 56));
        tr->fog_start = 180.0f; tr->fog_end = 900.0f; tr->zfar = 1300.0f;
        break;
    case TRACK_ESPINAZO:
        grass = col(110, 142, 76);
        tr->terrain = 3; tr->street_y = 0.0f;
        tr->curb_a = col(240, 60, 150); tr->curb_b = col(250, 250, 250);
        tr->sky_top = col(40, 100, 210); tr->sky_horizon = col(220, 214, 236); tr->fog = col(214, 220, 236);
        build_road(tr, col(240, 240, 244), col(250, 190, 40), grass);
        mesh_init(&tr->world);
        build_world_terrain(tr);
        scenery_espinazo(tr);
        add_sierra_y_nubes(tr);
        tr->fog_start = 900.0f; tr->fog_end = 3800.0f; tr->zfar = 4400.0f;
        break;
    case TRACK_ISLA:
        grass = col(110, 170, 80);
        tr->sky_top = col(20, 120, 230); tr->sky_horizon = col(190, 232, 250); tr->fog = col(196, 232, 248);
        build_road(tr, col(250, 250, 250), col(40, 170, 200), grass);
        scenery_isla(tr);
        build_world(tr, col(110, 170, 80), col(118, 176, 84), col(80, 130, 90));
        add_mar_isla(tr);
        tr->fog_start = 800.0f; tr->fog_end = 3600.0f; tr->zfar = 4200.0f;
        break;
    case TRACK_SEMINARIO:
        grass = col(110, 150, 80);
        tr->curb_a = col(236, 190, 40); tr->curb_b = col(60, 60, 64);
        tr->sky_top = col(56, 120, 210); tr->sky_horizon = col(214, 222, 230); tr->fog = col(214, 222, 230);
        build_road(tr, col(214, 210, 200), col(186, 180, 168), grass);
        scenery_seminario(tr);
        build_world(tr, col(140, 150, 96), col(150, 146, 120), col(110, 112, 116));
        add_cerros_mty(tr);
        tr->fog_start = 800.0f; tr->fog_end = 3600.0f; tr->zfar = 4200.0f;
        break;
    case TRACK_AZTLAN:
        grass = col(104, 160, 78);
        tr->terrain = 2; tr->street_y = 0.0f;
        tr->curb_a = col(30, 150, 220); tr->curb_b = col(250, 250, 250);
        tr->sky_top = col(30, 120, 226); tr->sky_horizon = col(196, 228, 248); tr->fog = col(206, 230, 246);
        build_road(tr, col(250, 200, 40), col(40, 140, 220), grass);
        mesh_init(&tr->world);
        build_world_terrain(tr);
        scenery_aztlan(tr);
        add_cerros_mty(tr);
        tr->fog_start = 800.0f; tr->fog_end = 3600.0f; tr->zfar = 4200.0f;
        break;
    case TRACK_VILLA:
        grass = col(98, 156, 76);
        tr->sky_top = col(46, 112, 214); tr->sky_horizon = col(206, 224, 240); tr->fog = col(210, 224, 238);
        build_road(tr, col(40, 90, 200), col(240, 240, 244), grass);
        scenery_villa(tr);
        build_world(tr, col(98, 156, 76), col(92, 148, 72), col(110, 112, 116));
        add_cerros_mty(tr);
        tr->fog_start = 800.0f; tr->fog_end = 3600.0f; tr->zfar = 4200.0f;
        break;
    case TRACK_FUNDIDORA:
        grass = col(96, 146, 70);
        tr->sky_top = col(56, 120, 210); tr->sky_horizon = col(214, 222, 228); tr->fog = col(214, 220, 226);
        build_road(tr, col(236, 236, 240), col(220, 60, 40), grass);
        scenery_fundidora(tr);
        build_world(tr, col(96, 146, 70), col(104, 150, 76), col(110, 112, 116));
        add_silla(tr);
        tr->fog_start = 800.0f; tr->fog_end = 3600.0f; tr->zfar = 4200.0f;
        break;
    case TRACK_CUMBRES:
        grass = col(92, 146, 74);
        tr->sky_top = col(36, 92, 200); tr->sky_horizon = col(196, 218, 240); tr->fog = col(206, 222, 240);
        build_road(tr, col(230, 232, 238), col(200, 40, 40), grass);
        scenery_cumbres(tr);
        build_world(tr, col(92, 146, 74), col(86, 138, 70), col(90, 100, 120));
        add_cordillera_sur(tr);
        tr->fog_start = 900.0f; tr->fog_end = 4400.0f; tr->zfar = 5000.0f;
        break;
    case TRACK_TUCAN:
        grass = col(100, 150, 70);
        tr->terrain = 1; tr->street_y = 15.0f;
        tr->curb_a = col(236, 190, 40); tr->curb_b = col(150, 150, 150);
        tr->sky_top = col(70, 130, 210); tr->sky_horizon = col(214, 222, 228); tr->fog = col(214, 220, 226);
        build_road(tr, col(64, 168, 72), col(56, 150, 64), grass);
        mesh_init(&tr->world);
        build_world_terrain(tr);
        scenery_tucan(tr);
        add_cerros_mty(tr);
        tr->fog_start = 700.0f; tr->fog_end = 3200.0f; tr->zfar = 3800.0f;
        break;
    case TRACK_BURGOS:
        grass = col(170, 168, 96);
        tr->sky_top = col(52, 112, 208); tr->sky_horizon = col(206, 222, 240); tr->fog = col(214, 222, 232);
        build_road(tr, col(214, 200, 166), col(186, 170, 136), grass);
        scenery_burgos(tr);
        build_world(tr, col(206, 182, 102), col(150, 164, 84), col(150, 132, 118));
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
