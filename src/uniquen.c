#include "caemovir.h"

int uniqueN_sorted(const int * xp, int n) {
  if (n <= 1) {
    return n;
  }
  int o = 1;
  for (int i = 1; i < n; ++i) {
    o += xp[i - 1] != xp[i];
  }
  return o;
}
