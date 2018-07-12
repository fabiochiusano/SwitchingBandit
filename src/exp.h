#ifndef EXP_H
#define EXP_H

#include "mabalg.h"
#include "utils.h"

class EXP3: public MABAlgorithm {
private:
	double beta, nu;
	vector<double> ws;
public:
	EXP3(string name, int num_of_arms, double beta, double nu);
	int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset(int action = -1) override;
};

class EXP3_S: public MABAlgorithm {
private:
	double beta, alpha;
	vector<double> ws;
public:
	EXP3_S(string name, int num_of_arms, double beta, double alpha);
	int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset(int action = -1) override;
};

class REXP3: public MABAlgorithm {
private:
	double beta;
	int window_size;
	vector<double> ws;
public:
	REXP3(string name, int num_of_arms, double beta, int window_size);
	int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset(int action = -1) override;
};

#endif
