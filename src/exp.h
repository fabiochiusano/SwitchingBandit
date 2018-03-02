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
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
	void reset() override;
};

class EXP3_S: public MABAlgorithm {
private:
	double beta, alpha;
	vector<double> ws;
public:
	EXP3_S(string name, MAB& mab, double beta, double alpha);
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
	void reset() override;
};

class REXP3: public MABAlgorithm {
private:
	double beta;
	int window_size;
	vector<double> ws;
public:
	REXP3(string name, MAB& mab, double beta, int window_size);
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
	void reset() override;
};

#endif
