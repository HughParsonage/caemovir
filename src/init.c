#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* FIXME: 
   Check these declarations against the C/Fortran source code.
*/

/* .Call calls */
extern SEXP C_caemovir(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP C_check_seq(SEXP, SEXP);
extern SEXP C_Encode_linelist2Health(SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP C_sample_fixed_TRUE(SEXP, SEXP);
extern SEXP C_SEIR(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP C_test_roundtrip_H2U2H(SEXP);
extern SEXP C_trand_pcg(SEXP, SEXP);
extern SEXP Collapse(SEXP);
extern SEXP CResetRNG(SEXP);

static const R_CallMethodDef CallEntries[] = {
    {"C_caemovir",               (DL_FUNC) &C_caemovir,               8},
    {"C_check_seq",              (DL_FUNC) &C_check_seq,              2},
    {"C_Encode_linelist2Health", (DL_FUNC) &C_Encode_linelist2Health, 5},
    {"C_sample_fixed_TRUE",      (DL_FUNC) &C_sample_fixed_TRUE,      2},
    {"C_SEIR",                   (DL_FUNC) &C_SEIR,                   9},
    {"C_test_roundtrip_H2U2H",   (DL_FUNC) &C_test_roundtrip_H2U2H,   1},
    {"C_trand_pcg",              (DL_FUNC) &C_trand_pcg,              2},
    {"Collapse",                 (DL_FUNC) &Collapse,                 1},
    {"CResetRNG",                (DL_FUNC) &CResetRNG,                1},
    {NULL, NULL, 0}
};

void R_init_caemovir(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
