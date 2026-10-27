/* sobrecosto.c - Mide cuanto cuesta crear los hilos y hacer la reduccion.
   Uso: ./sobrecosto <hilos> */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, rand, atol */
#include <omp.h>      /* OpenMP */

#define REPETICIONES 20000   /* se repite muchas veces y se promedia */

int main(int argc, char *argv[]) {
    int p = (argc > 1) ? atoi(argv[1]) : 4;
    omp_set_num_threads(p);
    volatile long basura = 0;   /* evita que el compilador borre las regiones vacias */

    /* La primera region crea los hilos */
    double t0 = omp_get_wtime();   /* reloj al empezar */
    #pragma omp parallel
    { if (omp_get_thread_num() < 0) basura++; }
    double creacion = omp_get_wtime() - t0;   /* costo de crear los hilos */

    /* Las siguientes reutilizan los hilos ya creados */
    t0 = omp_get_wtime();
    for (int r = 0; r < REPETICIONES; r++) {
        #pragma omp parallel
        { if (omp_get_thread_num() < 0) basura++; }
    }
    double region = (omp_get_wtime() - t0) / REPETICIONES;   /* costo de abrir y cerrar una region */

    t0 = omp_get_wtime();
    for (int r = 0; r < REPETICIONES; r++) {
        long s = 0;
        #pragma omp parallel for reduction(+:s)   /* reduccion casi sin trabajo */
        for (int i = 0; i < p; i++)
            s += i;
        basura += s;
    }
    double reduccion = (omp_get_wtime() - t0) / REPETICIONES;   /* costo de la reduccion */

    printf("p=%d  crear hilos: %7.1f us   region: %6.2f us   parallel for + reduction: %6.2f us\n",
           p, creacion * 1e6, region * 1e6, reduccion * 1e6);
    return 0;
}
