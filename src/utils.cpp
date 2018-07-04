#include "utils.h"
#include "allalgorithms.h"
#include <stdlib.h>
#include <string>
#include <sstream>

using namespace std;

double random_unit() {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

MABAlgorithm* get_algorithm(string line, int num_of_arms, boost::mt19937* rng) {
	stringstream ss(line);

	string name, alg_name;
	ss >> name >> alg_name;

	if (alg_name == "ucb1") { // Gaussian
		return new UCB1(name, num_of_arms);
	} else if (alg_name == "alg_with_exploration") { // Uniform
		int num_sub_alg_params;
		ss >> num_sub_alg_params;
		string sub_alg_line;
		for (int i = 0; i < num_sub_alg_params; i++) {
			string temp;
			ss >> temp;
			sub_alg_line += temp + " ";
		}

		double alpha;
		ss >> alpha;

		return new Algorithm_With_Uniform_Exploration(name, num_of_arms, sub_alg_line, alpha, *rng);
	} else if (alg_name == "round") {
		int num_sub_alg_params;
		ss >> num_sub_alg_params;
		string sub_alg_line;
		for (int i = 0; i < num_sub_alg_params; i++) {
			string temp;
			ss >> temp;
			sub_alg_line += temp + " ";
		}

		return new Round_Algorithm(name, num_of_arms, sub_alg_line, *rng);
	} else if (alg_name == "ucbt") { // Uniform
		return new UCBT(name, num_of_arms);
	} /*else if (alg_name == "glie") {
		double epsilon;
		ss >> epsilon;
		return new GLIE(name, num_of_arms, epsilon);
	}*/ else if (alg_name == "ts_g") { // Thompson Sampling for gaussians
		return new ThompsonSamplingGaussian(name, num_of_arms, *rng);
	} else if (alg_name == "ts_b") { // Thompson Sampling for bernoullis
		return new ThompsonSamplingBernoulli(name, num_of_arms, *rng);
	} else if (alg_name == "exp3") { // EXP3
		double beta, nu;
		ss >> beta >> nu;
		return new EXP3(name, num_of_arms, beta, nu);
	} else if (alg_name == "d_ucb") { // D_UCB
		double B, gamma, epsilon;
		ss >> B >> gamma >> epsilon;
		return new D_UCB(name, num_of_arms, gamma, B, epsilon);
	} else if (alg_name == "sw_ucb") { // SW_UCB
		int tau;
		double B, epsilon;
		ss >> B >> tau >> epsilon;
		return new SW_UCB(name, num_of_arms, tau, B, epsilon);
	} else if (alg_name == "exp3_s") { // EXP3_S
		double beta, alpha;
		ss >> beta >> alpha;
		return new EXP3_S(name, num_of_arms, beta, alpha);
	} else if (alg_name == "rexp3") { // REXP3
		double beta;
		int window_size;
		ss >> beta >> window_size;
		return new REXP3(name, num_of_arms, beta, window_size);
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

		return new ADAPT_EVE(name, num_of_arms, meta_duration, cdt_line, sub_alg_line, *rng);
	} else if (alg_name == "global_cts") { // GlobalCTS
		double gamma;
		ss >> gamma;
		return new GlobalCTS(name, num_of_arms, *rng, gamma);
	} else if (alg_name == "per_arm_cts") { // PerArmCTS
		double gamma;
		ss >> gamma;
		return new PerArmCTS(name, num_of_arms, *rng, gamma);
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

		int use_history;
		ss >> use_history;

		double alpha;
		ss >> alpha;

		return new CD_Algorithm(name, num_of_arms, cdt_line, sub_alg_line, use_history==1, *rng);
	} /*else if (alg_name == "glr") {
		int M, mod;
		double alpha;

		ss >> M >> mod >> alpha;
		return new GLR(name, num_of_arms, M, mod, alpha, *rng);
	}*/
	else {
		cout << "Not valid algorithm: " << line << endl;
		return new UCB1(name, num_of_arms);
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
		int gaussian;
		ss >> M >> epsilon >> threshold >> gaussian;
		return new Two_Sided_CUSUM(M, epsilon, threshold, gaussian!=0);
	} else if (cdt_name == "ici") {
		int S0, nu;
		double gamma;

		ss >> S0 >> nu >> gamma;
		return new ICI(S0, nu, gamma);
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
	} else if (distr == "NormalNonStationary") {
		int distr_changes;
		vector<double>* means = new vector<double>();
		vector<double>* stddevs = new vector<double>();
		vector<int>* ends = new vector<int>();

		ss >> distr_changes;
		for (int i = 0; i < distr_changes; i++) {
			double mean, stddev;
			int end;
			ss >> mean >> stddev >> end;
			means->push_back(mean);
			stddevs->push_back(stddev);
			ends->push_back(end);
		}

		return new NormalNonStationaryDistribution("NNS", *means, *stddevs, *ends, *rng);
	} else if (distr == "SquareWave") { // SquareWave
		double v, cur_v;
		ss >> v >> cur_v;

		return new SquareWaveDistribution("SW", v, cur_v, *rng);
	} else {
		cout << "Not valid distribution: " << line << endl;
		return new NormalDistribution("G", 0, 1, *rng);
	}
}

double get_moment(vector<double> data, int moment) {
	// mu_{i} = E[X^{i}]
	double tot = 0;
	for (auto d : data) {
		tot += pow(d, moment);
	}
	if (data.size() > 0)
		return tot/data.size();
	return 0;
}

double get_cumulant(vector<double> data, int cumulant) {
	// http://www.scholarpedia.org/article/Cumulants#Examples
	if (cumulant == 1) {
		return get_moment(data, 1);
	} else if (cumulant == 2) {
		return get_moment(data, 2) - pow(get_moment(data, 1), 2);
	} else if (cumulant == 3) {
		return get_moment(data, 3) - 3*get_moment(data, 2)*get_moment(data, 1) + 2*pow(get_moment(data, 1), 3);
	} else {
		printf("CALLING get_cumulant with 'cumulant' parameter outside {1,2,3}");
		printf("Returning 0");
		return 0;
	}
}

void make_dir(string name) {
	system(("mkdir -p " + name).c_str());
}
