#include "caemovir.h"




#define STATUS_SUSCE_UNVA 1
#define STATUS_SUSCE_VACC 2
#define STATUS_INFEC_SYMP 3
#define STATUS_INFEC_ASYM 4
#define STATUS_INFEC_HOSP 5
#define STATUS_INFEC_CRIT 6
#define STATUS_RESOL_DEAD 7
#define STATUS_RESOL_WELL 8

#define VACC_UNVACC 0
#define VACC_PFIZER 1
#define VACC_ASTRAZ 2
#define VACC_MODERN 3

typedef struct {
  unsigned char date;
  unsigned int vaccine : 3;
  unsigned int status : 4;
  unsigned int max_status : 4;
} Person;

static inline unsigned char date_infected(Person * P) {
  return P->date;
}

static inline unsigned int vaccine_of(Person * P) {
  return P->vaccine;
}

static inline unsigned int status_of(Person * P) {
  return P->status;
}

static inline unsigned int max_status_of(Person * P) {
  return P->max_status;
}


void infect_now(Person * P, unsigned char d) {
  P->date = d;
}

void set_status(Person * P, unsigned int s) {
  P->status = s;
}

static Person u2P(unsigned int x) {
  unsigned char date = x & 255;
  unsigned int statuses = (x / 256u) & 255;
  unsigned int status = statuses & 15;
  unsigned int max_status = statuses / 16;
  unsigned int vaccine = ((x / 256) / 256) & 15;
  Person P;
  P.date = date;
  P.status = status;
  P.max_status = max_status;
  P.vaccine = vaccine;
  return P;
}

static unsigned int P2U(Person * P) {
  unsigned int status = status_of(P);
  unsigned int max_status = max_status_of(P);
  unsigned int vaccine = vaccine_of(P);
  unsigned int date_infect = date_infected(P);
  return (vaccine << 16) + ((status + (max_status << 4)) << 8) + date_infect;
}

static void do_1seir(Person * x, unsigned int r, unsigned char d, unsigned int Q2I) {
  if (r & 3) {
    return;
  }
  unsigned int status = status_of(x);
  unsigned int vaccine = vaccine_of(x);


  // Q2I is the unsigned int such that Q2I/UINT_MAX = % remaining uninfected
  // while susceptible+unvaccinted,
  // Q2J while susceptible+vaccintaed
  unsigned int Q2J = UINT_MAX - (UINT_MAX - Q2I) / 32;

  switch(status) {
  case STATUS_RESOL_DEAD:
    return;
  case STATUS_RESOL_WELL:
    return;
  case STATUS_SUSCE_VACC:
    switch(vaccine) {
    case VACC_PFIZER:
    case VACC_ASTRAZ:
    case VACC_MODERN:
      if (r > Q2J) {
        infect_now(x, d);
      }
    }
    return; // # nocov
  case STATUS_SUSCE_UNVA:
    if (r > Q2I) {
      infect_now(x, d);
    }
    return;
  case STATUS_INFEC_ASYM:
    if (d == date_infected(x) + 11) {
      set_status(x, STATUS_RESOL_WELL);
      return;
    } else {
      switch(vaccine) {
      case VACC_PFIZER:
      case VACC_ASTRAZ:
      case VACC_MODERN:
        if (r <= 171798692) {
          set_status(x, STATUS_INFEC_SYMP);
        }
        return;
      default:
        return (r <= 1610612736) ? set_status(x, STATUS_INFEC_SYMP) : x;
      }
    }
  case STATUS_INFEC_SYMP:
    if (r <= 214748365) {
      set_status(x, STATUS_INFEC_HOSP);
    }
    return;
  case STATUS_INFEC_HOSP:
    if (r >> 28) {
      set_status(x, STATUS_INFEC_CRIT);
    }
    return;
  case STATUS_INFEC_CRIT:
    if (r >> 27) {
      set_status(x, STATUS_RESOL_DEAD);
    }
  }
}

