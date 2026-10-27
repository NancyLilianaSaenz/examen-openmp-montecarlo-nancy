"""Figuras de la parte II a partir de los resultados reales (semilla 2224510).
Uso: python figuras.py  ->  ../figuras/*.png"""
from pathlib import Path
import re

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch, Rectangle, FancyArrowPatch

AQUI = Path(__file__).resolve().parent
RES = AQUI.parent / "resultados"
FIG = AQUI.parent / "figuras"
FIG.mkdir(exist_ok=True)

AZUL, NARANJA, GRIS, ROJO = "#2a78d6", "#eb6834", "#8a8984", "#c8412b"
TINTA, TENUE, LINEA = "#1f1f1e", "#5f5e5a", "#c9c7bf"
plt.rcParams.update({"font.family": "DejaVu Sans", "font.size": 11})


def coma(v, d):
    return f"{v:.{d}f}".replace(".", ",")


def limpiar(ax):
    for lado in ("top", "right"):
        ax.spines[lado].set_visible(False)
    for lado in ("left", "bottom"):
        ax.spines[lado].set_color(LINEA)
    ax.tick_params(colors=TENUE)


def caja(ax, x0, y0, w, h, texto, borde, relleno, tam=10.5, negrita=False, color=TINTA):
    ax.add_patch(FancyBboxPatch((x0, y0), w, h, boxstyle="round,pad=0.02,rounding_size=0.08",
                                facecolor=relleno, edgecolor=borde, lw=1.6))
    ax.text(x0 + w / 2, y0 + h / 2, texto, ha="center", va="center", color=color, fontsize=tam,
            fontweight="bold" if negrita else "normal", linespacing=1.4)


# 1. Como se reparte el trabajo: la secuencia de numeros partida en tramos
ii2 = (RES / "ii2_paralelo.txt").read_text()
estados = re.findall(r"^\s+(\d)\s+\S+\s+(\d+) - (\d+)\s+(\d+)\s+(\d+)\s+si", ii2, re.M)
fig, ax = plt.subplots(figsize=(9.0, 3.9), dpi=150)
ax.set_xlim(0, 10); ax.set_ylim(0, 4.4); ax.axis("off")
ax.text(5, 4.15, "La lista de números aleatorios que sale de la semilla 2224510 (una sola, fija)",
        ha="center", color=TINTA, fontsize=11.5, fontweight="bold")
colores = ["#2a78d6", "#4a8fde", "#6aa6e6", "#8abdee"]
for k, (h, ini, fin, est, _) in enumerate(estados):
    x0 = 0.3 + k * 2.35
    ax.add_patch(Rectangle((x0, 2.55), 2.35, 1.0, facecolor=colores[k], edgecolor="white", lw=2))
    ax.text(x0 + 1.17, 3.05, f"Hilo {h}\nbloques {k * 250}–{k * 250 + 249}", ha="center",
            va="center", color="white", fontsize=10, fontweight="bold")
    ax.text(x0 + 0.05, 2.25, f"empieza en la\nmuestra {int(ini):,}".replace(",", " "), ha="left",
            va="top", color=TENUE, fontsize=8.8)
caja(ax, 0.6, 0.15, 8.8, 1.05,
     "La función saltar lleva a cada bloque directo a su lugar de la lista.\n"
     "Ningún hilo repite números de otro y el resultado es el mismo con 1, 2, 4 u 8 hilos.",
     NARANJA, "#fdeee7", 10.2, True)
fig.tight_layout()
fig.savefig(FIG / "fig1_reparto_bloques.png")
plt.close(fig)

# 2. Condicion de carrera sobre el contador (II.4)
fig, ax = plt.subplots(figsize=(8.6, 3.7), dpi=150)
ax.set_xlim(0, 10); ax.set_ylim(0, 5.2); ax.axis("off")
filas = [("Tiempo", "Hilo 0", "Hilo 1", "dentro en memoria"),
         ("1", "lee dentro (100)", "", "100"),
         ("2", "", "lee dentro (100)", "100"),
         ("3", "suma 1 → 101", "suma 1 → 101", "100"),
         ("4", "escribe 101", "", "101"),
         ("5", "", "escribe 101", "101  ← se perdió un acierto")]
