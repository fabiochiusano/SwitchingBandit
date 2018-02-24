#include "greedy.h"


Greedy::Greedy(string name, MAB& mab) : MABAlgorithm(name, mab) {}

E_Greedy::E_Greedy(string name, MAB& mab, double epsilon) : Greedy(name, mab) {
	this->epsilon = epsilon;
}

GLIE::GLIE(string name, MAB& mab, double epsilon) : Greedy(name, mab) {
	this->epsilon = epsilon;
}


void E_Greedy::run(vector<double> pulls, int timestep, double highest_mean) {
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
		// Second phase:
		// with probability 1 - epsilon: pull arm that maximizes Q
		// with probability epsilon: choose a random arm
		float r = random_unit();
		if (r <= 1 - this->epsilon) {
			// pull arm that maximizes Q
			double bestQ = -100000;
			int bestArm = -1;
			for (int i = 0; i < this->mab->arms.size(); i++) {
				double sum_of_elems = this->totalReward[i];
				int num_of_pulls = this->collectedRewards[i].size();
				double Q = sum_of_elems / num_of_pulls;
				if (Q > bestQ) {
					bestQ = Q;
					bestArm = i;
				}
			}
			armToPull = bestArm;
		}
		else {
			// choose a random arm
			armToPull = rand() % this->mab->arms.size();
		}
	}

	this->process_chosen_arm(pulls, timestep, highest_mean, armToPull);
}

void GLIE::run(vector<double> pulls, int timestep, double highest_mean) {
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
		// Second phase:
		// with probability 1 - epsilon: pull arm that maximizes Q
		// with probability epsilon: choose a random arm
		float r = random_unit();
		double cur_epsilon = this->epsilon/(timestep - this->lastArmPulled);
		if (r <= 1 - cur_epsilon) {
			// pull arm that maximizes Q
			double bestQ = -100000;
			int bestArm = -1;
			for (int i = 0; i < this->mab->arms.size(); i++) {
				double sum_of_elems = this->totalReward[i];
				int num_of_pulls = this->collectedRewards[i].size();
				double Q = sum_of_elems / num_of_pulls;
				if (Q > bestQ) {
					bestQ = Q;
					bestArm = i;
				}
			}
			armToPull = bestArm;
		}
		else {
			// choose a random arm
			armToPull = rand() % this->mab->arms.size();
		}
	}

	this->process_chosen_arm(pulls, timestep, highest_mean, armToPull);
}
