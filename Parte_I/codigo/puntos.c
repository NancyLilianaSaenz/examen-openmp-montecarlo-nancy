/* puntos.c - Genera los primeros puntos (dardos) para dibujarlos.
   Uso: ./puntos <cantidad> > puntos.txt */
#include <stdio.h>
#include <stdlib.h>

#define SEMILLA 2224510u   /* semilla = mi codigo */

int main(int argc, char *argv[]) {
    long n = (argc > 1) ? atol(argv[1]) : 2000;
    unsigned int estado = SEMILLA;
    long dentro = 0;
    for (long i = 0; i < n; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;
        double y = rand_r(&estado) / (double)RAND_MAX;
        int acierto = (x * x + y * y <= 1.0);   /* 1 si cayo dentro, 0 si no */
        dentro += acierto;
        printf("%.6f %.6f %d\n", x, y, acierto);
    }
    fprintf(stderr, "%ld puntos, %ld dentro, pi ~ %.4f\n", n, dentro, 4.0 * dentro / n);
    return 0;
}
