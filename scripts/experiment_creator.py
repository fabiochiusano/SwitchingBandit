import random
import sys
import itertools

def environment_bernoulli_non_stationary(output_file, num_experiments, num_simulations, seed):
    """
            BERNOULLI NON STATIONARY TESTS
    """

    def write_line(f, line):
        f.write(line)
        f.write("\n")

    mab_type = 0 # 0 for stochastic, 1 for adversarial

    ps_arr = []
    ends_arr = []

    class Setting:
        def __init__(self, arms_interval, changes_interval, p_interval, T):
            self.arms_interval = arms_interval
            self.changes_interval = changes_interval
            self.p_interval = p_interval
            self.T = T

    arms_intervals = [(2, 4), (5, 9), (10, 50)]
    changes_intervals = [(1, 3), (4, 8), (9, 20)]
    p_intervals = [(0.1, 0.9)]
    T_intervals = [10000, 100000, 1000000]
    settings = []
    for element in itertools.product(arms_intervals, changes_intervals, p_intervals, T_intervals):
        settings.append(Setting(element[0], element[1], element[2], element[3]))

    for i in range(num_experiments):
        setting_index = int(i * len(settings) * 1.0 / num_experiments)
        setting = settings[setting_index]

        num_arms = random.randint(setting.arms_interval[0], setting.arms_interval[1]) # inclusive

        ps = []
        ends = []
        for j in range(num_arms):
            num_distr_changes = random.randint(setting.changes_interval[0], setting.changes_interval[1])

            p = []
            for k in range(num_distr_changes):
                p.append(random.uniform(setting.p_interval[0], setting.p_interval[1]))
            ps.append(p)

            end = []
            for k in range(num_distr_changes-1):
                end.append(random.randint(1, setting.T-1))
            end.append(setting.T)
            end = sorted(end)
            ends.append(end)

        ps_arr.append(ps)
        ends_arr.append(ends)


    algorithms = [
        "ucb1 ucb1",
        "sw_ucb round 5 sw_ucb sw_ucb 2 2000 2",
        "d_ucb round 5 d_ucb d_ucb 2 0.999 2",
        "cusum_2_b cd_algorithm 15 0 100 5 cusum 30 0.1 25 0 9 round round 6 alg_with_exploration alg_with_exploration 0.01 2 ucb1 ucb1",
        "cusum_hist_2_b cd_algorithm 15 1 100 5 cusum 30 0.1 25 0 9 round round 6 alg_with_exploration alg_with_exploration 0.01 2 ucb1 ucb1"
    ]

    write_line(output_file, str(num_experiments) + " " + str(num_simulations) + " " + str(seed))

    for i in range(num_experiments):
        setting_index = int(i * len(settings) * 1.0 / num_experiments)
        setting = settings[setting_index]
        print(setting_index)
        arms = []

        for j in range(len(ps_arr[i])):
            arms.append("BernoulliNonStationary" + " " + str(len(ps_arr[i][j])) + " ")
            for k in range(len(ps_arr[i][j])):
                arms[j] += "{0:.2f} {1} ".format(ps_arr[i][j][k], ends_arr[i][j][k])

        write_line(output_file, str(i) + " " + str(mab_type) + " " + str(len(ps_arr[i])) + " " + str(len(algorithms)) + " " + str(setting.T))

        for line in arms:
            write_line(output_file, line)

        for line in algorithms:
            write_line(output_file, line)

def main():
    output_file = open("exp_config/experiments_config.txt", "w")

    if len(sys.argv) != 5:
        print("Arguments:")
        print("     num_experiments")
        print("     num_simulations")
        print("     seed")
        print("     environment")
        print("Implemented environments:")
        print("     bernoulli_non_stat")
    else:
        # TODO: make environments editable with GUI

        num_experiments = int(sys.argv[1])
        num_simulations = int(sys.argv[2])
        seed = int(sys.argv[3])
        environment = sys.argv[4]

        if environment == "bernoulli_non_stat":
            environment_bernoulli_non_stationary(output_file, num_experiments, num_simulations, seed)
        else:
            print("Environment not found!")

main()
