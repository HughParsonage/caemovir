#include "caemovir.h"

#define RESISTANT_MAGIC_NUMBER 5

#define MAX_N_DAYS 250
#define MAX_HSIZE 8
#define MAX_HSIZD 7
#define MAX_WSIZE 51

#define RANDOM_LEN 16777216
#define RANDOM_LEM 16777215

typedef struct {
  unsigned int infected : 1;
  unsigned int maybe_hospital : 1;
  unsigned int maybe_icu : 1;
  unsigned int maybe_die : 1;
  unsigned int ever_infected : 1;
  unsigned int ever_hospital : 1;
  unsigned int ever_icu : 1;
  unsigned int ever_die : 1;
  unsigned int transmit_potential : 5;
  unsigned int transmit_day : 3;
  unsigned int date : 8;
  unsigned int vaccine : 2;
  unsigned int resistance : 6;
} Health;

Health initHealth(unsigned int x) {
  Health H;
  H.infected = 0;
  H.maybe_hospital = 0;
  H.maybe_icu = 0;
  H.maybe_die = 0;
  H.ever_infected = 0;
  H.ever_hospital = 0;
  H.ever_icu = 0;
  H.ever_die = 0;
  H.transmit_potential = 0;
  H.transmit_day= 0;
  H.date = 254;
  H.vaccine = 0;
  H.resistance = 0;
  return H;
}

unsigned int H2u32(Health H) {
  unsigned int o = 0;
  o += H.infected;
  o <<= 1;
  o += H.maybe_hospital;
  o <<= 1;
  o += H.maybe_icu;
  o <<= 1;
  o += H.maybe_die;
  o <<= 1;
  o += H.ever_infected;
  o <<= 1;
  o += H.ever_hospital;
  o <<= 1;
  o += H.ever_icu;
  o <<= 1;
  o += H.ever_die;
  o <<= 5;
  o += H.transmit_potential;
  o <<= 3;
  o += H.transmit_day;
  o <<= 8;
  o += H.date;
  o <<= 2;
  o += H.vaccine;
  o <<= 6;
  o += H.resistance;
  return o;
}

Health u2H(unsigned int x) {
  Health H;
  H.resistance = x & 63;
  x >>= 6;
  H.vaccine = x & 3;
  x >>= 2;
  H.date = x & 255;
  x >>= 8;
  H.transmit_day = x & 7;
  x >>= 3;
  H.transmit_potential = x & 31;
  x >>= 5;
  H.ever_die = x & 1;
  x >>= 1;
  H.ever_icu = x & 1;
  x >>= 1;
  H.ever_hospital = x & 1;
  x >>= 1;
  H.ever_infected = x & 1;
  x >>= 1;
  H.maybe_die = x & 1;
  x >>= 1;
  H.maybe_icu = x & 1;
  x >>= 1;
  H.maybe_hospital = x & 1;
  x >>= 1;
  H.infected = x & 1;
  return H;
}

SEXP C_test_roundtrip_H2U2H(SEXP xx) {
  unsigned int x = asInteger(xx);
  return ScalarInteger(H2u32(u2H(x)));
}

typedef struct {
  Health H;
  unsigned int h;
  unsigned int w;
  unsigned int age;
} Person;

typedef struct {
  unsigned int n_infected;
  unsigned int w_size;
  unsigned int pp[MAX_WSIZE]; // number of workers
} Workplace;

typedef struct {
  unsigned int n_infected;
  unsigned int h_size;
  unsigned int pp[MAX_HSIZE];
} House;

typedef struct {
  unsigned int q_work;
  unsigned int q_home;
  unsigned int q_symp;
  unsigned int q_hosp;
  unsigned int q_icu;
  unsigned int q_kill;
} Epi;

static bool is_infected(Person P) {
  return P.H.infected;
}

static bool is_infectious(Person P, int d) {
  return P.H.infected && (P.H.date - d) == (P.H.transmit_day);
}

static unsigned int transmissions(Person P, int d) {
  return is_infectious(P, d) ? P.H.transmit_potential : 0;
}

// Not just susceptible but also not sufficiently resistant
static bool is_susceptible(Person P, unsigned int r) {
  if (P.H.ever_infected) {
    return 0;
  }
  unsigned int resistant = P.H.resistance; // [0,63]
  if (P.H.vaccine) {
    // TODO: make this user-visible.
    resistant += RESISTANT_MAGIC_NUMBER;
  }
  return (r & 63) >= resistant;

}

static void infect_now(Person * P, int d) {
  P->H.date = d;
  P->H.infected = 1;
  P->H.ever_infected = 1;
}

static unsigned int new_infections(Person P, int d) {
  if (P.H.infected) {
    int tr = P.H.date + P.H.transmit_day;
    if (tr == d) {
      return P.H.transmit_potential;
    }
  }
  return 0;
}

