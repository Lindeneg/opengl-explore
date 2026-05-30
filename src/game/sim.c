#include "sim.h"

#include "../core/log.h"
#include "defs.h"

#define SIM_TICK_SECONDS 2.0
#define SIM_LOG_EVERY 5
#define SIM_PRICE_RATE 0.2f  // fraction of the gap to equilibrium closed per tick
#define SIM_NOISE_SCALE 0.05f // price jitter as a fraction of the value band, times volatility

static u64 xs_next(u64 *st) {
    u64 x = *st;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *st = x;
    return x;
}

// uniform f32 in [0,1)
static f32 rand_unit(sim_t *s) {
    return (f32)((f64)(xs_next(&s->rng) >> 40) / (f64)(1ull << 24));
}

static f32 clampf(f32 v, f32 lo, f32 hi) { return MAX(lo, MIN(hi, v)); }

static void sim_tick(sim_t *s, world_t *w) {
    const defs_t *defs = w->defs;
    u32 nres = defs->resource_count;

    for (u32 i = 0; i < w->site_count; ++i) {
        site_t *st = &w->sites[i];
        st->stock = MIN(st->stock + st->replenish, st->capacity);
    }

    for (u32 r = 0; r < nres; ++r) {
        const resource_def_t *rd = &defs->resources[r];
        f32 supply = 0.0f, demand = 0.0f;
        for (u32 c = 0; c < w->city_count; ++c) {
            supply += w->city_supply[c * nres + r];
            demand += w->city_demand[c * nres + r];
        }
        for (u32 i = 0; i < w->site_count; ++i)
            if (w->sites[i].resource == r && w->sites[i].stock > 0.0f)
                supply += w->sites[i].replenish; // raw supplied at its extraction rate

        f32 vmin = rd->value_min, vmax = rd->value_max;
        f32 eq;
        if (supply <= 0.0f && demand <= 0.0f) {
            eq = (vmin + vmax) * 0.5f; // no market signal yet (e.g. manufactured pre-industry)
        } else {
            f32 ratio = demand / (supply + 0.0001f); // >1 scarce, <1 glut
            f32 t = ratio / (ratio + 1.0f);          // 0.5 at balance
            eq = vmin + (vmax - vmin) * t;
        }
        f32 noise = rd->volatility * (rand_unit(s) * 2.0f - 1.0f) * (vmax - vmin) * SIM_NOISE_SCALE;
        // TODO: world events (resource discoveries, scandals) nudge price here
        f32 np = w->prices[r] + (eq - w->prices[r]) * SIM_PRICE_RATE + noise;
        w->prices[r] = clampf(np, vmin, vmax);
    }

    s->tick++;
    if (s->tick % SIM_LOG_EVERY == 0) {
        char buf[512];
        i32 off = 0;
        for (u32 r = 0; r < nres && off < (i32)sizeof buf; ++r)
            off += snprintf(buf + off, sizeof buf - (u64)off, "%s%s=%.1f", r ? " " : "",
                            defs->resources[r].id, (double)w->prices[r]);
        LOG_INFO("tick %llu market: %s", (unsigned long long)s->tick, buf);
    }
}

void sim_init(sim_t *s) {
    s->tick = 0;
    s->accum = 0.0;
    s->rng = 0x9e3779b97f4a7c15ull;
}

void sim_update(sim_t *s, world_t *w, f32 dt) {
    if (dt > (f32)SIM_TICK_SECONDS)
        dt = (f32)SIM_TICK_SECONDS; // a stall shouldn't dump a burst of ticks
    s->accum += (f64)dt;
    while (s->accum >= SIM_TICK_SECONDS) {
        sim_tick(s, w);
        s->accum -= SIM_TICK_SECONDS;
    }
}