static void do_seir(int * restrict ansp, unsigned char d, int N, int z, unsigned char * zj) {
  if (z == 0) {
    for (int i = 0; i < N; ++i) {
      unsigned int xpi = ansp[i];
      Person P = u2P(xpi);
      do_1seir(&P, rand_pcg(), d, 4227053236u);
      ansp[i] = P2U(&P);
    }
  } else {
    if (zj == NULL) {
      // Unusual but we will just make a very low probability of remaining uninfected
      // for the z required
      for (int i = 0; i < N; ++i) {
        unsigned int xpi = ansp[i];
        Person P = u2P(xpi);
        do_1seir(&P, rand_pcg(), d, i < z ? 10u : 4227053236u);
        ansp[i] = P2U(&P);
      }
    } else {
      csample_fixed_TRUE(zj, N, z);
      for (int i = 0; i < N; ++i) {
        unsigned int xpi = ansp[i];
        Person P = u2P(xpi);
        do_1seir(&P, rand_pcg(), d, zj[i] ? 10u : 3435973837u);
        ansp[i] = P2U(&P);
      }
    }
  }
}

unsigned int n_seir_infected(int * x, int n) {
  int o = 0;
  for (int i = 0; i < n; ++i) {
    Person P = u2P(x[i]);
    unsigned int si = status_of(&P);
    o += si == STATUS_INFEC_ASYM;
    o += si == STATUS_INFEC_SYMP;
  }
  return o;
}


SEXP C_SEIR(SEXP x, SEXP Ndays) {
  int N = length(x);
  const unsigned char ndays = asInteger(Ndays);
  if (ndays >= 254) {
    error("ndays >= 254");
  }
  const int * xp = INTEGER(x);
  int np = 0;
  SEXP ans = PROTECT(allocVector(INTSXP, N)); np++;
  int * restrict ansp = INTEGER(ans);
  for (int i = 0; i < N; ++i) {
    ansp[i] = xp[i];
  }
  unsigned char * zj = malloc(sizeof(char) * N);
  for (unsigned char d = 1; d < ndays; ++d) {
    int n_infected = n_seir_infected(ansp, N) >> 2;
    do_seir(ansp, d, N, n_infected, zj);
  }
  free(zj);
  UNPROTECT(np);
  return ans;
}

SEXP C_prepare_SEIR(SEXP nn, SEXP nn_infected, SEXP nn_vacc) {
  const int n = asInteger(nn);
  const int n_infected = asInteger(nn_infected);
  const int n_vacc = asInteger(nn_vacc);
  SEXP ans = PROTECT(allocVector(INTSXP, n));
  int * restrict ansp = INTEGER(ans);
  for (int i = 0; i < n; ++i) {
    Person P;
    P.date = 255;
    if (i < n_infected) {
      P.status = STATUS_INFEC_ASYM;
      P.date = 0;
      if (i < n_vacc) {
        P.vaccine = VACC_PFIZER;
      } else {
        P.vaccine = 0;
      }
    } else {
      if (i < n_vacc) {
        P.status = STATUS_SUSCE_VACC;
        P.vaccine = VACC_PFIZER;
        P.date = 255;
      } else {
        P.status = STATUS_SUSCE_UNVA;
        P.vaccine = 0;
        P.date = 255;
      }
    }
    ansp[i] = P2U(&P);
  }
  UNPROTECT(1);
  return ans;
}

SEXP C_extract_SEIR(SEXP x, SEXP mm) {
  const int m = asInteger(mm);
  const int * xp = INTEGER(x);
  R_xlen_t N = xlength(x);
  SEXP ans = PROTECT(allocVector(INTSXP, N));
  int * restrict ansp = INTEGER(ans);
  for (R_xlen_t i = 0; i < N; ++i) {
    unsigned int o = xp[i];
    Person P = u2P(o);
    switch(m) {
    case 0:
      o = date_infected(&P);
      break;
    case 1:
      o = vaccine_of(&P);
      break;
    case 2:
      o = status_of(&P);
      break;
    }
    ansp[i] = o;
  }
  UNPROTECT(1);
  return ans;
}


