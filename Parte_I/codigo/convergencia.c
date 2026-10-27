/* convergencia.c -- I.1: como baja el error al aumentar N.
   Para cada N calcula pi con la semilla asignada y, ademas, el error medio
   (raiz del error cuadratico medio) de 30 corridas independientes, y lo
   compara con el error tipico teorico 4*sqrt(q(1-q)/N), q = pi/4.
   Las 30 corridas usan tramos distintos y sin solapamiento de la secuencia
   de rand_r (se salta al inicio de cada tramo con la funcion saltar).
   Uso: ./convergencia */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SEMILLA 2224510u   /* codigo de estudiante */
#define CORRIDAS 30

/* Avanza el estado de rand_r k pasos del generador (3 pasos por llamada). */
static unsigned int saltar(unsigned int s, unsigned long long k) {
    unsigned int a = 1103515245u, c = 12345u, A = 1u, C = 0u;
    while (k) {
        if (k & 1ULL) { A = a * A; C = a * C + c; }
        c = a * c + c;
        a = a * a;
        k >>= 1;
    }
    return A * s + C;
}

static double estimar(long n, unsigned int semilla) {
    unsigned int estado = semilla;
    long dentro = 0;
    for (long i = 0; i < n; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;
        double y = rand_r(&estado) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0)
            dentro++;
    }
    return 4.0 * dentro / (double)n;
}

int main(void) {
    const double q = M_PI / 4.0;
    printf("%10s  %10s  %12s  %14s  %13s\n", "N", "pi (mia)", "error (mia)",
           "error medio 30", "teorico");
    for (long n = 100; n <= 100000000L; n *= 10) {
        double pi = estimar(n, SEMILLA);
        printf("%10ld  %10.6f  %12.2e  ", n, pi, fabs(pi - M_PI));
        /* 31 tramos de n muestras (6n pasos cada uno) deben caber en 2^32 pasos */
        if (31ULL * 6ULL * (unsigned long long)n <= 4294967296ULL) {
            double suma2 = 0.0;
            for (int k = 1; k <= CORRIDAS; k++) {
                unsigned int s = saltar(SEMILLA, (unsigned long long)k * 6ULL * n);
                double e = estimar(n, s) - M_PI;
                suma2 += e * e;
            }
            printf("%14.2e  ", sqrt(suma2 / CORRIDAS));
        } else {
            printf("%14s  ", "-");
        }
        printf("%13.2e\n", 4.0 * sqrt(q * (1.0 - q) / n));
    }
    return 0;
}
