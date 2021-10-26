#include "caemovir.h"

// from stats package
/* get the list element named str, or return NULL */

SEXP getListElement(SEXP list, const char *str) {
  if (TYPEOF(list) != VECSXP) {
    error("Internal error: TYPEOF(list) != VECSXP in getListElement."); // # nocov
  }
  SEXP elmt = R_NilValue, names = getAttrib(list, R_NamesSymbol);

  for (R_len_t i = 0; i < length(list); i++) {
    if (strcmp(CHAR(STRING_ELT(names, i)), str) == 0) {
      elmt = VECTOR_ELT(list, i);
      break;
    }
  }
  return elmt;
}

unsigned int list2q(SEXP L, const char * str, double default_p) {
  SEXP e = getListElement(L, str);
  if (isNull(e)) {
    error("Element '%s' not present in Epi list.", str);
  }
  if (!isInteger(e)) {
    if (!isReal(e)) {
      error("Element '%s' was type '%s' but must be double.", str, type2char(TYPEOF(e)));
    }
    double ed = asReal(e);
    unsigned int o = UINT_MAX * ifbw01(ed, default_p);
    return o;
  }
  return (unsigned int)asInteger(e);
}

