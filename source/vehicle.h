/* vehicle.h - los 3 modelos de fisica */
#ifndef VEHICLE_H
#define VEHICLE_H

#include "core.h"
#include "cars.h"
#include "track.h"

typedef enum {
    MODE_DERRAPE = 0,   /* arcade de derrapes largos (estilo Ridge Racer) */
    MODE_CHOQUES,       /* arcade agresivo con turbo y choques (estilo Burnout) */
    MODE_SIM,           /* semi-simulacion (estilo Gran Turismo) */
    MODE_COUNT
} PhysMode;

typedef struct {
    float steer;     /* -1 izquierda .. 1 derecha */
    float throttle;  /* 0..1 */
    float brake;     /* 0..1 */
    int boost;       /* boton de turbo */
} CarInput;

typedef struct {
    const CarSpec *spec;
    V3 pos;
    float yaw;           /* orientacion del auto */
    /* arcade */
    float speed;         /* m/s, con signo */
    float veldir;        /* direccion de la velocidad */
    int drifting;
    float gas_off_t;     /* tiempo desde que se solto el acelerador */
    float prev_throttle, prev_brake;
    /* simulacion */
    float vf, vr, yawrate, steer_cur, ax;
    /* comun */
    int idx;             /* segmento de pista */
    float s, d;
    V3 tan, right;
    float pitch;
    float rpm01;
    int gear;
    float boost;         /* 0..1 */
    int boosting;
    float wall_impact;   /* > 0 si choco contra el muro este paso */
    float slip;          /* angulo de deslizamiento visible */
} Vehicle;

const char *mode_name(int mode);
const char *mode_desc(int mode);

void veh_init(Vehicle *v, const CarSpec *spec, const Track *tr, float s, float d);
void veh_step(Vehicle *v, PhysMode mode, const CarInput *in, const Track *tr, float dt);
V3 veh_velocity(const Vehicle *v, PhysMode mode);
void veh_set_velocity(Vehicle *v, PhysMode mode, V3 vel);
void veh_place(Vehicle *v, const Track *tr, float s, float d, float speed);
float veh_top_speed(const CarSpec *spec, PhysMode mode);
V3 veh_forward(const Vehicle *v);

#endif
