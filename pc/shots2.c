/* capturas especificas: puente, tunel, choque */
#include <stdio.h>
#include "../source/plat.h"
#include "../source/game.h"
#include "../source/race.h"
void pc_set_pads(const PadIn p[2]); void pc_save_ppm(const char *path); Race *pc_race(void);
static void run_until(float s_target, int mode, const char *name, int extra) {
    PadIn p[2]; int f;
    for (f = 0; f < 60*200; f++) {
        Race *r = pc_race(); Entity *e = &r->ent[RACERS-1];
        memset(p,0,sizeof p);
        if (r->state != RS_COUNTDOWN) { CarInput in = race_autopilot(r,e,1/60.f); p[0].sx=in.steer; p[0].trig_r=in.throttle; p[0].trig_l=in.brake; }
        game_frame(p,1/60.f);
        if (r->state != RS_COUNTDOWN && e->lap >= 1 && fabsf(e->s - s_target) < 6) break;
    }
    for (f = 0; f < extra; f++) game_frame(p,1/60.f);
    { char b[64]; snprintf(b,sizeof b,"shots/%s.ppm",name); pc_save_ppm(b); }
}
int main(void) {
    Race *r;
    game_init();
    game_debug_start_race(0, 0, 1, 1, 0); r = pc_race();
    { int i; float best=0, bs=0; for(i=0;i<r->track->n;i++) if(r->track->p[i].y>best){best=r->track->p[i].y; bs=i*r->track->ds;}
      run_until(bs - 140, 0, "30_puente_antes", 0); }
    game_debug_start_race(0, 1, 1, 0, 1); r = pc_race();
    { int i; for(i=0;i<r->track->n;i++) if(r->track->p[i].y<0.5f && i*r->track->ds > 600) break;
      run_until(i*r->track->ds + 40, 1, "31_bajo_puente", 0); }
    game_debug_start_race(2, 0, 1, 3, 0); r = pc_race();
    { int i; for(i=0;i<r->track->n;i++) if(r->track->tunnel[i]) break;
      run_until(i*r->track->ds - 70, 0, "32_tunel_entrada", 0);
      run_until(i*r->track->ds + 30, 0, "33_tunel_dentro", 0); }
    game_debug_start_race(1, 1, 1, 2, 0); r = pc_race();
    run_until(400, 1, "34_fortaleza", 0);
    return 0;
}
