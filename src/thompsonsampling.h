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
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
	void reset() override;
};

class ThompsonSamplingGaussian: public ThompsonSampling {
private:
	vector<double> means;
public:
	ThompsonSamplingGaussian(string name, MAB& mab, boost::mt19937& rng);
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
	void reset() override;
};

#endif
