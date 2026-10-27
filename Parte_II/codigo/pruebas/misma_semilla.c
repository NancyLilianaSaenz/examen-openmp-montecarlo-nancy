/* misma_semilla.c - Prueba II.3 (con error a proposito):
   cada hilo tiene su generador, pero todos con la misma semilla,
   asi que todos repiten los mismos puntos.
   Uso: ./misma_semilla <puntos> <hilos> */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, rand, atol */
#include <math.h>     /* fabs, M_PI */
#include <omp.h>      /* OpenMP */

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;   /* puntos (10^8 si no se indica) */
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));   /* cuantos hilos usar */
    unsigned int estado = 2224510u;   /* la misma semilla para todos */
    long dentro = 0;   /* aciertos */

    double t0 = omp_get_wtime();   /* reloj al empezar */
    #pragma omp parallel for reduction(+:dentro) firstprivate(estado) schedule(static)   /* ERROR: cada hilo copia estado con el MISMO valor */
    for (long i = 0; i < N; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;   /* todos los hilos generan los mismos puntos */
        double y = rand_r(&estado) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0)   /* cayo dentro del circulo */
            dentro++;
    }
    double t = omp_get_wtime() - t0;   /* tiempo del calculo */

    double pi = 4.0 * dentro / (double)N;   /* pi = 4 x aciertos / puntos */
    printf("misma_semilla p=%d dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
