#ifndef CAEMOVIR_H
#define CAEMOVIR_H

#if _OPENMP
#include <omp.h>
#endif

#include <R.h>
#define USE_RINTERNALS
#include <Rinternals.h>
#include <stdint.h> // for uint64_t rather than unsigned long long
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

// erorr.c
void assertEquiInt(SEXP x, SEXP y);

int isntSorted(const int * x, int n);
int maxiy(const int * x, int n, int y);
int Maxi(const int * x, int N);

// pcg_hash
unsigned int rand_pcg();
unsigned int next_rand(unsigned int r);
void populateRandom_pcg32(int * answer, int size, int state, int inc);

// omp_diagnose.c
int as_nThread(SEXP x);

// ScalarLength.c
SEXP ScalarLength(R_xlen_t N);
R_xlen_t asLength(SEXP NN);


// sample.c
uint32_t fastrange32(uint32_t word, uint32_t p);
uint32_t sample_inrange(uint32_t N);
void csample_fixed_TRUE(unsigned char * xp, unsigned int n, unsigned int nTRUE);

// swap.c
void swap(int * a, int * b);
void ucswap(unsigned char * a, unsigned char * b);

// uniquen.c
int check_seq(const int * x, int n, int maxi);

#endif
