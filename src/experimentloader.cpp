#include "experimentloader.h"
#include <string>
#include <sstream>
#include <fstream>
#include "allalgorithms.h"

vector<Experiment*>* ExperimentLoader::load_experiments() {
	ifstream input_file("temp/experiments_config.txt", ifstream::in);

	int num_experiments, num_simulations, timesteps, seed;
  input_file >> num_experiments >> num_simulations >> timesteps >> seed;

	vector<Experiment*>* experiments = new vector<Experiment*>();

	while (num_experiments--) {
		 // This should be put inside the while loop, otherwise segmentation error may
		 // happen since it's shared across threads
		boost::mt19937* rng = new boost::mt19937(seed);

		// BUG: Actually this should be set at the beginning of each experiment for each thread independently
		srand(seed);

		int mab_type, num_arms, num_algs;

		input_file >> mab_type >> num_arms >> num_algs;

		Experiment* experiment = new Experiment(num_simulations, timesteps, seed);

		MABType* mt;
		if (mab_type == 0) {
			mt = new MABType(MABType::Stochastic);
		} else {
			mt = new MABType(MABType::Adversarial);
		}
		MAB* mab = new MAB(mt);

		string line;
		getline(input_file, line); // Read till newline character

		for (int arm_i = 0; arm_i < num_arms; arm_i++) {
			getline(input_file, line);
			mab->addArm(get_distribution(line, rng));
		}

		experiment->set_mab(mab);

		for (int alg_i = 0; alg_i < num_algs; alg_i++) {
			getline(input_file, line);
			experiment->add_alg(get_algorithm(mab, line, rng));
		}

		experiments->push_back(experiment);
	}

	return experiments;
}
