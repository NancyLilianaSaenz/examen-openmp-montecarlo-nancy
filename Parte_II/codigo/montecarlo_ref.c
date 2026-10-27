/* montecarlo.c -- version secuencial de referencia.
   Uso: ./montecarlo <numero de muestras> */
/* (Programa del enunciado, sin cambios salvo la semilla.) */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;   /* puntos (10^8 si no se indica) */
    unsigned int estado = 2224510u; /* mi codigo (el enunciado traia 20261018) */
    long dentro = 0;                                    /* aciertos */

    for (long i = 0; i < N; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;  /* numero al azar entre 0 y 1 */
        double y = rand_r(&estado) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0)                       /* cayo dentro del circulo */
            dentro++;
    }

    printf("Estimacion de pi: %.6f\n", 4.0 * dentro / (double)N);   /* pi = 4 x aciertos / puntos */
    return 0;
}
