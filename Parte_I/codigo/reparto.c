/* reparto.c -- I.3: repartir las N muestras en p tramos y sumar los aciertos
   parciales da exactamente el mismo conteo que hacerlas todas de corrido.
   Uso: ./reparto <N> <p> */
#include <stdio.h>
#include <stdlib.h>

#define SEMILLA 2224510u   /* codigo de estudiante */

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;
    int p = (argc > 2) ? atoi(argv[2]) : 4;

    /* Todas las muestras de corrido (como el programa secuencial). */
    unsigned int estado = SEMILLA;
    long total = 0;
    for (long i = 0; i < N; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;
        double y = rand_r(&estado) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0) total++;
    }

    /* Las mismas muestras partidas en p tramos consecutivos de N/p. */
    estado = SEMILLA;
    long suma = 0;
    for (int k = 0; k < p; k++) {
        long ini = k * (N / p), fin = (k == p - 1) ? N : (k + 1) * (N / p);
        long parcial = 0;
        for (long i = ini; i < fin; i++) {
            double x = rand_r(&estado) / (double)RAND_MAX;
            double y = rand_r(&estado) / (double)RAND_MAX;
            if (x * x + y * y <= 1.0) parcial++;
        }
        printf("tramo %d (muestras %9ld a %9ld): %9ld aciertos\n", k, ini, fin - 1, parcial);
        suma += parcial;
    }
    printf("suma de los %d tramos       = %ld   -> pi = %.6f\n", p, suma, 4.0 * suma / N);
    printf("todas de corrido (secuencial) = %ld   -> pi = %.6f\n", total, 4.0 * total / N);
    printf("%s\n", suma == total ? "iguales: mismo estimador" : "DISTINTOS");
    return 0;
}
