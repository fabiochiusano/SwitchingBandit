#ifndef MABALG_H
#define MABALG_H

#include <vector>
#include "mab.h"
#include <string>
#include <boost/random.hpp>

using namespace std;

class MAB;

class MABAlgorithm {
protected:
	vector<vector<double>> collectedRewards;
	vector<double> totalReward;
	bool allArmsPulled = false;
	int lastArmPulled = -1;

	double process_chosen_arm(vector<double> pulls, int timestep, double highest_mean, int arm_index);
public:
	MAB* mab;
	vector<double> regrets;
	string name;

	MABAlgorithm(string name, MAB& mab);
	virtual void run(vector<double> pulls, int timestep, double highest_mean) = 0;
	void reset(MAB& mab);
};





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





class ThompsonSampling: public MABAlgorithm {
protected:
	boost::mt19937* rng;
public:
	ThompsonSampling(string name, MAB& mab, boost::mt19937& rng);
};

class ThompsonSamplingBernoulli: public ThompsonSampling {
private:
	vector<int> alphas, betas;
public:
	ThompsonSamplingBernoulli(string name, MAB& mab, boost::mt19937& rng);
	void run(vector<double> pulls, int timestep, double highest_mean) override;
	void reset(MAB& mab);
};

class ThompsonSamplingGaussian: public ThompsonSampling {
public:
	ThompsonSamplingGaussian(string name, MAB& mab, boost::mt19937& rng);
	void run(vector<double> pulls, int timestep, double highest_mean) override;
};




class EXP3: public MABAlgorithm {
private:
	double beta, nu;
	vector<double> ws;
public:
	EXP3(string name, MAB& mab, double beta, double nu);
	void run(vector<double> pulls, int timestep, double highest_mean) override;
	void reset(MAB& mab);
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
