# Cálculo de π con Monte Carlo y OpenMP

Taller de Introducción a la computación paralela, Universidad Industrial de Santander, 2026-2.

**Estudiante:** Nancy Liliana Saenz Moreno · **Código:** 2224510 · **Semilla usada:** 2224510

## Qué hace

El programa lanza puntos al azar (como dardos) en un cuadrado y cuenta cuántos caen dentro de un
cuarto de círculo. Con eso calcula π:

**π ≈ 4 × dentro ÷ total**

El programa original del profesor usaba un solo hilo. Lo mejoré con OpenMP para que el trabajo se
reparta entre varios hilos del procesador.

## Archivos

| Archivo | Qué es |
|---|---|
| [`montecarlo.c`](montecarlo.c) | El programa final, con comentarios que indican qué parte del taller es cada sección |
| [`Documento_Final.pdf`](Documento_Final.pdf) | El taller completo: las cuatro partes, las pruebas, las capturas, la tabla de tiempos y la bitácora de IA |
| [`PRESENTACION - Tipo tesis como el profesor lo dijo y 3 minutos.pdf`](PRESENTACION%20-%20Tipo%20tesis%20como%20el%20profesor%20lo%20dijo%20y%203%20minutos.pdf) | La diapositiva de la presentación (Tesis en 3 Minutos) |

## Resultados

Con 100 millones de puntos, el resultado es **π = 3,141668**, con **78.541.707 aciertos**. El resultado
es el mismo con cualquier número de hilos; lo que cambia es el tiempo:

| Hilos | Tiempo (mediana de 7) | Veces más rápido |
|:-:|:-:|:-:|
| 1 | 1,25 s | 1,0× |
| 2 | 0,68 s | 1,8× |
| 4 | 0,37 s | 3,4× |
| 8 | 0,25 s | 5,0× |

Medido en un portátil AMD Ryzen 5 3450U (4 núcleos y 8 hilos), con Ubuntu en WSL y gcc 15.2.0.
No llega a 8 veces porque el procesador tiene 4 núcleos reales (Ley de Amdahl: una parte del trabajo
no se puede repartir).

## Cómo ejecutarlo

Se necesita Linux o WSL con `gcc`.

```bash
gcc -O2 -fopenmp montecarlo.c -o montecarlo -lm   # compilar
./montecarlo 100000000 8                          # 100 millones de puntos con 8 hilos
```

El uso de herramientas de inteligencia artificial está declarado en la bitácora de IA del documento final.
