#include "caemovir.h"

int isntSorted(const int * x, int n) {
  for (int i = 1; i < n; ++i) {
    if (x[i - 1] > x[i]) {
      return i + 1;
    }
  }
  return 0;
}
