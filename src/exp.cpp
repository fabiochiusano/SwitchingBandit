#include "exp.h"


EXP3::EXP3(string name, int num_of_arms, double beta, double nu): MABAlgorithm(name, num_of_arms) {
	this->reset();
	this->beta = beta;
	this->nu = nu;
}

EXP3_S::EXP3_S(string name, int num_of_arms, double beta, double alpha): MABAlgorithm(name, num_of_arms) {
	this->reset();
	this->beta = beta;
	this->alpha = alpha;
}

REXP3::REXP3(string name, int num_of_arms, double beta, int window_size): MABAlgorithm(name, num_of_arms) {
	this->reset();
	this->beta = beta;
	this->window_size = window_size;
}


void EXP3::reset() {
	this->MABAlgorithm::reset();
	this->ws.assign(this->num_of_arms, 1.);
}

void EXP3_S::reset() {
	this->MABAlgorithm::reset();
	this->ws.assign(this->num_of_arms, 1.);
}

void REXP3::reset() {
	this->MABAlgorithm::reset();
	this->ws.assign(this->num_of_arms, 1.);
}

int EXP3::choose_action() {
	// Find the arms probabilities
	double ws_sum = accumulate(this->ws.begin(), this->ws.end(), 0.);
	vector<double> arm_probabilities;
	for (int i = 0; i < this->num_of_arms; i++) {
		double arm_prob = (1 - this->beta) * (this->ws[i] / ws_sum) + (this->beta / this->num_of_arms);
		if (arm_prob < 0) {
			cout << "negative arm prob" << endl;
		}
		arm_probabilities.push_back(arm_prob);
	}

	// Sample an arm according to the probabilities
	double r = random_unit();
	double cur_sum = 0;
	int chosen_arm = -1;
	for (int i = 0; i < this->num_of_arms; i++) {
		cur_sum += arm_probabilities[i];
		if (r <= cur_sum) {
			chosen_arm = i;
			break;
		}
	}

	return chosen_arm;
}

void EXP3::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);

	// Find the arms probabilities
	double ws_sum = accumulate(this->ws.begin(), this->ws.end(), 0.);
	vector<double> arm_probabilities;
	for (int i = 0; i < this->num_of_arms; i++) {
		double arm_prob = (1 - this->beta) * (this->ws[i] / ws_sum) + (this->beta / this->num_of_arms);
		if (arm_prob < 0) {
			cout << "negative arm prob" << endl;
		}
		arm_probabilities.push_back(arm_prob);
	}

	this->ws[pulled_arm] *= exp(this->nu * (reward / arm_probabilities[pulled_arm]));
}

/*
ArmPull EXP3::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
	vector<double> pulls = all_pulls[timestep];
	// Find the arms probabilities
	double ws_sum = accumulate(this->ws.begin(), this->ws.end(), 0.);
	vector<double> arm_probabilities;
	for (int i = 0; i < this->num_of_arms; i++) {
		double arm_prob = (1 - this->beta) * (this->ws[i] / ws_sum) + (this->beta / this->num_of_arms);
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
		if (r <= cur_sum) {
			chosen_arm = i;
			break;
		}
	}
	if (chosen_arm == -1) cout << "Doh exp3" << endl;

	// Pull arm
	ArmPull armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, chosen_arm);

	// Update algorithm statistics
	this->ws[chosen_arm] *= exp(this->nu * (armpull.reward / arm_probabilities[chosen_arm]));
	//this->ws[chosen_arm] = max(0.00001, min(100000.0, this->ws[chosen_arm]));

	return armpull;
}*/

int EXP3_S::choose_action() {
	// Find the arms probabilities
	double ws_sum = accumulate(this->ws.begin(), this->ws.end(), 0.);
	vector<double> arm_probabilities;
	for (int i = 0; i < this->num_of_arms; i++) {
		double arm_prob = (1 - this->beta) * (this->ws[i] / ws_sum) + (this->beta / this->num_of_arms);
		if (arm_prob < 0) {
			cout << "negative arm prob" << endl;
		}
		arm_probabilities.push_back(arm_prob);
	}

	// Sample an arm according to the probabilities
	double r = random_unit();
	double cur_sum = 0;
	int chosen_arm = -1;
	for (int i = 0; i < this->num_of_arms; i++) {
		cur_sum += arm_probabilities[i];
		if (r <= cur_sum) {
			chosen_arm = i;
			break;
		}
	}

	return chosen_arm;
}

