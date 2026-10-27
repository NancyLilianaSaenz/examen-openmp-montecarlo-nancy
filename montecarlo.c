/* montecarlo.c - Calcula pi con Monte Carlo usando varios hilos (OpenMP).
   Uso: ./montecarlo <puntos> [hilos] [semilla]
   Compilar: gcc -O2 -fopenmp montecarlo.c -o montecarlo -lm */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* rand_r, atol, atoi */
#include <math.h>     /* fabs, M_PI (para calcular el error) */
#include <omp.h>      /* OpenMP: hilos y omp_get_wtime */

#define SEMILLA_BASE 2224510u /* mi codigo de estudiante (el profesor pidio usarlo como semilla) */
#define BLOQUE 100000L         /* puntos por paquete: 10^8 puntos = 1000 paquetes */

#ifndef PLAN
#define PLAN static            /* static: los paquetes se reparten en partes iguales (o dynamic) */
#endif

/* rand_r usa la formula s = 1103515245*s + 12345.
   Cada llamada da 3 pasos de la formula y cada punto
   (x, y) hace 2 llamadas: 6 pasos por punto. */
#define LCG_A 1103515245u      /* numero que multiplica en la formula */
#define LCG_C 12345u           /* numero que se suma en la formula */
#define PASOS_POR_MUESTRA 6ULL /* pasos de la formula que gasta cada punto */

/* Da el estado del generador tras avanzar k pasos, sin hacerlos uno por uno.
   Sirve para que cada paquete empiece justo en su parte de la lista de numeros. */
static unsigned int saltar(unsigned int s, unsigned long long k) {
    unsigned int a = LCG_A, c = LCG_C;  /* formula para 1, 2, 4, 8... pasos */
    unsigned int A = 1u, C = 0u;        /* formula acumulada (empieza en "no avanzar") */
    while (k) {                         /* se recorre k en binario: pocas vueltas */
        if (k & 1ULL) {                 /* si este bit de k esta en 1, se suma ese salto */
            A = a * A;
            C = a * C + c;
        }
        c = a * c + c;                  /* duplica el tamano del salto */
        a = a * a;
        k >>= 1;                        /* pasa al siguiente bit de k */
    }
    return A * s + C;                   /* estado despues de avanzar k pasos */
}

int main(int argc, char *argv[]) {
    /* Datos: puntos, hilos y semilla */
    long N = (argc > 1) ? atol(argv[1]) : 100000000L;          /* puntos (10^8 si no se indica) */
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));          /* hilos (o con OMP_NUM_THREADS) */
    unsigned int semilla = (argc > 3) ? (unsigned int)strtoul(argv[3], NULL, 10)
                                      : SEMILLA_BASE;          /* semilla (mi codigo si no se indica) */

    /* Revision: saltar debe dar lo mismo que rand_r */
    unsigned int prueba = semilla;
    rand_r(&prueba);                    /* avanzar un punto con rand_r de verdad (2 llamadas) */
    rand_r(&prueba);
    if (prueba != saltar(semilla, PASOS_POR_MUESTRA)) {   /* si no coincide, el resultado seria falso */
        fprintf(stderr, "error: rand_r de esta libc no usa el LCG esperado\n");
        return 1;
    }
    /* Aviso: con demasiados puntos la lista de numeros se
       repetiria y los paquetes usarian numeros iguales. */
    if ((unsigned long long)N * PASOS_POR_MUESTRA > 4294967296ULL)   /* la lista tiene 2^32 pasos */
        fprintf(stderr, "aviso: N supera el periodo del generador\n");

    long nbloques = (N + BLOQUE - 1) / BLOQUE;   /* cantidad de paquetes */
    long dentro = 0;                             /* aciertos */

    double t0 = omp_get_wtime();                 /* reloj al empezar (tiempo real) */
    /* Los hilos se reparten los paquetes. Cada uno cuenta en su propia
       copia de dentro y al final OpenMP las suma (reduction). */
    #pragma omp parallel for reduction(+:dentro) schedule(PLAN)
    for (long b = 0; b < nbloques; b++) {        /* b es privada: cada hilo tiene la suya */
        /* El paquete b empieza en su lugar de la lista de numeros */
        unsigned int estado =                    /* privada: declarada dentro del ciclo */
            saltar(semilla, (unsigned long long)b * BLOQUE * PASOS_POR_MUESTRA);
        long fin = (b + 1) * BLOQUE < N ? (b + 1) * BLOQUE : N;   /* ultimo punto del paquete */
        for (long i = b * BLOQUE; i < fin; i++) {                 /* el mismo ciclo del enunciado */
            double x = rand_r(&estado) / (double)RAND_MAX;        /* numero al azar entre 0 y 1 */
            double y = rand_r(&estado) / (double)RAND_MAX;
            if (x * x + y * y <= 1.0)   /* cayo dentro del circulo */
                dentro++;               /* suma en la copia de este hilo */
        }
    }
    double t = omp_get_wtime() - t0;             /* tiempo que tardo el calculo */

    double pi = 4.0 * dentro / (double)N;        /* pi = 4 x aciertos / puntos */
    printf("p=%d N=%ld dentro=%ld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), N, dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
