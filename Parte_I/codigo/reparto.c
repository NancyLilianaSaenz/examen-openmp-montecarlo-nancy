/* reparto.c - I.3: parte los puntos en p tramos y comprueba que la suma
   de los tramos da lo mismo que contar todo de corrido.
   Uso: ./reparto <puntos> <tramos> */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, rand, atol */

#define SEMILLA 2224510u   /* mi codigo de estudiante */

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;   /* puntos (10^8 si no se indica) */
    int p = (argc > 2) ? atoi(argv[2]) : 4;   /* cuantos tramos (como si fueran hilos) */

    /* 1) Contar todo de corrido, como el programa secuencial */
    unsigned int estado = SEMILLA;
    long total = 0;
    for (long i = 0; i < N; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;
        double y = rand_r(&estado) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0) total++;
    }

    /* 2) Contar los mismos puntos partidos en p tramos */
    estado = SEMILLA;
    long suma = 0;
    for (int k = 0; k < p; k++) {
        long ini = k * (N / p), fin = (k == p - 1) ? N : (k + 1) * (N / p);   /* primer y ultimo punto del tramo */
        long parcial = 0;   /* aciertos solo de este tramo */
        for (long i = ini; i < fin; i++) {
            double x = rand_r(&estado) / (double)RAND_MAX;
            double y = rand_r(&estado) / (double)RAND_MAX;
            if (x * x + y * y <= 1.0) parcial++;
        }
        printf("tramo %d (muestras %9ld a %9ld): %9ld aciertos\n", k, ini, fin - 1, parcial);
        suma += parcial;   /* se suman los aciertos de cada tramo */
    }
    printf("suma de los %d tramos       = %ld   -> pi = %.6f\n", p, suma, 4.0 * suma / N);
    printf("todas de corrido (secuencial) = %ld   -> pi = %.6f\n", total, 4.0 * total / N);
    printf("%s\n", suma == total ? "iguales: mismo estimador" : "DISTINTOS");   /* deben ser iguales */
    return 0;
}
