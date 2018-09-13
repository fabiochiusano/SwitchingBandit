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

def get_new_means(arms_means, phase, p_min, p_max, min_mean_change, max_mean_change):
    res = []
    for a in range(len(arms_means)):
        ll = max(arms_means[a][phase-1] - max_mean_change, p_min)
        lh = max(arms_means[a][phase-1] - min_mean_change, p_min)
        hl = min(arms_means[a][phase-1] + min_mean_change, p_max)
        hh = min(arms_means[a][phase-1] + max_mean_change, p_max)
        if random.randint(0, 1) == 0:
            res.append(random.uniform(ll, lh))
        else:
            res.append(random.uniform(hl, hh))
    return res

def gen_exp_various(output_file, algs, num_simulations, seed, num_exp_per_config):
    """
            OPTIMAL ARM ALWAYS CHANGE
    """

    mab_type = 0 # 0 for stochastic, 1 for adversarial

    p_min, p_max = 0.1, 0.9

    time_horizons = [10**5]
    num_breakpoints = [1, 8, 24]
    num_arms = [2, 8]
    change_type = [0] # 0 means global switching, 1 means one arm switches

    should_check_optimal_change = 0
    min_mean_change = 0.05 # min change on the same arm from t to t+1
    max_mean_change = 0.1 # max change on the same arm from t to t+1
    min_delta_optimal = 0.03 # optimal arm mean is at least (second optimal arm mean + min_delta_optimal)

    num_experiments = num_exp_per_config * len(time_horizons) * len(num_breakpoints) * len(num_arms) * len(change_type)
    write_line(output_file, str(num_experiments) + " " + str(num_simulations) + " " + str(seed))

    logperc = LogPerc(num_experiments)
    exp_index = 0
    for num_bp, num_a, ct, T in itertools.product(num_breakpoints, num_arms, change_type, time_horizons):
        for k in range(num_exp_per_config):
            num_phases = (num_bp + 1)
            breakpoint_step = int(T * 1.0 / num_phases)
            breakpoints_ends = [i*breakpoint_step for i in range(1, num_phases+1)]
            breakpoints_ends[-1] = T

            if ct == 0: # Global switching
                arms_means = [[0 for p in range(num_phases)] for a in range(num_a)] # arms_means[arm][phase]
                arms_ends = [breakpoints_ends for a in range(num_a)] # arms_ends[arm][phase]

                # Add random first phase to each arm and save best arm
                is_min_delta_ok = False
                while not is_min_delta_ok:
                    new_means = [random.uniform(p_min, p_max) for a in range(num_a)]
                    max_mean = max(new_means)
                    second_max_mean = [x for x in sorted(new_means, reverse=True)][1]
                    if max_mean - second_max_mean > min_delta_optimal:
                        is_min_delta_ok = True
                for i,m in enumerate(new_means):
                    arms_means[i][0] = m

                # Add num_phases-1 phases, making sure that each time the optimal arm changes
                for p in range(1, num_phases):
                    #print(p)
                    has_best_arm_changed = False or not should_check_optimal_change
                    is_mean_min_change_ok = False
                    is_mean_max_change_ok = False
                    is_min_delta_ok = False

                    # Add a random phase until a good configuration shows up
                    while not has_best_arm_changed or not is_mean_min_change_ok or not is_mean_max_change_ok or not is_min_delta_ok:
                        has_best_arm_changed = False or not should_check_optimal_change
                        is_mean_min_change_ok = False
                        is_mean_max_change_ok = False
                        is_min_delta_ok = False

                        new_means = get_new_means(arms_means, p, p_min, p_max, min_mean_change, max_mean_change)
                        max_mean = max(new_means)
                        best_arm = new_means.index(max_mean)
                        second_max_mean = [x for x in sorted(new_means, reverse=True)][1]
                        if max_mean - second_max_mean >= min_delta_optimal:
                            is_min_delta_ok = True
                        changes = [abs(new_means[i] - arms_means[i][p-1]) for i in range(num_a)]
                        if min(changes) >= min_mean_change:
                            is_mean_min_change_ok = True
                        if max(changes) <= max_mean_change:
                            is_mean_max_change_ok = True
                        prev_means = [arms_means[i][p-1] for i in range(num_a)]
                        prev_best_arm = prev_means.index(max(prev_means))
                        if best_arm != prev_best_arm:
                            has_best_arm_changed = True
                    for i,m in enumerate(new_means):
                        arms_means[i][p] = m
            else: # one arm switches
                arms_means = [[] for a in range(num_a)]
                arms_ends = [[] for a in range(num_a)]

                is_min_delta_ok = False
                while not is_min_delta_ok:
                    max_mean = -1
                    best_arm = -1
                    new_min_delta_optimal = 1

                    new_means = [random.uniform(p_min, p_max) for a in range(num_a)]
                    max_mean = max(new_means)
                    best_arm = new_means.index(max_mean)
                    second_best_arm_mean = [x for x in sorted(new_means, reverse=True)][1]
                    if max_mean - second_best_arm_mean >= min_delta_optimal:
                        is_min_delta_ok = True
                for i,m in enumerate(new_means):
                    arms_means[i].append(m)

                # Add num_phases phases-1, making sure that each time the optimal arm changes
                for p in range(num_phases-1):
                    has_best_arm_changed = False or not should_check_optimal_change
                    is_mean_min_change_ok = False
                    is_mean_max_change_ok = False
                    is_min_delta_ok = False

                    # Add a random phase until a good configuration shows up
                    while not has_best_arm_changed or not is_mean_min_change_ok or not is_min_delta_ok:
                        has_best_arm_changed = False or not should_check_optimal_change
                        is_mean_min_change_ok = False
                        is_mean_max_change_ok = False
                        is_min_delta_ok = False

                        a = random.randint(0, num_a-1) # inclusive
                        new_mean = random.uniform(p_min, p_max)
                        last_means = [arms_means[arm][-1] for arm in range(num_a)]
                        prev_max_mean = max(last_means)
                        prev_best_arm = last_means.index(prev_max_mean)
                        new_means = [arms_means[arm][-1] if arm != a else new_mean for arm in range(num_a)]
                        new_max_mean = max(new_means)
                        new_best_arm = new_means.index(new_max_mean)
                        new_second_max_mean = [x for x in sorted(new_means, reverse=True)][1]
                        if new_best_arm != prev_best_arm:
                            has_best_arm_changed = True
                        if abs(new_mean - arms_means[a][-1]) >= min_mean_change:
                            is_mean_min_change_ok = True
                        if abs(new_mean - arms_means[a][-1]) <= max_mean_change:
                            is_mean_max_change_ok = True
                        if new_max_mean - new_second_max_mean >= min_delta_optimal:
                            is_min_delta_ok = True
                    arms_means[a].append(new_mean)
                    arms_ends[a].append(breakpoints_ends[p])


                # Add an end to all the arms
                for a in range(num_a):
                    arms_ends[a].append(T)


            write_line(output_file, str(exp_index) + " " + str(mab_type) + " " + str(num_a) + " " + str(len(algs)) + " " + str(T))

            for i in range(num_a):
                s = "BernoulliNonStationary " + str(len(arms_ends[i]))
                for p in range(len(arms_means[i])):
                    s += " " + "{0:.3f}".format(arms_means[i][p]) + " " + str(arms_ends[i][p])
                write_line(output_file, s)

            for alg in algs:
                write_line(output_file, alg)

            exp_index += 1
            logperc.log(exp_index)




