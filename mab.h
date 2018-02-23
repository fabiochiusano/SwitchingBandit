#ifndef MAB_H
#define MAB_H

#include <vector>

#include "mabalg.h"
#include "distribution.h"

using namespace std;

class MABAlgorithm;

enum class MABType { Stochastic, Adversarial};

class MAB {
private:
	bool first_run = true;
	vector<vector<double>> pulls;
	vector<double> highest_means;

	int id;
	static int next_id;

	void initialize(vector<MABAlgorithm*>& algs, int total_timesteps);

public:
	MABType* mabtype;
	vector<Distribution*> arms;

	MAB(MABType* mabtype);

	void addArm(Distribution* arm);
	void run(vector<MABAlgorithm*>& algs, int timestep, int total_timesteps);
	double pullArm(int armIndex, int timestep);
	void reset(vector<MABAlgorithm*>& algs);

	void write_distributions();
	void write_means_in_time(int timesteps);
	void write_regrets(vector<MABAlgorithm*>& algs, bool should_append);
};

#endif
