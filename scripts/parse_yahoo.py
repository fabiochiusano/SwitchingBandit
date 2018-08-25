from os.path import isfile
from os import walk
import argparse
import math
import random
import matplotlib.pyplot as plt
import numpy as np

argparser = argparse.ArgumentParser()
argparser.add_argument("k", type=int, default=10, help="Number of top-k occurring items to consider")
argparser.add_argument("step", type=int, default=5000, help="Compute mean of an item every tot steps")
argparser.add_argument("day", type=int, default=2, help="Day to analyse, from 2 to 16 inclusive. If -1, do all")
args = argparser.parse_args()

class LogPerc:
    def __init__(self, maximum):
        self.maximum = maximum
        self.cur_perc = 0
        print("0%", end='\r')

    def log(self, v):
        perc = int(v/self.maximum * 10000) / 100
        if perc >= self.cur_perc + 0.01:
            print("{0:.2f}%".format(perc), end='\r')
            self.cur_perc = perc

DIR_NAME = "Webscope_R6B/"
OCC_NAME = "item_occurrencies.txt"
MAB_NAME = "mab_yahoo.txt"

def add_to_d(d, item):
    if item not in d:
        d[item] = 1
    else:
        d[item] += 1

def save_day_analysis(day):
    yahoo_filenames = [f for f in next(walk(DIR_NAME))[2] if f.startswith("ydata") and f.endswith(str(day).zfill(2))]

    # Load dictionary with items occurrencies
    d_item_occ = {} # key: item_id, value: item_occurrencies
    """if isfile(DIR_NAME + "/" + OCC_NAME):
        print("File with occurrencies found")
        with open(DIR_NAME + "/" + OCC_NAME) as infile:
            for line in infile:
                line_splitted = line.split(" ")
                d_item_occ[line_splitted[0]] = int(line_splitted[1])
        print("File with occurrencies loaded")
    else:"""
    print("File with occurrencies not found")
    print("Loading dictionary with occurrencies")
    logperc = LogPerc(len(yahoo_filenames))
    for p, filename in enumerate(yahoo_filenames):
        with open(DIR_NAME + "/" + filename) as infile:
            for line in infile:
                line_splitted = line.split(" ")
                item = line_splitted[1]
                add_to_d(d_item_occ, item)
        logperc.log(p + 1)
    print("Dictionary with occurrencies loaded")
    with open(DIR_NAME + "/" + OCC_NAME, "w") as occfile:
        for w in sorted(d_item_occ, key=d_item_occ.get, reverse=True):
            occfile.write("{0} {1}\n".format(w, d_item_occ[w]))

    # Take k most occurring items
    best_items = []
    print("Choose {0} items".format(args.k))
    for w in sorted(d_item_occ, key=d_item_occ.get, reverse=True)[:args.k]:
        best_items.append(w)
    print(best_items)


    # Fill d_mab
    d_mab = {} # key: timestamp, value: [(item, reward),...]
    d_plot = {} # key: item, value: [timestamp, ...]
    print("Loading mab dictionary")
    logperc = LogPerc(len(yahoo_filenames))
    for p, filename in enumerate(yahoo_filenames):
        with open(DIR_NAME + "/" + filename) as infile:
            for line in infile:
                line_splitted = line.split(" ")
                timestamp = int(line_splitted[0])
                item = line_splitted[1]
                reward = int(line_splitted[2])
                if item in best_items:
                    if timestamp not in d_mab:
                        d_mab[timestamp] = [(item, reward)]
                    else:
                        d_mab[timestamp].append((item, reward))

                    if item not in d_plot:
                        d_plot[item] = [timestamp]
                    else:
                        d_plot[item].append(timestamp)
        logperc.log(p + 1)
    print("Mab dictionary loaded")

    last_start = d_plot[best_items[0]][0]
    first_end = d_plot[best_items[0]][-1]
    for item in best_items:
        print("{0} starts at {1} and ends at {2}".format(item, d_plot[item][0], d_plot[item][-1]))
        last_start = max(last_start, d_plot[item][0])
        first_end = min(first_end, d_plot[item][-1])
    print("Last start at {0}".format(last_start))
    print("First end at {0}".format(first_end))
    tot_timesteps = first_end - last_start
    print("Diff: {0}".format(tot_timesteps))
    num_intervals = int(math.ceil(tot_timesteps / float(args.step)))
    print("Num intervals: " + str(num_intervals) + ", each one long " + str(args.step))
    intervals_begin = [last_start + args.step * i for i in range(num_intervals + 1)]
    print("First interval begins at {0}, last interval begins at {1}".format(intervals_begin[0], intervals_begin[-1]))

    # Delete from d_map entries with timestamp outside the domain
    keys_to_delete = [k for k in list(d_mab.keys()) if (k < last_start or k > first_end)]
    for k in keys_to_delete:
        d_mab.pop(k)

    # Compute the mean for each interval
    print("Computing the mean for each interval")
    d_means = {} # key: interval_index, value: dict(key: item, value: mean_reward)
    cur_interval = 0
    d_interval = {} # key: item, value: [reward,...]
    for k, v in list(d_mab.items()): # already sorted by key
        if k >= intervals_begin[cur_interval + 1]: # if this timestep belongs to the next interval, then save and advance to the next interval
            d_res = {} # key: item, value: mean_reward
            for i,l in d_interval.items():
                d_res[i] = float(sum(l))/len(l)
            d_means[cur_interval] = d_res
            cur_interval += 1
            d_interval = {}
        for (item, reward) in v:
            if item not in d_interval:
                d_interval[item] = [reward]
            else:
                d_interval[item].append(reward)
    # Add last interval...
    d_res = {}
    for i,l in d_interval.items():
        d_res[i] = float(sum(l))/len(l)
    d_means[cur_interval] = d_res

    xs = np.arange(last_start, first_end)
    plt.xlabel("time")
    plt.ylabel("mean")
    for ind,item in enumerate(best_items):
        ys_item = [d_means[int(float(x - last_start)*num_intervals/tot_timesteps)][item] for x in xs]
        plt.plot(np.arange(len(xs)), ys_item, alpha=0.8, markersize=2)
    plt.savefig("results/yahoo_" + str(day).zfill(2) + ".png", bbox_inches='tight')
    plt.show()

    # write means to file
    f = open("exp_config/yahoo_" + str(day).zfill(2) + ".txt", "w")
    for item in best_items:
        line = "BernoulliNonStationary " + str(len(d_means.keys()))
        for interval_index, d in d_means.items():
            mean = d[item]
            end = intervals_begin[interval_index+1] - intervals_begin[0]
            line += " {0} {1}".format(mean, end)
        f.write(line + "\n")
    f.close()

if args.day == -1:
    for i in range(2, 17):
        save_day_analysis(i)
else:
    save_day_analysis(args.day)
