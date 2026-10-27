/* montecarlo.c - Mi version con hilos (OpenMP) del programa del profesor.
   Uso: ./montecarlo <puntos> [hilos] [semilla]
   Compilar: gcc -O2 -fopenmp montecarlo.c -o montecarlo -lm */
#include <stdio.h>
#include <stdlib.h>   /* rand_r: numeros al azar */
#include <math.h>
#include <omp.h>   /* OpenMP: para usar varios hilos */

#define SEMILLA_BASE 2224510u /* aqui puse mi codigo como semilla */
#define BLOQUE 100000L   /* aqui dividi los dardos en paquetes: 10^8 = 1.000 */

#ifndef PLAN
#define PLAN static            /* aqui elegi repartir en partes iguales */
#endif

/* Formula de rand_r:
   s = 1103515245*s + 12345
   Cada punto usa 6 pasos. */
#define LCG_A 1103515245u
#define LCG_C 12345u
#define PASOS_POR_MUESTRA 6ULL

/* Aqui hice la funcion saltar: avanza k pasos en la lista de
   numeros de una vez, asi cada paquete empieza en su lugar. */
static unsigned int saltar(unsigned int s, unsigned long long k) {
    unsigned int a = LCG_A, c = LCG_C;
    unsigned int A = 1u, C = 0u;
    while (k) {   /* recorre k en binario: pocas vueltas */
        if (k & 1ULL) {
            A = a * A;
            C = a * C + c;
        }
        c = a * c + c;
        a = a * a;
        k >>= 1;
    }
    return A * s + C;   /* estado ya avanzado k pasos */
}

int main(int argc, char *argv[]) {
    /* Aqui leo los datos: dardos, hilos y semilla */
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;   /* dardos a lanzar (100 millones) */
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));          /* aqui se eligen los hilos (o con OMP_NUM_THREADS) */
    unsigned int semilla = (argc > 3) ? (unsigned int)strtoul(argv[3], NULL, 10)
                                      : SEMILLA_BASE;   /* si no se da, usa mi codigo */

    /* Aqui compruebo que saltar da lo mismo que rand_r */
    unsigned int prueba = semilla;
    rand_r(&prueba);
    rand_r(&prueba);
    if (prueba != saltar(semilla, PASOS_POR_MUESTRA)) {
        fprintf(stderr, "error: rand_r de esta libc no usa el LCG esperado\n");
        return 1;
    }
    /* Aviso: con demasiados puntos
       la lista de numeros se repite. */
    if ((unsigned long long)N * PASOS_POR_MUESTRA > 4294967296ULL)
        fprintf(stderr, "aviso: N supera el periodo del generador\n");

    long nbloques = (N + BLOQUE - 1) / BLOQUE;   /* cuantos paquetes hay */
    long dentro = 0;   /* aqui se cuentan los dardos que caen dentro */

    double t0 = omp_get_wtime();   /* aqui arranca el cronometro */
    /* AQUI AGREGUE OPENMP: reparte los paquetes entre los hilos;
       cada uno cuenta en su copia y al final se suman (reduction). */
    #pragma omp parallel for reduction(+:dentro) schedule(PLAN)
    for (long b = 0; b < nbloques; b++) {        /* b: privada */
        /* Aqui cada paquete salta a su lugar de la lista */
        unsigned int estado =                    /* privada */
            saltar(semilla, (unsigned long long)b * BLOQUE * PASOS_POR_MUESTRA);
        long fin = (b + 1) * BLOQUE < N ? (b + 1) * BLOQUE : N;   /* ultimo dardo de este paquete */
        for (long i = b * BLOQUE; i < fin; i++) {                 /* aqui va el mismo ciclo del profesor */
            double x = rand_r(&estado) / (double)RAND_MAX;   /* dardo: posicion x al azar (0 a 1) */
            double y = rand_r(&estado) / (double)RAND_MAX;   /* posicion y al azar (0 a 1) */
            if (x * x + y * y <= 1.0)   /* cayo dentro del circulo? */
                dentro++;               /* en la copia del hilo */
        }
    }
    double t = omp_get_wtime() - t0;   /* aqui paro el cronometro: tiempo que tardo */

    double pi = 4.0 * dentro / (double)N;        /* aqui calculo pi = 4 x dentro / total */
    printf("p=%d N=%ld dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",   /* aqui muestro el resultado */
           omp_get_max_threads(), N, dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
