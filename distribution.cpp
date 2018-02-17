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




StepDistribution::StepDistribution(string name, vector<double>& means, vector<int> ends, boost::mt19937& rng) : Distribution(name, rng) {
	this->means = means;
	this->ends = ends;
}

double StepDistribution::draw(int timestep) {
	int correct_i = this->ends.size() - 1;
	for (int i = 0; i < this->ends.size(); i++) {
		if (this->ends[i] > timestep) {
			correct_i = i;
			break;
		}
	}
	return this->means[correct_i];
}

string StepDistribution::toFile() {
	string result = this->name + " Step ";
	for (int i = 0; i < this->means.size(); i++) {
		result += to_string(this->means[i]) + " " + to_string(this->ends[i]) + " ";
	}
	return result;
}

double StepDistribution::get_mean(int timestep) {
	return this->draw(timestep);
}




BernoulliDistribution::BernoulliDistribution(string name, double p, double v, boost::mt19937& rng) : Distribution(name, rng) {
	this->p = p;
	this->v = v;
}

double BernoulliDistribution::draw(int timestep) {
	double r = random_unit();
	if (r < p) {
		return this->v;
	}
	return 0;
}

string BernoulliDistribution::toFile() {
	return this->name + " Bernoulli " + to_string(this->p) + " " + to_string(this->v);
}

double BernoulliDistribution::get_mean(int timestep) {
	return this->p * this->v;
}
