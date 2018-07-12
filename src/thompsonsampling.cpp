#include "thompsonsampling.h"
#include <boost/math/distributions.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <math.h>
#include <numeric>
#include <algorithm>

using namespace std;
using namespace boost::math;


ThompsonSampling::ThompsonSampling(string name, int num_of_arms, boost::mt19937& rng): MABAlgorithm(name, num_of_arms) {
	this->rng = &rng;
}

ThompsonSamplingBernoulli::ThompsonSamplingBernoulli(string name, int num_of_arms, boost::mt19937& rng) : ThompsonSampling(name, num_of_arms, rng) {
	this->reset(-1);
}

ThompsonSamplingGaussian::ThompsonSamplingGaussian(string name, int num_of_arms, boost::mt19937& rng) : ThompsonSampling(name, num_of_arms, rng) {
	this->reset(-1);
}

GlobalCTS::GlobalCTS(string name, int num_of_arms, boost::mt19937& rng, double gamma) : ThompsonSampling(name, num_of_arms, rng) {
	this->gamma = gamma;
	this->reset(-1);
}

PerArmCTS::PerArmCTS(string name, int num_of_arms, boost::mt19937& rng, double gamma) : ThompsonSampling(name, num_of_arms, rng) {
	this->gamma = gamma;
	this->reset(-1);
}



void ThompsonSamplingBernoulli::reset(int action) {
	this->MABAlgorithm::reset(action);
	if (action == -1) {
		this->alphas.assign(this->num_of_arms, 1);
		this->betas.assign(this->num_of_arms, 1);
	} else {
		this->alphas[action] = 1;
		this->betas[action] = 1;
	}
}

void ThompsonSamplingGaussian::reset(int action) {
	this->MABAlgorithm::reset(action);
	if (action == -1) {
		this->means.assign(this->num_of_arms, 0.);
	} else {
		this->means[action] = 0;
	}
}

void GlobalCTS::reset(int action) {
	this->MABAlgorithm::reset(action);
	if (action == -1) {
		this->runlength_distribution.clear();
		this->runlength_distribution.push_back(1.);
		this->alphas.clear();
		this->betas.clear();
		this->alphas.resize(this->num_of_arms);
		this->betas.resize(this->num_of_arms);
		for (int i = 0; i < this->num_of_arms; i++) {
			this->alphas[i].push_back(1.);
			this->betas[i].push_back(1.);
		}
	} else {
		cout << "Not possible to reset a single arm on GlobalCTS" << endl;
	}
}

void PerArmCTS::reset(int action) {
	this->MABAlgorithm::reset(action);
	if (action == -1) {
		this->runlength_distribution.clear();
		this->alphas.clear();
		this->betas.clear();
		this->runlength_distribution.resize(this->num_of_arms);
		this->alphas.resize(this->num_of_arms);
		this->betas.resize(this->num_of_arms);
		for (int i = 0; i < this->num_of_arms; i++) {
			this->runlength_distribution[i].push_back(1.);
			this->alphas[i].push_back(1.);
			this->betas[i].push_back(1.);
		}
	} else {
		this->runlength_distribution[action].clear();
		this->runlength_distribution[action].push_back(1.);
		this->alphas[action].clear();
		this->alphas[action].push_back(1.);
		this->betas[action].clear();
		this->betas[action].push_back(1.);
	}
}

int ThompsonSamplingBernoulli::choose_action() {
	int arm_to_pull = -1;

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

	return arm_to_pull;
}

void ThompsonSamplingBernoulli::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);

	if (reward > 0) {
		this->alphas[pulled_arm]++;
	}
	else {
		this->betas[pulled_arm]++;
	}
}

/*
ArmPull ThompsonSamplingBernoulli::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
	vector<double> pulls = all_pulls[timestep];
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
	ArmPull armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, arm_to_pull);

	// Update algorithm statistics
	if (armpull.reward > 0) {
		this->alphas[arm_to_pull]++;
	}
	else {
		this->betas[arm_to_pull]++;
	}

	return armpull;
}*/

int ThompsonSamplingGaussian::choose_action() {
	int arm_to_pull = -1;

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

	return arm_to_pull;
}

