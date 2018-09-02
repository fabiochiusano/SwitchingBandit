#include "mab.h"
#include <numeric>
#include <set>
#include "utils.h"

MAB::MAB(vector<Distribution*>& arms, int timesteps) {
	this->arms = arms;
	this->timesteps = timesteps;
}

vector<vector<double>> MAB::generate_pulls() {
	vector<vector<double>> rewards; // res[timestep][arm]
	for (int t = 0; t < this->timesteps; t++) {
		vector<double> pulls_timestep;
		for (auto arm : this->arms) {
			double r = arm->draw(t);
			pulls_timestep.push_back(r);
		}
		rewards.push_back(pulls_timestep);
	}
	this->pulls = rewards;
	return rewards;
}

vector<int> MAB::get_available_actions() {
	// Always return all actions
	vector<int> actions;
	for (int i = 0; i < this->pulls[0].size(); i++) {
		actions.push_back(i);
	}
	return actions;
}

double MAB::observe_reward(int action) {
	if (this->pulls.size() > 0) {
			return this->pulls[this->cur_timestep][action];
	}
	return this->arms[action]->draw(this->cur_timestep);
}

void MAB::next_step() {
	this->cur_timestep++;
}

void MAB::reset() {
	this->cur_timestep = 0;
	this->pulls.clear();
}

int MAB::get_num_changepoints(int arm) {
	set<int> res;

	if (arm == -1) {
		for (auto distr : this->arms) {
			if(NonStationaryAbruptDistribution* d = dynamic_cast<NonStationaryAbruptDistribution*>(distr)) {
				for (auto cp : d->get_changepoints()) {
					res.insert(cp);
				}
			} else {
				cout << "Not Non-Stationary Abrupt MAB (maybe it's a consequence of a fake MAB of ADAPT-EVE)" << endl;
			}
		}
	} else {
		auto distr = this->arms[arm];
		if(NonStationaryAbruptDistribution* d = dynamic_cast<NonStationaryAbruptDistribution*>(distr)) {
			for (auto cp : d->get_changepoints()) {
				res.insert(cp);
			}
		} else {
			cout << "Not Non-Stationary Abrupt MAB (maybe it's a consequence of a fake MAB of ADAPT-EVE)" << endl;
		}
	}

	return res.size()-1;
}

int MAB::get_num_of_arms() {
	return this->arms.size();
}

double MAB::get_min_change(int arm) {
	double res = POS_INF;

	if (arm == -1) {
		for (int i = 0; i < this->get_num_of_arms(); i++) {
			res = min(res, this->get_min_change(i));
		}
	} else {
		auto distr = this->arms[arm];
		if(NonStationaryAbruptDistribution* d = dynamic_cast<NonStationaryAbruptDistribution*>(distr)) {
			vector<double> means = d->get_means();
			double cur_mean = means[0];
			for (int i = 1; i < means.size(); i++) {
				double next_mean = means[i];
				double diff = abs(next_mean - cur_mean);
				res = min(res, diff);
				cur_mean = next_mean;
			}
		} else {
			cout << "Not Non-Stationary Abrupt MAB (maybe it's a consequence of a fake MAB of ADAPT-EVE)" << endl;
		}
	}

	return res;
}

int MAB::get_shortest_interval_between_changepoints(int arm) {
	int res = POS_INF;

	if (arm == -1) {
		for (int i = 0; i < this->get_num_of_arms(); i++) {
			res = min(res, this->get_shortest_interval_between_changepoints(i));
		}
	} else {
		auto distr = this->arms[arm];
		if(NonStationaryAbruptDistribution* d = dynamic_cast<NonStationaryAbruptDistribution*>(distr)) {
			vector<int> cps = d->get_changepoints();
			int cur_cp = cps[0];
			res = min(res, cps[0]);
			for (int i = 1; i < cps.size(); i++) {
				int next_cp = cps[i];
				int diff = next_cp - cur_cp;
				res = min(res, diff);
				cur_cp = next_cp;
			}
		} else {
			cout << "Not Non-Stationary Abrupt MAB (maybe it's a consequence of a fake MAB of ADAPT-EVE)" << endl;
		}
	}

	return res;
}

double MAB::get_minimal_grid_gap_bernoulli(int arm, double epsilon, int M) {
	double res = POS_INF;

	if (arm == -1) {
		for (int i = 0; i < this->get_num_of_arms(); i++) {
			res = min(res, this->get_minimal_grid_gap_bernoulli(i, epsilon, M));
		}
	} else {
		auto distr = this->arms[arm];
		if(BernoulliNonStationaryDistribution* d = dynamic_cast<BernoulliNonStationaryDistribution*>(distr)) {
			vector<double> means = d->get_means();
			for (auto mean : means) {
				double candidate_1 = (mean - epsilon) - floor((mean - epsilon)*M) * 1.0 / M;
				double candidate_2 = ceil((mean + epsilon)*M) * 1.0 / M - (mean + epsilon);
				res = min(res, candidate_1);
				res = min(res, candidate_2);
			}
		} else {
			cout << "Not BernoulliNonStationary MAB (maybe it's a consequence of a fake MAB of ADAPT-EVE)" << endl;
		}
	}

	return res;
}
