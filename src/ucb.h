#ifndef UCB_H
#define UCB_H

#include "mabalg.h"

class UCB: public MABAlgorithm {
public:
	UCB(string name, int num_of_arms);
};

class UCB1: public UCB {
public:
	vector<double> means;
	UCB1(string name, int num_of_arms);
	int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset(int action = -1) override;
};

class UCBT: public UCB {
private:
	vector<double> means;
	vector<vector<double>> collected_rewards;
public:
	UCBT(string name, int num_of_arms);
	int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset(int action = -1) override;
};

class D_UCB: public UCB {
private:
	vector<double> means, ns;
	double B, gamma, epsilon;
public:
	D_UCB(string name, int num_of_arms, double gamma, double B, double epsilon);
	int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset(int action = -1) override;
};

class SW_UCB: public UCB {
private:
	vector<vector<double>> windowed_arm_pulls_values;
	vector<vector<int>> windowed_arm_pulls;
	double B, epsilon;
	int tau;
public:
	SW_UCB(string name, int num_of_arms, int tau, double B, double epsilon);
	int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset(int action = -1) override;
};

#endif
