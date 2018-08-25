import random
import sys
import itertools

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

def write_line(f, line):
    f.write(line)
    f.write("\n")

def gen_exp_1(output_file, algs, num_simulations, seed):
    """
            OPTIMAL ARM ALWAYS CHANGE
    """

    mab_type = 0 # 0 for stochastic, 1 for adversarial

    p_min, p_max = 0.1, 0.9

    time_horizons = [10**5, 10**6]
    num_breakpoints = [1, 2, 4, 8]
    num_arms = [2, 4, 8]
    change_type = [0, 1] # 0 means global switching, 1 means one arm switches
    min_mean_change = 0.1 # min change on the same arm from t to t+1
    min_delta_optimal = 0.1 # optimal arm mean is at least (second optimal arm mean + min_delta_optimal)

    num_experiments = len(time_horizons) * len(num_breakpoints) * len(num_arms) * len(change_type)
    write_line(output_file, str(num_experiments) + " " + str(num_simulations) + " " + str(seed))

    logperc = LogPerc(num_experiments)
    exp_index = 0
    for T, num_bp, num_a, ct in itertools.product(time_horizons, num_breakpoints, num_arms, change_type):
        num_phases = (num_bp + 1)
        breakpoint_step = int(T * 1.0 / num_phases)
        breakpoints_ends = [i*breakpoint_step for i in range(1, num_phases+1)]
        breakpoints_ends[-1] = T

        if ct == 0: # Global switching
            arms_means = [[0 for p in range(num_phases)] for a in range(num_a)] # arms_means[arm][phase]
            arms_ends = [[0 for p in range(num_phases)] for a in range(num_a)] # arms_ends[arm][phase]

            # Add random first phase to each arm and save best arm
            max_mean = -1
            best_arm = -1
            new_min_delta_optimal = -1

            while new_min_delta_optimal < min_delta_optimal:
                max_mean = -1
                best_arm = -1
                new_min_delta_optimal = 1

                new_means = []
                for a in range(num_a):
                    new_mean = random.uniform(p_min, p_max)
                    new_means.append(new_mean)
                    arms_means[a][0] = new_mean
                    arms_ends[a][0] = breakpoints_ends[0]
                    if arms_means[a][0] > max_mean:
                        max_mean = arms_means[a][0]
                        best_arm = a
                second_optimal_arm_mean = [x for x in sorted(new_means, reverse=True)][1]
                new_min_delta_optimal = min(new_min_delta_optimal, max_mean - second_optimal_arm_mean)

            # Add num_phases-1 phases, making sure that each time the optimal arm changes
            for p in range(1, num_phases):
                new_best_arm = best_arm
                new_min_mean_change = -1
                new_min_delta_optimal = -1

                # Add a random phase until a good configuration shows up
                while new_best_arm == best_arm or new_min_mean_change < min_mean_change or new_min_delta_optimal < min_delta_optimal:
                    new_max_mean = arms_means[best_arm][p] # reinitialize
                    new_min_mean_change = 1 # reinitialize
                    new_min_delta_optimal = 1 # reinitialize

                    new_means = []
                    for a in range(num_a):
                        new_mean = random.uniform(p_min, p_max)
                        new_means.append(new_mean)
                        arms_means[a][p] = new_mean
                        arms_ends[a][p] = breakpoints_ends[p]
                        if arms_means[a][p] > new_max_mean:
                            new_max_mean = arms_means[a][p]
                            new_best_arm = a
                        new_min_mean_change = min(new_min_mean_change, abs(arms_means[a][p] - arms_means[a][p-1]))
                    second_optimal_arm_mean = [x for x in sorted(new_means, reverse=True)][1]
                    new_min_delta_optimal = min(new_min_delta_optimal, new_max_mean - second_optimal_arm_mean)

                best_arm = new_best_arm
        else: # one arm switches
            arms_means = [[0] for a in range(num_a)]
            arms_ends = [[] for a in range(num_a)]

            # Add random first mean to each arm and save best arm
            max_mean = -1
            best_arm = -1
            new_min_delta_optimal = -1

            while new_min_delta_optimal < min_delta_optimal:
                max_mean = -1
                best_arm = -1
                new_min_delta_optimal = 1

                new_means = []
                for a in range(num_a):
                    new_mean = random.uniform(p_min, p_max)
                    new_means.append(new_mean)
                    arms_means[a][0] = new_mean
                    if arms_means[a][0] > max_mean:
                        max_mean = arms_means[a][0]
                        best_arm = a
                second_optimal_arm_mean = [x for x in sorted(new_means, reverse=True)][1]
                new_min_delta_optimal = min(new_min_delta_optimal, max_mean - second_optimal_arm_mean)

            # Add num_phases phases-1, making sure that each time the optimal arm changes
            for p in range(num_phases-1):
                new_best_arm = best_arm
                new_min_mean_change = -1
                new_min_delta_optimal = -1

                # Add a random phase until a good configuration shows up
                while new_best_arm == best_arm or new_min_mean_change < min_mean_change or new_min_delta_optimal < min_delta_optimal:
                    new_max_mean = arms_means[best_arm][-1] # reinitialize
                    new_min_mean_change = 1 # reinitialize
                    new_min_delta_optimal = 1 # reinitialize

                    a = random.randint(0, num_a-1) # inclusive
                    new_mean = random.uniform(p_min, p_max)
                    if new_mean > new_max_mean:
                        new_max_mean = new_mean
                        new_best_arm = a
                    new_min_mean_change = min(new_min_mean_change, abs(new_mean - arms_means[a][-1]))
                    #second_optimal_arm_mean = [x for x in sorted(new_means, reverse=True)][1]
                    #new_min_delta_optimal = min(new_min_delta_optimal, new_max_mean - second_optimal_arm_mean)

                arms_means[a].append(new_mean)
                arms_ends[a].append(breakpoints_ends[p])

                best_arm = new_best_arm

            # Add an end to all the arms
            for a in range(num_a):
                arms_ends[a].append(T)


        write_line(output_file, str(exp_index) + " " + str(mab_type) + " " + str(num_a) + " " + str(len(algs)) + " " + str(T))

        for i in range(num_a):
            s = "BernoulliNonStationary " + str(len(arms_ends[i]))
            for p in range(len(arms_means[i])):
                s += " " + "{0:.2f}".format(arms_means[i][p]) + " " + str(arms_ends[i][p])
            write_line(output_file, s)

        for alg in algs:
            write_line(output_file, alg)

        exp_index += 1
        logperc.log(exp_index)