def gen_exp_adversarial(output_file, algs, num_simulations, seed, num_exp_per_config):
    """
        ADVERSARIAL CHANGES
    """
    mab_type = 0 # 0 for stochastic, 1 for adversarial

    p_min, p_max = 0.1, 0.9

    time_horizons = [10**5]
    num_breakpoints = [2, 4, 8, 24]
    num_arms = [4, 8]
    change_type = [0] # 0 means from second best they change, 1 means from third best they change

    min_mean_change = 0.05 # min change on the same arm from t to t+1
    max_mean_change = 1 # max change on the same arm from t to t+1
    min_delta_optimal = 0.03 # min difference between 1st and 2nd, and between 2nd and third if ct=1

    num_experiments = num_exp_per_config * len(time_horizons) * len(num_breakpoints) * len(num_arms) * len(change_type)
    write_line(output_file, str(num_experiments) + " " + str(num_simulations) + " " + str(seed))

    logperc = LogPerc(num_experiments)
    exp_index = 0
    for num_bp, num_a, ct, T in itertools.product(num_breakpoints, num_arms, change_type, time_horizons):
        for k in range(num_exp_per_config):
            num_phases = (num_bp + 1)
            breakpoint_step = int(T * 1.0 / num_phases)
            breakpoints_ends = [i*breakpoint_step for i in range(1, num_phases+1)]
            breakpoints_ends[-1] = T

            arms_means = [[] for a in range(num_a)]
            arms_ends = [[] for a in range(num_a)]

            # Add random first mean to each arm
            is_min_delta_ok = False
            while not is_min_delta_ok:
                new_means = [random.uniform(p_min, p_max) for a in range(num_a)]
                max_mean = max(new_means)
                second_max_mean = sorted(new_means, reverse=True)[1]
                if ct == 0 and max_mean - second_max_mean >= min_delta_optimal:
                    is_min_delta_ok = True
                elif ct == 1:
                    third_max_mean = sorted(new_means, reverse=True)[2]
                    if max_mean - second_max_mean >= min_delta_optimal and second_max_mean - third_max_mean >= min_delta_optimal:
                        is_min_delta_ok = True
            for i,m in enumerate(new_means):
                arms_means[i].append(m)
            best_arm_mean = max(new_means)
            best_arm = new_means.index(best_arm_mean)
            second_best_arm_mean = [x for x in sorted(new_means, reverse=True)][1]
            second_best_arm = new_means.index(second_best_arm_mean)

            # From now on, the best arms must not have changes
            for p in range(num_phases-1):
                 is_mean_min_change_ok = False
                 is_mean_max_change_ok = False

                 # Add a random phase until a good configuration shows up
                 while not is_mean_min_change_ok or not is_mean_max_change_ok:
                     is_mean_min_change_ok = False
                     is_mean_max_change_ok = False

                     a = random.randint(0, num_a-1) # inclusive
                     if ct == 0: # from second
                         while a == best_arm:
                             a = random.randint(0, num_a-1)
                         new_mean = random.uniform(p_min, best_arm_mean - min_delta_optimal)
                     elif ct == 1: # from third
                         while a == best_arm or a == second_best_arm:
                             a = random.randint(0, num_a-1)
                         new_mean = random.uniform(p_min, second_best_arm_mean - min_delta_optimal)

                     prev_mean = arms_means[a][-1]
                     if abs(new_mean - prev_mean) >= min_mean_change:
                         is_mean_min_change_ok = True
                     if abs(new_mean - prev_mean) <= max_mean_change:
                         is_mean_max_change_ok = True

                 arms_means[a].append(new_mean)
                 arms_ends[a].append(breakpoints_ends[p])

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


