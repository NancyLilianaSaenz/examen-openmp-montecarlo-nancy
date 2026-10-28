/* montecarlo.c - Calcula pi con el metodo de Monte Carlo usando varios hilos (OpenMP).
   Nancy Liliana Saenz Moreno - codigo 2224510
   Uso: ./montecarlo <puntos> [hilos] [semilla]
   Compilar: gcc -O2 -fopenmp montecarlo.c -o montecarlo -lm */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

/* ===== PARTE II: datos para repartir el trabajo entre hilos ===== */
#define SEMILLA_BASE 2224510u  /* semilla = mi codigo de estudiante */
#define BLOQUE 100000L         /* puntos por paquete: 10^8 puntos = 1.000 paquetes */

#ifndef PLAN
#define PLAN static            /* los paquetes se reparten en partes iguales */
#endif

/* rand_r usa la formula s = 1103515245*s + 12345; cada punto gasta 6 pasos. */
#define LCG_A 1103515245u
#define LCG_C 12345u
#define PASOS_POR_MUESTRA 6ULL

/* ===== PARTE II: funcion saltar =====
   Da el estado del generador despues de k pasos, sin hacerlos uno por uno.
   Asi cada paquete empieza en su propio lugar de la lista de numeros al azar
   y ningun hilo repite los numeros de otro. */
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
    /* Datos: cuantos puntos, cuantos hilos y que semilla */
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));
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
    if ((unsigned long long)N * PASOS_POR_MUESTRA > 4294967296ULL)
        fprintf(stderr, "aviso: N supera el periodo del generador\n");

    long nbloques = (N + BLOQUE - 1) / BLOQUE;
    long dentro = 0;                             /* puntos que caen dentro del circulo */

    /* ===== PARTE III: medir el tiempo (inicio del cronometro) ===== */
    double t0 = omp_get_wtime();

    /* ===== PARTE II: lo que agregue con OpenMP =====
       parallel for -> reparte los paquetes entre los hilos
       reduction    -> cada hilo cuenta en su propia copia de "dentro"
                       y al final se suman, para que no se pisen */
    #pragma omp parallel for reduction(+:dentro) schedule(PLAN)
    for (long b = 0; b < nbloques; b++) {
        unsigned int estado =
            saltar(semilla, (unsigned long long)b * BLOQUE * PASOS_POR_MUESTRA);
        long fin = (b + 1) * BLOQUE < N ? (b + 1) * BLOQUE : N;

        /* ===== CODIGO DEL PROFESOR (Parte I: el metodo de Monte Carlo) =====
           un punto (x, y) al azar entre 0 y 1; si cae dentro del circulo, se cuenta */
        for (long i = b * BLOQUE; i < fin; i++) {
            double x = rand_r(&estado) / (double)RAND_MAX;
            double y = rand_r(&estado) / (double)RAND_MAX;
            if (x * x + y * y <= 1.0)
                dentro++;
        }
    }

    /* ===== PARTE III: medir el tiempo (fin del cronometro) ===== */
    double t = omp_get_wtime() - t0;

    /* ===== PARTE I: resultado, pi = 4 x dentro / total ===== */
    double pi = 4.0 * dentro / (double)N;
    printf("p=%d N=%ld dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), N, dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
