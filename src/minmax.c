#include "caemovir.h"

int check_max_uniqueN(const int * xp, int n, int mx) {
  // ensure that no numbers are beyond [0, mx]
  for (int i = 0; i < n; ++i) {
    unsigned int xpi = xp[i];
    if (xpi > mx) {
      return i + 1;
    }
  }
  return 0;
}

int maxiy(const int * x, int n, int y) {
  for (int i = 0; i < n; ++i) {
    int xi = x[i];
    if (xi < 0) {
      return -i;
    }
    if (xi > y) {
      return i + 1;
    }
  }
  return y;
}
