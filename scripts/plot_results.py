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
import sys

def write_line(f, line):
    f.write(line)
    f.write("\n")

def plot_means_in_time(experiment_name, axarr):
	styles = itertools.cycle((':','-.','--','-'))

	file = open("exp_config/experiments_config.txt", "r")

	line_splitted = file.readline().split(" ")
	while(line_splitted[0] != experiment_name):
		line_splitted = file.readline().split(" ")
	timesteps = int(line_splitted[4])

	file = open("temp/" + experiment_name + "/distributions.txt", "r")

	patches_2 = []
	x_span = 20

	axarr[0].set_xlabel("time")
	axarr[0].set_ylabel("mean")
	axarr[0].set_title("means in time")

	axarr[0].set_ylim(ymin=ymin_mean, ymax=ymax_mean)

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
						p = axarr[0].plot([prev_end, new_end], [prev_mean, prev_mean], linestyle=next(styles))
					else:
						p = axarr[0].plot([prev_end, new_end], [prev_mean, prev_mean], color=p[0].get_color(), linestyle=p[0].get_linestyle())
					prev_end = int(piece)

		if distribution_type == "SineBernoulli":
			A = float(line_splitted[2])
			omega = float(line_splitted[3])
			phi = float(line_splitted[4])
			mean = float(line_splitted[5])

			x = np.arange(timesteps)
			p = axarr[0].plot(x, A * np.sin(omega * x + phi) + mean, linestyle=next(styles))

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

def plot_regret(cur_experiment, axarr):
	file = open("exp_config/experiments_config.txt", "r")

	line_splitted = file.readline().split(" ")
	while(line_splitted[0] != experiment_name):
		line_splitted = file.readline().split(" ")
	num_algs = int(line_splitted[3])

	axarr[1].set_xlabel("time")
	axarr[1].set_ylabel("total regret")
	axarr[1].set_title("regret of algorithms")

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
	means_and_stddevs = []
	lowest_final_regret = 10000000
	for alg_name, ll in yss.items():
		sums = reduce(lambda l1,l2: [x+y for x,y in zip(l1, l2)], ll)
		means = [x/len(ll) for x in sums]
		lowest_final_regret = min(lowest_final_regret, means[-1])

		# for confidence intervals
		asd = list(map(lambda l: list(map(lambda t: (t[1] - means[t[0]])**2, enumerate(l))), ll))
		sums_for_variances = reduce(lambda l1,l2: [x+y for x,y in zip(l1, l2)], asd)
		stddevs = [math.sqrt(x/len(ll)) for x in sums_for_variances]

		means_and_stddevs.append((means, stddevs))

	axarr[1].set_ylim(ymin=ymin_regret, ymax=lowest_final_regret*regr_mult)

	for i, (alg_name, ll) in enumerate(yss.items()):
		# xs
		xs = np.arange(0, len(ll[0])*write_every, write_every)

		# plot line
		p = axarr[1].plot(xs, means_and_stddevs[i][0], linestyle='-')

		# plot confidence intervals
		polycollection = axarr[1].fill_between(xs, [x-y*stddev_amplifier_regrets/math.sqrt(num_simulations) for x,y in zip(means_and_stddevs[i][0],means_and_stddevs[i][1])], [x+y*stddev_amplifier_regrets/math.sqrt(num_simulations) for x,y in zip(means_and_stddevs[i][0],means_and_stddevs[i][1])], alpha=0.2)

		patches_3.append(mpatches.Patch(color=p[0].get_color(), label=alg_name))

	final_info_file = open("results/" + cur_exp + ".txt", "w")
	for i,alg_name in enumerate(yss.keys()):
		last_mean = means_and_stddevs[i][0][-1]
		last_stddev = means_and_stddevs[i][1][-1]
		write_line(final_info_file, alg_name + " " + "{0:.2f}".format(last_mean) + " " + "{0:.2f}".format(last_stddev*stddev_amplifier_regrets/math.sqrt(num_simulations)))

	box = axarr[1].get_position()
	axarr[1].set_position([box.x0, box.y0 + box.height * 0.1, box.width, box.height * 0.9])

	# Put a legend below current axis
	plt.rcParams.update({'font.size': small_font_size})
	axarr[1].legend(handles=patches_3, loc='upper center', bbox_to_anchor=(0.5, -0.15), fancybox=True, shadow=True, ncol=3)
	plt.rcParams.update({'font.size': big_font_size})

big_font_size = 22
small_font_size = 18
plt.rcParams.update({'font.size': big_font_size})

stddev_amplifier_means = 0#1/4 #1/10
# 2.228 95% with 10 dof
# 2.042 95% with 30 dof
# 1.984 95% with 100 dof
stddev_amplifier_regrets =  1.984
ymin_mean = 0
ymax_mean = 1
ymin_regret = 0
regr_mult = 3


file = open("exp_config/experiments_config.txt", "r")
num_simulations = int(file.readline().split(" ")[1])
cur_exp = sys.argv[1]

dirs = next(os.walk('temp/'))[1]
num_experiments = len(dirs)

for experiment_name in dirs:
	if experiment_name == cur_exp:
		f, axarr = plt.subplots(nrows=1, ncols=2, figsize=(20,8), dpi=80)
		plot_means_in_time(experiment_name, axarr)
		plot_regret(experiment_name, axarr)
		#plot_reward(experiment_name, axarr)
		#plot_alarms(experiment_name, axarr)
		plt.savefig("results/experiment_" + experiment_name + ".png", bbox_inches='tight')
		plt.close()

#concatenate_images()
os.system('say "finito {0}"'.format(cur_exp))




"""
def plot_alarms(experiment_name, axarr):
	cdt_file = open("temp/cdt.txt", "r")

	line = cdt_file.readline()[0:-1]
	while (line != experiment_name):
		line = cdt_file.readline()[0:-1]

	alarms = cdt_file.readline()[0:-2].split(" ")
	d = {}
	for alarm in alarms:
		if alarm not in d:
			d[alarm] = 1
		else:
			d[alarm] += 1

	x = 0.05
	y = 0.8
	y_step = 0.05
	axarr[1,0].text(x, y, "AVERAGE ALARMS:")
	y -= y_step
	for alarm in sorted(d, key=lambda k: d[k])[::-1]:
		axarr[1,0].text(x, y, alarm + ": " + "{0:.2f}".format(d[alarm]/num_simulations))
		y -= y_step
"""

"""
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
"""


"""
def plot_reward(cur_experiment, axarr):
	file = open("cur_exp/experiments_config.txt", "r")

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
		polycollection = axarr[1,1].fill_between(xs, [x-(stddev_amplifier_regrets*y/math.sqrt(num_simulations)) for x,y in zip(means,stddevs)], [x+(stddev_amplifier_regrets*y/math.sqrt(num_simulations)) for x,y in zip(means,stddevs)], alpha=0.2)

		patches_3.append(mpatches.Patch(color=p[0].get_color(), label=alg_name))


	box = axarr[1,1].get_position()
	axarr[1,1].set_position([box.x0, box.y0 + box.height * 0.1, box.width, box.height * 0.9])

	# Put a legend below current axis
	axarr[1,1].legend(handles=patches_3, loc='upper center', bbox_to_anchor=(0.5, -0.1), fancybox=True, shadow=True, ncol=5)
"""


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
