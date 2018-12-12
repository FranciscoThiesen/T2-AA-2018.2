import numpy as np
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt

def func(x, a, b, c, d):
    return a*(x**3) + b*(x**2) + c*x + d

x = []
y = []
f = open("highest_preflow_push_undirected_results.csv", "r")
fst = True
for line in f:
    string = line.split(',')
    if fst == False:
        x.append(float(string[0]))
        y.append(float(string[2]))
        print(string[0], string[2])
    else:
        fst = False

x = np.array(x)
y = np.array(y)

print(x)
print(y)

popt, pcov= curve_fit(func, x, y)

plt.figure()
plt.xlabel("Tamanho da instancia (nós) ")
plt.ylabel("Tempo de CPU(ms)")
plt.title("Analise dos tempos do highest_preflow_push")
plt.plot(x, y, 'ko', label="experimentos")
plt.plot(x, func(x, *popt), '-', label="fit cubico")
plt.legend()
plt.show()
