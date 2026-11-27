/* cars.h - autos originales generados por codigo */
#ifndef CARS_H
#define CARS_H

#include "core.h"

#define CAR_COUNT 25
#define CAR_POLICE 24
#define CAR_SUPERPOLICE CAR_COUNT   /* super patrulla: solo persigue a los autos clase A, no es seleccionable */  /* la patrulla (siempre la ultima): jugable y usada por la policia */

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
/* ficha del garage: motor, dos lineas de historia y quien lo dibujo */
typedef struct { const char *motor, *line1, *line2, *credit; } CarInfo;
extern const CarInfo CAR_INFO[CAR_COUNT];
/* clase de cada auto: 'C' los mas lentos, 'B' intermedios, 'A' los mas rapidos */
extern const char CAR_CLASS[CAR_COUNT];
extern const CarSpec SUPER_PATRULLA;
const CarSpec *car_spec(int i);   /* incluye la super patrulla (CAR_SUPERPOLICE) */
/* rango de vertices de cada rueda de la ultima malla generada (orden: del-izq, del-der, tras-izq, tras-der) */
extern int g_wheel_first[4], g_wheel_count[4];
/* una rueda suelta centrada en el origen (para las que salen volando) */
void wheel_build_mesh(Mesh *m);
/* camioneta de policia (solo la maneja la policia) */
void suv_build_mesh(Mesh *m);

#endif
