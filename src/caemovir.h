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

int isntSorted(const int * x, int n);
int maxiy(const int * x, int n, int y);

unsigned int rand_pcg();


// sample.c
void csample_fixed_TRUE(unsigned char * xp, unsigned int n, unsigned int nTRUE);

// swap.c
void swap(int * a, int * b);
void ucswap(unsigned char * a, unsigned char * b);

#endif
