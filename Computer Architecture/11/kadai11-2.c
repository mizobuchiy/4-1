#include <math.h>
#include <stdio.h>
#include <time.h>

void Leibniz(const int n, const int omp) {
  int i;
  clock_t start = clock();

  double pi = 0.0f;
#pragma omp parallel for reduction(+ : pi) if (omp)
  for (i = 0; i < n; i++) {
    pi += (double)(pow(-1, i)) / (double)(2 * i + 1);
  }

  pi *= 4.0f;

  clock_t stop = clock();

  fprintf(stdout, " n=%11d", n);
  fprintf(stdout, " elapsed time =%15.10f [sec], pi=%.16f\n",
          (float)(stop - start) / CLOCKS_PER_SEC, pi);
}

int main(int argc, char const *argv[]) {
  for (int n = 1000000; n <= 1000000000; n *= 10) {
    fprintf(stdout, "     C:");
    Leibniz(n, 0);
    fprintf(stdout, "OpenMP:");
    Leibniz(n, 1);
  }

  return 0;
}

/*
$ ./kadai11-2
C: n=    1000000 elapsed time =   0.0297279991 [sec], pi=31415916535897743
OpenMP: n=    1000000 elapsed time =   0.0413350016 [sec], pi=31415916535897810
C: n=   10000000 elapsed time =   0.3225699961 [sec], pi=3.1415925535897915
OpenMP: n=   10000000 elapsed time =   0.4692240059 [sec], pi=3.1415925535897422
C: n=  100000000 elapsed time =   2.8864200115 [sec], pi=3.1415926435893260
OpenMP: n=  100000000 elapsed time =   4.5534868240 [sec], pi=3.1415926435898172
C: n= 1000000000 elapsed time =  30.1262207031 [sec], pi=3.1415926525880504
OpenMP: n= 1000000000 elapsed time =  45.9534759521 [sec], pi=3.1415926525892104
*/
