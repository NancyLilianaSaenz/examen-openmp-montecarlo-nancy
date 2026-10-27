/* default_none.c - Prueba II.4.
   Es la misma region del programa final, pero con default(none):
   el compilador da un error por cada variable compartida.
   No compila a proposito. */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, rand, atol */
#include <math.h>     /* fabs, M_PI */
#include <omp.h>      /* OpenMP */

#define SEMILLA_BASE 2224510u /* mi codigo de estudiante */
#define BLOQUE 100000L         /* puntos por paquete */

#ifndef PLAN
#define PLAN static            /* reparto de paquetes (o dynamic) */
#endif

/* rand_r usa la formula s = 1103515245*s + 12345.
   Cada llamada da 3 pasos de la formula y cada punto
   (x, y) hace 2 llamadas: 6 pasos por punto. */
#define LCG_A 1103515245u
#define LCG_C 12345u
#define PASOS_POR_MUESTRA 6ULL

/* Da el estado del generador tras avanzar k pasos, sin hacerlos uno por uno. */
static unsigned int saltar(unsigned int s, unsigned long long k) {
    unsigned int a = LCG_A, c = LCG_C;  /* formula para 1, 2, 4, 8... pasos */
    unsigned int A = 1u, C = 0u;        /* formula acumulada */
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
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;   /* puntos (10^8 si no se indica) */
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));   /* cuantos hilos usar */
    unsigned int semilla = (argc > 3) ? (unsigned int)strtoul(argv[3], NULL, 10)
                                      : SEMILLA_BASE;

    /* Revision: saltar debe dar lo mismo que rand_r */
    unsigned int prueba = semilla;
    rand_r(&prueba);
    rand_r(&prueba);
    if (prueba != saltar(semilla, PASOS_POR_MUESTRA)) {
        fprintf(stderr, "error: rand_r de esta libc no usa el LCG esperado\n");
        return 1;
    }
    /* Aviso: con demasiados puntos la lista de numeros se
       repetiria y los paquetes usarian numeros iguales. */
    if ((unsigned long long)N * PASOS_POR_MUESTRA > 4294967296ULL)
        fprintf(stderr, "aviso: N supera el periodo del generador\n");

    long nbloques = (N + BLOQUE - 1) / BLOQUE;
    long dentro = 0;   /* aciertos */

    double t0 = omp_get_wtime();   /* reloj al empezar */
    #pragma omp parallel for default(none) reduction(+:dentro) schedule(PLAN)   /* obliga a declarar todo */
    for (long b = 0; b < nbloques; b++) {
        unsigned int estado =
            saltar(semilla, (unsigned long long)b * BLOQUE * PASOS_POR_MUESTRA);
        long fin = (b + 1) * BLOQUE < N ? (b + 1) * BLOQUE : N;
        for (long i = b * BLOQUE; i < fin; i++) {
            double x = rand_r(&estado) / (double)RAND_MAX;
            double y = rand_r(&estado) / (double)RAND_MAX;
            if (x * x + y * y <= 1.0)
                dentro++;
        }
    }
    double t = omp_get_wtime() - t0;   /* tiempo del calculo */

    double pi = 4.0 * dentro / (double)N;   /* pi = 4 x aciertos / puntos */
    printf("default_none p=%d N=%ld dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), N, dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
