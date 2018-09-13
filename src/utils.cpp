#include "utils.h"
#include "allalgorithms.h"
#include <stdlib.h>
#include <string>
#include <sstream>

using namespace std;

double random_unit() {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

MABAlgorithm* get_algorithm(string line, boost::mt19937* rng, MAB* mab) {
	stringstream ss(line);

	string name, alg_name;
	ss >> name >> alg_name;
	bool use_best_parameter = false;

	int num_of_arms = mab->get_num_of_arms();
	int timesteps = mab->timesteps;
	int num_global_changepoints = mab->get_num_changepoints(-1);

	if (alg_name[0] == '_') {
		use_best_parameter = true;
		alg_name.erase(0, 1); // Remove '_' from name
	}

	if (alg_name == "ucb1") { // Gaussian
		return new UCB1(name, num_of_arms);
	} else if (alg_name == "alg_with_exploration") { // Uniform
		double alpha;
		ss >> alpha;

		int num_sub_alg_params;
		ss >> num_sub_alg_params;
		string sub_alg_line;
		for (int i = 0; i < num_sub_alg_params; i++) {
			string temp;
			ss >> temp;
			sub_alg_line += temp + " ";
		}

		return new Algorithm_With_Uniform_Exploration(name, num_of_arms, sub_alg_line, alpha, *rng, mab);
	} else if (alg_name == "round") {
		int num_sub_alg_params;
		ss >> num_sub_alg_params;
		string sub_alg_line;
		for (int i = 0; i < num_sub_alg_params; i++) {
			string temp;
			ss >> temp;
			sub_alg_line += temp + " ";
		}

		return new Round_Algorithm(name, num_of_arms, sub_alg_line, *rng, mab);
	} else if (alg_name == "ucbt") { // Uniform
		return new UCBT(name, num_of_arms);
	}  else if (alg_name == "ts_g") { // Thompson Sampling for gaussians
		return new ThompsonSamplingGaussian(name, num_of_arms, *rng);
	} else if (alg_name == "ts_b") { // Thompson Sampling for bernoullis
		return new ThompsonSamplingBernoulli(name, num_of_arms, *rng);
	} else if (alg_name == "exp3") { // EXP3
		double beta;
		if (use_best_parameter) {
			beta = min(1.0, sqrt((num_of_arms * log(num_of_arms))/((exp(1) - 1) * timesteps)));
		} else {
			ss >> beta;
		}
		return new EXP3(name, num_of_arms, beta);
	} else if (alg_name == "d_ucb") { // D_UCB
		double B, gamma, epsilon;
		if (use_best_parameter) {
				B = 1.0;
				gamma = 1 - 1.0/(4*B) * sqrt(num_global_changepoints * 1.0 / timesteps);
				epsilon = 2;
		} else {
				ss >> B >> gamma >> epsilon;
		}
		cout << "D_UCB " << gamma << endl;
		return new D_UCB(name, num_of_arms, gamma, B, epsilon);
	} else if (alg_name == "sw_ucb") { // SW_UCB
		int tau;
		double B, epsilon;
		if (use_best_parameter) {
				B = 1.0;
				if (num_global_changepoints > 0)
					  tau = int(2*B * sqrt(timesteps * 1.0 * log(timesteps) / num_global_changepoints));
				else
						tau = timesteps;
				epsilon = 2;
		} else {
				ss >> B >> tau >> epsilon;
		}
		cout << "SW_UCB " << tau << endl;
		return new SW_UCB(name, num_of_arms, tau, B, epsilon);
	} else if (alg_name == "exp3_s") { // EXP3_S
		double beta, alpha;
		if (use_best_parameter) {
			beta = min(1.0, sqrt((num_of_arms * log(num_of_arms * timesteps))/(timesteps)));
			alpha = 1.0 / timesteps;
		} else {
			ss >> beta >> alpha;
		}
		return new EXP3_S(name, num_of_arms, beta, alpha);
	} else if (alg_name == "rexp3") { // REXP3
		double beta;
		int window_size;
		if (use_best_parameter) {
			double variation_budget = 1.0; // Not always true, check the paper
			window_size = ceil(pow(num_of_arms * log(num_of_arms), 1.0/3) * pow(timesteps/variation_budget, 2.0/3));
			beta = min(1.0, sqrt((num_of_arms * log(num_of_arms))/((exp(1) - 1) * window_size)));
		} else {
			ss >> beta >> window_size;
		}
		return new REXP3(name, num_of_arms, beta, window_size);
	} else if (alg_name == "adapt_eve") { // Adapt_EvE
		int meta_duration;
		string cdt_line, sub_alg_line;

		if (use_best_parameter) {
			// CDT

			meta_duration = 1000;
			cdt_line = "ph 0.005 80";

			/*
			meta_duration = min(int(mab->get_shortest_interval_between_changepoints(-1) * 1.0 / num_of_arms), 100);
			double epsilon = mab->get_min_change(-1) / 2.0;
			double lambda = mab->get_minimal_grid_gap_bernoulli(-1, epsilon, meta_duration);
			double alpha = sqrt(num_global_changepoints/timesteps * log(timesteps/num_global_changepoints));
			double h = log(timesteps/num_global_changepoints);
			cdt_line = "2cusum " + to_string(meta_duration) + " " + to_string(epsilon) + " " + to_string(h) + " 0";
			*/

			// Sub-algorithm
			//sub_alg_line = "round round 2 ucbt ucbt";
			sub_alg_line = "round round 2 ucb1 ucb1";
		} else {
			int num_cdt_params, num_sub_alg_params;
			ss >> meta_duration;

			ss >> num_cdt_params;
			for (int i = 0; i < num_cdt_params; i++) {
				string temp;
				ss >> temp;
				cdt_line += temp + " ";
			}

			ss >> num_sub_alg_params;
			for (int i = 0; i < num_sub_alg_params; i++) {
				string temp;
				ss >> temp;
				sub_alg_line += temp + " ";
			}
		}

		return new ADAPT_EVE(name, num_of_arms, meta_duration, cdt_line, sub_alg_line, *rng, mab);
	} else if (alg_name == "global_cts") { // GlobalCTS
		double gamma;

		if (use_best_parameter) {
			gamma = num_global_changepoints * 1.0 / timesteps;
		} else {
			ss >> gamma;
		}

		return new GlobalCTS(name, num_of_arms, *rng, gamma);
	} else if (alg_name == "per_arm_cts") { // PerArmCTS
		double gamma, gamma_sum = 0;

		if (use_best_parameter) {
			for (int i = 0; i < num_of_arms; i++) {
				gamma_sum += mab->get_num_changepoints(i) * 1.0 / timesteps;
			}
			gamma = gamma_sum / num_of_arms;
		} else {
			ss >> gamma;
		}

		return new PerArmCTS(name, num_of_arms, *rng, gamma);
	} else if (alg_name == "cd_algorithm") { // CD_Algorithm
		int M, use_history, max_history, use_gaussian_cdt, smart_resets;
		string cdt_line, sub_alg_line;

		ss >> use_history; // 0 -> no history, 1 -> log history, 2 -> inf history
		ss >> use_gaussian_cdt;
		ss >> smart_resets;

		if (use_best_parameter) {
			M = min(int(mab->get_shortest_interval_between_changepoints(-1) * 1.0 / num_of_arms), 30);
			double epsilon = mab->get_min_change(-1) / 2.0;
			double lambda = mab->get_minimal_grid_gap_bernoulli(-1, epsilon, M);
			double alpha;
			if (num_global_changepoints > 0)
					alpha = sqrt(num_global_changepoints * 1.0 / timesteps * log(timesteps * 1.0 /num_global_changepoints));
			else
					alpha = 0;
			//double alpha = 0;
			double h;
			if (num_global_changepoints > 0)
			 		h = log(timesteps * 1.0 / num_global_changepoints);
			else
					h = POS_INF;
			if (use_history == 0) {
					max_history = 0;

					/*
					cout << "M: " << M << endl;
					cout << "epsilon: " << epsilon << endl;
					cout << "lambda: " << lambda << endl;

					double C1m_temp = log(4*epsilon) - 2*log(1 - epsilon) + M*log(2*epsilon);
					int C1m_d = floor(2*epsilon*M);
					for(int i = 1; i <= M; i++) C1m_temp += log(i);
					for(int i = 1; i <= C1m_d; i++) C1m_temp -= log(i);
					for(int i = 1; i <= M - C1m_d; i++) C1m_temp -= log(i);
					cout << "C1m_temp: " << C1m_temp << endl;
					double C1m = log(exp(C1m_temp) + 1);
					cout << "C1m: " << C1m << endl;

					double C1p_temp = log(4*epsilon) - 2*log(1 + epsilon) + M*log(2*epsilon);
					int C1p_d = ceil(2*epsilon*M);
					for(int i = 1; i <= M; i++) C1p_temp += log(i);
					for(int i = 1; i <= C1p_d; i++) C1p_temp -= log(i);
					for(int i = 1; i <= M - C1p_d; i++) C1p_temp -= log(i);
					cout << "C1p_temp: " << C1p_temp << endl;
					double C1p = log(exp(C1p_temp) + 1);
					cout << "C1p: " << C1p << endl;

					double C1 = min(C1m, C1p);
					double C2 = log(3) + 2*exp(-2 * pow(epsilon, 2) * M) / lambda;
					cout << "C1: " << C1 << endl;
					cout << "C2: " << C2 << endl;

					double alpha = num_of_arms * sqrt((C2 * num_global_changepoints)/(C1 * timesteps) * log(timesteps * 1.0 / num_global_changepoints));
					double h = log(timesteps * 1.0 / num_global_changepoints) / C1;
					*/
			} else {
				if (num_global_changepoints > 0){
						if (use_history == 2) max_history = POS_INF;
						else max_history = log(timesteps * 1.0 / num_global_changepoints);
					}
				else
						max_history = POS_INF;
				cout << "max history: " << max_history << endl;
			}
			cdt_line = "cusum " + to_string(M) + " " + to_string(epsilon) + " " + to_string(h) + " " + to_string(use_gaussian_cdt);
			sub_alg_line = "round round 6 alg_with_exploration alg_with_exploration " + to_string(alpha) + " 2 ucb1 ucb1";
			cout << cdt_line << endl;
			cout << sub_alg_line << endl;
		} else {
			ss >> M >> max_history >> use_gaussian_cdt;

			int num_cdt_params, num_sub_alg_params;

			ss >> num_cdt_params;
			for (int i = 0; i < num_cdt_params; i++) {
				string temp;
				ss >> temp;
				cdt_line += temp + " ";
			}

			ss >> num_sub_alg_params;
			for (int i = 0; i < num_sub_alg_params; i++) {
				string temp;
				ss >> temp;
				sub_alg_line += temp + " ";
			}
		}

		return new CD_Algorithm(name, num_of_arms, M, cdt_line, sub_alg_line, use_history!=0, max_history, smart_resets!=0, *rng, mab);
	} /*else if (alg_name == "glr") {
		int M;
		ss >> M;

		int max_history;
		ss >> max_history;

		int num_sub_alg_params;
		ss >> num_sub_alg_params;
		string sub_alg_line;
		for (int i = 0; i < num_sub_alg_params; i++) {
			string temp;
			ss >> temp;
			sub_alg_line += temp + " ";
		}

		return new GLR(name, num_of_arms, M, max_history, sub_alg_line, *rng, mab);
	} */else {
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
	} /*else if (cdt_name == "2cusum") {
		int M;
		double epsilon, threshold;
		int gaussian;
		ss >> M >> epsilon >> threshold >> gaussian;
		return new Two_Sided_CUSUM(M, epsilon, threshold, gaussian!=0);
	} */else if (cdt_name == "cusum") {
		int M;
		double epsilon, threshold;
		int gaussian, increase;
		ss >> M >> epsilon >> threshold >> gaussian >> increase;
		return new CUSUM(M, epsilon, threshold, gaussian!=0, increase!=0);
	} /*else if (cdt_name == "ici") {
		int S0, nu;
		double gamma;

		ss >> S0 >> nu >> gamma;
		return new ICI(S0, nu, gamma);
	}*/ else {
		cout << "Not valid cdt: " << line << endl;
		return new CDT_PH(0, 0);
	}
}

Distribution* get_distribution(string line, boost::mt19937* rng) {
	stringstream ss(line);

	string distr;
	ss >> distr;

	if (distr == "Normal") { // Gaussian
		double mean, variance;
		ss >> mean >> variance;

		return new NormalDistribution("G", mean, variance, *rng);
	} else if (distr == "Fixed") { // Fixed
		double v;
		ss >> v;

		return new FixedDistribution("F", v, *rng);
	} else if (distr == "FixedNonStationary") { // Uniform
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

		return new FixedNonStationaryDistribution("FNS", *ps, *ends, *rng);
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
	} else if (distr == "SineBernoulli") {
		double A, omega, phi, mean;
		ss >> A >> omega >> phi >> mean;

		return new SineBernoulliDistribution("SB", A, omega, phi, mean, *rng);
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

void remove_dirs() {
	system("rm -R -f -- ./temp/*/");
}
