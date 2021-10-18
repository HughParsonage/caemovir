#include "caemovir.h"

void swap(int * a, int * b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void ucswap(unsigned char * a, unsigned char * b) {
  unsigned char tmp = *a;
  *a = *b;
  *b = tmp;
}
