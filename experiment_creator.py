import random

output_file = open("experiments_config.txt", "w")

def write_line(f, line):
    f.write(line)
    f.write("\n")

num_simulations = 100
num_timesteps = 4000
seed = 661
mab_type = 0 # 0 for stochastic, 1 for adversarial


"""
        GAUSSIAN TESTS
"""
"""
gm = []
gd = []

num_experiments = 50
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


algorithms = ["ucb1", "ucbt", "glie 1", "ts_g"]


write_line(output_file, str(len(gm)))

for i in range(len(gm)):
    arms = []
    for j in range(len(gm[i])):
        arms.append("g " + str(gm[i][j]) + " " + str(gd[i][j]))

    write_line(output_file, str(num_simulations) + " " + str(num_timesteps) + " " + str(seed) + " " + str(mab_type) + " " + str(len(gm[i])) + " " + str(len(algorithms)));

    for line in arms:
        write_line(output_file, line)

    for line in algorithms:
        write_line(output_file, line)
"""

"""
        BERNOULLI TESTS
"""

p_arr = []
v_arr = []

num_experiments = 20
min_num_arms = 2
max_num_arms = 8
min_p = 0.1
max_p = 0.8
min_v = 0.5
max_v = 8

for i in range(num_experiments):
    num_arms = random.randint(min_num_arms, max_num_arms)

    ps = []
    for j in range(num_arms):
        ps.append(random.uniform(min_p, max_p))
    p_arr.append(ps)

    vs = []
    for j in range(num_arms):
        vs.append(random.uniform(min_v, max_v))
    v_arr.append(vs)


algorithms = ["ucb1", "ucbt", "glie 1", "ts_g", "ts_b"]


write_line(output_file, str(len(v_arr)))

for i in range(len(v_arr)):
    arms = []
    for j in range(len(v_arr[i])):
        arms.append("b " + str(v_arr[i][j]) + " " + str(p_arr[i][j]))

    write_line(output_file, str(num_simulations) + " " + str(num_timesteps) + " " + str(seed) + " " + str(mab_type) + " " + str(len(v_arr[i])) + " " + str(len(algorithms)));

    for line in arms:
        write_line(output_file, line)

    for line in algorithms:
        write_line(output_file, line)
