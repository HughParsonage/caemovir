#include "caemovir.h"

#define MAX_HSIZE 8
#define MAX_HSIZD 7
#define RANDOM_LEN 16777216
#define RANDOM_LEM 16777215

typedef struct {
  int status : 4;
  unsigned char date;
  int hh;
  int ww;
  int ss;
  unsigned char resistance;
} Person;

typedef struct {
  int hh;
  int pp[MAX_HSIZE];
  unsigned char rr_pp[MAX_HSIZE];
  unsigned char dd_pp[MAX_HSIZE];
  int size : 3;
} House;

typedef struct {
  int ww;
  int * pp;
  int size;
} Work;




void enhouse(House * ans,
             const int * pp,
             const int * hh,
             const unsigned char * rr,
             int n,
             int n_houses) {

  int i_house = 0;
  // first house
  ans[0].hh = 0;
  ans[0].pp[0] = pp[0];
  ans[0].rr_pp[0] = rr[0];
  ans[0].dd_pp[0] = 0;
  int j = 1;

  for (int i = 1; i < n; ++i) {
    if (hh[i - 1] != hh[i]) {
      // new house
      i_house++;
      ans[i_house].size = 0;
      j = 0;
    }
    ans[i_house].hh = i_house;
    ans[i_house].pp[j] = pp[i];
    ans[i_house].rr_pp[j] = rr[i];
    ans[i_house].dd_pp[j] = 0;
    ans[i_house].size++;
    ++j;
  }
}

House house_of(int p, const int * pp, const int * hh, House * Houses) {
  int hi =  hh[p - 1];
  return Houses[hi];
}

unsigned int d2uc(double p) {
  unsigned int pi = p * 256;
  return pi & 255;
}

// given a probability in (0, 1) and an array of the given size,
// returns yes/no
void h_q2array(int pp[MAX_HSIZE], int size, double q, const unsigned int random[RANDOM_LEN], unsigned int rj) {
  unsigned int p = d2uc(q);
  for (int j = 0; j < size; ++j) {
    unsigned int r = random[(rj + j) & RANDOM_LEM] & 255;
    if (r <= p) {
      pp[j] = 1;
    }
  }



}

bool is_susceptible(unsigned char q, unsigned char resistance, unsigned char diagnosis_date, unsigned char d) {
  return d < diagnosis_date && q > resistance;
}


void infect_house(House H,
                  unsigned char * ans,
                  int new_infections,
                  unsigned char d) {

  if (new_infections == 0) {
    return;
  }
  // which members will be the new infections?
  //
  for (int j = 0; j < new_infections; ++j) {
    unsigned int rj = rand_pcg();
    int p = H.pp[rj & MAX_HSIZD];
    unsigned char resist = H.rr_pp[rj & MAX_HSIZD];
    unsigned char q = rj & 255;
    if (is_susceptible(q, resist, ans[p], d)) {
      ans[p] = d;
    }
  }
}

static const unsigned int R16[16] = {0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1};
static const unsigned int SEQ8[8] = {0, 1, 2, 3, 4, 5, 6, 7};

unsigned int rar(void) {
  unsigned int p = rand_pcg();
  return R16[p & 15];
}



void sample_house(int pp[MAX_HSIZE]) {
#if MAX_HSIZE == 8
  memcpy(pp, SEQ8, sizeof(SEQ8));
#else
  for (int j = 0; j < MAX_HSIZE; ++j) {
    pp[j] = j;
  }
#endif
  for (int j = MAX_HSIZD; j > 0; --j) {
    int i = rand_pcg() % MAX_HSIZE;
    swap(&pp[i], &pp[j]);
  }
}


SEXP C_caemovir(SEXP daysToSimulate,
                SEXP PatientsZero,
                SEXP Resistance,
                SEXP PP,
                SEXP HH,
                SEXP WW,
                SEXP R0,
                SEXP Random) {
  const int n_persons = length(PP);
  if (length(Resistance) != n_persons) {
    error("length(Resistance) != n_persons");
  }
  int N = length(PP);
  const int * patientsZerop = INTEGER(PatientsZero);
  const unsigned char * resistance = RAW(Resistance);
  const int * pp = INTEGER(PP);
  const int * hh = INTEGER(HH);
  const int * ww = INTEGER(WW);
  const int days_to_simulate = asInteger(daysToSimulate);
  if (isntSorted(hh, N)) {
    error("hh not sorted at position %d", isntSorted(hh, N));
  }
  if (hh[0] != 0) {
    error("First element of hh is %d but must be zero (i.e. hh must bezero-indexed)", hh[0]);
  }
  const double r0 = asReal(R0);
  const unsigned int r0u = UINT_MAX * (r0 - 1);

  int n_infected = length(PatientsZero);
  unsigned int * infectedi = malloc(sizeof(int) * n_persons);
  if (infectedi == NULL) {
    error("infectedi malloc unable."); // # nocov
  }
  for (int i = 0; i < n_persons; ++i) {
    infectedi[i] = i < n_infected ? patientsZerop[i] : 0;
  }
  int n_houses = maxiy(hh, N, hh[N - 1]) + 1;
  House * Houses = malloc(sizeof(House) * n_houses);
  if (Houses == NULL) {
    free(infectedi);
    error("malloc Houses failure."); // # nocov
  }
  enhouse(Houses, pp, hh, resistance, N, n_houses);
  unsigned int * infectedh = calloc(n_houses, sizeof(int));
  if (infectedh == NULL) {
    free(infectedi);
    free(infectedh);
    error("malloc infectedh failure."); // # nocov
  }

  SEXP ans = PROTECT(allocVector(RAWSXP, N));
  unsigned char * ansp = RAW(ans);
  memset(ansp, 0, N);

  for (int d = 0; d < days_to_simulate; ++d) {
    int n_houses_infected_d = 0;
    for (int pz = 0; pz < n_infected; ++pz) {
      // First which houses are infected?
      int p = infectedi[pz];
      int h_pz = hh[p];
      if (infectedh[h_pz] == 0) {
        // Record the location of the next
        infectedh[n_houses_infected_d] = h_pz;
        n_houses_infected_d++;
      }
      // Then which workplaces are infected

    }
    for (int hz = 0; hz < n_houses_infected_d; ++hz) {
      int hh = infectedh[hz];
      int new_infections = rar();
      if (!new_infections) {
        continue;
      }
      int pp_hz[MAX_HSIZE] = {0};
      sample_house(pp_hz);
      House H = Houses[hh];
      infect_house(H, ansp, new_infections, d);
    }

  }
  free(infectedi);
  free(infectedh);
  free(Houses);
  UNPROTECT(1);
  return ans;
}
