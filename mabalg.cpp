#include "mabalg.h"
#include "utils.h"

#include <iostream>
#include <math.h>
#include <numeric>
#include <boost/math/distributions.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

using namespace std;
using namespace boost::math;


MABAlgorithm::MABAlgorithm(string name, MAB& mab) {
	this->name = name;
	this->mab = &mab;
	this->collectedRewards.resize(mab.arms.size());
}

UCB::UCB(string name, MAB& mab) : MABAlgorithm(name, mab) {}

UCB1::UCB1(string name, MAB& mab) : UCB(name, mab) {}

UCBT::UCBT(string name, MAB& mab) : UCB(name, mab) {}

Greedy::Greedy(string name, MAB& mab) : MABAlgorithm(name, mab) {}

E_Greedy::E_Greedy(string name, MAB& mab, double epsilon) : Greedy(name, mab) {
	this->epsilon = epsilon;
}

GLIE::GLIE(string name, MAB& mab, double epsilon) : Greedy(name, mab) {
	this->epsilon = epsilon;
}

ThompsonSampling::ThompsonSampling(string name, MAB& mab, boost::mt19937& rng): MABAlgorithm(name, mab) {
	this->rng = &rng;
}

ThompsonSamplingBernoulli::ThompsonSamplingBernoulli(string name, MAB& mab, boost::mt19937& rng) : ThompsonSampling(name, mab, rng) {
	this->alphas.assign(mab.arms.size(), 1);
	this->betas.assign(mab.arms.size(), 1);
}

ThompsonSamplingGaussian::ThompsonSamplingGaussian(string name, MAB& mab, boost::mt19937& rng) : ThompsonSampling(name, mab, rng) {}



void MABAlgorithm::reset(MAB& mab) {
	this->allArmsPulled = false;
	this->lastArmPulled = -1;
	this->collectedRewards.clear();
	this->collectedRewards.resize(mab.arms.size());
	this->totalReward = 0;
	this->regrets.clear();
}

void ThompsonSamplingBernoulli::reset(MAB& mab) {
	this->alphas.assign(mab.arms.size(), 1);
	this->betas.assign(mab.arms.size(), 1);
}


void MABAlgorithm::process_chosen_arm(vector<double> pulls, int timestep, double highest_mean, int arm_index) {
	double reward = pulls[arm_index];

	this->collectedRewards[arm_index].push_back(reward);
	this->totalReward += reward;

	double mean_of_pulled_arm = this->mab->arms[arm_index]->get_mean(timestep);
	this->regrets.push_back(highest_mean - mean_of_pulled_arm);

	//cout << this->name << " selected arm " << arm_index << " with reward " << reward << " and regret " << (highest_mean - mean_of_pulled_arm) << endl;
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
				double sum_of_elems = accumulate(this->collectedRewards[i].begin(), this->collectedRewards[i].end(), 0);
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
				double sum_of_elems = accumulate(this->collectedRewards[i].begin(), this->collectedRewards[i].end(), 0);
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
			double sum_of_elems = accumulate(this->collectedRewards[i].begin(), this->collectedRewards[i].end(), 0);
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
			double sum_of_elems = accumulate(this->collectedRewards[i].begin(), this->collectedRewards[i].end(), 0);
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

void ThompsonSamplingBernoulli::run(vector<double> pulls, int timestep, double highest_mean) {
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
		// Sample from all the arms anc choose the best arm
		double max_sample = -10000;
		for (int i = 0; i < this->mab->arms.size(); i++) {
			float r = random_unit();
			beta_distribution<> dist(this->alphas[i], this->betas[i]);
			double sample = quantile(dist, r);
			if (sample > max_sample) {
				max_sample = sample;
				armToPull = i;
			}
		}
	}

	// Pull best arm
	double reward = pulls[armToPull];

	// Update arm distribution
	if (reward > 0) {
		this->alphas[armToPull]++;
	}
	else {
		this->betas[armToPull]++;
	}

	this->process_chosen_arm(pulls, timestep, highest_mean, armToPull);
}

void ThompsonSamplingGaussian::run(vector<double> pulls, int timestep, double highest_mean) {
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
		// Sample from all the arms anc choose the best arm
		double max_sample = -10000;
		for (int i = 0; i < this->mab->arms.size(); i++) {

			double sum_of_elems = accumulate(this->collectedRewards[i].begin(), this->collectedRewards[i].end(), 0);
			int num_of_pulls = this->collectedRewards[i].size();
			double Q = 0;
			if (num_of_pulls > 0) {
				Q = sum_of_elems / num_of_pulls;
			}

			boost::normal_distribution<> nd = boost::normal_distribution<>(Q, 1.f/(num_of_pulls + 1));
			boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > vg = boost::variate_generator<boost::mt19937&, boost::normal_distribution<> >(*(this->rng), nd);
			double sample = vg();

			if (sample >= max_sample) {
				max_sample = sample;
				armToPull = i;
			}
		}
	}

	// Pull best arm
	this->process_chosen_arm(pulls, timestep, highest_mean, armToPull);
}
