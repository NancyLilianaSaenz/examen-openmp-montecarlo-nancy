/* convergencia.c - I.1: muestra como baja el error al usar mas puntos.
   Compara mi corrida y el promedio de 30 corridas con la formula.
   Uso: ./convergencia */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, rand, atol */
#include <math.h>     /* fabs, M_PI */

#define SEMILLA 2224510u   /* mi codigo de estudiante */
#define CORRIDAS 30   /* corridas para sacar el error medio */

/* Da el estado del generador tras avanzar k pasos */
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

static double estimar(long n, unsigned int semilla) {   /* calcula pi con n puntos */
    unsigned int estado = semilla;
    long dentro = 0;
    for (long i = 0; i < n; i++) {
        double x = rand_r(&estado) / (double)RAND_MAX;
        double y = rand_r(&estado) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0)   /* cayo dentro del circulo */
            dentro++;
    }
    return 4.0 * dentro / (double)n;
}

int main(void) {
    const double q = M_PI / 4.0;   /* probabilidad de caer dentro */
    printf("%10s  %10s  %12s  %14s  %13s\n", "N", "pi (mia)", "error (mia)",
           "error medio 30", "teorico");
    for (long n = 100; n <= 100000000L; n *= 10) {   /* N = 100, 1000, ..., 10^8 */
        double pi = estimar(n, SEMILLA);
        printf("%10ld  %10.6f  %12.2e  ", n, pi, fabs(pi - M_PI));
        /* Solo si los 31 tramos caben en la lista sin repetirse */
        if (31ULL * 6ULL * (unsigned long long)n <= 4294967296ULL) {
            double suma2 = 0.0;
            for (int k = 1; k <= CORRIDAS; k++) {
                unsigned int s = saltar(SEMILLA, (unsigned long long)k * 6ULL * n);   /* tramo k: no se repite con otros */
                double e = estimar(n, s) - M_PI;
                suma2 += e * e;
            }
            printf("%14.2e  ", sqrt(suma2 / CORRIDAS));   /* error medio de las 30 corridas */
        } else {
            printf("%14s  ", "-");
        }
        printf("%13.2e\n", 4.0 * sqrt(q * (1.0 - q) / n));   /* formula: 1,64 / raiz(N) */
    }
    return 0;
}
