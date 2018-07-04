import matplotlib.pyplot as plt
import numpy as np
import math
import matplotlib
import matplotlib.mlab as mlab
import matplotlib.patches as mpatches
import itertools
from functools import reduce
import sys
from matplotlib.widgets import Slider
from PIL import Image
from random import randint
import os
from os import walk

"""
def plot_distributions(experiment_name):
	file = open("temp/" + experiment_name + "/distributions.txt", "r")

	patches_1 = []
	x_span = 20

	axarr[0].set_xlabel("value")
	axarr[0].set_ylabel("probability")
	axarr[0].set_title("arms distributions")

	axarr[0].set_ylim(ymin=0, ymax=1)

	for line in file:
		line_splitted = line.split(" ")
		distribution_name = line_splitted[0]
		distribution_type = line_splitted[1]

		x = np.linspace(-x_span/2, x_span/2, 1000)

		if distribution_type == "Normal":
			mean = float(line_splitted[2])
			stddev = float(line_splitted[3])
			p = axarr[0].plot(x, mlab.normpdf(x, mean, stddev))

		elif distribution_type == "Uniform":
			mean = float(line_splitted[2])
			p = axarr[0].plot(x, [0]*len(x))
			axarr[0].plot([mean, mean], [0, 0.5], color=p[0].get_color())


		patches_1.append(mpatches.Patch(color=p[0].get_color(), label=distribution_name))

	#axarr[0].legend(handles=patches_1)
"""


def plot_means_in_time(experiment_name, axarr):
	styles = itertools.cycle((':','-.','--','-'))

	file = open("temp/experiments_config.txt", "r")

	line_splitted = file.readline().split(" ")
	while(line_splitted[0] != experiment_name):
		line_splitted = file.readline().split(" ")
	timesteps = int(line_splitted[4])

	file = open("temp/" + experiment_name + "/distributions.txt", "r")

	patches_2 = []
	x_span = 20

	axarr[0,0].set_xlabel("time")
	axarr[0,0].set_ylabel("mean")
	axarr[0,0].set_title("means in time")

	axarr[0,0].set_ylim(ymin=ymin_mean, ymax=ymax_mean)

	x = list(range(timesteps))

	for line in file:
		line_splitted = line.split(" ")
		distribution_name = line_splitted[0]
		distribution_type = line_splitted[1]
		"""
		if distribution_type == "Normal":
			mean = float(line_splitted[2])
			stddev = float(line_splitted[3])
			p = axarr[1].plot(x, [mean]*timesteps, linestyle=next(styles))
			polycollection = axarr[1].fill_between(x, [mean-stddev*stddev_amplifier_means]*timesteps, [mean+stddev*stddev_amplifier_means]*timesteps, alpha=0.2)

		elif distribution_type == "Uniform":
			mean = float(line_splitted[2])
			p = axarr[1].plot(x, [mean]*timesteps, linestyle=next(styles))
		"""
		if distribution_type == "UniformNonStationary" or distribution_type == "BernoulliNonStationary":
			prev_mean = 0
			prev_end = 0
			for i, piece in enumerate(line_splitted[2:len(line_splitted)-1]):
				if i % 2 == 0:
					prev_mean = float(piece)
				else:
					new_end = int(piece)
					if i == 1:
						p = axarr[0,0].plot([prev_end, new_end], [prev_mean, prev_mean], linestyle=next(styles))
					else:
						p = axarr[0,0].plot([prev_end, new_end], [prev_mean, prev_mean], color=p[0].get_color(), linestyle=p[0].get_linestyle())
					prev_end = int(piece)
		"""
		elif distribution_type == "NormalNonStationary":
			prev_mean = 0
			prev_std = 0
			prev_end = 0
			for i, piece in enumerate(line_splitted[2:len(line_splitted)-1]):
				if i % 3 == 0:
					prev_mean = float(piece)
				elif i % 3 == 1:
					prev_std = float(piece)
				else:
					new_end = int(piece)
					if i == 2:
						p = axarr[1].plot([prev_end, new_end], [prev_mean, prev_mean], linestyle=next(styles))
					else:
						p = axarr[1].plot([prev_end, new_end], [prev_mean, prev_mean], color=p[0].get_color(), linestyle=p[0].get_linestyle())
					prev_end = int(piece)

		elif distribution_type == "Bernoulli":
			pr = float(line_splitted[2])
			mean = pr
			stddev = math.sqrt(pr*(1-pr))
			p = axarr[1].plot(x, [mean]*timesteps, linestyle=next(styles))
			polycollection = axarr[1].fill_between(x, [mean-stddev*stddev_amplifier_means]*timesteps, [mean+stddev*stddev_amplifier_means]*timesteps, alpha=0.2)

		elif distribution_type == "SquareWave":
			v = float(line_splitted[2])
			cur_v = float(line_splitted[3])
			mean = v/2
			stddev = math.sqrt(0.25)
			p = axarr[1].plot(x, [mean]*timesteps, linestyle=next(styles))
			polycollection = axarr[1].fill_between(x, [mean-stddev*stddev_amplifier_means]*timesteps, [mean+stddev*stddev_amplifier_means]*timesteps, alpha=0.2)
		"""

		patches_2.append(mpatches.Patch(color=p[0].get_color(), label=distribution_name))

	#axarr[1].legend(handles=patches_2)



