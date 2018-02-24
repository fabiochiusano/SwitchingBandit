import random
import sys

def environment_gaussian():
    """
            GAUSSIAN TESTS
    """
    mab_type = 0 # 0 for stochastic, 1 for adversarial

    gm = []
    gd = []

    min_num_arms = 2
    max_num_arms = 8
    min_mean = -5
    max_mean = 5
    min_var = 0.2
    max_var = 10

    for i in range(num_experiments):
        num_arms = random.randint(min_num_arms, max_num_arms)

        means = []
        for j in range(num_arms):
            means.append(random.uniform(min_mean, max_mean))
        gm.append(means)

        variances = []
        for j in range(num_arms):
            variances.append(random.uniform(min_var, max_var))
        gd.append(variances)

    # TODO: Add support for algorithms with name (here, in Experimentloader, in plot_results)
    algorithms = ["ucb1 ucb1", "ucbt ucbt", "ts_g ts_g", "exp3_0.02_0.02 exp3 0.02 0.02", "exp3_0.02_0.002 exp3 0.02 0.002", "exp3_0.002_0.02 exp3 0.002 0.02", "exp3_0.2_0.02 exp3 0.2 0.02", "exp3_0.02_0.2 exp3 0.02 0.2", "exp3_0.2_0.2 exp3 0.2 0.2"]


    write_line(output_file, str(len(gm)))

    for i in range(len(gm)):
        arms = []
        for j in range(len(gm[i])):
            arms.append(distr_names["gaussian"] + " " + str(gm[i][j]) + " " + str(gd[i][j]))

        write_line(output_file, str(num_simulations) + " " + str(num_timesteps) + " " + str(seed) + " " + str(mab_type) + " " + str(len(gm[i])) + " " + str(len(algorithms)));

        for line in arms:
            write_line(output_file, line)

        for line in algorithms:
            write_line(output_file, line)

def environment_bernoulli():
    """
            BERNOULLI TESTS
    """
    mab_type = 0 # 0 for stochastic, 1 for adversarial

    p_arr = []

    min_num_arms = 2
    max_num_arms = 8
    min_p = 0.1
    max_p = 0.9

    for i in range(num_experiments):
        num_arms = random.randint(min_num_arms, max_num_arms)

        ps = []
        for j in range(num_arms):
            ps.append(random.uniform(min_p, max_p))
        p_arr.append(ps)


    algorithms = ["ucb1 ucb1", "ucbt ucbt", "glie_1 glie 1", "ts_g ts_g", "ts_b ts_b"]

    write_line(output_file, str(len(p_arr)))

    for i in range(len(p_arr)):
        arms = []
        for j in range(len(p_arr[i])):
            arms.append(distr_names["bernoulli"] + " " + str(p_arr[i][j]))

        write_line(output_file, str(num_simulations) + " " + str(num_timesteps) + " " + str(seed) + " " + str(mab_type) + " " + str(len(p_arr[i])) + " " + str(len(algorithms)))

        for line in arms:
            write_line(output_file, line)

        for line in algorithms:
            write_line(output_file, line)

def environment_adversarial():
    """
            ADVERSARIAL TESTS
    """
    mab_type = 1 # 0 for stochastic, 1 for adversarial

    v_arr = []
    cur_v_arr = []

    min_num_arms = 2
    max_num_arms = 3
    min_v = 1
    max_v = 1

    for i in range(num_experiments):
        num_arms = random.randint(min_num_arms, max_num_arms)

        vs = []
        cur_vs = []
        for j in range(num_arms):
            vs.append(random.uniform(min_v, max_v))
            if j%2 == 0:
                cur_vs.append(0.0)
            else:
                cur_vs.append(1.0)
        v_arr.append(vs)
        cur_v_arr.append(cur_vs)


    algorithms = ["ucb1 ucb1", "ucbt ucbt", "ts_g ts_g", "exp3_0.02_0.02 exp3 0.02 0.02", "exp3_0.02_0.002 exp3 0.02 0.002", "exp3_0.002_0.02 exp3 0.002 0.02", "exp3_0.2_0.02 exp3 0.2 0.02", "exp3_0.02_0.2 exp3 0.02 0.2", "exp3_0.2_0.2 exp3 0.2 0.2"]

    write_line(output_file, str(len(v_arr)))

    for i in range(len(v_arr)):
        arms = []
        for j in range(len(v_arr[i])):
            arms.append(distr_names["squarewave"] + " " + str(v_arr[i][j]) + " " + str(cur_v_arr[i][j]))

        write_line(output_file, str(num_simulations) + " " + str(num_timesteps) + " " + str(seed) + " " + str(mab_type) + " " + str(len(v_arr[i])) + " " + str(len(algorithms)))

        for line in arms:
            write_line(output_file, line)

        for line in algorithms:
            write_line(output_file, line)

