"""Figuras de la parte I a partir de los resultados reales (semilla 2224510).
Uso: python figuras.py  ->  ../figuras/*.png"""
from pathlib import Path
import math
import re

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch, Rectangle, FancyArrowPatch

AQUI = Path(__file__).resolve().parent
RES = AQUI.parent / "resultados"
FIG = AQUI.parent / "figuras"
FIG.mkdir(exist_ok=True)

AZUL, NARANJA, GRIS = "#2a78d6", "#eb6834", "#8a8984"
TINTA, TENUE, LINEA, FONDO = "#1f1f1e", "#5f5e5a", "#c9c7bf", "#f1efe8"
plt.rcParams.update({"font.family": "DejaVu Sans", "font.size": 11})


def limpiar(ax):
    for lado in ("top", "right"):
        ax.spines[lado].set_visible(False)
    for lado in ("left", "bottom"):
        ax.spines[lado].set_color(LINEA)
    ax.tick_params(colors=TENUE)


# 1. Los "dardos": puntos al azar dentro y fuera del cuarto de circulo
pts = [l.split() for l in (RES / "puntos.txt").read_text().splitlines()]
x = [float(a) for a, b, c in pts]
y = [float(b) for a, b, c in pts]
d = [c == "1" for a, b, c in pts]
dentro = sum(d)
fig, ax = plt.subplots(figsize=(6.2, 6.0), dpi=150)
ax.scatter([a for a, k in zip(x, d) if k], [b for b, k in zip(y, d) if k], s=7, color=AZUL,
           label=f"Dentro del círculo: {dentro:,}".replace(",", "."))
ax.scatter([a for a, k in zip(x, d) if not k], [b for b, k in zip(y, d) if not k], s=7,
           color=NARANJA, label=f"Fuera: {len(d) - dentro}")
t = [i / 400 * math.pi / 2 for i in range(401)]
ax.plot([math.cos(v) for v in t], [math.sin(v) for v in t], color=TINTA, lw=2)
ax.set_xlim(0, 1); ax.set_ylim(0, 1); ax.set_aspect("equal")
ax.set_xlabel("x", color=TINTA); ax.set_ylabel("y", color=TINTA)
limpiar(ax)
ax.legend(loc="upper right", frameon=True, framealpha=0.95, fontsize=10)
ax.set_title(f"π ≈ 4 × {dentro:,} / {len(d):,} = ".replace(",", ".")
             + f"{4 * dentro / len(d):.3f}".replace(".", ","),
             color=TINTA, fontsize=13, pad=10)
fig.tight_layout()
fig.savefig(FIG / "fig1_dardos.png")
plt.close(fig)

# 2. Convergencia: el error baja como 1/raiz(N)
filas = []
for l in (RES / "convergencia.txt").read_text().splitlines()[1:]:
    c = l.split()
    filas.append((int(c[0]), float(c[2]), None if c[3] == "-" else float(c[3]), float(c[4])))
N = [f[0] for f in filas]
fig, ax = plt.subplots(figsize=(7.2, 4.6), dpi=150)
ax.plot(N, [f[3] for f in filas], color=NARANJA, lw=2, label="Error típico teórico  1,64 / √N")
med = [(f[0], f[2]) for f in filas if f[2] is not None]
ax.plot([a for a, b in med], [b for a, b in med], color=AZUL, lw=2, marker="o", ms=7,
        markeredgecolor="white", markeredgewidth=1.5, label="Error medio de 30 corridas")
ax.scatter(N, [f[1] for f in filas], color=GRIS, s=28, zorder=4,
           label="Una sola corrida (semilla 2224510)")
ax.set_xscale("log"); ax.set_yscale("log")
ax.set_xlabel("Número de puntos N", color=TINTA)
ax.set_ylabel("Error |π estimado − π|", color=TINTA)
ax.grid(color="#e6e4dd", lw=1, which="major")
limpiar(ax)
ax.annotate("100 veces más puntos\n→ error 10 veces menor", xy=(1e6, 1.64e-3),
            xytext=(2.5e2, 3e-4), color=TINTA, fontsize=10,
            arrowprops=dict(arrowstyle="->", color=TENUE))
ax.legend(frameon=False, loc="upper right", fontsize=9.5)
fig.tight_layout()
fig.savefig(FIG / "fig2_convergencia.png")
plt.close(fig)


