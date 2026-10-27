/* rand_global.c - Prueba II.3 (con error a proposito):
   todos los hilos usan rand(), que tiene un solo estado para todos.
   Uso: ./rand_global <puntos> <hilos> */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, rand, atol */
#include <math.h>     /* fabs, M_PI */
#include <omp.h>      /* OpenMP */

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;   /* puntos (10^8 por defecto) */
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));   /* hilos */
    srand(2224510u);   /* un solo generador para todos */
    long dentro = 0;   /* aciertos (con reduction) */

    double t0 = omp_get_wtime();   /* reloj al empezar */
    #pragma omp parallel for reduction(+:dentro)
    for (long i = 0; i < N; i++) {
        double x = rand() / (double)RAND_MAX;   /* ERROR: rand() compartido */
        double y = rand() / (double)RAND_MAX;   /* se turnan: lento y cambia */
        if (x * x + y * y <= 1.0)   /* dentro del circulo */
            dentro++;
    }
    double t = omp_get_wtime() - t0;   /* tiempo */

    double pi = 4.0 * dentro / (double)N;   /* pi = 4 x aciertos / puntos */
    printf("rand_global p=%d dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
