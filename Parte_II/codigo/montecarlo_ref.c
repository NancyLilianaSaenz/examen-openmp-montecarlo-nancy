/* montecarlo.c -- version secuencial de referencia.
   Uso: ./montecarlo <numero de muestras> */
/* (Programa del enunciado; solo cambia la semilla) */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;
    unsigned int estado = 2224510u; /* semilla = mi codigo */
    long dentro = 0;

    for (long i = 0; i < N; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;   /* dardo: posicion x al azar (0 a 1) */
        double y = rand_r(&estado) / (double)RAND_MAX;   /* posicion y al azar (0 a 1) */
        if (x * x + y * y <= 1.0)   /* cayo dentro del circulo? */
            dentro++;   /* sumar un acierto */
    }

    printf("Estimacion de pi: %.6f\n", 4.0 * dentro / (double)N);   /* pi = 4 x aciertos / puntos */
    return 0;
}
