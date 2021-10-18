#include "caemovir.h"

unsigned int rng_state;
unsigned int rng_states[8];

SEXP CResetRNG(SEXP x) {
  if (TYPEOF(x) == INTSXP && xlength(x) == 1) {
    rng_state = INTEGER_ELT(x, 0);
  }
  return ScalarInteger(rng_state);
}

unsigned int rand_pcg() {
  unsigned int state = rng_state;
  rng_state = rng_state * 747796405u + 2891336453u;
  unsigned int word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}

// given a uniform random variable p, and an unsigned int
// a, return TRUE on realization (concretetly when p < U(0, 1))
bool unif_p2b(double p, unsigned int a) {
  unsigned int p_ = p * UINT_MAX;
  return a <= p_;
}

unsigned int r2r(double r) {
  return r * UINT_MAX;
}