def gen_exp_2(output_file, algs, num_simulations, seed):
    mab_type = 0 # 0 for stochastic, 1 for adversarial

    p_min, p_max = 0.1, 0.9

    time_horizons = [10**5, 10**6]
    num_breakpoints = [1, 2, 4, 8]
    num_arms = [3, 5, 8]
    change_type = [0, 1] # 0 means from second best they change, 1 means from third best they change
    min_delta_optimal = 0.1 # optimal arm mean is at least (second optimal arm mean + min_delta_optimal)

    num_experiments = len(time_horizons) * len(num_breakpoints) * len(num_arms) * len(change_type)
    write_line(output_file, str(num_experiments) + " " + str(num_simulations) + " " + str(seed))

    logperc = LogPerc(num_experiments)
    exp_index = 0
    for T, num_bp, num_a, ct in itertools.product(time_horizons, num_breakpoints, num_arms, change_type):
        num_phases = (num_bp + 1)
        breakpoint_step = int(T * 1.0 / num_phases)
        breakpoints_ends = [i*breakpoint_step for i in range(1, num_phases+1)]
        breakpoints_ends[-1] = T

        arms_means = [[0] for a in range(num_a)]
        arms_ends = [[] for a in range(num_a)]

        # Add random first mean to each arm and save best arm
        max_mean = -1
        best_arm = -1
        new_min_delta_optimal = -1
        second_optimal_arm = -1

        while new_min_delta_optimal < min_delta_optimal:
            max_mean = -1
            best_arm = -1
            new_min_delta_optimal = 1

            new_means = []
            for a in range(num_a):
                new_mean = random.uniform(p_min, p_max)
                new_means.append(new_mean)
                arms_means[a][0] = new_mean
                if arms_means[a][0] > max_mean:
                    max_mean = arms_means[a][0]
                    best_arm = a
            second_optimal_arm = [new_means.index(x) for x in sorted(new_means, reverse=True)][1]
            new_min_delta_optimal = min(new_min_delta_optimal, max_mean - new_means[second_optimal_arm])

        # From now on, the best arms must not have changes
        for p in range(num_phases-1):
             new_best_arm = best_arm
             new_min_delta_optimal = -1

             # Add a random phase until a good configuration shows up
             while new_best_arm != best_arm or new_min_delta_optimal < min_delta_optimal:
                 new_max_mean = arms_means[best_arm][-1] # reinitialize
                 new_min_delta_optimal = 1 # reinitialize

                 a = random.randint(0, num_a-1) # inclusive
                 if ct == 0: # from second
                     while a == best_arm:
                         a = random.randint(0, num_a-1)
                     new_mean = random.uniform(p_min, arms_means[best_arm][-1])
                 elif ct == 1: # from third
                     while a == best_arm or a == second_optimal_arm:
                         a = random.randint(0, num_a-1)
                     new_mean = random.uniform(p_min, arms_means[second_optimal_arm][-1])

                 if new_mean > new_max_mean:
                     new_max_mean = new_mean
                     new_best_arm = a
                 #second_optimal_arm_mean = [x for x in sorted(new_means, reverse=True)][1]
                 #new_min_delta_optimal = min(new_min_delta_optimal, new_max_mean - second_optimal_arm_mean)

             arms_means[a].append(new_mean)
             arms_ends[a].append(breakpoints_ends[p])

             best_arm = new_best_arm

        # Add an end to all the arms
        for a in range(num_a):
            arms_ends[a].append(T)

        write_line(output_file, str(exp_index) + " " + str(mab_type) + " " + str(num_a) + " " + str(len(algs)) + " " + str(T))

        for i in range(num_a):
            s = "BernoulliNonStationary " + str(len(arms_ends[i]))
            for p in range(len(arms_means[i])):
                s += " " + "{0:.2f}".format(arms_means[i][p]) + " " + str(arms_ends[i][p])
            write_line(output_file, s)

        for alg in algs:
            write_line(output_file, alg)

        exp_index += 1
        logperc.log(exp_index)


