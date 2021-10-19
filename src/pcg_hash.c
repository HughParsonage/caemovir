#include "caemovir.h"
#include "simdpcg32.h"

unsigned int rng_state;
unsigned int rng_states[8];

SEXP CResetRNG(SEXP x) {
  if (TYPEOF(x) == INTSXP && xlength(x) == 1) {
    rng_state = INTEGER_ELT(x, 0);
  }
  return ScalarInteger(rng_state);
}

unsigned int rand_pcg() {
  unsigned int state = rng_state;
  rng_state = rng_state * 747796405u + 2891336453u;
  unsigned int word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}

// given a uniform random variable p, and an unsigned int
// a, return TRUE on realization (concretetly when p < U(0, 1))
bool unif_p2b(double p, unsigned int a) {
  unsigned int p_ = p * UINT_MAX;
  return a <= p_;
}

unsigned int r2r(double r) {
  return r * UINT_MAX;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcg32.h"
#include "simdpcg32.h"

static uint32_t counter;
void populateRandom_pcg32(uint32_t *answer, uint32_t size, int state, int inc) {
  pcg32_random_t key = {
    .state = state,
    .inc = inc}; // I am a crazy man using bleeding-edge C99 in 2018
  // pcg32_random_t key = {
  //   .state = 324,
  //   .inc = 4444}; // I am a crazy man using bleeding-edge C99 in 2018
  for (uint32_t i = 0; i < size; i++) {
    answer[i] = pcg32_random_r(&key);
  }
}
#ifdef AVX2PCG
void populateRandom_avx2_pcg32(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx2_pcg32_random_t key = {
    .state = {_mm256_set1_epi64x(1111), _mm256_set1_epi64x(1111)},
    .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 8) {
    for (; i < size - 8; i += 8) {
      __m256i r = avx2_pcg32_random_r(&key);
      _mm256_storeu_si256((__m256i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m256i r = avx2_pcg32_random_r(&key);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx2_pcg32_two(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx2_pcg32_random_t key1 = {
    .state = {_mm256_set1_epi64x(1111), _mm256_set1_epi64x(1111)},
    .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx2_pcg32_random_t key2 = {
    .state = {_mm256_set1_epi64x(2222), _mm256_set1_epi64x(1111)},
    .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m256i r1 = avx2_pcg32_random_r(&key1);
      __m256i r2 = avx2_pcg32_random_r(&key2);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
    }
  }
  if (size - i >= 8) {
    __m256i r = avx2_pcg32_random_r(&key1);
    _mm256_storeu_si256((__m256i *)(answer + i), r);
    i += 8;
  }
  if (i < size) {
    __m256i r = avx2_pcg32_random_r(&key1);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx2_pcg32_four(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx2_pcg32_random_t key1 = {
    .state = {_mm256_set1_epi64x(1111), _mm256_set1_epi64x(1111)},
    .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx2_pcg32_random_t key2 = {
    .state = {_mm256_set1_epi64x(2222), _mm256_set1_epi64x(1111)},
    .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx2_pcg32_random_t key3 = {
    .state = {_mm256_set1_epi64x(3333), _mm256_set1_epi64x(1111)},
    .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx2_pcg32_random_t key4 = {
    .state = {_mm256_set1_epi64x(4444), _mm256_set1_epi64x(1111)},
    .inc = {_mm256_set_epi64x(15, 13, 11, 9), _mm256_set_epi64x(7, 5, 3, 1)},
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 32) {
    for (; i < size - 32; i += 32) {
      __m256i r1 = avx2_pcg32_random_r(&key1);
      __m256i r2 = avx2_pcg32_random_r(&key2);
      __m256i r3 = avx2_pcg32_random_r(&key3);
      __m256i r4 = avx2_pcg32_random_r(&key4);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
      _mm256_storeu_si256((__m256i *)(answer + i + 16), r3);
      _mm256_storeu_si256((__m256i *)(answer + i + 24), r4);
    }
  }
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m256i r1 = avx2_pcg32_random_r(&key1);
      __m256i r2 = avx2_pcg32_random_r(&key2);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
    }
  }
  if (size - i >= 8) {
    __m256i r = avx2_pcg32_random_r(&key1);
    _mm256_storeu_si256((__m256i *)(answer + i), r);
    i += 8;
  }
  if (i < size) {
    __m256i r = avx2_pcg32_random_r(&key1);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx256_pcg32(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx256_pcg32_random_t key = {
    .state = _mm256_set1_epi64x(1111),
    .inc = _mm256_set_epi64x(15, 13, 11, 9),
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 4) {
    for (; i < size - 4; i += 4) {
      __m128i r = avx256_pcg32_random_r(&key);
      _mm_storeu_si128((__m128i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m128i r = avx256_pcg32_random_r(&key);
    uint32_t buffer[4];
    _mm_storeu_si128((__m128i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx256_pcg32_two(uint32_t *answer, uint32_t size, uint32_t r1, uint32_t r2) {
  uint32_t i = 0;
  avx256_pcg32_random_t key1 = {
    .state = _mm256_set1_epi64x(1111 + r1),
    .inc = _mm256_set_epi64x(15, 13, 11, 9),
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx256_pcg32_random_t key2 = {
    .state = _mm256_set1_epi64x(2222 + r2),
    .inc = _mm256_set_epi64x(7, 5, 3, 1),
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 8) {
    for (; i < size - 8; i += 8) {
      __m128i r1 = avx256_pcg32_random_r(&key1);
      __m128i r2 = avx256_pcg32_random_r(&key2);
      _mm_storeu_si128((__m128i *)(answer + i), r1);
      _mm_storeu_si128((__m128i *)(answer + i + 4), r2);
    }
  }
  if (size - i >= 4) {
    __m128i r = avx256_pcg32_random_r(&key1);
    _mm_storeu_si128((__m128i *)(answer + i), r);
    i += 4;
  }
  if (i < size) {
    __m128i r = avx256_pcg32_random_r(&key1);
    uint32_t buffer[8];
    _mm_storeu_si128((__m128i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx256_pcg32_four(uint32_t *answer, uint32_t size, uint32_t r1, uint32_t r2) {
  uint32_t i = 0;
  avx256_pcg32_random_t key1 = {
    .state = _mm256_set1_epi64x(1111u + r1),
    .inc = _mm256_set_epi64x(15, 13, 11, 9),
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx256_pcg32_random_t key2 = {
    .state = _mm256_set1_epi64x(2222u + r2),
    .inc = _mm256_set_epi64x(15, 13, 11, 9),
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx256_pcg32_random_t key3 = {
    .state = _mm256_set1_epi64x(3333u - r1),
    .inc = _mm256_set_epi64x(15, 13, 11, 9),
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  avx256_pcg32_random_t key4 = {
    .state = _mm256_set1_epi64x(4444u - r2),
    .inc = _mm256_set_epi64x(15, 13, 11, 9),
    .pcg32_mult_l =
  _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) & 0xffffffff),
  .pcg32_mult_h = _mm256_set1_epi64x(UINT64_C(0x5851f42d4c957f2d) >> 32)};
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m128i r1 = avx256_pcg32_random_r(&key1);
      __m128i r2 = avx256_pcg32_random_r(&key2);
      __m128i r3 = avx256_pcg32_random_r(&key3);
      __m128i r4 = avx256_pcg32_random_r(&key4);
      _mm_storeu_si128((__m128i *)(answer + i), r1);
      _mm_storeu_si128((__m128i *)(answer + i + 4), r2);
      _mm_storeu_si128((__m128i *)(answer + i + 8), r3);
      _mm_storeu_si128((__m128i *)(answer + i + 12), r4);
    }
  }
  if (size >= 8) {
    for (; i < size - 8; i += 8) {
      __m128i r1 = avx256_pcg32_random_r(&key1);
      __m128i r2 = avx256_pcg32_random_r(&key2);
      _mm_storeu_si128((__m128i *)(answer + i), r1);
      _mm_storeu_si128((__m128i *)(answer + i + 4), r2);
    }
  }
  if (size - i >= 4) {
    __m128i r = avx256_pcg32_random_r(&key1);
    _mm_storeu_si128((__m128i *)(answer + i), r);
    i += 8;
  }
  if (i < size) {
    __m128i r = avx256_pcg32_random_r(&key1);
    uint32_t buffer[4];
    _mm_storeu_si128((__m128i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}
#endif

#ifdef AVX512PCG

void populateRandom_avx512_pcg32(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512_pcg32_random_t key = {
    .state = _mm512_set1_epi64(1111),
    .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
    .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 8) {
    for (; i < size - 8; i += 8) {
      __m256i r = avx512_pcg32_random_r(&key);
      _mm256_storeu_si256((__m256i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m256i r = avx512_pcg32_random_r(&key);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx512bis_pcg32(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512bis_pcg32_random_t key = {
    .state = {_mm512_set1_epi64(1111), _mm512_set1_epi64(2222)},
    .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
  _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
  .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m512i r = avx512bis_pcg32_random_r(&key);
      _mm512_storeu_si512((__m512i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m512i r = avx512bis_pcg32_random_r(&key);
    uint32_t buffer[16];
    _mm512_storeu_si512((__m512i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx512bis_pcg32_two(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512bis_pcg32_random_t key1 = {
    .state = {_mm512_set1_epi64(1111), _mm512_set1_epi64(2222)},
    .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
  _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
  .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512bis_pcg32_random_t key2 = {
    .state = {_mm512_set1_epi64(3333), _mm512_set1_epi64(4444)},
    .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
  _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
  .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 32) {
    for (; i < size - 32; i += 32) {
      __m512i r1 = avx512bis_pcg32_random_r(&key1);
      __m512i r2 = avx512bis_pcg32_random_r(&key2);
      _mm512_storeu_si512((__m512i *)(answer + i), r1);
      _mm512_storeu_si512((__m512i *)(answer + i + 16), r2);
    }
  }
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m512i r = avx512bis_pcg32_random_r(&key1);
      _mm512_storeu_si512((__m512i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m512i r = avx512bis_pcg32_random_r(&key1);
    uint32_t buffer[16];
    _mm512_storeu_si512((__m512i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx512bis_pcg32_four(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512bis_pcg32_random_t key1 = {
    .state = {_mm512_set1_epi64(1111), _mm512_set1_epi64(2222)},
    .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
  _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
  .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512bis_pcg32_random_t key2 = {
    .state = {_mm512_set1_epi64(3333), _mm512_set1_epi64(4444)},
    .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
  _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
  .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512bis_pcg32_random_t key3 = {
    .state = {_mm512_set1_epi64(5555), _mm512_set1_epi64(6666)},
    .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
  _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
  .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512bis_pcg32_random_t key4 = {
    .state = {_mm512_set1_epi64(7777), _mm512_set1_epi64(8888)},
    .inc = {_mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
  _mm512_set_epi64(155, 135, 115, 95, 75, 55, 35, 15)},
  .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 64) {
    for (; i < size - 64; i += 64) {
      __m512i r1 = avx512bis_pcg32_random_r(&key1);
      __m512i r2 = avx512bis_pcg32_random_r(&key2);
      __m512i r3 = avx512bis_pcg32_random_r(&key3);
      __m512i r4 = avx512bis_pcg32_random_r(&key4);
      _mm512_storeu_si512((__m512i *)(answer + i), r1);
      _mm512_storeu_si512((__m512i *)(answer + i + 16), r2);
      _mm512_storeu_si512((__m512i *)(answer + i + 32), r3);
      _mm512_storeu_si512((__m512i *)(answer + i + 48), r4);
    }
  }
  if (size >= 32) {
    for (; i < size - 32; i += 32) {
      __m512i r1 = avx512bis_pcg32_random_r(&key1);
      __m512i r2 = avx512bis_pcg32_random_r(&key2);
      _mm512_storeu_si512((__m512i *)(answer + i), r1);
      _mm512_storeu_si512((__m512i *)(answer + i + 16), r2);
    }
  }
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m512i r = avx512bis_pcg32_random_r(&key1);
      _mm512_storeu_si512((__m512i *)(answer + i), r);
    }
  }
  if (i < size) {
    __m512i r = avx512bis_pcg32_random_r(&key1);
    uint32_t buffer[16];
    _mm512_storeu_si512((__m512i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}
void populateRandom_avx512_pcg32_two(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512_pcg32_random_t key1 = {
    .state = _mm512_set1_epi64(1111),
    .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
    .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512_pcg32_random_t key2 = {
    .state = _mm512_set1_epi64(1111222),
    .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
    .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m256i r1 = avx512_pcg32_random_r(&key1);
      __m256i r2 = avx512_pcg32_random_r(&key2);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
    }
  }
  if (size - i >= 8) {
    __m256i r = avx512_pcg32_random_r(&key1);
    _mm256_storeu_si256((__m256i *)(answer + i), r);
    i += 8;
  }
  if (i < size) {
    __m256i r = avx512_pcg32_random_r(&key1);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

void populateRandom_avx512_pcg32_four(uint32_t *answer, uint32_t size) {
  uint32_t i = 0;
  avx512_pcg32_random_t key1 = {
    .state = _mm512_set1_epi64(1111),
    .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
    .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512_pcg32_random_t key2 = {
    .state = _mm512_set1_epi64(1111222),
    .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
    .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512_pcg32_random_t key3 = {
    .state = _mm512_set1_epi64(111133333),
    .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
    .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  avx512_pcg32_random_t key4 = {
    .state = _mm512_set1_epi64(1111444444),
    .inc = _mm512_set_epi64(15, 13, 11, 9, 7, 5, 3, 1),
    .multiplier = _mm512_set1_epi64(0x5851f42d4c957f2d)};
  if (size >= 32) {
    for (; i < size - 32; i += 32) {
      __m256i r1 = avx512_pcg32_random_r(&key1);
      __m256i r2 = avx512_pcg32_random_r(&key2);
      __m256i r3 = avx512_pcg32_random_r(&key3);
      __m256i r4 = avx512_pcg32_random_r(&key4);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
      _mm256_storeu_si256((__m256i *)(answer + i + 16), r3);
      _mm256_storeu_si256((__m256i *)(answer + i + 24), r4);
    }
  }
  if (size >= 16) {
    for (; i < size - 16; i += 16) {
      __m256i r1 = avx512_pcg32_random_r(&key1);
      __m256i r2 = avx512_pcg32_random_r(&key2);
      _mm256_storeu_si256((__m256i *)(answer + i), r1);
      _mm256_storeu_si256((__m256i *)(answer + i + 8), r2);
    }
  }
  if (size - i >= 8) {
    __m256i r = avx512_pcg32_random_r(&key1);
    _mm256_storeu_si256((__m256i *)(answer + i), r);
    i += 8;
  }
  if (i < size) {
    __m256i r = avx512_pcg32_random_r(&key1);
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i *)buffer, r);
    memcpy(answer + i, buffer, sizeof(uint32_t) * (size - i));
  }
  counter += answer[size - 1];
}

#endif


SEXP C_trand_pcg(SEXP NN, SEXP RR, SEXP M) {
  int N = asInteger(NN);
  const int * rr = INTEGER(RR);
  unsigned int r1 = rr[0];
  unsigned int r2 = rr[2];
  r1 += rr[1];
  r2 += rr[3];
  SEXP ans = PROTECT(allocVector(INTSXP, N));
  unsigned int * ansp = (unsigned int *)INTEGER(ans);
  populateRandom_pcg32(ansp, N, r1, r2);
  UNPROTECT(1);
  return ans;
}

