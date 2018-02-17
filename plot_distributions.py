import matplotlib.pyplot as plt
import numpy as np
import math
import matplotlib.mlab as mlab
import matplotlib.patches as mpatches

file = open("temp/distributions.txt", "r")

patches = []
x_span = 20

for line in file:
	line_splitted = line.split(" ")
	distribution_name = line_splitted[0]
	distribution_type = line_splitted[1]

	x = np.linspace(-x_span/2, x_span/2, 1000)

	if distribution_type == "Normal":
		mean = float(line_splitted[2])
		stddev = float(line_splitted[3])
		p = plt.plot(x, mlab.normpdf(x, mean, stddev))

	elif distribution_type == "Uniform":
		mean = float(line_splitted[2])
		p = plt.plot(x, [0]*len(x))
		plt.plot([mean, mean], [0, 0.5], color=p[0].get_color())

	
	patches.append(mpatches.Patch(color=p[0].get_color(), label=distribution_name))

plt.legend(handles=patches)

plt.show()