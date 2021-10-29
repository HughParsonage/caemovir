#include "caemovir.h"
#include "pcg32.h"

unsigned int rng_state;
unsigned int rng_states[8];

inline uint64_t c32(uint32_t x, uint32_t y) {
  // combine 32 to 64 bit
  return ((uint64_t)x) << 32 | y;
}

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

unsigned int next_rand(unsigned int r) {
  r = r * 747796405u + 2891336453u;
  unsigned int word = ((r >> ((r >> 28u) + 4u)) ^ r) * 277803737u;
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

void populateRandom_pcg32(unsigned int * answer, int size, uint64_t state, uint64_t inc) {
  inc += (inc & 1u) + 1u;
  pcg32_random_t key = {
    .state = state,
    .inc = inc};
  // pcg32_random_t key = {
  //   .state = 324,
  //   .inc = 4444}; // I am a crazy man using bleeding-edge C99 in 2018
  int warmup = state & 63;
  for (int i = 0; i <= warmup; ++i) {
    pcg32_random_r(&key);
  }

  for (int i = 0; i < size; i++) {
    answer[i] = pcg32_random_r(&key);
  }
}

SEXP C_trand_pcg(SEXP NN, SEXP RR) {
  int N = asInteger(NN);
  const int * rr = INTEGER(RR);
  if (length(RR) < 4) {
    error("length(RR) < 4");
  }
  uint64_t r1 = c32(rr[0], rr[1]);
  uint64_t r2 = c32(rr[2], rr[3]);
  SEXP ans = PROTECT(allocVector(INTSXP, N));
  unsigned int * ansp = (unsigned int *)INTEGER(ans);
  populateRandom_pcg32(ansp, N, r1, r2);
  UNPROTECT(1);
  return ans;
}

