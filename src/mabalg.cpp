#include "mabalg.h"

using namespace std;

MABAlgorithm::MABAlgorithm(string name, MAB& mab) {
	this->name = name;
	this->mab = &mab;
	this->collectedRewards.resize(mab.arms.size());
	this->totalReward.assign(mab.arms.size(), 0.);
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


// TODO: EXP3.S

// TODO: REXP3

// TODO: Adapt-EvE

// TODO: CTS

// TODO: EXP3.R

// TODO: spread the algorithms in multiple files
