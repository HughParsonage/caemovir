#include "caemovir.h"

SEXP ScalarLength(R_xlen_t N) {
  return (N <= INT_MAX) ? ScalarInteger(N) : ScalarReal(N);
}

R_xlen_t asLength(SEXP NN) {
  return isReal(NN) ? asReal(NN) : asInteger(NN);
}
