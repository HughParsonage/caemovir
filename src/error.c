#include "caemovir.h"

void assertEquiInt(SEXP x, SEXP y) {
  if (!isInteger(x)) {
    error("x was type '%s' but must be type integer.", type2char(TYPEOF(x)));
  }
  if (!isInteger(y)) {
    error("y was type '%s' but must be type integer.", type2char(TYPEOF(y)));
  }
  if (xlength(x) >= INT_MAX || xlength(y) >= INT_MAX) {
    error("length(x) >= INT_MAX");
  }
  if (length(x) != length(y)) {
    error("length(x) != length(y)");
  }
}



