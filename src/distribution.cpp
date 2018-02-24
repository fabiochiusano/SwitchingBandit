#include "distribution.h"
#include "utils.h"
#include <math.h>


Distribution::Distribution(string name, boost::mt19937& rng) {
	this->name = name;
	this->rng = &rng;
}


NormalDistribution::NormalDistribution(string name, double mean, double variance, boost::mt19937& rng) : Distribution(name, rng) {
	this->mean = mean;
	this->stddev = sqrt(variance);
	this->nd = new boost::normal_distribution<>(mean, this->stddev);
	this->vg = new boost::variate_generator<boost::mt19937&, boost::normal_distribution<> >(rng, *(this->nd));
}

double NormalDistribution::draw(int timestep) {
	return (double)(*(this->vg))();
}

string NormalDistribution::toFile() {
	return this->name + " Normal " + to_string(this->mean) + " " + to_string(this->stddev);
}

double NormalDistribution::get_mean(int timestep) {
	return this->mean;
}




UniformDistribution::UniformDistribution(string name, double v, boost::mt19937& rng) : Distribution(name, rng) {
	this->mean = v;
}

double UniformDistribution::draw(int timestep) {
	return this->mean;
}

string UniformDistribution::toFile() {
	return this->name + " Uniform " + to_string(this->mean);
}

double UniformDistribution::get_mean(int timestep) {
	return this->mean;
}




UniformNonStationaryDistribution::UniformNonStationaryDistribution(string name, vector<double>& means, vector<int> ends, boost::mt19937& rng) : Distribution(name, rng) {
	this->means = means;
	this->ends = ends;
}

double UniformNonStationaryDistribution::draw(int timestep) {
	int correct_i = this->ends.size() - 1;
	for (int i = 0; i < this->ends.size(); i++) {
		if (this->ends[i] > timestep) {
			correct_i = i;
			break;
		}
	}
	return this->means[correct_i];
}

string UniformNonStationaryDistribution::toFile() {
	string result = this->name + " UniformNonStationary ";
	for (int i = 0; i < this->means.size(); i++) {
		result += to_string(this->means[i]) + " " + to_string(this->ends[i]) + " ";
	}
	return result;
}

double UniformNonStationaryDistribution::get_mean(int timestep) {
	return this->draw(timestep);
}




BernoulliDistribution::BernoulliDistribution(string name, double p, boost::mt19937& rng) : Distribution(name, rng) {
	this->p = p;
}

double BernoulliDistribution::draw(int timestep) {
	double r = random_unit();
	if (r < this->p) {
		return 1;
	}
	return 0;
}

string BernoulliDistribution::toFile() {
	return this->name + " Bernoulli " + to_string(this->p);
}

double BernoulliDistribution::get_mean(int timestep) {
	return this->p;
}



BernoulliNonStationaryDistribution::BernoulliNonStationaryDistribution(string name, vector<double>& ps, vector<int>& ends, boost::mt19937& rng) : Distribution(name, rng) {
	this->ps = ps;
	this->ends = ends;
}

double BernoulliNonStationaryDistribution::draw(int timestep) {
	int correct_i = this->ends.size() - 1;
	for (int i = 0; i < this->ends.size(); i++) {
		if (this->ends[i] >= timestep) {
			correct_i = i;
			break;
		}
	}
	double p = this->ps[correct_i];
	double r = random_unit();
	if (r < p) {
		return 1.0;
	}
	return 0.0;
}

string BernoulliNonStationaryDistribution::toFile() {
	string result = this->name + " BernoulliNonStationary ";
	for (int i = 0; i < this->ps.size(); i++) {
		result += to_string(this->ps[i]) + " " + to_string(this->ends[i]) + " ";
	}
	return result;
}

double BernoulliNonStationaryDistribution::get_mean(int timestep) {
	int correct_i = this->ends.size() - 1;
	for (int i = 0; i < this->ends.size(); i++) {
		if (this->ends[i] >= timestep) {
			correct_i = i;
			break;
		}
	}
	double p = this->ps[correct_i];
	return p;
}




SquareWaveDistribution::SquareWaveDistribution(string name, double v, double cur_v, boost::mt19937& rng) : Distribution(name, rng) {
	this->v = v;
	this->cur_v = cur_v;
}

double SquareWaveDistribution::draw(int timestep) {
	if (this->cur_v > 0) {
		this->cur_v = 0;
	} else {
		this->cur_v = this->v;
	}
	return this->cur_v;
}

string SquareWaveDistribution::toFile() {
	return this->name + " SquareWave " + to_string(this->v) + " " + to_string(this->cur_v);
}

double SquareWaveDistribution::get_mean(int timestep) {
	return this->v / 2;
}