def gen_exp_3(output_file, algs, num_simulations, seed):
    mab_type = 0 # 0 for stochastic, 1 for adversarial

    p_min, p_max = 0.1, 0.9

    time_horizons = [10**5, 10**6]
    num_arms = 2
    change_type = [0, 1] # 0 means from down to up, 1 means from up to down
    change_amounts = [0.04, 0.1, 0.3]

    num_experiments = len(time_horizons) * len(change_amounts) * len(change_type)
    write_line(output_file, str(num_experiments) + " " + str(num_simulations) + " " + str(seed))

    logperc = LogPerc(num_experiments)
    exp_index = 0
    for T, ch_amount, ct in itertools.product(time_horizons, change_amounts, change_type):

        write_line(output_file, str(exp_index) + " " + str(mab_type) + " 2 " + str(len(algs)) + " " + str(T))

        s_still = "BernoulliNonStationary 1 0.5 " + str(T)
        if ct == 0:
            s_change = "BernoulliNonStationary 2 " + str(0.5 - ch_amount) + " " + str(T // 2) + " " + str(0.5 + ch_amount) + " " + str(T)
        elif ct == 1:
            s_change = "BernoulliNonStationary 2 " + str(0.5 + ch_amount) + " " + str(T // 2) + " " + str(0.5 - ch_amount) + " " + str(T)

        write_line(output_file, s_change)
        write_line(output_file, s_still)

        for alg in algs:
            write_line(output_file, alg)

        exp_index += 1
        logperc.log(exp_index)


def main():
    output_file = open("exp_config/experiments_config.txt", "w")

    algs = [
        "ucb1 ucb1",
        "d_ucb round 2 d_ucb _d_ucb",
        "sw_ucb round 2 sw_ucb _sw_ucb",
        "adapt_eve _adapt_eve",
        "cusum_ucb _cd_algorithm 0 1",
        "cusum_ucb_history _cd_algorithm 1 1"
    ]
    num_simulations = 10

    experiment = int(sys.argv[1])
    seed = int(sys.argv[2])

    if experiment == 1:
        gen_exp_1(output_file, algs, num_simulations, seed)
    elif experiment == 2:
        gen_exp_2(output_file, algs, num_simulations, seed)
    elif experiment == 3:
        gen_exp_3(output_file, algs, num_simulations, seed)


main()
