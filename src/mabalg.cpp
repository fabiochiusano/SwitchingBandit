#include "mabalg.h"

using namespace std;

ArmPull::ArmPull(double reward, int arm_index) {
	this->reward = reward;
	this->arm_index = arm_index;
}


MABAlgorithm::MABAlgorithm(string name, MAB& mab) {
	this->name = name;
	this->mab = &mab;
	this->num_of_arms = mab.arms.size();
	mabalg_reset();
}

void MABAlgorithm::mabalg_reset() {
	this->num_of_pulls.assign(this->num_of_arms, 0);
}

ArmPull MABAlgorithm::pull_arm(vector<double>& pulls, bool generate_new_pulls, int arm_to_pull) {
	ArmPull armpull(0,0);
	if (generate_new_pulls) {
		if (this->mab->arms[arm_to_pull]->is_mab) { // TODO: actually we should generate pulls only when the selected arm is a meta-mab
			// In this case, we should return an armpull relative to the MAB inside the arm
			MABDistribution* d = (MABDistribution*)(this->mab->arms[arm_to_pull]);

			d->set_pulls(pulls);
			int fake_timestep = 0;

			double reward = d->draw(fake_timestep);

			armpull.reward = reward;
			armpull.arm_index = d->get_pulled_arm();
		} else {
			cout << "Error: called get_reward with generate_new_pulls=true but not MABDistribution" << endl;
			armpull.reward = 0;
			armpull.arm_index = 0;
		}
	} else {
		armpull.reward = pulls[arm_to_pull];
		armpull.arm_index = arm_to_pull;
	}

	this->num_of_pulls[arm_to_pull]++;

	return armpull;
}


// TODO: add the .reset in the constructors (remove duplicate code)

// TODO: Adapt-EvE

// TODO: CTS

// TODO: EXP3.R
