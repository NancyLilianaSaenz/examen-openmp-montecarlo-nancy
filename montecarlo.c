/* montecarlo.c - Calcula pi con Monte Carlo usando varios hilos (OpenMP).
   Compilar: gcc -O2 -fopenmp montecarlo.c -o montecarlo -lm
   Uso: ./montecarlo <puntos> [hilos] [semilla] */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define SEMILLA_BASE 2224510u  /* mi codigo */
#define BLOQUE 100000L         /* puntos por paquete */

#ifndef PLAN
#define PLAN static
#endif

#define LCG_A 1103515245u
#define LCG_C 12345u
#define PASOS_POR_MUESTRA 6ULL

/* PARTE II: cada paquete salta a su lugar en la lista de numeros al azar */
static unsigned int saltar(unsigned int s, unsigned long long k) {
    unsigned int a = LCG_A, c = LCG_C;
    unsigned int A = 1u, C = 0u;
    while (k) {
        if (k & 1ULL) {
            A = a * A;
            C = a * C + c;
        }
        c = a * c + c;
        a = a * a;
        k >>= 1;
    }
    return A * s + C;
}

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));
    unsigned int semilla = (argc > 3) ? (unsigned int)strtoul(argv[3], NULL, 10)
                                      : SEMILLA_BASE;

    unsigned int prueba = semilla;
    rand_r(&prueba);
    rand_r(&prueba);
    if (prueba != saltar(semilla, PASOS_POR_MUESTRA)) {
        fprintf(stderr, "error: rand_r de esta libc no usa el LCG esperado\n");
        return 1;
    }
    if ((unsigned long long)N * PASOS_POR_MUESTRA > 4294967296ULL)
        fprintf(stderr, "aviso: N supera el periodo del generador\n");

    long nbloques = (N + BLOQUE - 1) / BLOQUE;
    long dentro = 0;

    double t0 = omp_get_wtime();                 /* PARTE III: cronometro */

    /* PARTE II: reparte los paquetes entre los hilos; reduction suma lo de cada uno */
    #pragma omp parallel for reduction(+:dentro) schedule(PLAN)
    for (long b = 0; b < nbloques; b++) {
        unsigned int estado =
            saltar(semilla, (unsigned long long)b * BLOQUE * PASOS_POR_MUESTRA);
        long fin = (b + 1) * BLOQUE < N ? (b + 1) * BLOQUE : N;

        /* CODIGO DEL PROFESOR (Parte I): punto al azar; si cae dentro, se cuenta */
        for (long i = b * BLOQUE; i < fin; i++) {
            double x = rand_r(&estado) / (double)RAND_MAX;
            double y = rand_r(&estado) / (double)RAND_MAX;
            if (x * x + y * y <= 1.0)
                dentro++;
        }
    }

    double t = omp_get_wtime() - t0;             /* PARTE III: tiempo */

    double pi = 4.0 * dentro / (double)N;        /* PARTE I: pi = 4 x dentro / total */
    printf("p=%d N=%ld dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), N, dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
