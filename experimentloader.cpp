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
		//srand(seed); // BUG: Actually this should be set at the beginning of each experiment for each thread independently

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

// TODO: use reflection for loading distributions and algorithms

void ExperimentLoader::parse_arm_line(MAB* mab, string line, boost::mt19937* rng) {
	stringstream ss(line);

	string distr;
	ss >> distr;

	if (distr == "Gaussian") { // Gaussian
		double mean, variance;
		ss >> mean >> variance;

		mab->addArm(new NormalDistribution("G", mean, variance, *rng));
	} else if (distr == "Uniform") { // Uniform
		double v;
		ss >> v;

		mab->addArm(new UniformDistribution("U", v, *rng));
	} else if (distr == "UniformNonStationary") { // Uniform
		cout << "NOT IMPLEMENTED UniformNonStationary" << endl;
		// TODO: implement UniformNonStationary in experimentloader
	} else if (distr == "Bernoulli") { // Bernoulli
		double p;
		ss >> p;

		mab->addArm(new BernoulliDistribution("B", p, *rng));
	} else if (distr == "BernoulliNonStationary") { // Bernoulli Non Stationary
		int distr_changes;
		vector<double>* ps = new vector<double>();
		vector<int>* ends = new vector<int>();

		ss >> distr_changes;
		for (int i = 0; i < distr_changes; i++) {
			double p;
			int end;
			ss >> p >> end;
			ps->push_back(p);
			ends->push_back(end);
		}

		mab->addArm(new BernoulliNonStationaryDistribution("BNS", *ps, *ends, *rng));
	} else if (distr == "SquareWave") { // SquareWave
		double v, cur_v;
		ss >> v >> cur_v;

		mab->addArm(new SquareWaveDistribution("SW", v, cur_v, *rng));
	}
}

void ExperimentLoader::parse_alg_line(Experiment* experiment, MAB* mab, string line, boost::mt19937* rng) {
	stringstream ss(line);

	string name, alg_name;
	ss >> name >> alg_name;

	if (alg_name == "ucb1") { // Gaussian
		experiment->add_alg(new UCB1(name, *mab));
	} else if (alg_name == "ucbt") { // Uniform
		experiment->add_alg(new UCBT(name, *mab));
	} else if (alg_name == "glie") {
		double epsilon;
		ss >> epsilon;
		experiment->add_alg(new GLIE(name, *mab, epsilon));
	} else if (alg_name == "ts_g") { // Thompson Sampling for gaussians
		experiment->add_alg(new ThompsonSamplingGaussian(name, *mab, *rng));
	} else if (alg_name == "ts_b") { // Thompson Sampling for bernoullis
		experiment->add_alg(new ThompsonSamplingBernoulli(name, *mab, *rng));
	} else if (alg_name == "exp3") { // EXP3
		double beta, nu;
		ss >> beta >> nu;
		experiment->add_alg(new EXP3(name, *mab, beta, nu));
	} else if (alg_name == "d_ucb") { // D_UCB
		double B, gamma, epsilon;
		ss >> B >> gamma >> epsilon;
		experiment->add_alg(new D_UCB(name, *mab, gamma, B, epsilon));
	} else if (alg_name == "sw_ucb") { // SW_UCB
		int tau;
		double B, epsilon;
		ss >> B >> tau >> epsilon;
		experiment->add_alg(new SW_UCB(name, *mab, tau, B, epsilon));
	}
}
