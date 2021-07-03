#include <stdio.h>

int main(int argc, char const *argv[]) {
#pragma omp parallel num_threads(10)
  { printf("hello openMP!\n"); }
  return 0;
}