def gen_exp_changetype(output_file, algs, num_simulations, seed):
    """
        ANALYSE OPTIMAL VS SUBOPTIMAL CHANGES
    """
    mab_type = 0 # 0 for stochastic, 1 for adversarial

    p_min, p_max = 0.1, 0.9

    time_horizons = [10**5, 10**6]
    num_arms = 2
    change_type = [0, 1] # 0 means from down to up, 1 means from up to down
    change_amounts = [0.05, 0.1, 0.2]

    num_experiments = len(time_horizons) * len(change_amounts) * len(change_type)
    write_line(output_file, str(num_experiments) + " " + str(num_simulations) + " " + str(seed))

    logperc = LogPerc(num_experiments)
    exp_index = 0
    for ch_amount, ct, T in itertools.product(change_amounts, change_type, time_horizons):

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

def gen_exp_false_alarms(output_file, algs, num_simulations, seed):
    """
        ANALYSE FALSE ALARMS
    """
    mab_type = 0 # 0 for stochastic, 1 for adversarial

    p_min, p_max = 0.1, 0.9

    time_horizons = [10**5]
    num_arms = 2
    distributions_type = [0, 1, 2] # 0 means 0.8 and 0.2, 1 means 0.8 and 0.7, 2 means 0.2 and 0.1
    epsilons = [0.01, 0.02, 0.04]
    hs = [8, 9, 10]

    num_experiments = len(time_horizons) * len(distributions_type) * len(epsilons) * len(hs)
    write_line(output_file, str(num_experiments) + " " + str(num_simulations) + " " + str(seed))

    logperc = LogPerc(num_experiments)
    exp_index = 0
    for d_type, epsilon, h, T in itertools.product(distributions_type, epsilons, hs, time_horizons):
        algs = [
            "ucb1 round 2 ucb1 ucb1",
            "cusum_ucb cd_algorithm 0 1 0 30 0 1 5 cusum 30 {0} {1} 1 9 round round 6 alg_with_exploration alg_with_exploration 0 2 ucb1 ucb1".format(epsilon, h),
            "cusum_ucb_history_all cd_algorithm 2 1 0 30 100000 1 5 cusum 30 {0} {1} 1 9 round round 6 alg_with_exploration alg_with_exploration 0 2 ucb1 ucb1".format(epsilon, h)
        ]

        write_line(output_file, str(exp_index) + " " + str(mab_type) + " 2 " + str(len(algs)) + " " + str(T))

        if d_type == 0:
            low = 0.2
            high = 0.8
        elif d_type == 1:
            low = 0.7
            high = 0.8
        elif d_type == 2:
            low = 0.1
            high = 0.2
        s_1 = "BernoulliNonStationary 1 " + str(low) + " " + str(T)
        s_2 = "BernoulliNonStationary 1 " + str(high) + " " + str(T)
        write_line(output_file, s_1)
        write_line(output_file, s_2)

        for alg in algs:
            write_line(output_file, alg)

        exp_index += 1
        logperc.log(exp_index)


def main():
    output_file = open("exp_config/experiments_config.txt", "w")


    algs = [
        "ucb1 round 2 ucb1 ucb1",
        "d_ucb _d_ucb",
        "sw_ucb _sw_ucb",
        "adapt_eve _adapt_eve",
        "cusum_ucb _cd_algorithm 0 1 0",
        "cusum_ucb_history_log _cd_algorithm 1 1 0",
        "cusum_ucb_history_all _cd_algorithm 2 1 0"
    ]
    num_simulations = 100
    num_exp_per_config = 5

    experiment = int(sys.argv[1])
    seed = int(sys.argv[2])
    random.seed(seed)

    if experiment == 1:
        gen_exp_various(output_file, algs, num_simulations, seed, num_exp_per_config)
    elif experiment == 2:
        gen_exp_adversarial(output_file, algs, num_simulations, seed, num_exp_per_config)
    elif experiment == 3:
        gen_exp_changetype(output_file, algs, num_simulations, seed)
    elif experiment == 4:
        gen_exp_false_alarms(output_file, algs, num_simulations, seed)


main()
