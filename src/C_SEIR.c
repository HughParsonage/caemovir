#include "caemovir.h"

#define VACC_UNVACC 0
#define VACC_PFIZER 1
#define VACC_ASTRAZ 2
#define VACC_MODERN 3

#define INFECTIOUS_PERIOD 8

static inline double ifbw01(double x, double y) {
  return (x >= 0 && x <= 1) ? x : y;
}


typedef struct {
  unsigned int vaccine : 2;
  bool infected : 1;
  bool exposed : 1;
  bool maybe_hospital : 1;
  bool maybe_icu : 1;
  bool maybe_die : 1;
  bool ever_infected : 1;
  bool ever_hospital : 1;
  bool ever_icu : 1;
  bool ever_die : 1;
  unsigned int transmit_potential : 5;
  unsigned int transmit_day : 3;
  unsigned int date : 8;
} Person;

bool is_infected(Person P) {
  return P.infected;
}

bool is_infectious(Person P, int d) {
  return P.infected && (P.transmit_day + P.date) == d;
}

void check_recovered(Person * P, int d) {
  if (P->infected) {
    if (d == P->date + INFECTIOUS_PERIOD) {
      P->ever_infected = 1;
      P->ever_hospital = P->maybe_hospital;
      P->ever_icu = P->maybe_icu;
      P->ever_die = P->maybe_die;
      P->infected = 0;
    }
  }
}


static void scramble(Person * PP, int N, int state) {
  int * rr = malloc(sizeof(int) * N);
  if (rr == NULL) {
    return;
  }
  // void populateRandom_pcg32(int * answer, int size, int state, int inc)
  populateRandom_pcg32(rr, N, state, 4444);
  unsigned int M = N;
  for (int j = N - 1; j > 0; --j) {
    unsigned int rrj = rr[j];
    unsigned int i = rrj % M;
    Person P = PP[j];
    PP[j] = PP[i];
    PP[i] = P;
  }
  free(rr);
}

static void populate(Person * PP,
                     int N,
                     const int * rr,
                     int n0_infected,
                     int n0_vaccinated,
                     unsigned int q_symp,
                     unsigned int q_hosp,
                     unsigned int q_icu,
                     unsigned int q_kill) {
  unsigned int q_vaccinated = (((double)n0_vaccinated) / ((double)N) * UINT_MAX);
  unsigned int r = rand_pcg();
  for (int i = 0; i < N; ++i) {
    Person P = PP[i];
    P.vaccine = r <= q_vaccinated ? 1 : 0;
    P.infected = i <= n0_infected;
    P.exposed = 0;
    r = rand_pcg();
    P.maybe_hospital = r <= q_hosp;
    P.maybe_icu = r <= q_icu;
    P.maybe_die = r <= q_kill;

    P.ever_infected = 0;
    P.ever_hospital = 0;
    P.ever_icu = 0;
    P.ever_die = 0;

    r >>= 2; //
    P.transmit_potential = rr[i] & 31;
    r >>= 5;
    P.transmit_day = r & 7;
    P.date = i <= n0_infected ? 0 : 255;
    PP[i] = P;
  }
  scramble(PP, N, r);
}

int count_infected(Person * P, int N) {
  int o = 0;
  for (int i = 0; i < N; ++i) {
    o += is_infected(P[i]);
  }
  return o;
}

int sum_transmissions(Person * PP, int N, int d) {
  int o = 0;
  for (int i = 0; i < N; ++i) {
    Person P = PP[i];
    if (is_infectious(P, d)) {
      o += P.transmit_potential;
    }
  }
  return o;
}

void infect_now(Person * P, int d) {
  P->infected = 1;
  P->date= d;
}

bool isnt_resistant(Person P, unsigned int r) {
  if (is_infected(P)) {
    return 0;
  }
  unsigned int v = P.vaccine;
  switch(v) {
  case VACC_UNVACC:
    return 1;
  default:
    return r <= 858993459u; // 20%
  }
}

bool q_ru(unsigned int p, unsigned int q, unsigned int r) {
  // p/q > r/2^32
  uint64_t rq = r;
  rq *= q;
  rq >>= 32;
  return p > rq;
}




void do_seir(Person * PP, int N, int d, int * rand, unsigned char * crand, unsigned int x_infections) {
  int r0 = rand[1];
  populateRandom_pcg32(rand, N, r0, 4444);
  uint64_t new_infections = sum_transmissions(PP, N, d) + x_infections;
  if ((new_infections - 1u) >= N) {
    // unlikely
    return;
  }
  unsigned int Q = (new_infections << 32) / ((uint64_t)N);
  for (int i = 0; i < N; ++i) {
    Person P = PP[i];
    check_recovered(&P, d);
    if (rand[i] < Q) {
      P.exposed = 1;
      // if (isnt_resistant(P, rand[i])) {
      if (isnt_resistant(P, rand_pcg())) {
        infect_now(&P, d);
      }
    }
  }
}




SEXP C_SEIR(SEXP NN,
            SEXP NDays,
            SEXP nnInfected,
            SEXP nnVaccinations,
            SEXP nnExternalInfections,
            SEXP Epi,
            SEXP RR) {
  if (!isInteger(NN) || !isInteger(NDays) || !isReal(Epi)) {
    error("Wrong types.");
  }
  const int N = asInteger(NN);
  const int ndays = asInteger(NDays);

  const double * epi = REAL(Epi);
  const int R_len = length(RR);
  if (R_len != N) {
    error("R_len != N");
  }
  const int * r = INTEGER(RR);
  if (length(nnVaccinations) != ndays ||
      length(nnExternalInfections) != ndays) {
    error("Wrong lengths.");
  }
  const int * nVaccinations = INTEGER(nnVaccinations);
  const int * nExternalInfections = INTEGER(nnExternalInfections);
  const int n0_infected = asInteger(nnInfected);
  const int n0_vaccinated = nVaccinations[0];


  if (length(Epi) != 4) {
    error("length(Epi) != 4");
  }
  const double p_symp = ifbw01(epi[0], 0.20);
  const double p_hosp = ifbw01(epi[1], 0.03);
  const double p_icu  = ifbw01(epi[2], 0.01);
  const double p_kill = ifbw01(epi[3], 0.01);

  const unsigned int q_kill = UINT_MAX * p_kill;
  const unsigned int q_icu  = UINT_MAX * p_icu;
  const unsigned int q_hosp = UINT_MAX * p_hosp;
  const unsigned int q_symp = UINT_MAX * p_symp;

  Person * pp = malloc(N * sizeof(Person));
  if (pp == NULL) {
    error("Person * pp could not be malloc."); // # nocov
  }
  populate(pp, N, r, n0_infected, n0_vaccinated, q_symp, q_hosp, q_icu, q_kill);

  int * rand = malloc(sizeof(int) * N);
  if (rand == NULL) {
    free(pp);
    error("rand not allocatable."); // # nocov
  }

  unsigned char * crand = malloc(sizeof(char) * N);
  if (crand == NULL) {
    free(pp);
    free(rand);
    error("crand not allocatable."); // # nocov
  }


  for (int d = 0; d < ndays; ++d) {
    do_seir(pp, N, d, rand, crand, nExternalInfections[d]);
  }
  SEXP ans = PROTECT(allocVector(LISTSXP, 4));
  free(crand);
  free(rand);
  free(pp);
  UNPROTECT(1);
  return R_NilValue;

}