static unsigned int sum_new_infections(Person * PP, int N, int d) {
  int o = 0;
  for (int i = 0; i < N; ++i) {
    o += new_infections(PP[i], d);
  }
  return o;
}

static void populate_houses(House * hh,
                            int n_houses,
                            const int * hidp, int N) {
  unsigned int * lhs = malloc(sizeof(int) * N);
  unsigned int * rhs = malloc(sizeof(int) * N);
  if (lhs == NULL || rhs == NULL) {
    free(lhs);
    return;
  }
  lhs[0] = 0;
  rhs[0] = 1;
  int j = 0;
  for (int i = 1; i < N; ++i) {
    if (hidp[i - 1] != hidp[i]) {
      ++j;
      lhs[j] = i;
    }
    rhs[j] = i + 1;
  }

  for (int h = 0; h < n_houses; ++h) {
    int h_size = rhs[h] - lhs[h];
    hh[h].h_size = h_size;
    hh[h].n_infected = 0;
    for (int j = 0; j < MAX_HSIZE; ++j) {
      hh[h].pp[j] = j < h_size ? (lhs[h] + j + 1) : 0;
    }
  }

  free(lhs);
  free(rhs);
}

static void populate_workplaces(Workplace * ww, int n_workplaces,
                                const int * widp, int N) {
  for (int j = 0; j < n_workplaces; ++j) {
    Workplace wj = ww[j];
    wj.n_infected = 0;
    for (int p = 0; p < MAX_WSIZE; ++p) {
      wj.pp[p] = 0;
    }
    wj.w_size = 0;
    ww[j] = wj;
  }

  for (int i = 0; i < N; ++i) {
    unsigned int wi = widp[i];
    --wi;
    if (wi >= n_workplaces) {
      continue;
    }
    Workplace wj = ww[wi];
    unsigned int size = wj.w_size;
    if (size >= MAX_WSIZE) continue;
    wj.pp[size] = i + 1;
    wj.w_size++;
    ww[wi] = wj;
  }
}

static void populate_persons(Person * pp, int N,
                             const int * hidp,
                             const int * widp,
                             const int * agep,
                             unsigned int * randp,
                             Epi E) {
  for (int i = 0; i < N; ++i) {
    Person P;
    Health H = u2H(randp[i]);
    P.H = H;
    P.age = agep[i];
    P.h = hidp[i];
    P.w = widp[i];
    pp[i] = P;
  }
}

Epi list2Epi(SEXP L) {
  if (TYPEOF(L) != VECSXP) {
    error("Epi was type '%s' but must be type list.", type2char(TYPEOF(L)));
  }
  Epi E;
  E.q_symp = list2q(L, "p_symp", 0.20);
  E.q_hosp = list2q(L, "p_hosp", 0.03);
  E.q_icu =  list2q(L, "p_icu", 0.015);
  E.q_kill = list2q(L, "p_kill", 0.009);
  E.q_home = (unsigned int)(UINT_MAX * 0.1);
  return E;
}

static void aggregate_infections(Person * pp, House * hh, Workplace * ww, int N, int d) {
  for (int i = 0; i < N; ++i) {
    if (is_infected(pp[i])) {
      int h = pp[i].h - 1;
      hh[h].n_infected += 1;
      int w = pp[i].w;
      if (w > 0) {
        ww[w - 1].n_infected += 1;
      }
    }
  }
}

static void reinfect(Person * pp, House * hh, Workplace * ww, int N, int d, Epi E, unsigned int * rand) {
  for (int i = 0; i < N; ++i) {
    Person P = pp[i];
    unsigned int transmissions_i = transmissions(P, d);

    if (transmissions_i) {
      int h = P.h;
      House H = hh[h - 1];
      int w = P.w;
      if (w) {
        Workplace W = ww[w - 1];
        int n_colleagues = W.w_size;
        if (n_colleagues) {
          for (int t = 0; t < transmissions_i; t += 2) {
            // alternate from house to workers
            int h_j = sample_inrange(H.h_size);  // position in household
            int w_j = sample_inrange(n_colleagues); // position in workplace
            int pp_hj = H.pp[h_j];
            int pp_wj = W.pp[w_j];
            Person P_h = pp[pp_hj];
            Person P_w = pp[pp_wj];
            if (is_susceptible(P_h, rand[pp_hj])) {
              infect_now(&P_h, d);
              pp[pp_hj] = P_h;
            }
            if (is_susceptible(P_w, rand[pp_wj])) {
              infect_now(&P_w, d);
              pp[pp_wj] = P_w;
            }
          }
        } else {
          for (int t = 0; t < transmissions_i; ++t) {
            int h_j = sample_inrange(H.h_size);
            int pp_hj = H.pp[h_j];
            Person P_h = pp[pp_hj];
            if (is_susceptible(P_h, rand[pp_hj])) {
              infect_now(&P_h, d);
              pp[pp_hj] = P_h;
            }

          }
        }
      }
    }
  }
}

