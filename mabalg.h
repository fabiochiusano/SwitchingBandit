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

	void process_chosen_arm(vector<double> pulls, int timestep, double highest_mean, int arm_index);
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
	vector<double> alphas, betas;
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

#endif
