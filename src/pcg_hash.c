#include "caemovir.h"
#include "pcg32.h"

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

void populateRandom_pcg32(int * answer, int size, int state, int inc) {
  pcg32_random_t key = {
    .state = state,
    .inc = inc};
  // pcg32_random_t key = {
  //   .state = 324,
  //   .inc = 4444}; // I am a crazy man using bleeding-edge C99 in 2018
  for (int i = 0; i < size; i++) {
    answer[i] = pcg32_random_r(&key);
  }
}

SEXP C_trand_pcg(SEXP NN, SEXP RR) {
  int N = asInteger(NN);
  const int * rr = INTEGER(RR);
  unsigned int r1 = rr[0];
  unsigned int r2 = rr[2];
  r1 += rr[1];
  r2 += rr[3];
  SEXP ans = PROTECT(allocVector(INTSXP, N));
  int * ansp = INTEGER(ans);
  populateRandom_pcg32(ansp, N, r1, r2);
  UNPROTECT(1);
  return ans;
}

