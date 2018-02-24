#ifndef MABALG_H
#define MABALG_H

#include <vector>
#include "mab.h"
#include <string>

using namespace std;

class MAB;

class MABAlgorithm {
protected:
	vector<vector<double>> collectedRewards;
	vector<double> totalReward;
	bool allArmsPulled = false;
	int lastArmPulled = -1;

	double process_chosen_arm(vector<double> pulls, int timestep, double highest_mean, int arm_index);
public:
	MAB* mab;
	vector<double> regrets;
	string name;

	MABAlgorithm(string name, MAB& mab);
	virtual void run(vector<double> pulls, int timestep, double highest_mean) = 0;
	void reset(MAB& mab);
};

#endif
