#include "caemovir.h"

// equivalent to word % p;
// https://github.com/lemire/fastrange/blob/master/fastrange.h
uint32_t fastrange32(uint32_t word, uint32_t p) {
  return (uint32_t)(((uint64_t)word * (uint64_t)p) >> 32);
}

uint32_t sample_inrange(uint32_t N) {
  return fastrange32(rand_pcg(), N);
}

//' @noRd
//' @param x The array from which a sample is to be taken
//' @param n The length of the array (the number of available choices)
//' @param r A random unsigned integer.
int sample1(int * x, unsigned int n, unsigned int r) {
  unsigned int j = r / (UINT_MAX / n);
  return x[j];
}

void vsample(int * ans, int n_out, int * x, unsigned int n, unsigned int * r) {
  for (int i = 0; i < n_out; ++i) {
    ans[i] = sample1(x, n, r[i]);
  }
}

void isample_fixed_TRUE(int * xp, unsigned int n, unsigned int nTRUE) {
  if (nTRUE >= n) {
    for (int i = 0; i < n; ++i) {
      xp[i] = 1;
    }
    return;
  }
  int nFALSE = n - nTRUE;
  if (nFALSE >= n) {
    for (int i = 0; i < n; ++i) {
      xp[i] = 0;
    }
    return;
  }

  for (int i = 0; i < n; ++i) {
    xp[i] = i < nTRUE;
  }

  for (int i = n - 1; i > 0; --i) {
    int j = rand_pcg() % n;
    swap(&xp[i], &xp[j]);
  }

}

void csample_fixed_TRUE(unsigned char * xp, unsigned int n, unsigned int nTRUE) {
  if (nTRUE >= n) {
    for (int i = 0; i < n; ++i) {
      xp[i] = 1;
    }
    return;
  }
  int nFALSE = n - nTRUE;
  if (nFALSE >= n) {
    for (int i = 0; i < n; ++i) {
      xp[i] = 0;
    }
    return;
  }

  for (int i = 0; i < n; ++i) {
    xp[i] = i < nTRUE;
  }

  for (int i = n - 1; i > 0; --i) {
    int j = rand_pcg() % n;
    ucswap(&xp[i], &xp[j]);
  }

}



SEXP C_sample_fixed_TRUE(SEXP nn, SEXP nnTRUE) {
  const unsigned int n = asInteger(nn);
  const unsigned int nTRUE = asInteger(nnTRUE);
  SEXP ans = PROTECT(allocVector(LGLSXP, n));
  int * ansp = LOGICAL(ans);
  isample_fixed_TRUE(ansp, n, nTRUE);
  UNPROTECT(1);
  return ans;
}



