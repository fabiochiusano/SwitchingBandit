#include "ucb.h"


UCB::UCB(string name, MAB& mab) : MABAlgorithm(name, mab) {}

UCB1::UCB1(string name, MAB& mab) : UCB(name, mab) {}

UCBT::UCBT(string name, MAB& mab) : UCB(name, mab) {}


D_UCB::D_UCB(string name, MAB& mab, double gamma, double B, double epsilon) : UCB(name, mab) {
	this->gamma = gamma;
	this->B = B;
	this->epsilon = epsilon;
	this->means.assign(mab.arms.size(), 0.);
	this->ns.assign(mab.arms.size(), 0.);
}

SW_UCB::SW_UCB(string name, MAB& mab, int tau, double B, double epsilon) : UCB(name, mab) {
	this->tau = tau;
	this->B = B;
	this->epsilon = epsilon;
	this->arm_pulls.resize(mab.arms.size());
	this->arm_pulls_values.resize(mab.arms.size());
}


void D_UCB::reset(MAB& mab) {
	MABAlgorithm::reset(mab);
	this->means.assign(mab.arms.size(), 0.);
	this->ns.assign(mab.arms.size(), 0.);
}

void SW_UCB::reset(MAB& mab) {
	MABAlgorithm::reset(mab);
	this->arm_pulls.clear();
	this->arm_pulls_values.clear();
	this->arm_pulls.resize(mab.arms.size());
	this->arm_pulls_values.resize(mab.arms.size());
}


void UCB1::run(vector<double> pulls, int timestep, double highest_mean) {
	int armToPull = -1;

	if (!this->allArmsPulled) {
		// First phase: pull each arm once
		armToPull = this->lastArmPulled + 1;
		this->lastArmPulled++;
		if (this->lastArmPulled == this->mab->arms.size() - 1) {
			this->allArmsPulled = true;
		}
	}
	else {
		// Second phase: pull arm that maximizes Q+B
		double bestQB = -100000;
		int bestArm = -1;
		for (int i = 0; i < this->mab->arms.size(); i++) {
			double sum_of_elems = this->totalReward[i];
			int num_of_pulls = this->collectedRewards[i].size();
			double Q = sum_of_elems / num_of_pulls;
			double B = sqrt((2*log(timestep))/(num_of_pulls));
			double QB = Q + B;
			if (QB > bestQB) {
				bestQB = QB;
				bestArm = i;
			}
		}
		armToPull = bestArm;
	}

	this->process_chosen_arm(pulls, timestep, highest_mean, armToPull);
}


void UCBT::run(vector<double> pulls, int timestep, double highest_mean) {
	int armToPull = -1;

	if (!this->allArmsPulled) {
		// First phase: pull each arm once
		armToPull = this->lastArmPulled + 1;
		this->lastArmPulled++;
		if (this->lastArmPulled == this->mab->arms.size() - 1) {
			this->allArmsPulled = true;
		}
	}
	else {
		// Second phase: pull arm that maximizes Q+B
		double bestQB = -100000;
		int bestArm = -1;
		for (int i = 0; i < this->mab->arms.size(); i++) {
			double sum_of_elems = this->totalReward[i];
			int num_of_pulls = this->collectedRewards[i].size();
			double Q = sum_of_elems / num_of_pulls;
			double variance = 0;
			for (int j = 0; j < this->collectedRewards[i].size(); j++) {
				variance += pow((this->collectedRewards[i][j] - Q), 2);
			}
			variance /= num_of_pulls;
			double B = sqrt((2*log(timestep)*min(0.25, variance))/(num_of_pulls));
			double QB = Q + B;
			if (QB > bestQB) {
				bestQB = QB;
				bestArm = i;
			}
		}
		armToPull = bestArm;
	}

	this->process_chosen_arm(pulls, timestep, highest_mean, armToPull);
}

void D_UCB::run(vector<double> pulls, int timestep, double highest_mean) {
	int armToPull = -1;

	if (!this->allArmsPulled) {
		// First phase: pull each arm once
		armToPull = this->lastArmPulled + 1;
		this->lastArmPulled++;
		if (this->lastArmPulled == this->mab->arms.size() - 1) {
			this->allArmsPulled = true;
		}
	}
	else {
		// Second phase: pull arm that maximizes Q+B
		double bestQB = -100000;
		int bestArm = -1;
		double ns_sum = accumulate(this->ns.begin(), this->ns.end(), 0.);
		for (int i = 0; i < this->mab->arms.size(); i++) {
			double Q = this->means[i];
			double B = (2 * this->B) * sqrt((this->epsilon * log(ns_sum)) / (this->ns[i]));
			double QB = Q + B;

			if (QB > bestQB) {
				bestQB = QB;
				bestArm = i;
			}
		}
		armToPull = bestArm;
	}

	double reward = this->process_chosen_arm(pulls, timestep, highest_mean, armToPull);

	// update this->ns
	vector<double> old_ns;
	old_ns.assign(this->mab->arms.size(), 0.);;
	copy(this->ns.begin(), this->ns.end(), old_ns.begin());

	for (int i = 0; i < this->mab->arms.size(); i++) {
		this->ns[i] *= this->gamma;
	}
	this->ns[armToPull] += 1.0;

	// update this->means
	for (int i = 0; i < this->mab->arms.size(); i++) {
		this->means[i] *= old_ns[i];
	}
	this->means[armToPull] += reward;
	for (int i = 0; i < this->mab->arms.size(); i++) {
		if (this->ns[i] > 0) {
			this->means[i] *= this->gamma / this->ns[i];
		}
	}
}

void SW_UCB::run(vector<double> pulls, int timestep, double highest_mean) {
	int armToPull = -1;

	if (!this->allArmsPulled) {
		// First phase: pull each arm once
		armToPull = this->lastArmPulled + 1;
		this->lastArmPulled++;
		if (this->lastArmPulled == this->mab->arms.size() - 1) {
			this->allArmsPulled = true;
		}
	}
	else {
		// Second phase: pull arm that maximizes Q+B
		double bestQB = -100000;
		int bestArm = -1;
		for (int i = 0; i < this->mab->arms.size(); i++) {
			int windowed_N = accumulate(this->arm_pulls[i].begin(), this->arm_pulls[i].end(), 0);
			double windowed_reward = accumulate(this->arm_pulls_values[i].begin(), this->arm_pulls_values[i].end(), 0.);
			double Q = windowed_reward / windowed_N;
			double B = this->B * sqrt((this->epsilon * log(min(this->tau, timestep))) / windowed_N);
			double QB = Q + B;

			if (QB > bestQB) {
				bestQB = QB;
				bestArm = i;
			}
		}
		armToPull = bestArm;
	}

	double reward = this->process_chosen_arm(pulls, timestep, highest_mean, armToPull);

	// move the window
	for (int i = 0; i < this->mab->arms.size(); i++) {
		if (i == armToPull) {
			this->arm_pulls[i].push_back(1);
			this->arm_pulls_values[i].push_back(reward);
		} else {
			this->arm_pulls[i].push_back(0);
			this->arm_pulls_values[i].push_back(0.);
		}

		if (this->arm_pulls[i].size() > this->tau) this->arm_pulls[i].erase(this->arm_pulls[i].begin());
		if (this->arm_pulls_values[i].size() > this->tau) this->arm_pulls_values[i].erase(this->arm_pulls_values[i].begin());
	}
}
