#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <math.h>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <string>

using namespace std;

class Distribution {
protected:
	boost::mt19937* rng;
public:
	string name;

	Distribution(string name, boost::mt19937& rng);

	virtual double draw(int timestep) = 0;
	virtual string toFile() = 0;
	virtual double get_mean(int timestep) = 0;
};



class NormalDistribution: public Distribution {
private:
	boost::normal_distribution<>* nd;
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > *vg;
	double mean, stddev;
public:
	NormalDistribution(string name, double mean, double variance, boost::mt19937& rng);
	double draw(int timestep) override;
	string toFile() override;
	double get_mean(int timestep) override;
};

class UniformDistribution: public Distribution {
private:
	double mean, stddev;
public:
	UniformDistribution(string name, double v, boost::mt19937& rng);
	double draw(int timestep) override;
	string toFile() override;
	double get_mean(int timestep) override;
};





class StepDistribution: public Distribution {
private:
	vector<double> means;
	vector<int> ends;
public:
	StepDistribution(string name, vector<double>& means, vector<int> ends, boost::mt19937& rng);
	double draw(int timestep) override;
	string toFile() override;
	double get_mean(int timestep) override;
};



class BernoulliDistribution: public Distribution {
private:
	double p, v;
public:
	BernoulliDistribution(string name, double p, double v, boost::mt19937& rng);
	double draw(int timestep) override;
	string toFile() override;
	double get_mean(int timestep) override;
};



#endif
