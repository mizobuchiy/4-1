#include <immintrin.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Normal
void dgemm(int n, double *A, double *B, double *C) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      double cij = C[i + j * n];
      for (int k = 0; k < n; k++) {
        cij += A[i + k * n] * B[k + j * n];
      }
      C[i + j * n] = cij;
    }
  }
}

// AVX
void dgemm_avx(int n, double *A, double *B, double *C) {
  for (int i = 0; i < n; i += 4) {
    for (int j = 0; j < n; j++) {
      __m256d c0 = _mm256_load_pd(C + i + j * n);
      for (int k = 0; k < n; k++) {
        c0 = _mm256_add_pd(c0,
                           _mm256_mul_pd(_mm256_load_pd(A + i + k * n),
                                         _mm256_broadcast_sd(B + k + j * n)));
      }
      _mm256_store_pd(C + i + j * n, c0);
    }
  }
}

#define UNROLL (4)

// AVX+LU
void dgemm_avx_lu(int n, double *A, double *B, double *C) {
  for (int i = 0; i < n; i += UNROLL * 4) {
    for (int j = 0; j < n; j++) {
      __m256d c[4];
      for (int x = 0; x < UNROLL; x++) {
        c[x] = _mm256_load_pd(C + i + x * 4 + j * n);
      }
      for (int k = 0; k < n; k++) {
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

// AVX+LU+CB
void dgemm_avx_lu_cb(int n, double *A, double *B, double *C) {
  for (int sj = 0; sj < n; sj += BLOCKSIZE) {
    for (int si = 0; si < n; si += BLOCKSIZE) {
      for (int sk = 0; sk < n; sk += BLOCKSIZE) {
        do_block(n, si, sj, sk, A, B, C);
      }
    }
  }
}

// AVX+LU+CB+OMP
void dgemm_avx_lu_cb_omp(int n, double *A, double *B, double *C) {
  int sj;
#pragma omp parallel for
  for (sj = 0; sj < n; sj += BLOCKSIZE) {
    for (int si = 0; si < n; si += BLOCKSIZE) {
      for (int sk = 0; sk < n; sk += BLOCKSIZE) {
        do_block(n, si, sj, sk, A, B, C);
      }
    }
  }
}

#undef BLOCKSIZE

void verify(const int n, const double *a, const double *b) {
  for (int i = 0; i < n; i++) {
    if (fabs(a[i] - b[i]) > 1e-10) {
      fprintf(stderr, "error: a[%d] = %f, b[%d]=%f\n", i, a[i], i, b[i]);
      return;
    }
  }
}

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
  double *d = (double *)_mm_malloc(sizeof(double) * nn, 32);

  for (int i = 0; i < nn; i++) {
    a[i] = (double)(rand() / 4096);
    b[i] = (double)(rand() / 4096);
    c[i] = 0;
    d[i] = 0;
  }

  // Normal
  start = clock();
  dgemm(n, a, b, c);
  stop = clock();
  // fprintf(stdout, "[Normal]        elapsed time = %.5f [sec]\n",
  //         (double)(stop - start) / CLOCKS_PER_SEC);
  fprintf(stdout, "%.5f\n", (double)(stop - start) / CLOCKS_PER_SEC);

  // AVX
  start = clock();
  dgemm_avx(n, a, b, d);
  stop = clock();
  verify(nn, c, d);
  // fprintf(stdout, "[AVX]           elapsed time = %.5f [sec]\n",
  //         (double)(stop - start) / CLOCKS_PER_SEC);
  fprintf(stdout, "%.5f\n", (double)(stop - start) / CLOCKS_PER_SEC);
  memset(d, 0, nn * sizeof(d));

  // AVX+LU
  start = clock();
  dgemm_avx_lu(n, a, b, d);
  stop = clock();
  verify(nn, c, d);
  // fprintf(stdout, "[AVX+LU]        elapsed time = %.5f [sec]\n",
  //         (double)(stop - start) / CLOCKS_PER_SEC);
  fprintf(stdout, "%.5f\n", (double)(stop - start) / CLOCKS_PER_SEC);
  memset(d, 0, nn * sizeof(d));

  // AVX+LU+CB
  start = clock();
  dgemm_avx_lu_cb(n, a, b, d);
  stop = clock();
  verify(nn, c, d);
  // fprintf(stdout, "[AVX+LU+CB]     elapsed time = %.5f [sec]\n",
  //         (double)(stop - start) / CLOCKS_PER_SEC);
  fprintf(stdout, "%.5f\n", (double)(stop - start) / CLOCKS_PER_SEC);
  memset(d, 0, nn * sizeof(d));

  // AVX+LU+CB+OMP
  start = clock();
  dgemm_avx_lu_cb_omp(n, a, b, d);
  stop = clock();
  verify(nn, c, d);
  // fprintf(stdout, "[AVX+LU+CB+OMP] elapsed time = %.5f [sec]\n",
  //         (double)(stop - start) / CLOCKS_PER_SEC);
  fprintf(stdout, "%.5f\n", (double)(stop - start) / CLOCKS_PER_SEC);

  _mm_free(a);
  _mm_free(b);
  _mm_free(c);
  _mm_free(d);

  return 0;
}
