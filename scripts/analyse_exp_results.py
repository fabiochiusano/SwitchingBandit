from os.path import isfile
from os import walk
import argparse
import math
import random
import matplotlib.pyplot as plt
import numpy as np
import itertools

argparser = argparse.ArgumentParser()
argparser.add_argument("e", type=int, default=10, help="Number of experiment (from 1 to 4)")
args = argparser.parse_args()

def add_d(d, k, v):
    if k not in d:
        d[k] = v
    else:
        d[k] += v


def analyse_1_2():
    def add_d(d, k1, k2, k3, v):
        if k1 in d:
            if k2 in d[k1]:
                if k3 in d[k1][k2]:
                    d[k1][k2][k3] += v
                else:
                    d[k1][k2][k3] = v
            else:
                d[k1][k2] = {k3:v}
        else:
            d[k1] = {k2:{k3:v}}

    d_mean = {} # d[t][ch_type][alg]
    d_bound = {} # d[t][ch_type][alg]

    time_horizons = [10**5, 10**6]
    num_breakpoints = [1, 8]
    num_arms = [2, 8]
    change_type = [0, 1]

    selected_num_arms = 8
    selected_num_bp = 1
    of = open("results/analysis_arms{0}_bp{1}.txt".format(selected_num_arms, selected_num_bp), "w")

    num_exp_per_config = 5
    num_experiments = len(time_horizons) * len(num_breakpoints) * len(num_arms) * len(change_type) * num_exp_per_config

    for num_bp, num_a, ct, T, e in itertools.product(num_breakpoints, num_arms, change_type, time_horizons, range(num_exp_per_config)):
        if num_bp == selected_num_bp and num_a == selected_num_arms:
            T_index = time_horizons.index(T)
            bp_index = num_breakpoints.index(num_bp)
            na_index = num_arms.index(num_a)
            ct_index = change_type.index(ct)

            exp_id = (T_index + ct_index*len(change_type) + na_index*len(change_type)*len(num_arms) + bp_index*len(change_type)*len(num_arms)*len(num_breakpoints)) * num_exp_per_config + e
            print(exp_id)
            if exp_id == 38:
                print("----")
                continue
            f = open("results/" + str(exp_id) + ".txt", "r")
            for line in f:
                splitted = line.split(" ")
                add_d(d_mean, T_index, ct_index, splitted[0], float(splitted[1]))
                add_d(d_bound, T_index, ct_index, splitted[0], float(splitted[2]))
    """
    for t_index,_ in enumerate(time_horizons):
        for ct_index,_ in enumerate(change_type):
            for k,v in d_mean[t_index][ct_index].items():
                of.write(k + " " + str(int(v / num_exp_per_config)) + " " + str(int(d_bound[t_index][ct_index][k] / num_exp_per_config)) + "\n")
            of.write("\n")
        of.write("\n")
    of.write("\n")
    """
    for k in d_mean[0][0]:
        of.write("$\\text{" + k + "}$")
        for t_index,_ in enumerate(time_horizons):
            for ct_index,_ in enumerate(change_type):
                m = int(d_mean[t_index][ct_index][k] / num_exp_per_config)
                b = int(d_bound[t_index][ct_index][k] / num_exp_per_config)
                of.write(" & $" + str(m) + "\\pm" + str(b) + "$")
        of.write(" \\\\ \n")

"""
def analyse_3():
    num_experiments = 12

    # By T: from 0 to 5, from 6 to 11 (inclusive)
    of = open("results/analysis_T.txt", "w")
    num_of_ts = 2
    tot = num_experiments/num_of_ts

    experiments = [[] for i in range(num_of_ts)]
    for i in range(num_experiments):
        experiments[int(i // tot)].append(i)

    for i in range(num_of_ts):
        d_t_mean = {}
        d_t_bound = {}
        for exp_id in experiments[i]:
            f = open("results/" + str(exp_id) + ".txt", "r")
            for line in f:
                splitted = line.split(" ")
                add_d(d_t_mean, splitted[0], float(splitted[1]))
                add_d(d_t_bound, splitted[0], float(splitted[2]))
        for k,v in d_t_mean.items():
            of.write(k + " " + str(v/tot) + " " + str(d_t_bound[k]/tot) + "\n")
        of.write("\n")

    # By change amount: 1,1   2,2    3,3  and repeat
    of = open("results/analysis_T.txt", "w")
    num_of_ca = 3
    tot = num_experiments/num_of_ca

    experiments = [[] for i in range(num_of_ca)]
    for i in range(num_experiments):
        experiments[int(i & tot)].append(i)

    for i in range(num_of_ca):
        d_t_mean = {}
        d_t_bound = {}
        for exp_id in experiments[i]:
            f = open("results/" + str(exp_id) + ".txt", "r")
            for line in f:
                splitted = line.split(" ")
                add_d(d_t_mean, splitted[0], float(splitted[1]))
                add_d(d_t_bound, splitted[0], float(splitted[2]))
        for k,v in d_t_mean.items():
            of.write(k + " " + str(v/tot) + " " + str(d_t_bound[k]/tot) + "\n")
        of.write("\n")
"""
def main():
    if args.e == 1 or args.e == 2:
        analyse_1_2()
    elif args.e == 3:
        analyse_3()

main()
