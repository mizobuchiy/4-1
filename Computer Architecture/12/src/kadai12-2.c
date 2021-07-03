#include <immintrin.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define UNROLL (4)
#define BLOCKSIZE (32)

void do_block(int n, int si, int sj, int sk, double *A, double *B, double *C) {
  for (int i = si; i < si + BLOCKSIZE; i += UNROLL * 4) {
    for (int j = sj; j < sj + BLOCKSIZE; j++) {
      __m256d c[4];
      for (int x = 0; x < UNROLL; x++) {
        c[x] = _mm256_load_pd(C + i + x * 4 + j * n);
      }
      for (int k = sk; k < sk + BLOCKSIZE; k++) {
        __m256d b = _mm256_broadcast_sd(B + k + j * n);
        for (int x = 0; x < UNROLL; x++) {
          c[x] = _mm256_add_pd(
              c[x], _mm256_mul_pd(_mm256_load_pd(A + n * k + x * 4 + i), b));
        }
      }
      for (int x = 0; x < UNROLL; x++) {
        _mm256_store_pd(C + i + x * 4 + j * n, c[x]);
      }
    }
  }
}
#undef UNROLL

// AVX+LU+CB+OMP
void dgemm_avx_lu_cb_omp(int n, double *A, double *B, double *C) {
  int sj;
#pragma omp parallel for num_threads(8)
  for (sj = 0; sj < n; sj += BLOCKSIZE) {
    for (int si = 0; si < n; si += BLOCKSIZE) {
      for (int sk = 0; sk < n; sk += BLOCKSIZE) {
        do_block(n, si, sj, sk, A, B, C);
      }
    }
  }
}

#undef BLOCKSIZE

int main(int argc, char const *argv[]) {
  clock_t start, stop;
  int n = 256;

  if (argc > 1) {
    n = atoi(argv[1]);
  }
  // fprintf(stdout, "matrix size = %d x %d\n", n, n);
  int nn = n * n;

  double *a = (double *)_mm_malloc(sizeof(double) * nn, 32);
  double *b = (double *)_mm_malloc(sizeof(double) * nn, 32);
  double *c = (double *)_mm_malloc(sizeof(double) * nn, 32);

  for (int i = 0; i < nn; i++) {
    a[i] = (double)(rand() / 4096);
    b[i] = (double)(rand() / 4096);
    c[i] = 0;
  }

  // AVX+LU+CB+OMP
  start = clock();
  dgemm_avx_lu_cb_omp(n, a, b, c);
  stop = clock();
  fprintf(stdout, "%.5f\n", (double)(stop - start) / CLOCKS_PER_SEC);

  _mm_free(a);
  _mm_free(b);
  _mm_free(c);

  return 0;
}