def plot_reward(cur_experiment, axarr):
	file = open("temp/experiments_config.txt", "r")

	line_splitted = file.readline().split(" ")
	while(line_splitted[0] != experiment_name):
		line_splitted = file.readline().split(" ")
	num_algs = int(line_splitted[3])

	axarr[1,1].set_xlabel("time")
	axarr[1,1].set_ylabel("total reward")
	axarr[1,1].set_title("reward of algorithms")
	axarr[1,1].set_ylim(ymin=ymin_reward, ymax=ymax_reward)

	yss = {}

	for sim in range(1, num_simulations + 1):
		file = open("temp/" + cur_experiment + "/rewards/" + str(sim) + ".txt", "r")
		write_every = int(file.readline())
		for line in file: # one line for each algorithm
			line_splitted = line.split(" ")
			alg_name = line_splitted[0]

			ys = [0] + list(map(lambda n: float(n), line_splitted[1:len(line_splitted)-1]))
			for i in range(1, len(ys)):
				ys[i] += ys[i-1]

			if alg_name not in yss:
				yss[alg_name] = [ys]
			else:
				yss[alg_name] = yss[alg_name] + [ys]


	patches_3 = []
	for alg_name, ll in yss.items():
		sums = reduce(lambda l1,l2: [x+y for x,y in zip(l1, l2)], ll)
		means = [x/len(ll) for x in sums]

		asd = list(map(lambda l: list(map(lambda t: (t[1] - means[t[0]])**2, enumerate(l))), ll))
		sums_for_variances = reduce(lambda l1,l2: [x+y for x,y in zip(l1, l2)], asd)
		stddevs = [math.sqrt(x/len(ll)) for x in sums_for_variances]

		xs = np.arange(0, len(ll[0])*write_every, write_every)

		p = axarr[1,1].plot(xs, means, linestyle='-')
		polycollection = axarr[1,1].fill_between(xs, [x-y*stddev_amplifier_regrets for x,y in zip(means,stddevs)], [x+y*stddev_amplifier_regrets for x,y in zip(means,stddevs)], alpha=0.2)

		patches_3.append(mpatches.Patch(color=p[0].get_color(), label=alg_name))


	box = axarr[1,1].get_position()
	axarr[1,1].set_position([box.x0, box.y0 + box.height * 0.1, box.width, box.height * 0.9])

	# Put a legend below current axis
	axarr[1,1].legend(handles=patches_3, loc='upper center', bbox_to_anchor=(0.5, -0.1), fancybox=True, shadow=True, ncol=5)


