#!/bin/bash
# Parte I: compila y ejecuta los experimentos de apoyo (semilla 2224510).
cd "$(dirname "$0")"
mkdir -p bin ../resultados
set -e
echo "$ gcc -O2 -Wall puntos.c -o puntos && gcc -O2 -Wall convergencia.c -o convergencia -lm"
gcc -O2 -Wall puntos.c -o bin/puntos
gcc -O2 -Wall convergencia.c -o bin/convergencia -lm
echo
echo "=== Primeros 2000 puntos con la semilla 2224510 ==="
./bin/puntos 2000 > ../resultados/puntos.txt
echo
echo "=== I.1: error frente a N ==="
./bin/convergencia | tee ../resultados/convergencia.txt
