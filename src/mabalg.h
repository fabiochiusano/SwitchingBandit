#ifndef MABALG_H
#define MABALG_H

#include <vector>
#include "mab.h"
#include <string>

using namespace std;

class MAB;

/*
class ArmPull {
public:
	double reward;
	int arm_index;
	ArmPull(double reward, int arm_index);
};*/


class MABAlgorithm {
protected:
	int num_of_arms = 0;

public:
	vector<int> num_of_pulls;
	string name;

	MABAlgorithm(string name, int num_of_arms);

	virtual int choose_action() = 0; // returns the number of the selected action
	virtual void receive_reward(double reward, int pulled_arm);
	virtual void reset();
};


#endif
