#include "greedy.h"

/*
Greedy::Greedy(string name, int num_of_arms, double epsilon) : MABAlgorithm(name, num_of_arms) {
	this->epsilon;
	this->reset();
}

E_Greedy::E_Greedy(string name, int num_of_arms, double epsilon) : Greedy(name, num_of_arms, epsilon) {
	this->reset();
}*/
/*
GLIE::GLIE(string name, int num_of_arms, double epsilon) : Greedy(name, num_of_arms, epsilon) {
	this->reset();
}*/

/*
void Greedy::reset() {
	mabalg_reset();
	this->means.assign(this->num_of_arms, 0.);
}


int E_Greedy::choose_action() {
	// pull arm that maximizes Q
	double bestQ = -100000;
	int best_arm = -1;
	for (int i = 0; i < this->num_of_arms; i++) {
		double Q = this->means[i];
		if (Q > bestQ) {
			bestQ = Q;
			best_arm = i;
		}
	}
	return best_arm;
}

void E_Greedy::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);
	this->means[pulled_arm] = (this->means[pulled_arm] * (this->num_of_pulls[pulled_arm] - 1) + reward) / this->num_of_pulls[pulled_arm];
}*/

/*
ArmPull E_Greedy::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
	vector<double> pulls = all_pulls[timestep];
	int arm_to_pull = -1;

	// Choose arm to pull
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0.);
	if (tot_pulls < this->num_of_arms) {
		// First phase: pull each arm once
		arm_to_pull = tot_pulls;
	}
	else {
		// Second phase:
		// with probability 1 - epsilon: pull arm that maximizes Q
		// with probability epsilon: choose a random arm
		float r = random_unit();
		if (r <= 1 - this->epsilon) {
			// pull arm that maximizes Q
			double bestQ = -100000;
			int best_arm = -1;
			for (int i = 0; i < this->num_of_arms; i++) {
				double Q = this->means[i];
				if (Q > bestQ) {
					bestQ = Q;
					best_arm = i;
				}
			}
			arm_to_pull = best_arm;
		}
		else {
			// choose a random arm
			arm_to_pull = rand() % this->num_of_arms;
		}
	}

	// Pull arm
	ArmPull armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, arm_to_pull);

	// Update algorithm statistics
	this->means[arm_to_pull] = (this->means[arm_to_pull] * (this->num_of_pulls[arm_to_pull] - 1) + armpull.reward) / this->num_of_pulls[arm_to_pull];

	return armpull;
}
*/

/*
ArmPull GLIE::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
	vector<double> pulls = all_pulls[timestep];
	int arm_to_pull = -1;

	// Choose arm to pull
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0.);
	if (tot_pulls < this->num_of_arms) {
		// First phase: pull each arm once
		arm_to_pull = tot_pulls;
	}
	else {
		// Second phase:
		// with probability 1 - epsilon: pull arm that maximizes Q
		// with probability epsilon: choose a random arm
		float r = random_unit();
		double cur_epsilon = this->epsilon/(tot_pulls - this->num_of_arms);
		if (r <= 1 - cur_epsilon) {
			// pull arm that maximizes Q
			double bestQ = -100000;
			int best_arm = -1;
			for (int i = 0; i < this->num_of_arms; i++) {
				double Q = this->means[i];
				if (Q > bestQ) {
					bestQ = Q;
					best_arm = i;
				}
			}
			arm_to_pull = best_arm;
		}
		else {
			// choose a random arm
			arm_to_pull = rand() % this->num_of_arms;
		}
	}

	// Pull arm
	ArmPull armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, arm_to_pull);

	// Update algorithm statistics
	this->means[arm_to_pull] = (this->means[arm_to_pull] * (this->num_of_pulls[arm_to_pull] - 1) + armpull.reward) / this->num_of_pulls[arm_to_pull];

	return armpull;
}
*/
