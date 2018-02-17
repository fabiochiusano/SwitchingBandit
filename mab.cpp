#include "mab.h"
#include <fstream>
#include <stdlib.h>
#include <numeric>

MAB::MAB(MABType& mabtype) {
	this->mabtype = &mabtype;
}




void MAB::addArm(Distribution* arm){
	this->arms.push_back(arm);
}

void MAB::initialize(vector<MABAlgorithm*>& algs, int total_timesteps) {
	// Generate all the pulls
	for (auto arm : this->arms) {
		vector<double> pulls;
		for (int t = 0; t < total_timesteps; t++) {
			double d = arm->draw(t);
			pulls.push_back(d);
		}
		this->pulls.push_back(pulls);
	}

	// Find the highest means
	if (*(this->mabtype) == MABType::Stochastic) { // If stochastic mab
		for (int t = 0; t < total_timesteps; t++) {
			double cur_best_mean = -10000;
			for (int a = 0; a < this->arms.size(); a++) {
				cur_best_mean = max(cur_best_mean, this->arms[a]->get_mean(t));
			}
			this->highest_means.push_back(cur_best_mean);
		}
	} else {// If adversarial mab
		int best_arm = -1;
		double max_sum = -10000;
		for (int a = 0; a < this->arms.size(); a++) {
			double sum_of_elems = accumulate(this->pulls[a].begin(), this->pulls[a].end(), 0);
			if (sum_of_elems > max_sum) {
				max_sum = sum_of_elems;
				best_arm = a;
			}
		}
		this->highest_means = this->pulls[best_arm];
	}
}

void MAB::run(vector<MABAlgorithm*>& algs, int timestep, int total_timesteps) {
	//cout << "Timestep: " << timestep << endl;

	// If first run: generate all the pulls
	if (this->first_run) {
		this->initialize(algs, total_timesteps);
		this->first_run = false;
	}

	double highest_mean = this->highest_means[timestep];

	vector<double> timestep_pulls;
	for (int a = 0; a < this->arms.size(); a++) {
		timestep_pulls.push_back(this->pulls[a][timestep]);
	}

	// Run the algorithms
	for (auto alg : algs) {
		alg->run(timestep_pulls, timestep, highest_mean);
	}

	//cout << endl;
}

double MAB::pullArm(int armIndex, int timestep) {
	return this->arms[armIndex]->draw(timestep);
}

void MAB::reset(vector<MABAlgorithm*>& algs) {
	this->first_run = true;
	this->pulls.clear();
	this->highest_means.clear();

	for (auto alg : algs) {
		alg->reset(*(this));
	}
}



void MAB::plot_distributions() {
	ofstream outputFile("temp/distributions.txt");

	for (auto d : this->arms) {
		outputFile << d->toFile() << endl;
	}

	system("python plot_distributions.py &");
}

void MAB::plot_means_in_time(int timesteps) {
	ofstream outputFile("temp/means_in_time.txt");

	outputFile << timesteps << endl;

	for (auto d : this->arms) {
		outputFile << d->toFile() << endl;
	}

	system("python plot_means_in_time.py &");
}

void MAB::write_regrets(vector<MABAlgorithm*>& algs, bool should_append) {
	ofstream outputFile("temp/regrets.txt", should_append ? ofstream::app : ofstream::out);

	for (auto alg : algs) {
		outputFile << alg->name << " ";
		for (auto r : alg->regrets) {
			outputFile << r << " ";
		}
		outputFile << endl;
	}
}

void MAB::plot_regrets() {
	system("python plot_regrets.py &");
}
