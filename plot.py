import numpy as np
import matplotlib.pyplot as plt

f = open("out.txt", "r")

xaxis = f.readline()
xaxis = xaxis.split()

yaxis = f.readline()
yaxis = yaxis.split()

X = np.array(xaxis);
Y = np.array(yaxis);

plt.plot(X,Y)
plt.show()

f.close()