# 3. Que comparten y que tiene cada hilo
def caja(ax, x0, y0, w, h, texto, borde, relleno, tam=10.5, negrita=False):
    ax.add_patch(FancyBboxPatch((x0, y0), w, h, boxstyle="round,pad=0.02,rounding_size=0.08",
                                facecolor=relleno, edgecolor=borde, lw=1.6))
    ax.text(x0 + w / 2, y0 + h / 2, texto, ha="center", va="center", color=TINTA, fontsize=tam,
            fontweight="bold" if negrita else "normal", linespacing=1.4)


fig, ax = plt.subplots(figsize=(8.6, 5.0), dpi=150)
ax.set_xlim(0, 10); ax.set_ylim(0, 6); ax.axis("off")
caja(ax, 2.2, 4.6, 5.6, 1.1, "COMPARTIDO (todos lo leen, nadie lo cambia)\nN  ·  semilla base",
     AZUL, "#e8f1fb", 11, True)
for k in range(4):
    x0 = 0.25 + k * 2.45
    caja(ax, x0, 2.0, 2.15, 1.7, f"Hilo {k}\n\nsus propios puntos\nestado del generador\nx, y\ncontador de aciertos",
         GRIS, "white", 9.5)
    ax.add_patch(FancyArrowPatch((5, 4.6), (x0 + 1.07, 3.72), arrowstyle="-|>", mutation_scale=10,
                                 color=AZUL, lw=1.1))
    ax.add_patch(FancyArrowPatch((x0 + 1.07, 2.0), (5, 1.15), arrowstyle="-|>", mutation_scale=10,
                                 color=NARANJA, lw=1.1))
ax.text(5, 4.12, "PRIVADO: cada hilo tiene el suyo", ha="center", color=TENUE, fontsize=10,
        bbox=dict(facecolor="white", edgecolor="none", pad=2))
caja(ax, 3.0, 0.15, 4.0, 1.0, "Al final: aciertos = suma de los contadores\n(reduction)",
     NARANJA, "#fdeee7", 10.5, True)
fig.tight_layout()
fig.savefig(FIG / "fig3_compartido_privado.png")
plt.close(fig)

# 4. Repartir en tramos y sumar da lo mismo
txt = (RES / "reparto.txt").read_text()
parciales = [int(v) for v in re.findall(r"\):\s+(\d+) aciertos", txt)]
total = int(re.search(r"de corrido \(secuencial\) = (\d+)", txt).group(1))
fig, ax = plt.subplots(figsize=(8.6, 3.6), dpi=150)
ax.set_xlim(0, 10); ax.set_ylim(0, 4.2); ax.axis("off")
ax.text(5, 3.95, "Los mismos 100.000.000 puntos, en 4 tramos de 25.000.000", ha="center",
        color=TINTA, fontsize=11.5, fontweight="bold")
colores = ["#2a78d6", "#4a8fde", "#6aa6e6", "#8abdee"]
for k, v in enumerate(parciales):
    x0 = 0.4 + k * 2.3
    ax.add_patch(Rectangle((x0, 2.3), 2.2, 1.1, facecolor=colores[k], edgecolor="white", lw=2))
    ax.text(x0 + 1.1, 2.85, f"Hilo {k}\n{v:,} aciertos".replace(",", "."), ha="center",
            va="center", color="white", fontsize=10, fontweight="bold")
    ax.add_patch(FancyArrowPatch((x0 + 1.1, 2.3), (5, 1.45), arrowstyle="-|>", mutation_scale=10,
                                 color=TENUE, lw=1))
s = sum(parciales)
caja(ax, 1.4, 0.25, 7.2, 1.15,
     f"Suma = {s:,}  →  π ≈ 4 × {s:,} / 10⁸ = ".replace(",", ".")
     + f"{4 * s / 1e8:.6f}\n".replace(".", ",")
     + f"Programa secuencial = {total:,}   ✔ idéntico".replace(",", "."),
     NARANJA, "#fdeee7", 10.5, True)
fig.tight_layout()
fig.savefig(FIG / "fig4_reparto.png")
plt.close(fig)
print("figuras listas:", sorted(p.name for p in FIG.glob("*.png")))
