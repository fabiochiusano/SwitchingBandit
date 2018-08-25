#include "distribution.h"
#include "utils.h"
#include <math.h>
#include <cmath>


Distribution::Distribution(string name, boost::mt19937& rng) {
	this->name = name;
	this->rng = &rng;
	this->is_mab = false;
}

Distribution::Distribution(string name) {
	this->name = name;
	this->is_mab = false;
}

StationaryDistribution::StationaryDistribution(string name) : Distribution(name) {}
StationaryDistribution::StationaryDistribution(string name, boost::mt19937& rng) : Distribution(name, rng) {}

NonStationaryDistribution::NonStationaryDistribution(string name) : Distribution(name) {}
NonStationaryDistribution::NonStationaryDistribution(string name, boost::mt19937& rng) : Distribution(name, rng) {}

NonStationaryAbruptDistribution::NonStationaryAbruptDistribution(string name) : NonStationaryDistribution(name) {}
NonStationaryAbruptDistribution::NonStationaryAbruptDistribution(string name, boost::mt19937& rng) : NonStationaryDistribution(name, rng) {}

NonStationarySmoothDistribution::NonStationarySmoothDistribution(string name) : NonStationaryDistribution(name) {}
NonStationarySmoothDistribution::NonStationarySmoothDistribution(string name, boost::mt19937& rng) : NonStationaryDistribution(name, rng) {}

AdversarialDistribution::AdversarialDistribution(string name) : Distribution(name) {}
AdversarialDistribution::AdversarialDistribution(string name, boost::mt19937& rng) : Distribution(name, rng) {}

vector<int> NonStationaryAbruptDistribution::get_changepoints() {
	vector<int> changepoints_copy(this->ends);
	return changepoints_copy;
}

vector<double> NonStationaryAbruptDistribution::get_means() {
	vector<double> means_copy(this->means);
	return means_copy;
}

NormalDistribution::NormalDistribution(string name, double mean, double variance, boost::mt19937& rng) : StationaryDistribution(name, rng) {
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




FixedDistribution::FixedDistribution(string name, double v, boost::mt19937& rng) : StationaryDistribution(name, rng) {
	this->v = v;
}

double FixedDistribution::draw(int timestep) {
	return this->v;
}

string FixedDistribution::toFile() {
	return this->name + " Fixed " + to_string(this->v);
}

double FixedDistribution::get_mean(int timestep) {
	return this->v;
}




FixedNonStationaryDistribution::FixedNonStationaryDistribution(string name, vector<double>& means, vector<int> ends, boost::mt19937& rng) : NonStationaryAbruptDistribution(name, rng) {
	this->means = means;
	this->ends = ends;
}

double FixedNonStationaryDistribution::draw(int timestep) {
	int correct_i = this->ends.size() - 1;
	for (int i = 0; i < this->ends.size(); i++) {
		if (this->ends[i] > timestep) {
			correct_i = i;
			break;
		}
	}
	return this->means[correct_i];
}

string FixedNonStationaryDistribution::toFile() {
	string result = this->name + " UniformNonStationary ";
	for (int i = 0; i < this->means.size(); i++) {
		result += to_string(this->means[i]) + " " + to_string(this->ends[i]) + " ";
	}
	return result;
}

double FixedNonStationaryDistribution::get_mean(int timestep) {
	return this->draw(timestep);
}




BernoulliDistribution::BernoulliDistribution(string name, double p, boost::mt19937& rng) : StationaryDistribution(name, rng) {
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



BernoulliNonStationaryDistribution::BernoulliNonStationaryDistribution(string name, vector<double>& means, vector<int>& ends, boost::mt19937& rng) : NonStationaryAbruptDistribution(name, rng) {
	this->means = means;
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
	double p = this->means[correct_i];
	double r = random_unit();
	if (r < p) {
		return 1.0;
	}
	return 0.0;
}

string BernoulliNonStationaryDistribution::toFile() {
	string result = this->name + " BernoulliNonStationary ";
	for (int i = 0; i < this->means.size(); i++) {
		result += to_string(this->means[i]) + " " + to_string(this->ends[i]) + " ";
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
	double p = this->means[correct_i];
	return p;
}



NormalNonStationaryDistribution::NormalNonStationaryDistribution(string name, vector<double>& means, vector<double>& stddevs, vector<int>& ends, boost::mt19937& rng) : NonStationaryAbruptDistribution(name, rng) {
	this->means = means;
	this->stddevs = stddevs;
	this->ends = ends;
}

double NormalNonStationaryDistribution::draw(int timestep) {
	int correct_i = this->ends.size() - 1;
	for (int i = 0; i < this->ends.size(); i++) {
		if (this->ends[i] >= timestep) {
			correct_i = i;
			break;
		}
	}
	double mean = this->means[correct_i];
	double stddev = this->stddevs[correct_i];
	boost::normal_distribution<> nd = boost::normal_distribution<>(mean, stddev);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > vg = boost::variate_generator<boost::mt19937&, boost::normal_distribution<> >(*(this->rng), nd);
	double res = (double)(vg)();
	return res;
}

string NormalNonStationaryDistribution::toFile() {
	string result = this->name + " NormalNonStationary ";
	for (int i = 0; i < this->means.size(); i++) {
		result += to_string(this->means[i]) + " " + to_string(this->stddevs[i]) + " " + to_string(this->ends[i]) + " ";
	}
	return result;
}

double NormalNonStationaryDistribution::get_mean(int timestep) {
	int correct_i = this->ends.size() - 1;
	for (int i = 0; i < this->ends.size(); i++) {
		if (this->ends[i] >= timestep) {
			correct_i = i;
			break;
		}
	}
	return this->means[correct_i];
}




SquareWaveDistribution::SquareWaveDistribution(string name, double v, double cur_v, boost::mt19937& rng) : AdversarialDistribution(name, rng) {
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




SineBernoulliDistribution::SineBernoulliDistribution(string name, double A, double omega, double phi, double mean, boost::mt19937& rng) : NonStationarySmoothDistribution(name, rng) {
	this->A = A;
	this->omega = omega;
	this->phi = phi;
	this->mean = mean;
}

double SineBernoulliDistribution::draw(int timestep) {
	double mean = this->A * sin(this->omega * timestep + this->phi) + this->mean;
	double r = random_unit();
	if (r < mean) {
		return 1;
	}
	return 0;
}

string SineBernoulliDistribution::toFile() {
	return this->name + " SineBernoulli " + to_string(this->A) + " " + to_string(this->omega) + " " + to_string(this->phi) + " " + to_string(this->mean);
}

double SineBernoulliDistribution::get_mean(int timestep) {
	return this->A * sin(this->omega * timestep + this->phi);
}