void EXP3_S::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);

	// Find the arms probabilities
	double ws_sum = accumulate(this->ws.begin(), this->ws.end(), 0.);
	vector<double> arm_probabilities;
	for (int i = 0; i < this->num_of_arms; i++) {
		double arm_prob = (1 - this->beta) * (this->ws[i] / ws_sum) + (this->beta / this->num_of_arms);
		if (arm_prob < 0) {
			cout << "negative arm prob" << endl;
		}
		arm_probabilities.push_back(arm_prob);
	}

	// Update algorithm statistics
	for (int i = 0; i < this->num_of_arms; i++) {
		double k = 0;
		if (i == pulled_arm) {
			k = reward / arm_probabilities[pulled_arm];
		}
		this->ws[i] = this->ws[i] * exp(this->beta * (k / this->num_of_arms)) + (this->alpha / this->num_of_arms) * ws_sum;
	}
}
/*
ArmPull EXP3_S::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
	vector<double> pulls = all_pulls[timestep];
	// Find the arms probabilities
	double ws_sum = accumulate(this->ws.begin(), this->ws.end(), 0.);
	vector<double> arm_probabilities;
	for (int i = 0; i < this->num_of_arms; i++) {
		double arm_prob = (1 - this->beta) * (this->ws[i] / ws_sum) + (this->beta / this->num_of_arms);
		if (arm_prob < 0) {
			cout << "negative arm prob" << endl;
		}
		arm_probabilities.push_back(arm_prob);
	}

	// Sample an arm according to the probabilities
	double r = random_unit();
	double cur_sum = 0;
	int chosen_arm = -1;
	for (int i = 0; i < this->num_of_arms; i++) {
		cur_sum += arm_probabilities[i];
		if (r <= cur_sum) {
			chosen_arm = i;
			break;
		}
	}

	// Pull arm
	ArmPull armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, chosen_arm);

	// Update algorithm statistics
	for (int i = 0; i < this->num_of_arms; i++) {
		double k = 0;
		if (i == chosen_arm) {
			k = armpull.reward / arm_probabilities[chosen_arm];
		}
		this->ws[i] = this->ws[i] * exp(this->beta * (k / this->num_of_arms)) + (this->alpha / this->num_of_arms) * ws_sum;
		//this->ws[i] = max(0.00001, min(100000.0, this->ws[i]));
	}

	return armpull;
}*/

int REXP3::choose_action() {
	// Reset if window is finished
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0.);
	if (tot_pulls % this->window_size == 0) {
		this->reset();
	}

	// Find the arms probabilities
	double ws_sum = accumulate(this->ws.begin(), this->ws.end(), 0.);
	vector<double> arm_probabilities;
	for (int i = 0; i < this->num_of_arms; i++) {
		double arm_prob = (1 - this->beta) * (this->ws[i] / ws_sum) + (this->beta / this->num_of_arms);
		if (arm_prob < 0) {
			cout << "negative arm prob" << endl;
		}
		arm_probabilities.push_back(arm_prob);
	}

	// Sample an arm according to the probabilities
	double r = random_unit();
	double cur_sum = 0;
	int chosen_arm = -1;
	for (int i = 0; i < this->num_of_arms; i++) {
		cur_sum += arm_probabilities[i];
		if (r <= cur_sum) {
			chosen_arm = i;
			break;
		}
	}

	return chosen_arm;
}

void REXP3::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);

	// Find the arms probabilities
	double ws_sum = accumulate(this->ws.begin(), this->ws.end(), 0.);
	vector<double> arm_probabilities;
	for (int i = 0; i < this->num_of_arms; i++) {
		double arm_prob = (1 - this->beta) * (this->ws[i] / ws_sum) + (this->beta / this->num_of_arms);
		if (arm_prob < 0) {
			cout << "negative arm prob" << endl;
		}
		arm_probabilities.push_back(arm_prob);
	}

	this->ws[pulled_arm] *= exp((this->beta / this->num_of_arms) * (reward / arm_probabilities[pulled_arm]));
}
/*
ArmPull REXP3::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
	vector<double> pulls = all_pulls[timestep];
	// Reset if window is finished
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0.);
	if (tot_pulls % this->window_size == 0) {
		this->reset();
	}

	// Find the arms probabilities
	double ws_sum = accumulate(this->ws.begin(), this->ws.end(), 0.);
	vector<double> arm_probabilities;
	for (int i = 0; i < this->num_of_arms; i++) {
		double arm_prob = (1 - this->beta) * (this->ws[i] / ws_sum) + (this->beta / this->num_of_arms);
		if (arm_prob < 0) {
			cout << "negative arm prob" << endl;
		}
		arm_probabilities.push_back(arm_prob);
	}

	// Sample an arm according to the probabilities
	double r = random_unit();
	double cur_sum = 0;
	int chosen_arm = -1;
	for (int i = 0; i < this->num_of_arms; i++) {
		cur_sum += arm_probabilities[i];
		if (r <= cur_sum) {
			chosen_arm = i;
			break;
		}
	}

	// Pull arm
	ArmPull armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, chosen_arm);

	// Update ws
	this->ws[chosen_arm] *= exp((this->beta / this->num_of_arms) * (armpull.reward / arm_probabilities[chosen_arm]));
	//this->ws[chosen_arm] = max(0.00001, min(100000.0, this->ws[chosen_arm]));

	return armpull;
}
*/
