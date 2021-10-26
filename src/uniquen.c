#include "caemovir.h"

int uniqueN_sorted(const int * xp, int n) {
  if (n <= 1) {
    return n;
  }
  int o = 1;
  for (int i = 1; i < n; ++i) {
    o += xp[i - 1] != xp[i];
  }
  return o;
}

// Ensure that x contains every value from 1 to maxi
int check_seq(const int * x, int n, int maxi) {
  if (maxi <= 0) {
    maxi = Maxi(x, n);
  }
  unsigned char * tbl = calloc(maxi, sizeof(char));
  if (tbl == NULL) {
    return -1;
  }

  for (int i = 0; i < n; ++i) {
    unsigned int xpi = x[i];
    --xpi;
    if (xpi >= maxi) {
      return xpi;
    }
    tbl[xpi] = 1;
  }
  for (int j = 0; j < maxi; ++j) {
    if (tbl[j] == 0) {
      return j;
    }
  }

  free(tbl);
  return 0;
}

SEXP C_check_seq(SEXP x, SEXP mm) {
  if (!isInteger(x)) {
    error("Not integer."); // # nocov
  }
  int n = length(x);
  const int * xp = INTEGER(x);
  const int m = asInteger(mm);
  int o = check_seq(xp, n, m);
  return ScalarInteger(o);
}

