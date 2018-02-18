#include "experimentloader.h"
#include <string>
#include <sstream>
#include <fstream>

vector<Experiment*>* ExperimentLoader::load_experiments() {
	ifstream input_file("experiments_config.txt", ifstream::in);

	int num_experiments;
  input_file >> num_experiments;

	vector<Experiment*>* experiments = new vector<Experiment*>();

	while (num_experiments--) {
		int num_simulations, timesteps, seed, mab_type, num_arms, num_algs;

		input_file >> num_simulations >> timesteps >> seed >> mab_type >> num_arms >> num_algs;

		boost::mt19937* rng = new boost::mt19937(seed);
		//srand(seed);

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
			parse_arm_line(mab, line, rng);
		}

		experiment->set_mab(mab);

		for (int alg_i = 0; alg_i < num_algs; alg_i++) {
			getline(input_file, line);
			parse_alg_line(experiment, mab, line, rng);
		}

		experiments->push_back(experiment);
	}

	return experiments;
}

void ExperimentLoader::parse_arm_line(MAB* mab, string line, boost::mt19937* rng) {
	stringstream ss(line);

	char distr;
	ss >> distr;

	if (distr == 'g') { // Gaussian
		double mean, variance;
		ss >> mean >> variance;

		mab->addArm(new NormalDistribution("G", mean, variance, *rng));
	} else if (distr == 'u') { // Uniform
		double v;
		ss >> v;

		mab->addArm(new UniformDistribution("U", v, *rng));
	} else if (distr == 'b') { // Bernoulli
		double v, p;
		ss >> v >> p;

		mab->addArm(new BernoulliDistribution("B", p, v, *rng));
	}
}

void ExperimentLoader::parse_alg_line(Experiment* experiment, MAB* mab, string line, boost::mt19937* rng) {
	stringstream ss(line);

	string alg_name;
	ss >> alg_name;

	if (alg_name == "ucb1") { // Gaussian
		experiment->add_alg(new UCB1("UCB1", *mab));
	} else if (alg_name == "ucbt") { // Uniform
		experiment->add_alg(new UCBT("UCBT", *mab));
	} else if (alg_name == "glie") {
		double epsilon;
		ss >> epsilon;
		experiment->add_alg(new GLIE("GLIE", *mab, epsilon));
	} else if (alg_name == "ts_g") { // Thompson Sampling for gaussians
		experiment->add_alg(new ThompsonSamplingGaussian("TS_G", *mab, *rng));
	} else if (alg_name == "ts_b") { // Thompson Sampling for bernoullis
		experiment->add_alg(new ThompsonSamplingBernoulli("TS_B", *mab, *rng));
	}
}