def environment_bernoulli_non_stationary():
    """
            BERNOULLI NON STATIONARY TESTS
    """
    mab_type = 0 # 0 for stochastic, 1 for adversarial

    ps_arr = []
    ends_arr = []

    min_num_arms = 2
    max_num_arms = 3
    min_distr_changes = 1
    max_distr_changes = 2
    min_p = 0.1
    max_p = 0.9

    for i in range(num_experiments):
        num_arms = random.randint(min_num_arms, max_num_arms) # inclusive

        ps = []
        ends = []
        for j in range(num_arms):
            num_distr_changes = random.randint(min_distr_changes, max_distr_changes)

            p = []
            for k in range(num_distr_changes):
                p.append(random.uniform(min_p, max_p))
            ps.append(p)

            end = []
            for k in range(num_distr_changes-1):
                end.append(random.randint(1, num_timesteps-1))
            end.append(num_timesteps)
            end = sorted(end)
            ends.append(end)

        ps_arr.append(ps)
        ends_arr.append(ends)


    algorithms = ["ucb1 ucb1", "ucbt ucbt", "ts_b ts_b"]
    Bs = [1]
    taus = [500, 1000, 2000, 3000]
    epsilons = [2]
    for B in Bs:
        for tau in taus:
            for epsilon in epsilons:
                algorithms.append("sw_ucb" + "_" + str(B) + "_" + str(tau) + "_" + str(epsilon) + " sw_ucb" + " " + str(B) + " " + str(tau) + " " + str(epsilon))

    write_line(output_file, str(len(ps_arr)))

    for i in range(len(ps_arr)):
        arms = []

        for j in range(len(ps_arr[i])):
            arms.append(distr_names["bernoullinonstat"] + " " + str(len(ps_arr[i][j])) + " ")
            for k in range(len(ps_arr[i][j])):
                arms[j] += str(ps_arr[i][j][k]) + " " + str(ends_arr[i][j][k]) + " "

        write_line(output_file, str(num_simulations) + " " + str(num_timesteps) + " " + str(seed) + " " + str(mab_type) + " " + str(len(ps_arr[i])) + " " + str(len(algorithms)))

        for line in arms:
            write_line(output_file, line)

        for line in algorithms:
            write_line(output_file, line)



output_file = open("experiments_config.txt", "w")

def write_line(f, line):
    f.write(line)
    f.write("\n")

distr_names = {
    "gaussian": "Gaussian",
    "bernoulli": "Bernoulli",
    "squarewave": "SquareWave",
    "uniform": "Uniform",
    "uniformnonstat": "UniformNonStationary",
    "bernoullinonstat": "BernoulliNonStationary"
}

if len(sys.argv) != 6:
    print("Arguments:")
    print("     num_experiments")
    print("     num_simulations")
    print("     num_timesteps")
    print("     seed")
    print("     environment")
    print("Implemented environments:")
    print("     gaussian")
    print("     bernoulli")
    print("     adversarial")
    print("     bernoulli_non_stat")
else:
    # TODO: make environments editable with GUI

    num_experiments = int(sys.argv[1])
    num_simulations = int(sys.argv[2])
    num_timesteps = int(sys.argv[3])
    seed = int(sys.argv[4])
    environment = sys.argv[5]

    if environment == "gaussian":
        environment_gaussian()
    elif environment == "bernoulli":
        environment_bernoulli()
    elif environment == "adversarial":
        environment_adversarial()
    elif environment == "bernoulli_non_stat":
        environment_bernoulli_non_stationary()
    else:
        print("No environment written!")
