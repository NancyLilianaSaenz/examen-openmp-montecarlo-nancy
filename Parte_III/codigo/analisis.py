"""Figuras de la parte III a partir de resultados/tiempos.txt (semilla 2224510).
Uso: python analisis.py  ->  ../evidencias/speedup.png y ../figuras/*.png"""
import re
import statistics
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

AQUI = Path(__file__).resolve().parent
FIG = AQUI.parent / "figuras"
FIG.mkdir(exist_ok=True)
lineas = (AQUI.parent / "resultados" / "tiempos.txt").read_text().splitlines()
tiempos = {}
orden = []
for linea in lineas:
    t = float(re.search(r"T=([0-9.]+)", linea).group(1))
    clave = "sec" if linea.startswith("sec") else int(re.search(r"p=(\d+)", linea).group(1))
    tiempos.setdefault(clave, []).append(t)
    orden.append((clave, t))

T1 = statistics.median(tiempos["sec"])
P = [1, 2, 4, 8]
S = {1: 1.0, **{p: T1 / statistics.median(tiempos[p]) for p in P[1:]}}
f = (1 - 1 / S[4]) / (1 - 1 / 4)
amdahl = lambda p, ff=f: 1 / ((1 - ff) + ff / p)

for p in P:
    print(f"p={p}  T={T1 / S[p]:.4f}  S={S[p]:.3f}  E={S[p] / p:.3f}  Amdahl={amdahl(p):.3f}")
print(f"f = {f:.4f}   S(64) = {amdahl(64):.2f}   techo = {1 / (1 - f):.1f}")

AZUL, NARANJA, GRIS, ROJO = "#2a78d6", "#eb6834", "#8a8984", "#c8412b"
TINTA, TENUE, LINEA = "#1f1f1e", "#5f5e5a", "#c9c7bf"
plt.rcParams.update({"font.family": "DejaVu Sans", "font.size": 11})
coma = lambda v, d: f"{v:.{d}f}".replace(".", ",")


def limpiar(ax):
    for lado in ("top", "right"):
        ax.spines[lado].set_visible(False)
    for lado in ("left", "bottom"):
        ax.spines[lado].set_color(LINEA)
    ax.tick_params(colors=TENUE)
    ax.grid(axis="y", color="#e6e4dd", lw=1)


# 1. Speedup medido frente al ideal y a Amdahl
fig, ax = plt.subplots(figsize=(7.2, 4.6), dpi=150)
ax.axvspan(4, 8.4, color="#f1efe8", zorder=0)
ax.text(6.2, 0.55, "más hilos que núcleos físicos", ha="center", color=TENUE, fontsize=9.5)
x = [i / 10 for i in range(10, 85)]
ax.plot(P, P, ls="--", lw=2, color=GRIS, label="Ideal  S(p) = p")
ax.plot(x, [amdahl(v) for v in x], lw=2, color=NARANJA, label=f"Amdahl con f = {coma(f, 3)}")
ax.plot(P, [S[p] for p in P], lw=2, color=AZUL, marker="o", ms=8,
        markeredgecolor="white", markeredgewidth=2, label="Medido (mediana de 7)", zorder=5)
for p in P[1:]:
    ax.annotate(coma(S[p], 2), (p, S[p]), textcoords="offset points", xytext=(0, -18),
                ha="center", color=TINTA, fontsize=10)
ax.set_xticks(P); ax.set_xlim(0.7, 8.4); ax.set_ylim(0, 8.4)
ax.set_xlabel("Número de hilos p", color=TINTA); ax.set_ylabel("Speedup S(p)", color=TINTA)
limpiar(ax)
ax.legend(frameon=False, loc="upper left")
fig.tight_layout()
fig.savefig(AQUI.parent / "evidencias" / "speedup.png")
plt.close(fig)

# 2. III.1: ley de Amdahl con f = 0,98 y f = 0,90
fig, ax = plt.subplots(figsize=(7.2, 4.4), dpi=150)
ps = [2 ** (i / 8) for i in range(0, 8 * 10 + 1)]
for ff, color, techo in ((0.98, AZUL, 50), (0.90, NARANJA, 10)):
    ax.plot(ps, [amdahl(v, ff) for v in ps], color=color, lw=2.2, label=f"f = {coma(ff, 2)}")
    ax.axhline(techo, color=color, ls=":", lw=1.3)
    ax.text(1.05, techo + 1.2, f"techo = 1/(1 − f) = {techo}", color=color, fontsize=9.5)
    s8 = amdahl(8, ff)
    ax.plot([8], [s8], "o", color=color, ms=7, markeredgecolor="white", markeredgewidth=1.5)
    ax.annotate(f"p = 8 → {coma(s8, 2)}", (8, s8), textcoords="offset points",
                xytext=(10, -14) if ff < 0.95 else (-92, 6), color=color, fontsize=9.5)
ax.set_xscale("log", base=2)
ax.set_xticks([1, 2, 4, 8, 16, 64, 256, 1024]); ax.set_xticklabels(["1", "2", "4", "8", "16", "64", "256", "1024"])
ax.set_ylim(0, 56)
ax.set_xlabel("Número de núcleos p (escala logarítmica)", color=TINTA)
ax.set_ylabel("Speedup máximo S(p)", color=TINTA)
limpiar(ax)
ax.legend(frameon=False, loc="center left")
fig.tight_layout()
fig.savefig(FIG / "fig1_amdahl.png")
plt.close(fig)

# 3. Mediana frente a promedio: tiempos del programa secuencial en las 7 rondas
sec = tiempos["sec"]
fig, ax = plt.subplots(figsize=(7.2, 3.8), dpi=150)
colores = [ROJO if t > 2 * statistics.median(sec) else AZUL for t in sec]
barras = ax.bar(range(1, len(sec) + 1), sec, color=colores, width=0.6)
for b, t in zip(barras, sec):
    ax.text(b.get_x() + b.get_width() / 2, t + 0.2, coma(t, 2), ha="center", color=TINTA, fontsize=9.5)
med, prom = statistics.median(sec), statistics.mean(sec)
ax.axhline(med, color=AZUL, lw=1.5, ls="--", label=f"mediana = {coma(med, 2)} s")
ax.axhline(prom, color=NARANJA, lw=1.5, ls="--", label=f"promedio = {coma(prom, 2)} s")
ax.legend(frameon=False, loc="upper right")
ax.set_xticks(range(1, len(sec) + 1))
ax.set_xlabel("Ronda", color=TINTA); ax.set_ylabel("Tiempo secuencial [s]", color=TINTA)
ax.set_ylim(0, max(sec) * 1.15)
limpiar(ax)
fig.tight_layout()
fig.savefig(FIG / "fig2_mediana.png")
plt.close(fig)
print("figuras listas")
