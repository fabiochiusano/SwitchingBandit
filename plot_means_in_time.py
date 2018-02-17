import matplotlib.pyplot as plt
import numpy as np
import math
import matplotlib.mlab as mlab
import matplotlib.patches as mpatches
import itertools

styles = itertools.cycle((':','-.','--','-')) 

file = open("temp/means_in_time.txt", "r")

patches = []
x_span = 20

timesteps = int(file.readline())
x = list(range(timesteps))

for line in file:
	line_splitted = line.split(" ")
	distribution_name = line_splitted[0]
	distribution_type = line_splitted[1]

	if distribution_type == "Normal" or distribution_type == "Uniform":
		mean = float(line_splitted[2])
		p = plt.plot(x, [mean]*timesteps, linestyle=next(styles))

	elif distribution_type == "Step":
		prev_mean = 0
		prev_end = 0
		for i, piece in enumerate(line_splitted[2:len(line_splitted)-1]):
			if i % 2 == 0:
				prev_mean = float(piece)
			else:
				new_end = int(piece)
				if i == 1:
					p = plt.plot([prev_end, new_end], [prev_mean, prev_mean], linestyle=next(styles))
				else:
					p = plt.plot([prev_end, new_end], [prev_mean, prev_mean], color=p[0].get_color(), linestyle=p[0].get_linestyle())
				prev_end = int(piece)

	elif distribution_type == "Bernoulli":
		pr = float(line_splitted[2])
		v = float(line_splitted[3])
		p = plt.plot(x, [pr*v]*timesteps, linestyle=next(styles))

	
	patches.append(mpatches.Patch(color=p[0].get_color(), label=distribution_name))

plt.legend(handles=patches)

plt.show()