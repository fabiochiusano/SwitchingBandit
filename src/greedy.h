#ifndef GREEDY_H
#define GREEDY_H

#include "mabalg.h"
#include "utils.h"

class Greedy: public MABAlgorithm {
protected:
	double epsilon;
public:
	Greedy(string name, MAB& mab);
};

class E_Greedy: public Greedy {
public:
	// epsilon corresponds to the amount of exploration
	E_Greedy(string name, MAB& mab, double epsilon);
	void run(vector<double> pulls, int timestep, double highest_mean) override;
};

class GLIE: public Greedy {
public:
	// epsilon corresponds to the amount of exploration
	GLIE(string name, MAB& mab, double epsilon);
	void run(vector<double> pulls, int timestep, double highest_mean) override;
};

#endif
