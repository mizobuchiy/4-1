#include <stdio.h>

int main(int argc, char const *argv[]) {
  int i = 0, j = 10, k = 20;

#pragma omp parallel private(i)
  { i = j + k; }

  printf("i=%d, j=%d, k=%d\n", i, j, k);

  return 0;
}