void ThompsonSamplingGaussian::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);

	this->means[pulled_arm] = (this->means[pulled_arm] * (this->num_of_pulls[pulled_arm] - 1) + reward) / this->num_of_pulls[pulled_arm];
}
/*
ArmPull ThompsonSamplingGaussian::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
	vector<double> pulls = all_pulls[timestep];
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
	ArmPull armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, arm_to_pull);

	// Update algorithm statistics
	this->means[arm_to_pull] = (this->means[arm_to_pull] * (this->num_of_pulls[arm_to_pull] - 1) + armpull.reward) / this->num_of_pulls[arm_to_pull];

	return armpull;
}*/

int GlobalCTS::choose_action() {
	int arm_to_pull = -1;

	// Sample runlength
	float r = random_unit();
	int sampled_runlength = -1;
	double acc = 0;
	for (int run = 0; run < this->runlength_distribution.size(); run++) {
		acc += this->runlength_distribution[run];
		if (r <= acc) {
			sampled_runlength = run;
			break;
		}
	}

	// Sample arm
	double max_sample = -10000;
	for (int i = 0; i < this->num_of_arms; i++) {
		float r = random_unit();
		beta_distribution<> dist(this->alphas[i][sampled_runlength], this->betas[i][sampled_runlength]);
		double sample = quantile(dist, r);
		if (sample > max_sample) {
			max_sample = sample;
			arm_to_pull = i;
		}
	}

	return arm_to_pull;
}