static void do_caemovir(int d,
                        Person * pp,
                        House * hh,
                        Workplace * ww,
                        int N,
                        unsigned int * rand,
                        Epi E) {
  populateRandom_pcg32(rand, N, rand_pcg(), 3333);
  aggregate_infections(pp, hh, ww, N, d);
  reinfect(pp, hh, ww, N, d, E, rand);
}



SEXP C_caemovir(SEXP nDays,
                SEXP hid,
                SEXP wid,
                SEXP Age,
                SEXP Policy,
                SEXP Init,
                SEXP EEpi,
                SEXP Returner) {
  const int n_days = asInteger(nDays);
  if (n_days > MAX_N_DAYS) {
    error("n_days = %d but MAX_N_DAYS = %d", n_days, MAX_N_DAYS);
  }
  assertEquiInt(hid, wid);
  const int * hidp = INTEGER(hid);
  const int * widp = INTEGER(wid);
  const int * agep = INTEGER(Age);
  unsigned int N = length(hid);

  if (isntSorted(hidp, N)) {
    error("hid was not sorted."); // # nocov
  }
  if (hidp[0] != 1) {
    error("hidp[0] = %d but must be 1.", hidp[0]);
  }

  int n_houses = hidp[N - 1];
  int n_workplaces = Maxi(widp, N);
  if (!check_seq(widp, N, n_workplaces)) {
    error("n_workers = %d, ERR_NO: %d", n_workplaces, check_seq(widp, N, n_workplaces));
  }
  Epi E = list2Epi(EEpi);

  const int returner = asInteger(Returner);
  if (returner != 10) {
    error("returner != 10 not yet supported.");
  }
  Person * pp = malloc(sizeof(Person) * N);
  House * hh = malloc(sizeof(House) * n_houses);
  Workplace * ww = malloc(sizeof(Workplace) * n_workplaces);
  unsigned int * rand = malloc(sizeof(int) * N);

  if (pp == NULL || ww == NULL || hh == NULL || rand == NULL) {
    free(pp);
    free(ww);
    free(hh);
    free(rand);
    error("Unable to allocate.");
  }
  populateRandom_pcg32(rand, N, rand_pcg(), rand_pcg());
  populate_houses(hh, n_houses, hidp, N);
  populate_workplaces(ww, n_workplaces, widp, N);
  populate_persons(pp, N, hidp, widp, agep, rand, E);

  for (int d = 0; d < n_days; ++d) {
    do_caemovir(d, pp, hh, ww, N, rand, E);
  }
  unsigned int sni = sum_new_infections(pp, N, 0);
  Rprintf("sni = %d\n", sni);



  free(pp);
  free(ww);
  free(hh);
  free(rand);
  return R_NilValue;
}

static void update_severity(Health * H, unsigned int s) {
  // these are fixed in dhhs
  H->ever_die = s == 6;
  H->ever_icu = s >= 5;
  H->ever_hospital = s >= 4;
}

SEXP C_Encode_linelist2Health(SEXP DateZero,
                              SEXP DiagnosisDate,
                              SEXP Classification,
                              SEXP Acquired,
                              SEXP Severity,
                              SEXP Random,
                              SEXP EEpi) {
  unsigned int date0 = asInteger(DateZero);
  R_xlen_t N = xlength(DiagnosisDate);
  if (N != xlength(Classification) ||
      N != xlength(Acquired) ||
      N != xlength(Severity) ||
      N != xlength(Random)) {
    error("Internal error: lengths differ."); // # nocov
  }
  const int * ddate = INTEGER(DiagnosisDate);
  const int * cclas = INTEGER(Classification);
  const int * acqui = INTEGER(Acquired);
  const int * sever = INTEGER(Severity);
  const int * rand = INTEGER(Random);

  const Epi E = list2Epi(EEpi);

  SEXP ans = PROTECT(allocVector(INTSXP, N));
  int * restrict ansp = INTEGER(ans);

  for (R_xlen_t i = 0; i < N; ++i) {
    int date = ddate[i];
    unsigned int randi = rand[i];
    Health H = u2H(randi);
    if (date == NA_INTEGER) {
      ansp[i] = H2u32(H);
      continue;
    }
    if (date < date0 || date - date0 >= 255) {
      // already infected.
      H.date = 250;
      update_severity(&H, sever[i]);
      ansp[i] = H2u32(H);
      continue;
    }
    H.date = date - date0;
    H.infected = 1;
    H.maybe_die = randi <= E.q_kill;
    H.maybe_hospital = randi <= E.q_hosp;
    H.maybe_icu = randi <= E.q_icu;
    ansp[i] = H2u32(H);
  }
  UNPROTECT(1);
  return ans;
}
