from os.path import isfile
from os import walk
import argparse
import math
import random

argparser = argparse.ArgumentParser()
argparser.add_argument("k", type=int, default=10, help="Number of top-k occurring items to consider")
argparser.add_argument("step", type=int, default=5000, help="Compute mean of an item every tot steps")
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

yahoo_filenames = [f for f in next(walk(DIR_NAME))[2] if f.startswith("ydata")]

# Load dictionary with items occurrencies
d_item_occ = {}
if isfile(DIR_NAME + "/" + OCC_NAME):
    print("File with occurrencies found")
    with open(DIR_NAME + "/" + OCC_NAME) as infile:
        for line in infile:
            line_splitted = line.split(" ")
            d_item_occ[line_splitted[0]] = line_splitted[1]
    print("File with occurrencies loaded")
else:
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

num_intervals_ok_max = 0
best_items = []
for iteration in range(10):
    # Take k items
    print("Choose {0} items".format(args.k))
    items = []
    while len(items) < args.k:
        new_item = random.choice(list(d_item_occ.keys()))
        while new_item in items:
            new_item = random.choice(list(d_item_occ.keys()))
        items.append(new_item)
    print(items)

    # Fill d_mab
    d_mab = {}
    print("Loading mab dictionary")
    logperc = LogPerc(len(yahoo_filenames))
    for p, filename in enumerate(yahoo_filenames):
        with open(DIR_NAME + "/" + filename) as infile:
            for line in infile:
                line_splitted = line.split(" ")
                timestamp = line_splitted[0]
                item = line_splitted[1]
                reward = line_splitted[2]
                if item in items:
                    if timestamp not in d_mab:
                        d_mab[timestamp] = [(item, reward)]
                    else:
                        d_mab[timestamp].append((item, reward))
        logperc.log(p + 1)
    print("Mab dictionary loaded")

    min_timestep = min(list(map(int, list(d_mab.keys()))))
    print("Min timestep: " + str(min_timestep))
    max_timestep = max(list(map(int, list(d_mab.keys()))))
    print("Max timestep: " + str(max_timestep))
    tot_timesteps = max_timestep - min_timestep
    print("Diff: " + str(tot_timesteps))
    num_intervals = int(math.ceil(tot_timesteps / float(args.step)))
    print("Num intervals: " + str(num_intervals) + ", each one long " + str(args.step))
    intervals_begin = [min_timestep + args.step * i for i in range(num_intervals + 1)]

    # Compute the mean for each interval
    print("Computing the mean for each interval")
    d_means = {}
    cur_interval = 0
    d_interval = {}
    for k, v in list(d_mab.items()): # already sorted by key
        if int(k) >= intervals_begin[cur_interval + 1]:
            d_res = {}
            for i,l in d_interval.items():
                d_res[i] = float(sum(l))/len(l)
            d_means[cur_interval] = d_res
            cur_interval += 1
            d_interval = {}
        for (item, reward) in v:
            if item not in d_interval:
                d_interval[item] = [float(reward)]
            else:
                d_interval[item].append(float(reward))
    d_res = {}
    for i,l in d_interval.items():
        d_res[i] = float(sum(l))/len(l)
    d_means[cur_interval] = d_res

    num_intervals_ok = 0
    for interval in range(num_intervals):
        ok = True
        print(str(d_means[interval]))
        for item in items:
            if item not in d_means[interval]:
                ok = False
                break
        if ok:
            num_intervals_ok += 1
    print("Num intervals ok: {0}".format(num_intervals_ok))

    if num_intervals_ok > num_intervals_ok_max:
        num_intervals_ok_max = num_intervals_ok
        best_items = items
        print("New high with items: {0}".format(items))



# Best k

items = best_items

d_mab = {}
print("Loading mab dictionary")
logperc = LogPerc(len(yahoo_filenames))
for p, filename in enumerate(yahoo_filenames):
    with open(DIR_NAME + "/" + filename) as infile:
        for line in infile:
            line_splitted = line.split(" ")
            timestamp = line_splitted[0]
            item = line_splitted[1]
            reward = line_splitted[2]
            if item in items:
                if timestamp not in d_mab:
                    d_mab[timestamp] = [(item, reward)]
                else:
                    d_mab[timestamp].append((item, reward))
    logperc.log(p + 1)
print("Mab dictionary loaded")

min_timestep = min(list(map(int, list(d_mab.keys()))))
print("Min timestep: " + str(min_timestep))
max_timestep = max(list(map(int, list(d_mab.keys()))))
print("Max timestep: " + str(max_timestep))
tot_timesteps = max_timestep - min_timestep
print("Diff: " + str(tot_timesteps))
num_intervals = int(math.ceil(tot_timesteps / float(args.step)))
print("Num intervals: " + str(num_intervals) + ", each one long " + str(args.step))
intervals_begin = [min_timestep + args.step * i for i in range(num_intervals + 1)]

print("Computing the mean for each interval")
d_means = {}
cur_interval = 0
d_interval = {}
for k, v in list(d_mab.items()): # already sorted by key
    if int(k) >= intervals_begin[cur_interval + 1]:
        d_res = {}
        for i,l in d_interval.items():
            d_res[i] = float(sum(l))/len(l)
        d_means[cur_interval] = d_res
        cur_interval += 1
        d_interval = {}
    for (item, reward) in v:
        if item not in d_interval:
            d_interval[item] = [float(reward)]
        else:
            d_interval[item].append(float(reward))
d_res = {}
for i,l in d_interval.items():
    d_res[i] = float(sum(l))/len(l)
d_means[cur_interval] = d_res

# Write best choice to mab_file
print("Writing mab to file")
with open(DIR_NAME + "/" + MAB_NAME, "w") as mabfile:
    for interval in range(num_intervals):
        ok = True
        for item in items:
            if item not in d_means[interval]:
                ok = False
        if ok:
            mabfile.write("{0}".format(interval))
            for item in items:
                mabfile.write(" {0}".format(d_means[interval][item]));
            mabfile.write("\n")
        else:
            print("Interval {0} is not ok".format(interval))
