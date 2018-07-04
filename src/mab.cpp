#include "mab.h"
#include <numeric>

//int MAB::next_id = 1;

/*
MAB::MAB(MABType* mabtype) {
	this->mabtype = mabtype;
	this->id = MAB::next_id;
	MAB::next_id++;
}
*/

MABExperiment::MABExperiment(vector<Distribution*>& arms) {
	this->arms = arms;
}

/*
int MAB::get_id() {
	return this->id;
}*/

vector<vector<double>> MABExperiment::generate_pulls(int timesteps) {
	vector<vector<double>> rewards; // res[timestep][arm]
	for (int t = 0; t < timesteps; t++) {
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

vector<int> MABExperiment::get_available_actions() {
	// Always return all actions
	vector<int> actions;
	for (int i = 0; i < this->pulls[0].size(); i++) {
		actions.push_back(i);
	}
	return actions;
}

double MABExperiment::observe_reward(int action) {
	if (this->pulls.size() > 0) {
			return this->pulls[this->cur_timestep][action];
	}
	return this->arms[action]->draw(this->cur_timestep);
}

void MABExperiment::next_step() {
	this->cur_timestep++;
}

void MABExperiment::reset() {
	this->cur_timestep = 0;
	this->pulls.clear();
}
