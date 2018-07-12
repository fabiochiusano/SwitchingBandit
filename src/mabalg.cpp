#include "mabalg.h"

using namespace std;

MABAlgorithm::MABAlgorithm(string name, int num_of_arms) {
	this->name = name;
	this->num_of_arms = num_of_arms;
	this->reset(-1);
}

void MABAlgorithm::receive_reward(double reward, int pulled_arm) {
	this->num_of_pulls[pulled_arm]++;
}

void MABAlgorithm::reset(int action) {
	if (action == -1)
		this->num_of_pulls.assign(this->num_of_arms, 0);
	else
		this->num_of_pulls[action] = 0;
}
