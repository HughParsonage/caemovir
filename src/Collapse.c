#include "caemovir.h"

void collapse_ui(unsigned int * x, int n_out, int n) {
  for (int i = 0, j = 0; i < n; ++i) {
    x[j] = x[i];
    j += x[i] > 0;
  }
}

SEXP Collapse(SEXP x) {
  if (!isInteger(x) || xlength(x) <= 1 || xlength(x) >= INT_MAX) {
    return x;
  }
  int N = length(x);
  int uniquen = 0;
  const int * xp = INTEGER(x);
  for (int i = 0; i < N; ++i) {
    uniquen += xp[i] > 0;
  }
  SEXP ans = PROTECT(allocVector(INTSXP, uniquen));
  int * restrict ansp = INTEGER(ans);
  for (int i = 0, j = 0; i < N; ++i) {
    ansp[j] = xp[i];
    j += xp[i] > 0;
  }
  UNPROTECT(1);
  return ans;
}
