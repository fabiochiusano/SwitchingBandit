#include "mab.h"
#include <numeric>

int MAB::next_id = 1;

MAB::MAB(MABType* mabtype) {
	this->mabtype = mabtype;
	this->id = MAB::next_id;
	MAB::next_id++;
}


void MAB::addArm(Distribution* arm){
	this->arms.push_back(arm);
}

vector<vector<double>> MAB::generate_pulls(int total_timesteps) {
	vector<vector<double>> res; // res[timestep][arm]

	for (int t = 0; t < total_timesteps; t++) {
		vector<double> pulls_timestep;
		for (auto arm : this->arms) {
			double d = arm->draw(t);
			pulls_timestep.push_back(d);
		}
		res.push_back(pulls_timestep);
	}

	return res;
}

int MAB::get_id() {
	return this->id;
}
