/* misma_semilla.c - Prueba II.3 (con error a proposito):
   cada hilo tiene su generador, pero todos con la misma semilla,
   asi que todos repiten los mismos puntos.
   Uso: ./misma_semilla <puntos> <hilos> */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));
    unsigned int estado = 2224510u;
    long dentro = 0;

    double t0 = omp_get_wtime();
    #pragma omp parallel for reduction(+:dentro) firstprivate(estado) schedule(static)
    for (long i = 0; i < N; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;
        double y = rand_r(&estado) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0)
            dentro++;
    }
    double t = omp_get_wtime() - t0;

    double pi = 4.0 * dentro / (double)N;
    printf("misma_semilla p=%d dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
