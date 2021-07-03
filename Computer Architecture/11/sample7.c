#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 100000000

void verify(const int n, const float *a, const float *b) {
  for (int i = 0; i < n; i++) {
    if (fabs(a[i] - b[i]) > .000001f) {
      fprintf(stderr, "error: a[%d] = %f, b[%d]=%f\n", i, a[i], i, b[i]);
      return;
    }
  }
}

int main(int argc, char const *argv[]) {
  float *a, *b;
  int i;
  double dts, dte;
  double ddot = 0, ddot2 = 0;

  a = (float *)malloc(sizeof(float) * N);
  b = (float *)malloc(sizeof(float) * N);

  for (i = 0; i < N; i++) {
    a[i] = (float)(i + 1000);
    b[i] = (float)i / 10.f;
  }

  dts = omp_get_wtime();
  for (i = 0; i < N; i++) {
    ddot += a[i] * b[i];
  }

  dte = omp_get_wtime();
  fprintf(stdout, "     C:");
  fprintf(stdout, "elapsed time = %.20f [sec]\n", dte - dts);

  dts = omp_get_wtime();
#pragma omp parallel for reduction(+ : ddot2)
  for (i = 0; i < N; i++) {
    ddot2 += a[i] * b[i];
  }

  dte = omp_get_wtime();
  fprintf(stdout, "OpenMP:");
  fprintf(stdout, "elapsed time = %.20f [sec]\n", dte - dts);

  if (fabs(ddot - ddot2) > .000001f) {
    fprintf(stderr, "error: \n");
  }

  free(a);
  free(b);

  return 0;
}
