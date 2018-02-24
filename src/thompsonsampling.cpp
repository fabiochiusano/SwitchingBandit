#include "thompsonsampling.h"
#include <boost/math/distributions.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <math.h>
#include <numeric>
#include <algorithm>

using namespace std;
using namespace boost::math;


ThompsonSampling::ThompsonSampling(string name, MAB& mab, boost::mt19937& rng): MABAlgorithm(name, mab) {
	this->rng = &rng;
}

ThompsonSamplingBernoulli::ThompsonSamplingBernoulli(string name, MAB& mab, boost::mt19937& rng) : ThompsonSampling(name, mab, rng) {
	this->alphas.assign(mab.arms.size(), 1);
	this->betas.assign(mab.arms.size(), 1);
}

ThompsonSamplingGaussian::ThompsonSamplingGaussian(string name, MAB& mab, boost::mt19937& rng) : ThompsonSampling(name, mab, rng) {}



void ThompsonSamplingBernoulli::reset(MAB& mab) {
	MABAlgorithm::reset(mab);
	this->alphas.assign(mab.arms.size(), 1);
	this->betas.assign(mab.arms.size(), 1);
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
