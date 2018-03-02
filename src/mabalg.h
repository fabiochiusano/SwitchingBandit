#ifndef MABALG_H
#define MABALG_H

#include <vector>
#include "mab.h"
#include <string>

using namespace std;

class MAB;

class ArmPull {
public:
	double reward;
	int arm_index;
	ArmPull(double reward, int arm_index);
};


class MABAlgorithm {
protected:
	int num_of_arms = 0;

	void mabalg_reset();
	ArmPull pull_arm(vector<double>& pulls, bool generate_new_pulls, int arm_to_pull);
public:
	vector<int> num_of_pulls;
	MAB* mab;
	string name;

	MABAlgorithm(string name, MAB& mab);
	virtual ArmPull run(vector<double>& pulls, bool generate_new_pulls) = 0;
	virtual void reset() = 0;
};


#endif
