/* estado_compartido.c - Prueba II.3 (con error a proposito):
   todos los hilos usan el mismo generador y se pisan.
   Uso: ./estado_compartido <puntos> <hilos> */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, rand, atol */
#include <math.h>     /* fabs, M_PI */
#include <omp.h>      /* OpenMP */

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;   /* puntos (10^8 si no se indica) */
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));   /* cuantos hilos usar */
    unsigned int estado = 2224510u;   /* ERROR: declarada fuera, la comparten todos los hilos */
    long dentro = 0;   /* aciertos (con reduction, asi que este no es el error) */

    double t0 = omp_get_wtime();   /* reloj al empezar */
    #pragma omp parallel for reduction(+:dentro)
    for (long i = 0; i < N; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;   /* varios hilos leen y escriben estado a la vez */
        double y = rand_r(&estado) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0)   /* cayo dentro del circulo */
            dentro++;
    }
    double t = omp_get_wtime() - t0;   /* tiempo del calculo */

    double pi = 4.0 * dentro / (double)N;   /* pi = 4 x aciertos / puntos */
    printf("estado_compartido p=%d dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
