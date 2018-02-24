#ifndef UCB_H
#define UCB_H

#include "mabalg.h"

class UCB: public MABAlgorithm {
public:
	UCB(string name, MAB& mab);
};

class UCB1: public UCB {
public:
	UCB1(string name, MAB& mab);
	void run(vector<double> pulls, int timestep, double highest_mean) override;
};

class UCBT: public UCB {
public:
	UCBT(string name, MAB& mab);
	void run(vector<double> pulls, int timestep, double highest_mean) override;
};

class D_UCB: public UCB {
private:
	vector<double> means, ns;
	double B, gamma, epsilon;
public:
	D_UCB(string name, MAB& mab, double gamma, double B, double epsilon);
	void run(vector<double> pulls, int timestep, double highest_mean) override;
	void reset(MAB& mab);
};

class SW_UCB: public UCB {
private:
	vector<vector<double>> arm_pulls_values;
	vector<vector<int>> arm_pulls;
	double B, epsilon;
	int tau;
public:
	SW_UCB(string name, MAB& mab, int tau, double B, double epsilon);
	void run(vector<double> pulls, int timestep, double highest_mean) override;
	void reset(MAB& mab);
};

#endif
