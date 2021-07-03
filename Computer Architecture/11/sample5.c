#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 4096

void verify(const int n, const float *a, const float *b) {
  for (int i = 0; i < n; i++) {
    if (fabs(a[i] - b[i]) > .000001f) {
      fprintf(stderr, "error: a[%d] = %f, b[%d]=%f\n", i, a[i], i, b[i]);
      return;
    }
  }
}

int main(int argc, char const *argv[]) {
  float a[N], b[N], c[N], d[N];
  int i;

  for (i = 0; i < N; i++) {
    a[i] = (float)(i + 1000);
    b[i] = (float)i / 10.f;
  }

  for (i = 0; i < N; i++) {
    c[i] = a[i] * b[i];
  }

#pragma omp parallel for
  for (i = 0; i < N; i++) {
    d[i] = a[i] * b[i];
  }

  printf("(a * b = c)\n");
  for (i = 0; i < 10; i++) {
    printf("%f * %f = %f\n", a[i], b[i], d[i]);
  }

  verify(N, c, d);

  return 0;
}
