output_file = open("experiments_config.txt", "w")

def write_line(f, line):
    f.write(line)
    f.write("\n")

num_simulations = 100
num_timesteps = 1000
seed = 661
mab_type = 0 # 0 for stochastic, 1 for adversarial

gm = [
    [1,3,5,7],
    [1,2,3,7],
    [3,5,6,7],

    [1,3,5,7],
    [1,2,3,7],
    [3,5,6,7],

    [1,3,5,7],
    [1,2,3,7],
    [3,5,6,7],

    [1,3,5,7],
    [1,2,3,7],
    [3,5,6,7],

    [1,3,5,7],
    [1,2,3,7],
    [3,5,6,7],

    [1,3,5,7],
    [1,2,3,7],
    [3,5,6,7]
]

gd = [
    [0.6,0.6,0.6,0.6],
    [0.6,0.6,0.6,0.6],
    [0.6,0.6,0.6,0.6],

    [2,2,2,2],
    [2,2,2,2],
    [2,2,2,2],

    [4,4,4,4],
    [4,4,4,4],
    [4,4,4,4],

    [1,2,3,4],
    [1,2,3,4],
    [1,2,3,4],

    [4,3,2,1],
    [4,3,2,1],
    [4,3,2,1],

    [1,3,5,7],
    [7,5,3,1],
    [1,3,5,7]
]

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
