/* montecarlo.c -- version secuencial de referencia.
   Uso: ./montecarlo <numero de muestras> */
/* (Programa del enunciado; solo cambia la semilla) */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;   /* puntos (10^8 por defecto) */
    unsigned int estado = 2224510u; /* semilla = mi codigo */
    long dentro = 0;                                    /* aciertos */

    for (long i = 0; i < N; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;  /* azar entre 0 y 1 */
        double y = rand_r(&estado) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0)                       /* dentro del circulo */
            dentro++;
    }

    printf("Estimacion de pi: %.6f\n", 4.0 * dentro / (double)N);   /* pi = 4 x aciertos / puntos */
    return 0;
}
