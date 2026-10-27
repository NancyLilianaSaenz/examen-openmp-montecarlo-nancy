/* montecarlo_secuencial.c - Version de un solo hilo (la del enunciado)
   que ademas mide el tiempo con omp_get_wtime().
   Uso: ./montecarlo_secuencial <puntos> */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;
    unsigned int estado = 2224510u; /* aqui cambie la semilla por mi codigo */
    long dentro = 0;

    double t0 = omp_get_wtime();
    for (long i = 0; i < N; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;   /* dardo: posicion x al azar (0 a 1) */
        double y = rand_r(&estado) / (double)RAND_MAX;   /* posicion y al azar (0 a 1) */
        if (x * x + y * y <= 1.0)   /* cayo dentro del circulo? */
            dentro++;   /* sumar un acierto */
    }
    double t = omp_get_wtime() - t0;                    /* aqui paro el cronometro: este tiempo es T(1) */

    double pi = 4.0 * dentro / (double)N;               /* pi = 4 x aciertos / puntos */
    printf("secuencial N=%ld dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           N, dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
