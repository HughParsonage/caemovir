#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* FIXME: 
   Check these declarations against the C/Fortran source code.
*/

/* .Call calls */
extern SEXP C_caemovir(SEXP, SEXP);
extern SEXP C_extract_SEIR(SEXP, SEXP);
extern SEXP C_prepare_SEIR(SEXP, SEXP, SEXP);
extern SEXP C_sample_fixed_TRUE(SEXP, SEXP);
extern SEXP C_SEIR(SEXP, SEXP);
extern SEXP Collapse(SEXP);

static const R_CallMethodDef CallEntries[] = {
    {"C_caemovir",          (DL_FUNC) &C_caemovir,          2},
    {"C_extract_SEIR",      (DL_FUNC) &C_extract_SEIR,      2},
    {"C_prepare_SEIR",      (DL_FUNC) &C_prepare_SEIR,      3},
    {"C_sample_fixed_TRUE", (DL_FUNC) &C_sample_fixed_TRUE, 2},
    {"C_SEIR",              (DL_FUNC) &C_SEIR,              2},
    {"Collapse",            (DL_FUNC) &Collapse,            1},
    {NULL, NULL, 0}
};

void R_init_caemovir(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
