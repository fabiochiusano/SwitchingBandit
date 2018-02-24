#ifndef EXP_H
#define EXP_H

#include "mabalg.h"
#include "utils.h"

class EXP3: public MABAlgorithm {
private:
	double beta, nu;
	vector<double> ws;
public:
	EXP3(string name, MAB& mab, double beta, double nu);
	void run(vector<double> pulls, int timestep, double highest_mean) override;
	void reset(MAB& mab);
};

#endif
