#ifndef UCB_H
#define UCB_H

#include "mabalg.h"

class UCB: public MABAlgorithm {
public:
	UCB(string name, MAB& mab);
};

class UCB1: public UCB {
private:
	vector<double> means;
public:
	UCB1(string name, MAB& mab);
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
	void reset() override;
};

class UCBT: public UCB {
private:
	vector<double> means;
	vector<vector<double>> collected_rewards;
public:
	UCBT(string name, MAB& mab);
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
	void reset() override;
};

class D_UCB: public UCB {
private:
	vector<double> means, ns;
	double B, gamma, epsilon;
public:
	D_UCB(string name, MAB& mab, double gamma, double B, double epsilon);
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
	void reset() override;
};

class SW_UCB: public UCB {
private:
	vector<vector<double>> windowed_arm_pulls_values;
	vector<vector<int>> windowed_arm_pulls;
	double B, epsilon;
	int tau;
public:
	SW_UCB(string name, MAB& mab, int tau, double B, double epsilon);
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
	void reset() override;
};

#endif
