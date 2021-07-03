#include <stdio.h>

int main(int argc, char const *argv[]) {
#pragma omp parallel
  { printf("hello openMP!\n"); }
  return 0;
}
