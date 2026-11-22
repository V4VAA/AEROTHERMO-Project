import csv
from pathlib import Path

import matplotlib.pyplot as plt

folder = Path(__file__).resolve().parent
cases = {}

with (folder / "aero_results.csv").open(newline="") as file:
    for row in csv.DictReader(file):
        case = (float(row["mach"]), float(row["altitude_km"]))
        point = {key: float(row[key]) for key in ("alpha_deg", "CA", "CN", "CL", "CD", "LD")}
        cases.setdefault(case, []).append(point)

for points in cases.values():
    points.sort(key=lambda point: point["alpha_deg"])

fig, ax = plt.subplots(figsize=(7, 4.5))
for (mach, altitude), points in cases.items():
    alpha = [point["alpha_deg"] for point in points]
    ld = [point["LD"] for point in points]
    ax.plot(alpha, ld, label=f"Mach {mach:g}, {altitude:g} km")

ax.set(xlabel="Angle of attack (deg)", ylabel="L/D")
ax.grid(alpha=0.3)
ax.legend()
fig.tight_layout()
fig.savefig(folder / "L_over_D.png", dpi=300)

fig, axes = plt.subplots(2, 2, figsize=(9, 6.5), sharex=True)
panels = [
    ("CA", "Axial force", r"$C_A$"),
    ("CN", "Normal force", r"$C_N$"),
    ("CL", "Lift", r"$C_L$"),
    ("CD", "Drag", r"$C_D$"),
]
line_styles = ["-", "--", ":"]

for ax, (column, title, label) in zip(axes.flat, panels):
    for i, ((mach, altitude), points) in enumerate(cases.items()):
        alpha = [point["alpha_deg"] for point in points]
        coefficient = [point[column] for point in points]
        ax.plot(alpha, coefficient, linestyle=line_styles[i % len(line_styles)],
                label=f"Mach {mach:g}, {altitude:g} km")
    ax.set(title=title, ylabel=label)
    ax.grid(alpha=0.3)

for ax in axes[1]:
    ax.set_xlabel("Angle of attack (deg)")

handles, labels = axes[0, 0].get_legend_handles_labels()
fig.legend(handles, labels, loc="upper center", bbox_to_anchor=(0.5, 0.93),
           ncol=3, frameon=False)
fig.tight_layout(rect=(0, 0, 1, 0.87))
fig.savefig(folder / "aero_coefficients.png", dpi=300)
