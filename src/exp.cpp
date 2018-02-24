#include "exp.h"


EXP3::EXP3(string name, MAB& mab, double beta, double nu): MABAlgorithm(name, mab) {
	this->beta = beta;
	this->nu = nu;
	this->ws.assign(mab.arms.size(), 1.);
}


void EXP3::reset(MAB& mab) {
	MABAlgorithm::reset(mab);
	this->ws.assign(mab.arms.size(), 1.);
}


void EXP3::run(vector<double> pulls, int timestep, double highest_mean) {
	// Find the arms probabilities
	double ws_sum = accumulate(this->ws.begin(), this->ws.end(), 0.);
	//cout << ws_sum << endl;
	vector<double> arm_probabilities;
	for (int i = 0; i < this->mab->arms.size(); i++) {
		double arm_prob = (1 - this->beta) * (this->ws[i] / ws_sum) + (this->beta / this->mab->arms.size());
		if (arm_prob < 0) {
			cout << "negative arm prob" << endl;
		}
		arm_probabilities.push_back(arm_prob);
	}

	// Sample an arm according to the probabilities
	double r = random_unit();
	double cur_sum = 0;
	int chosen_arm = -1;
	for (int i = 0; i < arm_probabilities.size(); i++) {
		cur_sum += arm_probabilities[i];
		if (r < cur_sum) {
			chosen_arm = i;
			break;
		}
	}

	// Pull best arm
	double reward = this->process_chosen_arm(pulls, timestep, highest_mean, chosen_arm);

	// Update ws
	this->ws[chosen_arm] *= exp(this->nu * (reward / arm_probabilities[chosen_arm]));
	this->ws[chosen_arm] = max(0.0001, min(100000.0, this->ws[chosen_arm]));
}
