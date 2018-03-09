#include "utils.h"
#include "allalgorithms.h"
#include <stdlib.h>
#include <string>
#include <sstream>

using namespace std;

double random_unit() {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

MABAlgorithm* get_algorithm(MAB* mab, string line, boost::mt19937* rng) {
	stringstream ss(line);

	string name, alg_name;
	ss >> name >> alg_name;

	if (alg_name == "ucb1") { // Gaussian
		return new UCB1(name, *mab);
	} else if (alg_name == "ucb1_with_exploration") { // Uniform
		double alpha;
		ss >> alpha;
		return new UCB1_With_Exploration(name, *mab, alpha);
	} else if (alg_name == "ucbt") { // Uniform
		return new UCBT(name, *mab);
	} else if (alg_name == "glie") {
		double epsilon;
		ss >> epsilon;
		return new GLIE(name, *mab, epsilon);
	} else if (alg_name == "ts_g") { // Thompson Sampling for gaussians
		return new ThompsonSamplingGaussian(name, *mab, *rng);
	} else if (alg_name == "ts_b") { // Thompson Sampling for bernoullis
		return new ThompsonSamplingBernoulli(name, *mab, *rng);
	} else if (alg_name == "exp3") { // EXP3
		double beta, nu;
		ss >> beta >> nu;
		return new EXP3(name, *mab, beta, nu);
	} else if (alg_name == "d_ucb") { // D_UCB
		double B, gamma, epsilon;
		ss >> B >> gamma >> epsilon;
		return new D_UCB(name, *mab, gamma, B, epsilon);
	} else if (alg_name == "sw_ucb") { // SW_UCB
		int tau;
		double B, epsilon;
		ss >> B >> tau >> epsilon;
		return new SW_UCB(name, *mab, tau, B, epsilon);
	} else if (alg_name == "exp3_s") { // EXP3_S
		double beta, alpha;
		ss >> beta >> alpha;
		return new EXP3_S(name, *mab, beta, alpha);
	} else if (alg_name == "rexp3") { // REXP3
		double beta;
		int window_size;
		ss >> beta >> window_size;
		return new REXP3(name, *mab, beta, window_size);
	} else if (alg_name == "adapt_eve") { // Adapt_EvE
		int meta_duration;
		int num_cdt_params, num_sub_alg_params;

		ss >> meta_duration;

		ss >> num_cdt_params;
		string cdt_line;
		for (int i = 0; i < num_cdt_params; i++) {
			string temp;
			ss >> temp;
			cdt_line += temp + " ";
		}

		ss >> num_sub_alg_params;
		string sub_alg_line;
		for (int i = 0; i < num_sub_alg_params; i++) {
			string temp;
			ss >> temp;
			sub_alg_line += temp + " ";
		}

		return new ADAPT_EVE(name, *mab, meta_duration, cdt_line, sub_alg_line, *rng);
	} else if (alg_name == "global_cts") { // GlobalCTS
		double gamma;
		ss >> gamma;
		return new GlobalCTS(name, *mab, *rng, gamma);
	} else if (alg_name == "per_arm_cts") { // PerArmCTS
		double gamma;
		ss >> gamma;
		return new PerArmCTS(name, *mab, *rng, gamma);
	} else if (alg_name == "cd_algorithm") { // CD_Algorithm
		int num_cdt_params, num_sub_alg_params;

		ss >> num_cdt_params;
		string cdt_line;
		for (int i = 0; i < num_cdt_params; i++) {
			string temp;
			ss >> temp;
			cdt_line += temp + " ";
		}

		ss >> num_sub_alg_params;
		string sub_alg_line;
		for (int i = 0; i < num_sub_alg_params; i++) {
			string temp;
			ss >> temp;
			sub_alg_line += temp + " ";
		}

		return new CD_Algorithm(name, *mab, cdt_line, sub_alg_line, *rng);
	}
	else {
		cout << "Not valid algorithm: " << line << endl;
		return new UCB1(name, *mab);
	}
}

CDT* get_cdt(string line) {
	stringstream ss(line);

	string cdt_name;
	ss >> cdt_name;

	if (cdt_name == "ph") {
		double gamma, lambda;
		ss >> gamma >> lambda;
		return new CDT_PH(gamma, lambda);
	} else if (cdt_name == "ph_rho") {
		double gamma, lambda, rho;
		ss >> gamma >> lambda >> rho;
		return new CDT_PH_RHO(gamma, lambda, rho);
	} else if (cdt_name == "cusum") {
		int M;
		double epsilon, threshold;
		ss >> M >> epsilon >> threshold;
		return new Two_Sided_CUSUM(M, epsilon, threshold);
	} else {
		cout << "Not valid cdt: " << line << endl;
		return new CDT_PH(0, 0);
	}
}

Distribution* get_distribution(string line, boost::mt19937* rng) {
	stringstream ss(line);

	string distr;
	ss >> distr;

	if (distr == "Gaussian") { // Gaussian
		double mean, variance;
		ss >> mean >> variance;

		return new NormalDistribution("G", mean, variance, *rng);
	} else if (distr == "Uniform") { // Uniform
		double v;
		ss >> v;

		return new UniformDistribution("U", v, *rng);
	} else if (distr == "UniformNonStationary") { // Uniform
		cout << "NOT IMPLEMENTED UniformNonStationary" << endl;
		// TODO: implement UniformNonStationary in experimentloader
	} else if (distr == "Bernoulli") { // Bernoulli
		double p;
		ss >> p;

		return new BernoulliDistribution("B", p, *rng);
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

		return new BernoulliNonStationaryDistribution("BNS", *ps, *ends, *rng);
	} else if (distr == "SquareWave") { // SquareWave
		double v, cur_v;
		ss >> v >> cur_v;

		return new SquareWaveDistribution("SW", v, cur_v, *rng);
	} else {
		cout << "Not valid distribution: " << line << endl;
		return new NormalDistribution("G", 0, 1, *rng);
	}
}
