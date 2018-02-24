#ifndef THOMPSONSAMPLING_H
#define THOMPSONSAMPLING_H

#include "mabalg.h"
#include "utils.h"

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

#endif