void GlobalCTS::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);

	// Update runlength model
	vector<double> old_runlength_distribution(this->runlength_distribution.size());
	copy(this->runlength_distribution.begin(), this->runlength_distribution.end(), old_runlength_distribution.begin());
	// push a random number (e.g. a zero) since we want to update its size only. The value will be overwritten in the loop
	this->runlength_distribution.push_back(0);
	vector<double> likelihoods;
	for (int run = 0; run < this->runlength_distribution.size()-1; run++) {
		double likelihood;
		if (reward > 0) {
			likelihood = this->alphas[pulled_arm][run] / (this->alphas[pulled_arm][run] + this->betas[pulled_arm][run]);
		}
		else {
			likelihood = this->betas[pulled_arm][run] / (this->alphas[pulled_arm][run] + this->betas[pulled_arm][run]);
		}
		likelihoods.push_back(likelihood);
		this->runlength_distribution[run+1] = (1 - this->gamma) * likelihood * old_runlength_distribution[run];
	}
	this->runlength_distribution[0] = 0;
	for (int l = 0; l < likelihoods.size(); l++) {
		this->runlength_distribution[0] += this->gamma * likelihoods[l] * old_runlength_distribution[l];
	}
	// Normalize this->runlength_distribution
	double runlength_sum = accumulate(this->runlength_distribution.begin(), this->runlength_distribution.end(), 0.);
	for (int run = 0; run < this->runlength_distribution.size(); run++) {
		this->runlength_distribution[run] /= runlength_sum;
	}

	// Update arm models
	// push a random number (e.g. a zero) since we want to update its size only. The value will be overwritten in the loop
	for (int a = 0; a < this->num_of_arms; a++) {
		this->alphas[a].push_back(0);
		this->betas[a].push_back(0);
	}
	for (int i = this->alphas[pulled_arm].size() - 2; i >= 0; i--) {
		for (int a = 0; a < this->num_of_arms; a++) {
			if (a == pulled_arm) {
				if (reward > 0) {
					this->alphas[a][i+1] = this->alphas[a][i] + 1;
					this->betas[a][i+1] = this->betas[a][i];
				} else {
					this->betas[a][i+1] = this->betas[a][i] + 1;
					this->alphas[a][i+1] = this->alphas[a][i];
				}
			} else {
				this->alphas[a][i+1] = this->alphas[a][i];
				this->betas[a][i+1] = this->betas[a][i];
			}
		}
	}
	for (int a = 0; a < this->num_of_arms; a++) {
		this->alphas[a][0] = 1;
		this->betas[a][0] = 1;
	}
}
/*
ArmPull GlobalCTS::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
	vector<double> pulls = all_pulls[timestep];
	int arm_to_pull = -1;

	// Choose arm to pull
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0.);
	if (tot_pulls < this->num_of_arms) {
		// First phase: pull each arm once
		arm_to_pull = tot_pulls;
	}
	else {
		// Sample runlength
		float r = random_unit();
		int sampled_runlength = -1;
		double acc = 0;
		for (int run = 0; run < this->runlength_distribution.size(); run++) {
			acc += this->runlength_distribution[run];
			if (r <= acc) {
				sampled_runlength = run;
				break;
			}
		}

		// Sample arm
		double max_sample = -10000;
		for (int i = 0; i < this->num_of_arms; i++) {
			float r = random_unit();
			beta_distribution<> dist(this->alphas[i][sampled_runlength], this->betas[i][sampled_runlength]);
			double sample = quantile(dist, r);
			if (sample > max_sample) {
				max_sample = sample;
				arm_to_pull = i;
			}
		}
	}

	// Pull arm
	ArmPull armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, arm_to_pull);

	// Update algorithm statistics

	// Update change model
	vector<double> old_runlength_distribution(this->runlength_distribution.size());
	copy(this->runlength_distribution.begin(), this->runlength_distribution.end(), old_runlength_distribution.begin());
	// push a random number (e.g. a zero) since we want to update its size only. The value will be overwritten in the loop
	this->runlength_distribution.push_back(0);
	vector<double> likelihoods;
	for (int run = 0; run < this->runlength_distribution.size()-1; run++) {
		double likelihood;
		if (armpull.reward > 0) {
			likelihood = this->alphas[arm_to_pull][run] / (this->alphas[arm_to_pull][run] + this->betas[arm_to_pull][run]);
		}
		else {
			likelihood = this->betas[arm_to_pull][run] / (this->alphas[arm_to_pull][run] + this->betas[arm_to_pull][run]);
		}
		likelihoods.push_back(likelihood);
		this->runlength_distribution[run+1] = (1 - this->gamma) * likelihood * old_runlength_distribution[run];
	}
	this->runlength_distribution[0] = 0;
	for (int l = 0; l < likelihoods.size(); l++) {
		this->runlength_distribution[0] += this->gamma * likelihoods[l] * old_runlength_distribution[l];
	}
	// Normalize this->runlength_distribution
	double runlength_sum = accumulate(this->runlength_distribution.begin(), this->runlength_distribution.end(), 0.);
	for (int run = 0; run < this->runlength_distribution.size(); run++) {
		this->runlength_distribution[run] /= runlength_sum;
	}

	// Update arm models
	// push a random number (e.g. a zero) since we want to update its size only. The value will be overwritten in the loop
	for (int a = 0; a < this->num_of_arms; a++) {
		this->alphas[a].push_back(0);
		this->betas[a].push_back(0);
	}
	for (int i = this->alphas[arm_to_pull].size() - 2; i >= 0; i--) {
		for (int a = 0; a < this->num_of_arms; a++) {
			if (a == arm_to_pull) {
				if (armpull.reward > 0) {
					this->alphas[a][i+1] = this->alphas[a][i] + 1;
					this->betas[a][i+1] = this->betas[a][i];
				} else {
					this->betas[a][i+1] = this->betas[a][i] + 1;
					this->alphas[a][i+1] = this->alphas[a][i];
				}
			} else {
				this->alphas[a][i+1] = this->alphas[a][i];
				this->betas[a][i+1] = this->betas[a][i];
			}
		}
	}
	for (int a = 0; a < this->num_of_arms; a++) {
		this->alphas[a][0] = 1;
		this->betas[a][0] = 1;
	}

	return armpull;
}
*/

int PerArmCTS::choose_action() {
	int arm_to_pull = -1;

	// Sample runlength for each arm
	vector<int> runlengths;
	for (int i = 0; i < this->num_of_arms; i++) {
		float r = random_unit();
		int sampled_runlength = -1;
		double acc = 0;
		for (int run = 0; run < this->runlength_distribution[i].size(); run++) {
			acc += this->runlength_distribution[i][run];
			if (r <= acc) {
				sampled_runlength = run;
				break;
			}
		}
		runlengths.push_back(sampled_runlength);
	}

	// Sample arm
	double max_sample = -10000;
	for (int i = 0; i < this->num_of_arms; i++) {
		float r = random_unit();
		beta_distribution<> dist(this->alphas[i][runlengths[i]], this->betas[i][runlengths[i]]);
		double sample = quantile(dist, r);
		if (sample > max_sample) {
			max_sample = sample;
			arm_to_pull = i;
		}
	}

	return arm_to_pull;
}

