#ifndef TRADING_STUFF_SIM_H_
#define TRADING_STUFF_SIM_H_

#include "../common.h"
#include "world.h"

// fixed-step economic clock: advances discrete ticks at a wall-clock interval,
// replenishing sites and moving resource prices toward a supply/demand equilibrium
typedef struct {
    u64 tick;  // ticks elapsed
    f64 accum; // unspent wall-clock seconds
    u64 rng;   // xorshift state (deterministic; no save/seed yet)
} sim_t;

void sim_init(sim_t *s);
void sim_update(sim_t *s, world_t *w, f32 dt);

#endif // TRADING_STUFF_SIM_H_