def plot_regret(cur_experiment, axarr):
	file = open("temp/experiments_config.txt", "r")

	line_splitted = file.readline().split(" ")
	while(line_splitted[0] != experiment_name):
		line_splitted = file.readline().split(" ")
	num_algs = int(line_splitted[3])

	axarr[0,1].set_xlabel("time")
	axarr[0,1].set_ylabel("total regret")
	axarr[0,1].set_title("regret of algorithms")
	axarr[0,1].set_ylim(ymin=ymin_regret, ymax=ymax_regret)

	yss = {}

	for sim in range(1, num_simulations + 1):
		file = open("temp/" + cur_experiment + "/regrets/" + str(sim) + ".txt", "r")
		write_every = int(file.readline())
		for line in file: # one line for each algorithm
			line_splitted = line.split(" ")
			alg_name = line_splitted[0]

			ys = [0] + list(map(lambda n: float(n), line_splitted[1:len(line_splitted)-1]))
			for i in range(1, len(ys)):
				ys[i] += ys[i-1]

			if alg_name not in yss:
				yss[alg_name] = [ys]
			else:
				yss[alg_name] = yss[alg_name] + [ys]


	patches_3 = []
	for alg_name, ll in yss.items():
		sums = reduce(lambda l1,l2: [x+y for x,y in zip(l1, l2)], ll)
		means = [x/len(ll) for x in sums]

		asd = list(map(lambda l: list(map(lambda t: (t[1] - means[t[0]])**2, enumerate(l))), ll))
		sums_for_variances = reduce(lambda l1,l2: [x+y for x,y in zip(l1, l2)], asd)
		stddevs = [math.sqrt(x/len(ll)) for x in sums_for_variances]

		xs = np.arange(0, len(ll[0])*write_every, write_every)

		p = axarr[0,1].plot(xs, means, linestyle='-')
		polycollection = axarr[0,1].fill_between(xs, [x-y*stddev_amplifier_regrets for x,y in zip(means,stddevs)], [x+y*stddev_amplifier_regrets for x,y in zip(means,stddevs)], alpha=0.2)

		patches_3.append(mpatches.Patch(color=p[0].get_color(), label=alg_name))


	box = axarr[0,1].get_position()
	axarr[0,1].set_position([box.x0, box.y0 + box.height * 0.1, box.width, box.height * 0.9])

	# Put a legend below current axis
	axarr[0,1].legend(handles=patches_3, loc='upper center', bbox_to_anchor=(0.5, -0.1), fancybox=True, shadow=True, ncol=5)



def concatenate_images():
	images = list(map(Image.open, ["images/experiment_" + exp_name + ".png" for exp_name in dirs]))
	widths, heights = zip(*(i.size for i in images))

	total_height = sum(heights)
	max_width = max(widths)

	new_im = Image.new("RGB", (max_width, total_height))

	y_offset = 0
	for im in images:
		new_im.paste(im, (0, y_offset))
		y_offset += im.size[1]

	# Remove temporary images
	import os, shutil
	folder = 'images'
	for the_file in os.listdir(folder):
	    file_path = os.path.join(folder, the_file)
	    try:
	        if os.path.isfile(file_path):
	            os.unlink(file_path)
	    except Exception as e:
	        print(e)

	# Create final image
	new_im.save("images/experiment_final.png")


stddev_amplifier_means = 1/4 #1/10
stddev_amplifier_regrets = 1/5 #1/10
ymin_mean = -0.5
ymax_mean = 1.5
ymin_regret = 0
ymax_regret = 500
ymin_reward = 0
ymax_reward = 10000

file = open("temp/experiments_config.txt", "r")
num_simulations = int(file.readline().split(" ")[1])

dirs = next(os.walk('temp/'))[1]
num_experiments = len(dirs)

for experiment_name in dirs:
	f, axarr = plt.subplots(nrows=2, ncols=2, figsize=(20,15), dpi=80)
	plot_means_in_time(experiment_name, axarr)
	plot_regret(experiment_name, axarr)
	plot_reward(experiment_name, axarr)
	plt.savefig("images/experiment_" + experiment_name + ".png")
	plt.close()

concatenate_images()
os.system('say "finito"')