anchos = [1.2, 2.6, 2.6, 3.4]
for r, fila in enumerate(filas):
    y = 4.5 - r * 0.82
    x = 0.1
    for j, celda in enumerate(fila):
        cab = r == 0
        ax.add_patch(Rectangle((x, y), anchos[j], 0.78, facecolor=AZUL if cab else ("#fdeee7" if r == 5 and j == 3 else "white"),
                               edgecolor=LINEA, lw=1))
        ax.text(x + anchos[j] / 2, y + 0.39, celda, ha="center", va="center", fontsize=10,
                color="white" if cab else (ROJO if r == 5 and j == 3 else TINTA),
                fontweight="bold" if cab or (r == 5 and j == 3) else "normal")
        x += anchos[j]
fig.tight_layout()
fig.savefig(FIG / "fig2_carrera.png")
plt.close(fig)

# 3. Que pasa con cada error (II.3 y II.4), con los datos medidos
ii3 = (RES / "ii3_rand.txt").read_text()
ii4 = (RES / "ii4_comparticion.txt").read_text()
t_rand1 = float(re.search(r"rand_global p=1 .*T=([\d.]+)", ii3).group(1))
t_rand8 = [float(v) for v in re.findall(r"rand_global p=8 .*T=([\d.]+)", ii3)]
t_final8 = float(re.search(r"^p=8 .*T=([\d.]+)", ii3, re.M).group(1))
pi_sin = [float(v) for v in re.findall(r"sin_reduccion p=8 .*pi=([\d.]+)", ii4)]
pi_bien = float(re.search(r"^p=8 .*pi=([\d.]+)", ii4, re.M).group(1))
fig, (a1, a2) = plt.subplots(1, 2, figsize=(9.4, 3.9), dpi=150)
nombres = ["rand() compartido\n1 hilo", "rand() compartido\n8 hilos", "versión final\n8 hilos"]
vals = [t_rand1, sorted(t_rand8)[1], t_final8]
barras = a1.bar(nombres, vals, color=[GRIS, ROJO, AZUL], width=0.6)
for b, v in zip(barras, vals):
    a1.text(b.get_x() + b.get_width() / 2, v + 0.25, coma(v, 2) + " s", ha="center", color=TINTA, fontsize=10)
a1.set_ylabel("Tiempo [s]", color=TINTA)
a1.set_title("Generador compartido: los hilos se turnan", color=TINTA, fontsize=11)
a1.set_ylim(0, max(vals) * 1.18)
limpiar(a1)
nombres2 = ["sin reduction\ncorrida 1", "sin reduction\ncorrida 2", "sin reduction\ncorrida 3", "con reduction"]
vals2 = pi_sin + [pi_bien]
barras = a2.bar(nombres2, vals2, color=[ROJO] * 3 + [AZUL], width=0.6)
for b, v in zip(barras, vals2):
    a2.text(b.get_x() + b.get_width() / 2, v + 0.06, coma(v, 3), ha="center", color=TINTA, fontsize=10)
a2.axhline(3.14159, color=TENUE, ls="--", lw=1)
a2.text(-0.45, 3.22, "π real", ha="left", color=TENUE, fontsize=9)
a2.set_ylabel("π estimado (8 hilos)", color=TINTA)
a2.set_title("Contador sin reduction: se pierden aciertos", color=TINTA, fontsize=11)
a2.set_ylim(0, 3.6)
a2.tick_params(axis="x", labelsize=8.5)
a1.tick_params(axis="x", labelsize=8.5)
limpiar(a2)
fig.tight_layout()
fig.savefig(FIG / "fig3_errores.png")
plt.close(fig)
print("figuras listas:", sorted(p.name for p in FIG.glob("*.png")))
