#ifndef MAB_H
#define MAB_H

#include <vector>

#include "mabalg.h"
#include "distribution.h"

using namespace std;

class MABAlgorithm;
class Distribution;

enum class RegretType { Stochastic, Adversarial};

class MAB {
public:
	virtual vector<int> get_available_actions() = 0;
	virtual double observe_reward(int action) = 0;
};

class MABExperiment : public MAB {
private:
	int cur_timestep = 0;
	vector<vector<double>> pulls;
public:
	MABExperiment(vector<Distribution*>& arms); // pulls[timestep][arm]
	vector<Distribution*> arms;
	vector<vector<double>> generate_pulls(int timesteps);

	vector<int> get_available_actions() override;
	double observe_reward(int action) override;
	void next_step();

	void reset();
};

#endif
