# Integración por Monte Carlo en memoria compartida con OpenMP

Estimación de π por el método de Monte Carlo (acierto y fallo), paralelizada con OpenMP en C.
Evaluación de Introducción a la computación paralela, Universidad Industrial de Santander, 2026-2.

**Estudiante:** Nancy Liliana Saenz Moreno · **Código:** 2224510 · **Semilla usada:** 2224510

La idea: se lanzan N puntos al azar en el cuadrado [0, 1] × [0, 1] y se cuentan los que caen dentro
del cuarto de círculo x² + y² ≤ 1. La proporción tiende a π/4, así que π ≈ 4 · dentro / N.

## Archivos

- [`montecarlo.c`](montecarlo.c): el programa paralelo final, con comentarios que explican cada parte.
- [`Documento_Final.pdf`](Documento_Final.pdf): las cuatro partes del examen, con las figuras, las capturas de las ejecuciones, la tabla de tiempos, la salida de `lscpu` y la bitácora de IA.
- [`Informe_Parte_IV.pdf`](Informe_Parte_IV.pdf): el informe de la Parte IV con la bitácora de IA como anexo.
- [`PRESENTACION - Tipo tesis como el profesor lo dijo y 3 minutos.pdf`](PRESENTACION%20-%20Tipo%20tesis%20como%20el%20profesor%20lo%20dijo%20y%203%20minutos.pdf): la diapositiva de la presentación, con la metodología Tesis en 3 Minutos.

## Resultados principales

Con N = 10⁸ y la semilla 2224510 el programa da **π ≈ 3,141668**, con un error de 7,56 × 10⁻⁵
(menor que el error típico esperado, 1,64 × 10⁻⁴). El conteo de aciertos es **78.541.707** con
1, 2, 4 y 8 hilos, igual al del programa secuencial: el resultado no depende del número de hilos.

Mediana de 7 ejecuciones en un AMD Ryzen 5 3450U (4 núcleos, 8 hilos), Ubuntu en WSL, gcc 15.2.0:

| Hilos p | Mediana T(p) [s] | Speedup S(p) | Eficiencia E(p) |
|:-:|:-:|:-:|:-:|
| 1 | 1,2549 | 1,000 | 1,000 |
| 2 | 0,6826 | 1,838 | 0,919 |
| 4 | 0,3700 | 3,392 | 0,848 |
| 8 | 0,2499 | 5,022 | 0,628 |

La gráfica de speedup frente al ideal y las demás evidencias están en `Documento_Final.pdf`.

## Cómo se paralelizó

El ciclo del programa del profesor se conserva. Las N muestras se dividen en 1.000 bloques de 100.000
y OpenMP reparte los bloques entre los hilos:

```c
#pragma omp parallel for reduction(+:dentro) schedule(static)
for (long b = 0; b < nbloques; b++) {
    unsigned int estado = saltar(semilla, b * BLOQUE * 6);
    /* el mismo ciclo del programa del profesor, sobre las muestras del bloque b */
}
```

- `reduction(+:dentro)`: cada hilo cuenta en una copia propia y al final se suman.
- `saltar`: calcula en pocos pasos el estado que tendría `rand_r` después de k pasos, así que cada
  bloque empieza justo donde estaría el programa secuencial. Los flujos no se solapan y el resultado
  es el mismo con cualquier número de hilos.

## Cómo ejecutarlo

Requisitos: Linux o WSL y `gcc` con OpenMP.

```bash
gcc -O2 -fopenmp montecarlo.c -o montecarlo -lm      # compila sin advertencias

./montecarlo 100000000 4              # muestras, hilos y (opcional) semilla
OMP_NUM_THREADS=8 ./montecarlo 100000000
```

El uso de herramientas de inteligencia artificial generativa está declarado en la bitácora de IA del informe.
