#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <math.h>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <string>
#include "mabalg.h"


class MABAlgorithm;

using namespace std;

class Distribution {
protected:
	boost::mt19937* rng;
public:
	string name;
	bool is_mab;

	Distribution(string name);
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

class UniformNonStationaryDistribution: public Distribution {
private:
	vector<double> means;
	vector<int> ends;
public:
	UniformNonStationaryDistribution(string name, vector<double>& means, vector<int> ends, boost::mt19937& rng);
	double draw(int timestep) override;
	string toFile() override;
	double get_mean(int timestep) override;
};


class BernoulliDistribution: public Distribution {
private:
	double p;
public:
	BernoulliDistribution(string name, double p, boost::mt19937& rng);
	double draw(int timestep) override;
	string toFile() override;
	double get_mean(int timestep) override;
};

class BernoulliNonStationaryDistribution: public Distribution {
private:
	vector<double> ps;
	vector<int> ends;
public:
	BernoulliNonStationaryDistribution(string name, vector<double>& ps, vector<int>& ends, boost::mt19937& rng);
	double draw(int timestep) override;
	string toFile() override;
	double get_mean(int timestep) override;
};

class NormalNonStationaryDistribution: public Distribution {
private:
	vector<double> means, stddevs;
	vector<int> ends;
public:
	NormalNonStationaryDistribution(string name, vector<double>& means, vector<double>& stddevs, vector<int>& ends, boost::mt19937& rng);
	double draw(int timestep) override;
	string toFile() override;
	double get_mean(int timestep) override;
};



// It's equivalent to a UniformNonStationaryDistribution where the mean flips between 0 and v at each timestep
class SquareWaveDistribution: public Distribution {
private:
	double v, cur_v;
public:
	SquareWaveDistribution(string name, double v, double cur_v, boost::mt19937& rng);
	double draw(int timestep) override;
	string toFile() override;
	double get_mean(int timestep) override;
};

// Used in meta bandits
class MABDistribution: public Distribution {
private:
	MABAlgorithm* mabalg;
	vector<vector<double>> all_pulls;
	int pulled_arm;
public:
	MABDistribution(string name, MABAlgorithm* mabalg);
	double draw(int timestep) override;
	string toFile() override;
	double get_mean(int timestep) override;

	void set_pulls(vector<vector<double>>& pulls);
	int get_pulled_arm();
};


// TODO: SineDistribution

// TODO: manage in a beautiful way stationary distributions and nonstationary distributions


#endif
