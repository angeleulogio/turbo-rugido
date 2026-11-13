/* cars.h - autos originales generados por codigo */
#ifndef CARS_H
#define CARS_H

#include "core.h"

#define CAR_COUNT 8

typedef struct {
    const char *name;
    const char *desc;
    /* estadisticas 0..1 para mostrar */
    float stat_speed, stat_accel, stat_handling;
    /* fisica arcade */
    float vmax;        /* m/s */
    float accel;       /* m/s^2 a baja velocidad */
    float turn;        /* multiplicador de giro */
    float drift;       /* facilidad para derrapar */
    /* semi-simulacion */
    float mass;        /* kg */
    float power;       /* W */
    float grip;        /* coef. de friccion */
    int drive;         /* 0 = traccion trasera, 1 = delantera, 2 = 4x4 */
    float weight_front;/* reparto de peso delante 0..1 */
    /* diseno */
    int shape;         /* 0 cuna, 1 coupe GT, 2 rally compacto, 3 gran turismo largo, 4 sedan 4 puertas, 5 cuna de los 80, 6 sedan cuadrado de los 80, 7 sedan V10 */
    Col paint, accent;
} CarSpec;

extern const CarSpec CAR_SPECS[CAR_COUNT];
extern const Col CAR_ALT_PAINTS[6];

/* genera la malla (coordenadas locales: x derecha, y arriba, -z adelante) */
void car_build_mesh(Mesh *m, int shape, Col paint, Col accent);
/* auto de trafico (sedan o furgoneta) */
void traffic_build_mesh(Mesh *m, int kind, Col paint);

#endif
