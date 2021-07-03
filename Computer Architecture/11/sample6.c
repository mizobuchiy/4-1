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
  float *a, *b, *c, *d;
  int i;
  double dts, dte;

  a = (float *)malloc(sizeof(float) * N);
  b = (float *)malloc(sizeof(float) * N);
  c = (float *)malloc(sizeof(float) * N);
  d = (float *)malloc(sizeof(float) * N);

  for (i = 0; i < N; i++) {
    a[i] = (float)(i + 1000);
    b[i] = (float)i / 10.f;
  }

  dts = omp_get_wtime();
  for (i = 0; i < N; i++) {
    c[i] = a[i] * b[i];
  }

  dte = omp_get_wtime();
  fprintf(stdout, "     C:");
  fprintf(stdout, "elapsed time = %.20f [sec]\n", dte - dts);

  dts = omp_get_wtime();
#pragma omp parallel for
  for (i = 0; i < N; i++) {
    d[i] = a[i] * b[i];
  }

  dte = omp_get_wtime();
  fprintf(stdout, "OpenMP:");
  fprintf(stdout, "elapsed time = %.20f [sec]\n", dte - dts);

  printf("(a * b = c)\n");
  for (i = 0; i < 10; i++) {
    printf("%f * %f = %f\n", a[i], b[i], d[i]);
  }

  verify(N, c, d);

  free(a);
  free(b);
  free(c);
  free(d);

  return 0;
}
