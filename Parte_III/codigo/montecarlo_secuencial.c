/* montecarlo_secuencial.c - Version de un solo hilo (la del enunciado)
   que ademas mide el tiempo con omp_get_wtime().
   Uso: ./montecarlo_secuencial <puntos> */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, atol */
#include <math.h>     /* fabs, M_PI */
#include <omp.h>      /* omp_get_wtime */

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;   /* puntos (10^8 por defecto) */
    unsigned int estado = 2224510u; /* semilla = mi codigo */
    long dentro = 0;                                    /* aciertos */

    double t0 = omp_get_wtime();                        /* reloj al empezar */
    for (long i = 0; i < N; i++) {                      /* un solo hilo */
        double x = rand_r(&estado) / (double)RAND_MAX;  /* azar entre 0 y 1 */
        double y = rand_r(&estado) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0)                       /* dentro del circulo */
            dentro++;
    }
    double t = omp_get_wtime() - t0;                    /* este tiempo es T(1) */

    double pi = 4.0 * dentro / (double)N;               /* pi = 4 x aciertos / puntos */
    printf("secuencial N=%ld dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           N, dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
