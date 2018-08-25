#include "experimentloader.h"
#include <string>
#include <sstream>
#include <fstream>
#include "allalgorithms.h"
#include <map>

vector<Experiment*>* ExperimentLoader::load_experiments() {
	ifstream input_file("exp_config/experiments_config.txt", ifstream::in);

	int num_experiments, num_simulations, seed;
  input_file >> num_experiments >> num_simulations >> seed;

	vector<Experiment*>* experiments = new vector<Experiment*>();

	ofstream cdt_file("temp/cdt.txt");

	while (num_experiments--) {
		boost::mt19937* rng = new boost::mt19937(seed);

		// BUG: Actually this should be set at the beginning of each experiment for each thread independently
		srand(seed);

		string exp_name;
		int mab_type, num_arms, num_algs, timesteps;

		input_file >> exp_name >> mab_type >> num_arms >> num_algs >> timesteps;

		Experiment* experiment = new Experiment(exp_name, num_simulations, timesteps, seed);

		if (mab_type == 0) {
			experiment->set_mab_type(RegretType::Stochastic);
		} else {
			experiment->set_mab_type(RegretType::Adversarial);
		}

		string line;
		getline(input_file, line); // Read till newline character

		vector<Distribution*> distributions;
		for (int arm_i = 0; arm_i < num_arms; arm_i++) {
			getline(input_file, line);
			distributions.push_back(get_distribution(line, rng));
		}

		MAB* mab = new MAB(distributions, timesteps);
		experiment->set_mab(mab);

		for (int alg_i = 0; alg_i < num_algs; alg_i++) {
			getline(input_file, line);
			experiment->add_alg(get_algorithm(line, rng, mab));
		}

		experiments->push_back(experiment);
	}

	return experiments;
}
