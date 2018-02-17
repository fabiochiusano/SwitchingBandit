import matplotlib.pyplot as plt
import numpy as np
import math
import matplotlib.mlab as mlab
import matplotlib.patches as mpatches
import itertools
from functools import reduce

file = open("temp/regrets.txt", "r")

patches = []

yss = {}

for line in file:
	line_splitted = line.split(" ")
	alg_name = line_splitted[0]

	ys = [0] + list(map(lambda n: float(n), line_splitted[1:len(line_splitted)-1]))
	for i in range(1, len(ys)):
		ys[i] += ys[i-1]

	if alg_name not in yss:
		yss[alg_name] = [ys]
	else:
		yss[alg_name] = yss[alg_name] + [ys]

for alg_name, ll in yss.items():
	sums = reduce(lambda l1,l2: [x+y for x,y in zip(l1, l2)], ll)
	means = [x/len(ll) for x in sums]

	asd = list(map(lambda l: list(map(lambda t: (t[1] - means[t[0]])**2, enumerate(l))), ll))
	sums_for_variances = reduce(lambda l1,l2: [x+y for x,y in zip(l1, l2)], asd)
	stddevs = [math.sqrt(x/len(ll)) for x in sums_for_variances]

	xs = np.arange(0, len(ll[0]), 1)

	p = plt.plot(xs, means, linestyle='-')
	#p = plt.plot(xs, means, color=p[0].get_color(), linestyle=':')
	polycollection = plt.fill_between(xs, [x-y for x,y in zip(means,stddevs)], [x+y for x,y in zip(means,stddevs)], alpha=0.2)
	polycollection.alpha = 0.2

	patches.append(mpatches.Patch(color=p[0].get_color(), label=alg_name))

plt.legend(handles=patches)

plt.show()
