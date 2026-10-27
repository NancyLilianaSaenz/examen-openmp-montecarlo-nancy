/* montecarlo.c - Calcula pi con Monte Carlo usando varios hilos (OpenMP).
   Uso: ./montecarlo <puntos> [hilos] [semilla]
   Compilar: gcc -O2 -fopenmp montecarlo.c -o montecarlo -lm */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, atol, atoi */
#include <math.h>     /* fabs, M_PI */
#include <omp.h>      /* OpenMP */

#define SEMILLA_BASE 2224510u /* semilla = mi codigo */
#define BLOQUE 100000L         /* puntos por paquete */

#ifndef PLAN
#define PLAN static            /* reparto en partes iguales */
#endif

/* Formula de rand_r:
   s = 1103515245*s + 12345
   Cada punto usa 6 pasos. */
#define LCG_A 1103515245u      /* multiplicador */
#define LCG_C 12345u           /* sumando */
#define PASOS_POR_MUESTRA 6ULL /* pasos por punto */

/* Salta k pasos en la lista de numeros de una vez.
   Asi cada paquete empieza en su lugar. */
static unsigned int saltar(unsigned int s, unsigned long long k) {
    unsigned int a = LCG_A, c = LCG_C;  /* salto de 1, 2, 4, 8... pasos */
    unsigned int A = 1u, C = 0u;        /* salto acumulado */
    while (k) {                         /* k en binario: pocas vueltas */
        if (k & 1ULL) {                 /* si el bit es 1, sumar ese salto */
            A = a * A;
            C = a * C + c;
        }
        c = a * c + c;                  /* doblar el salto */
        a = a * a;
        k >>= 1;                        /* siguiente bit */
    }
    return A * s + C;                   /* estado ya avanzado */
}

int main(int argc, char *argv[]) {
    /* Datos: puntos, hilos y semilla */
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;          /* puntos (10^8 por defecto) */
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));          /* hilos (o con OMP_NUM_THREADS) */
    unsigned int semilla = (argc > 3) ? (unsigned int)strtoul(argv[3], NULL, 10)
                                      : SEMILLA_BASE;          /* semilla (mi codigo por defecto) */

    /* Comprobar que saltar = rand_r */
    unsigned int prueba = semilla;
    rand_r(&prueba);                    /* un punto con rand_r */
    rand_r(&prueba);
    if (prueba != saltar(semilla, PASOS_POR_MUESTRA)) {   /* si no coincide, parar */
        fprintf(stderr, "error: rand_r de esta libc no usa el LCG esperado\n");
        return 1;
    }
    /* Aviso: con demasiados puntos
       la lista de numeros se repite. */
    if ((unsigned long long)N * PASOS_POR_MUESTRA > 4294967296ULL)   /* la lista tiene 2^32 pasos */
        fprintf(stderr, "aviso: N supera el periodo del generador\n");

    long nbloques = (N + BLOQUE - 1) / BLOQUE;   /* cantidad de paquetes */
    long dentro = 0;                             /* aciertos */

    double t0 = omp_get_wtime();                 /* reloj al empezar */
    /* Reparte los paquetes entre los hilos; cada uno
       cuenta en su copia y al final se suman (reduction). */
    #pragma omp parallel for reduction(+:dentro) schedule(PLAN)
    for (long b = 0; b < nbloques; b++) {        /* b: privada */
        /* El paquete b salta a su lugar de la lista */
        unsigned int estado =                    /* privada */
            saltar(semilla, (unsigned long long)b * BLOQUE * PASOS_POR_MUESTRA);
        long fin = (b + 1) * BLOQUE < N ? (b + 1) * BLOQUE : N;   /* ultimo punto del paquete */
        for (long i = b * BLOQUE; i < fin; i++) {                 /* el mismo ciclo del enunciado */
            double x = rand_r(&estado) / (double)RAND_MAX;        /* azar entre 0 y 1 */
            double y = rand_r(&estado) / (double)RAND_MAX;
            if (x * x + y * y <= 1.0)   /* dentro del circulo */
                dentro++;               /* en la copia del hilo */
        }
    }
    double t = omp_get_wtime() - t0;             /* tiempo */

    double pi = 4.0 * dentro / (double)N;        /* pi = 4 x aciertos / puntos */
    printf("p=%d N=%ld dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), N, dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
