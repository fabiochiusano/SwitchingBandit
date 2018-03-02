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
	this->reset();
}

ThompsonSamplingGaussian::ThompsonSamplingGaussian(string name, MAB& mab, boost::mt19937& rng) : ThompsonSampling(name, mab, rng) {
	this->reset();
}



void ThompsonSamplingBernoulli::reset() {
	mabalg_reset();
	this->alphas.assign(this->num_of_arms, 1);
	this->betas.assign(this->num_of_arms, 1);
}

void ThompsonSamplingGaussian::reset() {
	mabalg_reset();
	this->means.assign(this->num_of_arms, 0.);
}


ArmPull ThompsonSamplingBernoulli::run(vector<double>& pulls, bool generate_new_pulls) {
	int arm_to_pull = -1;

	// Choose arm to pull
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0.);
	if (tot_pulls < this->num_of_arms) {
		// First phase: pull each arm once
		arm_to_pull = tot_pulls;
	}
	else {
		// Sample from all the arms anc choose the best arm
		double max_sample = -10000;
		for (int i = 0; i < this->num_of_arms; i++) {
			float r = random_unit();
			beta_distribution<> dist(this->alphas[i], this->betas[i]);
			double sample = quantile(dist, r);
			if (sample > max_sample) {
				max_sample = sample;
				arm_to_pull = i;
			}
		}
	}

	// Pull arm
	ArmPull armpull = this->pull_arm(pulls, generate_new_pulls, arm_to_pull);

	// Update algorithm statistics
	if (armpull.reward > 0) {
		this->alphas[arm_to_pull]++;
	}
	else {
		this->betas[arm_to_pull]++;
	}

	return armpull;
}

ArmPull ThompsonSamplingGaussian::run(vector<double>& pulls, bool generate_new_pulls) {
	int arm_to_pull = -1;

	// Choose arm to pull
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0);
	if (tot_pulls < this->num_of_arms) {
		// First phase: pull each arm once
		arm_to_pull = tot_pulls;
	}
	else {
		// Sample from all the arms anc choose the best arm
		double max_sample = -10000;
		for (int i = 0; i < this->num_of_arms; i++) {
			boost::normal_distribution<> nd = boost::normal_distribution<>(this->means[i], 1.f/(this->num_of_pulls[i] + 1));
			boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > vg = boost::variate_generator<boost::mt19937&, boost::normal_distribution<> >(*(this->rng), nd);
			double sample = vg();

			if (sample >= max_sample) {
				max_sample = sample;
				arm_to_pull = i;
			}
		}
	}
	if (arm_to_pull == -1) cout << "DOH ts_g" << endl;

	// Pull arm
	ArmPull armpull = this->pull_arm(pulls, generate_new_pulls, arm_to_pull);

	// Update algorithm statistics
	this->means[arm_to_pull] = (this->means[arm_to_pull] * (this->num_of_pulls[arm_to_pull] - 1) + armpull.reward) / this->num_of_pulls[arm_to_pull];

	return armpull;
}
