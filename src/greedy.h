#ifndef GREEDY_H
#define GREEDY_H

#include "mabalg.h"
#include "utils.h"

class Greedy: public MABAlgorithm {
protected:
	double epsilon;
	vector<double> means;
public:
	Greedy(string name, MAB& mab);
	void reset() override;
};

class E_Greedy: public Greedy {
public:
	// epsilon corresponds to the amount of exploration
	E_Greedy(string name, MAB& mab, double epsilon);
	ArmPull run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) override;
};

class GLIE: public Greedy {
public:
	// epsilon corresponds to the amount of exploration
	GLIE(string name, MAB& mab, double epsilon);
	ArmPull run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) override;
};

#endif