void PerArmCTS::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);

	// Update runlength model
	vector<vector<double>> old_runlength_distributions(this->num_of_arms);
	for (int i = 0; i < this->num_of_arms; i++) {
		old_runlength_distributions[i].resize(this->runlength_distribution[i].size());
		copy(this->runlength_distribution[i].begin(), this->runlength_distribution[i].end(), old_runlength_distributions[i].begin());
	}
	// push a random number (e.g. a zero) since we want to update its size only. The value will be overwritten in the loop
	for (int i = 0; i < this->num_of_arms; i++) {
		this->runlength_distribution[i].push_back(0);
	}
	vector<vector<double>> all_likelihoods;
	for (int i = 0; i < this->num_of_arms; i++) {
		vector<double> likelihoods;
		for (int run = 0; run < this->runlength_distribution[i].size()-1; run++) {
			double likelihood;
			if (i == pulled_arm) {
				if (reward > 0) {
					likelihood = this->alphas[pulled_arm][run] / (this->alphas[pulled_arm][run] + this->betas[pulled_arm][run]);
				}
				else {
					likelihood = this->betas[pulled_arm][run] / (this->alphas[pulled_arm][run] + this->betas[pulled_arm][run]);
				}
			} else {
				likelihood = 1;
			}
			likelihoods.push_back(likelihood);
			this->runlength_distribution[i][run+1] = (1 - this->gamma) * likelihood * old_runlength_distributions[i][run];
		}
		all_likelihoods.push_back(likelihoods);
	}
	for (int i = 0; i < this->num_of_arms; i++) {
		this->runlength_distribution[i][0] = 0;
	}
	for (int i = 0; i < this->num_of_arms; i++) {
		for (int l = 0; l < all_likelihoods[i].size(); l++) {
			this->runlength_distribution[i][0] += this->gamma * all_likelihoods[i][l] * old_runlength_distributions[i][l];
		}
	}
	// Normalize this->runlength_distribution
	for (int i = 0; i < this->num_of_arms; i++) {
		double runlength_sum = accumulate(this->runlength_distribution[i].begin(), this->runlength_distribution[i].end(), 0.);
		for (int run = 0; run < this->runlength_distribution[i].size(); run++) {
			this->runlength_distribution[i][run] /= runlength_sum;
		}
	}

	// Update arm models
	// push a random number (e.g. a zero) since we want to update its size only. The value will be overwritten in the loop
	for (int a = 0; a < this->num_of_arms; a++) {
		this->alphas[a].push_back(0);
		this->betas[a].push_back(0);
	}
	for (int i = this->alphas[pulled_arm].size() - 2; i >= 0; i--) {
		for (int a = 0; a < this->num_of_arms; a++) {
			if (a == pulled_arm) {
				if (reward > 0) {
					this->alphas[a][i+1] = this->alphas[a][i] + 1;
					this->betas[a][i+1] = this->betas[a][i];
				} else {
					this->betas[a][i+1] = this->betas[a][i] + 1;
					this->alphas[a][i+1] = this->alphas[a][i];
				}
			} else {
				this->alphas[a][i+1] = this->alphas[a][i];
				this->betas[a][i+1] = this->betas[a][i];
			}
		}
	}
	for (int a = 0; a < this->num_of_arms; a++) {
		this->alphas[a][0] = 1;
		this->betas[a][0] = 1;
	}
}
/*
ArmPull PerArmCTS::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
	vector<double> pulls = all_pulls[timestep];
	int arm_to_pull = -1;

	// Choose arm to pull
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0.);
	if (tot_pulls < this->num_of_arms) {
		// First phase: pull each arm once
		arm_to_pull = tot_pulls;
	}
	else {
		// Sample runlength for each arm
		vector<int> runlengths;
		for (int i = 0; i < this->num_of_arms; i++) {
			float r = random_unit();
			int sampled_runlength = -1;
			double acc = 0;
			for (int run = 0; run < this->runlength_distribution[i].size(); run++) {
				acc += this->runlength_distribution[i][run];
				if (r <= acc) {
					sampled_runlength = run;
					break;
				}
			}
			runlengths.push_back(sampled_runlength);
		}

		// Sample arm
		double max_sample = -10000;
		for (int i = 0; i < this->num_of_arms; i++) {
			float r = random_unit();
			beta_distribution<> dist(this->alphas[i][runlengths[i]], this->betas[i][runlengths[i]]);
			double sample = quantile(dist, r);
			if (sample > max_sample) {
				max_sample = sample;
				arm_to_pull = i;
			}
		}
	}

	// Pull arm
	ArmPull armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, arm_to_pull);

	// Update algorithm statistics

	// Update change model
	vector<vector<double>> old_runlength_distributions(this->num_of_arms);
	for (int i = 0; i < this->num_of_arms; i++) {
		old_runlength_distributions[i].resize(this->runlength_distribution[i].size());
		copy(this->runlength_distribution[i].begin(), this->runlength_distribution[i].end(), old_runlength_distributions[i].begin());
	}
	// push a random number (e.g. a zero) since we want to update its size only. The value will be overwritten in the loop
	for (int i = 0; i < this->num_of_arms; i++) {
		this->runlength_distribution[i].push_back(0);
	}
	vector<vector<double>> all_likelihoods;
	for (int i = 0; i < this->num_of_arms; i++) {
		vector<double> likelihoods;
		for (int run = 0; run < this->runlength_distribution[i].size()-1; run++) {
			double likelihood;
			if (i == arm_to_pull) {
				if (armpull.reward > 0) {
					likelihood = this->alphas[arm_to_pull][run] / (this->alphas[arm_to_pull][run] + this->betas[arm_to_pull][run]);
				}
				else {
					likelihood = this->betas[arm_to_pull][run] / (this->alphas[arm_to_pull][run] + this->betas[arm_to_pull][run]);
				}
			} else {
				likelihood = 1;
			}
			likelihoods.push_back(likelihood);
			this->runlength_distribution[i][run+1] = (1 - this->gamma) * likelihood * old_runlength_distributions[i][run];
		}
		all_likelihoods.push_back(likelihoods);
	}
	for (int i = 0; i < this->num_of_arms; i++) {
		this->runlength_distribution[i][0] = 0;
	}
	for (int i = 0; i < this->num_of_arms; i++) {
		for (int l = 0; l < all_likelihoods[i].size(); l++) {
			this->runlength_distribution[i][0] += this->gamma * all_likelihoods[i][l] * old_runlength_distributions[i][l];
		}
	}
	// Normalize this->runlength_distribution
	for (int i = 0; i < this->num_of_arms; i++) {
		double runlength_sum = accumulate(this->runlength_distribution[i].begin(), this->runlength_distribution[i].end(), 0.);
		for (int run = 0; run < this->runlength_distribution[i].size(); run++) {
			this->runlength_distribution[i][run] /= runlength_sum;
		}
	}

	// Update arm models
	// push a random number (e.g. a zero) since we want to update its size only. The value will be overwritten in the loop
	for (int a = 0; a < this->num_of_arms; a++) {
		this->alphas[a].push_back(0);
		this->betas[a].push_back(0);
	}
	for (int i = this->alphas[arm_to_pull].size() - 2; i >= 0; i--) {
		for (int a = 0; a < this->num_of_arms; a++) {
			if (a == arm_to_pull) {
				if (armpull.reward > 0) {
					this->alphas[a][i+1] = this->alphas[a][i] + 1;
					this->betas[a][i+1] = this->betas[a][i];
				} else {
					this->betas[a][i+1] = this->betas[a][i] + 1;
					this->alphas[a][i+1] = this->alphas[a][i];
				}
			} else {
				this->alphas[a][i+1] = this->alphas[a][i];
				this->betas[a][i+1] = this->betas[a][i];
			}
		}
	}
	for (int a = 0; a < this->num_of_arms; a++) {
		this->alphas[a][0] = 1;
		this->betas[a][0] = 1;
	}

	return armpull;
}
*/
