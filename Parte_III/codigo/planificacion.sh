#!/bin/bash
# III.3: con schedule(static) cada hilo tiene sus bloques fijos y todos esperan
# al mas lento; con schedule(dynamic) los hilos libres toman mas bloques.
# 5 rondas intercaladas con p = 4 y p = 8.
cd "$(dirname "$0")"
mkdir -p bin
gcc -O2 -fopenmp -DPLAN=dynamic montecarlo.c -o bin/montecarlo_dynamic -lm
cd bin
N=100000000
SAL=../../resultados/planificacion.txt
: > $SAL
printf "ronda   static p=4  dynamic p=4   static p=8  dynamic p=8\n"
for r in 1 2 3 4 5; do
    linea="  $r   "
    for p in 4 8; do
        for plan in static dynamic; do
            if [ $plan = static ]; then out=$(./montecarlo $N $p); else out=$(./montecarlo_dynamic $N $p); fi
            echo "$plan $out" >> $SAL
            linea="$linea   $(printf '%9s' $(echo "$out" | sed 's/.*T=\([0-9.]*\).*/\1/'))"
        done
    done
    echo "$linea"
done
echo
echo "Mediana de 5 corridas [s]:"
for p in 4 8; do
    for plan in static dynamic; do
        printf "  %-7s p=%d  %s\n" $plan $p $(grep "^$plan p=$p " $SAL | sed 's/.*T=\([0-9.]*\).*/\1/' | sort -n | sed -n 3p)
    done
done
echo "Conteos distintos: $(sed 's/.*dentro=\([0-9]*\).*/\1/' $SAL | sort -u | tr '\n' ' ')"
