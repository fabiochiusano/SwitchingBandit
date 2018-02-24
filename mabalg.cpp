#include "mabalg.h"
#include "utils.h"

#include <iostream>
#include <math.h>
#include <numeric>
#include <boost/math/distributions.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <algorithm>

using namespace std;
using namespace boost::math;


MABAlgorithm::MABAlgorithm(string name, MAB& mab) {
	this->name = name;
	this->mab = &mab;
	this->collectedRewards.resize(mab.arms.size());
	this->totalReward.assign(mab.arms.size(), 0.);
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

EXP3::EXP3(string name, MAB& mab, double beta, double nu): MABAlgorithm(name, mab) {
	this->beta = beta;
	this->nu = nu;
	this->ws.assign(mab.arms.size(), 1.);
}

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


void MABAlgorithm::reset(MAB& mab) {
	this->allArmsPulled = false;
	this->lastArmPulled = -1;
	this->collectedRewards.clear();
	this->collectedRewards.resize(mab.arms.size());
	this->totalReward.clear();
	this->totalReward.assign(mab.arms.size(), 0.);
	this->regrets.clear();
}

void ThompsonSamplingBernoulli::reset(MAB& mab) {
	MABAlgorithm::reset(mab);
	this->alphas.assign(mab.arms.size(), 1);
	this->betas.assign(mab.arms.size(), 1);
}

void EXP3::reset(MAB& mab) {
	MABAlgorithm::reset(mab);
	this->ws.assign(mab.arms.size(), 1.);
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

// TODO: add the .reset in the constructors (remove duplicate code)


double MABAlgorithm::process_chosen_arm(vector<double> pulls, int timestep, double highest_mean, int arm_index) {
	double reward = pulls[arm_index];

	this->collectedRewards[arm_index].push_back(reward);
	this->totalReward[arm_index] += reward;

	if (*(this->mab->mabtype) == MABType::Stochastic) { // If stochastic MAB
		double mean_of_pulled_arm = this->mab->arms[arm_index]->get_mean(timestep);
		this->regrets.push_back(highest_mean - mean_of_pulled_arm);
	} else { // If adversarial MAB
		this->regrets.push_back(highest_mean - reward);
	}

	return reward;
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

			double sum_of_elems = this->totalReward[i];
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

// TODO: EXP3.S

// TODO: REXP3

// TODO: Adapt-EvE

// TODO: CTS

// TODO: EXP3.R

// TODO: spread the algorithms in multiple files
