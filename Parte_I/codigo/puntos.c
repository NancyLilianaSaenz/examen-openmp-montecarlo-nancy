/* puntos.c - Genera los primeros puntos (dardos) para dibujarlos.
   Uso: ./puntos <cantidad> > puntos.txt */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, rand, atol */

#define SEMILLA 2224510u   /* mi codigo de estudiante */

int main(int argc, char *argv[]) {
    long n = (argc > 1) ? atol(argv[1]) : 2000;   /* cuantos puntos (2000 si no se indica) */
    unsigned int estado = SEMILLA;   /* el generador empieza en mi semilla */
    long dentro = 0;
    for (long i = 0; i < n; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;
        double y = rand_r(&estado) / (double)RAND_MAX;
        int acierto = (x * x + y * y <= 1.0);   /* 1 si cayo dentro, 0 si no */
        dentro += acierto;
        printf("%.6f %.6f %d\n", x, y, acierto);   /* una linea por punto, para la figura */
    }
    fprintf(stderr, "%ld puntos, %ld dentro, pi ~ %.4f\n", n, dentro, 4.0 * dentro / n);
    return 0;
}
