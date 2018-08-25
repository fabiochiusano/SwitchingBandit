/**
 *  @file    distribution.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 *
 * @brief contains a lot of Distribution classes, i.e. classes that represent arms of a MAB setting.
 */

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <math.h>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <string>
#include "mabalg.h"


class MABAlgorithm;

using namespace std;

/**
 * @brief Base class of all the Distribution classes.
 *
 * @param name string, id of the distribution
 */
class Distribution {
protected:
	boost::mt19937* rng;
public:
	string name;
	bool is_mab;

	/**
	 * @brief Constructor of Distribution.
	 *
	 * @param name string, id of the distribution
	 */
	Distribution(string name);

	/**
	 * @brief Constructor of Distribution.
	 *
	 * @param name string, id of the distribution
	 * @param rng boost::mt19937&, rng used to generate data
	 */
	Distribution(string name, boost::mt19937& rng);

	/**
	 * @brief draw the reward for a certain timestep
	 *
	 * @param  timestep int, timestep at which the reward is drawn
	 * @return a double which is the drawn reward
	 */
	virtual double draw(int timestep) = 0;

	/**
	 * @brief build a representation of this Distribution so that it can be printed to a file and be
	 * 	later analysed by another script
	 *
	 * @return a string that represents this Distribution
	 */
	virtual string toFile() = 0;

	/**
	 * @param  timestep integer, the timestep at which the mean is asked
	 * @return the mean (double) at the specified timestep
	 */
	virtual double get_mean(int timestep) = 0;
};

class StationaryDistribution: public Distribution {
public:
	/**
	 * @brief Constructor of StationaryDistribution.
	 *
	 * @param name string, id of the distribution
	 */
	StationaryDistribution(string name);

	/**
	 * @brief Constructor of StationaryDistribution.
	 *
	 * @param name string, id of the distribution
	 * @param rng boost::mt19937&, rng used to generate data
	 */
	StationaryDistribution(string name, boost::mt19937& rng);
};

class NonStationaryDistribution: public Distribution {
public:
	/**
	 * @brief Constructor of NonStationaryDistribution.
	 *
	 * @param name string, id of the distribution
	 */
	NonStationaryDistribution(string name);

	/**
	 * @brief Constructor of NonStationaryDistribution.
	 *
	 * @param name string, id of the distribution
	 * @param rng boost::mt19937&, rng used to generate data
	 */
	NonStationaryDistribution(string name, boost::mt19937& rng);
};

class AdversarialDistribution: public Distribution {
public:
	/**
	 * @brief Constructor of AdversarialDistribution.
	 *
	 * @param name string, id of the distribution
	 */
	AdversarialDistribution(string name);

	/**
	 * @brief Constructor of AdversarialDistribution.
	 *
	 * @param name string, id of the distribution
	 * @param rng boost::mt19937&, rng used to generate data
	 */
	AdversarialDistribution(string name, boost::mt19937& rng);
};

class NonStationaryAbruptDistribution: public NonStationaryDistribution {
protected:
	vector<int> ends;
	vector<double> means;
public:
	/**
	 * @brief Constructor of NonStationaryAbruptDistribution.
	 *
	 * @param name string, id of the distribution
	 */
	NonStationaryAbruptDistribution(string name);

	/**
	 * @brief Constructor of NonStationaryAbruptDistribution.
	 *
	 * @param name string, id of the distribution
	 * @param rng boost::mt19937&, rng used to generate data
	 */
	NonStationaryAbruptDistribution(string name, boost::mt19937& rng);

	/**
	 * @brief draw the reward for a certain timestep
	 *
	 * @param  timestep int, timestep at which the reward is drawn
	 * @return a double which is the drawn reward
	 */
	virtual double draw(int timestep) = 0;

	/**
	 * @brief build a representation of this Distribution so that it can be printed to a file and be
	 * 	later analysed by another script
	 *
	 * @return a string that represents this Distribution
	 */
	virtual string toFile() = 0;

	/**
	 * @param  timestep integer, the timestep at which the mean is asked
	 * @return the mean (double) at the specified timestep
	 */
	virtual double get_mean(int timestep) = 0;

	/**
	 * @return a vector of integers, i.e. the changepoints
	 */
	vector<int> get_changepoints();

	/**
	 * @return a vector of doubles, i.e. the means after the changepoints
	 */
	vector<double> get_means();
};


class NonStationarySmoothDistribution: public NonStationaryDistribution {
public:
	/**
	 * @brief Constructor of NonStationaryAbruptDistribution.
	 *
	 * @param name string, id of the distribution
	 */
	NonStationarySmoothDistribution(string name);

	/**
	 * @brief Constructor of NonStationaryAbruptDistribution.
	 *
	 * @param name string, id of the distribution
	 * @param rng boost::mt19937&, rng used to generate data
	 */
	NonStationarySmoothDistribution(string name, boost::mt19937& rng);
};


/**
 * @brief Distribution of a normal distribution
 * @param name     string, id of the distribution
 * @param mean     double, mean
 * @param variance double, variances
 * @param rng      boost::mt19937&, rng used to generate data
 */
class NormalDistribution: public StationaryDistribution {
private:
	boost::normal_distribution<>* nd;
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > *vg;
	double mean, stddev;
public:
	/**
	 * @param name     string, id of the distribution
 	 * @param mean     double, mean
 	 * @param variance double, variances
	 * @param rng      boost::mt19937&, rng used to generate data
	 */
	NormalDistribution(string name, double mean, double variance, boost::mt19937& rng);

	/**
	 * @param  timestep int, timestep at which the reward is drawn
	 * @return a double which is the drawn reward
	 */
	double draw(int timestep) override;

	/**
	 * @brief build a representation of this Distribution so that it can be printed to a file and be
	 * 	later analysed by another script
	 *
	 * @return a string that represents this Distribution
	 */
	string toFile() override;

	/**
	 * @param  timestep integer, the timestep at which the mean is asked
	 * @return the mean (double) at the specified timestep
	 */
	double get_mean(int timestep) override;
};

/**
 * @brief Distribution of a non-stationary normal distribution
 *
 * @param name     string, id of the distribution
 * @param means 	 vector<double>&, the means of the normal distributions in the time slots
 * @param stddevs  vector<double>&, the stddevs of the normal distributions in the time slots
 * @param ends 		 vector<double>&, the last timestep of each time slot
 * @param rng      boost::mt19937&, rng used to generate data
 */
class NormalNonStationaryDistribution: public NonStationaryAbruptDistribution {
private:
	vector<double> stddevs;
public:
	/**
	 * @param name     string, id of the distribution
 	 * @param means 	 vector<double>&, the means of the normal distributions in the time slots
	 * @param stddevs  vector<double>&, the stddevs of the normal distributions in the time slots
	 * @param ends 		 vector<double>&, the last timestep of each time slot
	 * @param rng      boost::mt19937&, rng used to generate data
	 */
	NormalNonStationaryDistribution(string name, vector<double>& means, vector<double>& stddevs, vector<int>& ends, boost::mt19937& rng);

	/**
	 * @param  timestep int, timestep at which the reward is drawn
	 * @return a double which is the drawn reward
	 */
	double draw(int timestep) override;

	/**
	 * @brief build a representation of this Distribution so that it can be printed to a file and be
	 * 	later analysed by another script
	 *
	 * @return a string that represents this Distribution
	 */
	string toFile() override;

	/**
	 * @param  timestep integer, the timestep at which the mean is asked
	 * @return the mean (double) at the specified timestep
	 */
	double get_mean(int timestep) override;
};

/**
 * @brief Distribution of a fixed distribution, i.e. it returns always the same value
 *
 * @param name     string, id of the distribution
 * @param v 			 double, the value always returned by the distribution
 * @param rng      boost::mt19937&, rng used to generate data
 */
class FixedDistribution: public StationaryDistribution {
private:
	double v;
public:
	/**
	 * @param name     string, id of the distribution
	 * @param v 			 double, the value always returned by the distribution
	 * @param rng      boost::mt19937&, rng used to generate data
	 */
	FixedDistribution(string name, double v, boost::mt19937& rng);

	/**
	 * @param  timestep int, timestep at which the reward is drawn
	 * @return a double which is the drawn reward
	 */
	double draw(int timestep) override;

	/**
	 * @brief build a representation of this Distribution so that it can be printed to a file and be
	 * 	later analysed by another script
	 *
	 * @return a string that represents this Distribution
	 */
	string toFile() override;

	/**
	 * @param  timestep integer, the timestep at which the mean is asked
	 * @return the mean (double) at the specified timestep
	 */
	double get_mean(int timestep) override;
};

/**
 * @brief Distribution of a non-stationary fixed distribution, i.e. it returns always the same value
 * 	inside the same slot
 *
 * @param name     string, id of the distribution
 * @param vs 			 vector<double>&, the values always returned by the distribution in the time slots
 * @param ends 		 vector<double>&, the last timestep of each time slot
 * @param rng      boost::mt19937&, rng used to generate data
 */
class FixedNonStationaryDistribution: public NonStationaryAbruptDistribution {
public:
	/**
	 * @param name     string, id of the distribution
	 * @param means 			 vector<double>&, the values always returned by the distribution in the time slots
	 * @param ends 		 vector<double>&, the last timestep of each time slot
	 * @param rng      boost::mt19937&, rng used to generate data
	 */
	FixedNonStationaryDistribution(string name, vector<double>& means, vector<int> ends, boost::mt19937& rng);

	/**
	 * @param  timestep int, timestep at which the reward is drawn
	 * @return a double which is the drawn reward
	 */
	double draw(int timestep) override;

	/**
	 * @brief build a representation of this Distribution so that it can be printed to a file and be
	 * 	later analysed by another script
	 *
	 * @return a string that represents this Distribution
	 */
	string toFile() override;

	/**
	 * @param  timestep integer, the timestep at which the mean is asked
	 * @return the mean (double) at the specified timestep
	 */
	double get_mean(int timestep) override;
};

/**
 * @brief Distribution of a bernoulli distribution
 *
 * @param name     string, id of the distribution
 * @param p        double, probability of obtaining 1 from the bernoulli distribution
 * @param rng      boost::mt19937&, rng used to generate data
 */
class BernoulliDistribution: public StationaryDistribution {
private:
	double p;
public:
	/**
	 * @param name     string, id of the distribution
	 * @param p        double, probability of obtaining 1 from the bernoulli distribution
	 * @param rng      boost::mt19937&, rng used to generate data
	 */
	BernoulliDistribution(string name, double p, boost::mt19937& rng);

	/**
	 * @param  timestep int, timestep at which the reward is drawn
	 * @return a double which is the drawn reward
	 */
	double draw(int timestep) override;

	/**
	 * @brief build a representation of this Distribution so that it can be printed to a file and be
	 * 	later analysed by another script
	 *
	 * @return a string that represents this Distribution
	 */
	string toFile() override;

	/**
	 * @param  timestep integer, the timestep at which the mean is asked
	 * @return the mean (double) at the specified timestep
	 */
	double get_mean(int timestep) override;
};

/**
 * @brief Distribution of a non-stationary bernoulli distribution
 *
 * @param name     string, id of the distribution
 * @param ps 			 vector<double>&, the probabilities of obtaining 1 from the bernoulli distributions in the time slots
 * @param ends 		 vector<double>&, the last timestep of each time slot
 * @param rng      boost::mt19937&, rng used to generate data
 */
class BernoulliNonStationaryDistribution: public NonStationaryAbruptDistribution {
public:
	/**
 	 * @param name     string, id of the distribution
	 * @param means 			 vector<double>&, the probabilities of obtaining 1 from the bernoulli distributions in the time slots
	 * @param ends 		 vector<double>&, the last timestep of each time slot
	 * @param rng      boost::mt19937&, rng used to generate data
	 */
	BernoulliNonStationaryDistribution(string name, vector<double>& means, vector<int>& ends, boost::mt19937& rng);

	/**
	 * @param  timestep int, timestep at which the reward is drawn
	 * @return a double which is the drawn reward
	 */
	double draw(int timestep) override;

	/**
	 * @brief build a representation of this Distribution so that it can be printed to a file and be
	 * 	later analysed by another script
	 *
	 * @return a string that represents this Distribution
	 */
	string toFile() override;

	/**
	 * @param  timestep integer, the timestep at which the mean is asked
	 * @return the mean (double) at the specified timestep
	 */
	double get_mean(int timestep) override;
};



/**
 * @brief Distribution equivalent to a FixedNonStationaryDistribution where the mean flips between 0 and v at each timestep
 *
 * @param name  string, id of the distribution
 * @param v     double, value of the distribution other than zero
 * @param cur_v double, first value that the distribution will returns
 * @param rng   boost::mt19937&, rng used to generate data
 */
class SquareWaveDistribution: public AdversarialDistribution {
private:
	double v, cur_v;
public:
	/**
	 * @param name  string, id of the distribution
	 * @param v     double, value of the distribution other than zero
	 * @param cur_v double, first value that the distribution will returns
	 * @param rng   boost::mt19937&, rng used to generate data
	 */
	SquareWaveDistribution(string name, double v, double cur_v, boost::mt19937& rng);

	/**
	 * @param  timestep int, timestep at which the reward is drawn
	 * @return a double which is the drawn reward
	 */
	double draw(int timestep) override;

	/**
	 * @brief build a representation of this Distribution so that it can be printed to a file and be
	 * 	later analysed by another script
	 *
	 * @return a string that represents this Distribution
	 */
	string toFile() override;

	/**
	 * @param  timestep integer, the timestep at which the mean is asked
	 * @return the mean (double) at the specified timestep
	 */
	double get_mean(int timestep) override;
};

class SineBernoulliDistribution: public NonStationarySmoothDistribution {
private:
	double A, omega, phi, mean;
public:
	/**
	 * @param name  string, id of the distribution
	 * @param A     double, amplitude of the sine function that gives the mean of the bernoulli
	 * @param omega double, angular velocity of the sine function that gives the mean of the bernoulli
	 * @param phi   double in [0, PI], phase of the sine function that gives the mean of the bernoulli
	 * @param mean  double in [0+A, 1-A], mean of the sine function that gives the mean of the bernoulli
	 * @param rng   boost::mt19937&, rng used to generate data
	 */
	SineBernoulliDistribution(string name, double A, double omega, double phi, double mean, boost::mt19937& rng);

	/**
	 * @param  timestep int, timestep at which the reward is drawn
	 * @return a double which is the drawn reward
	 */
	double draw(int timestep) override;

	/**
	 * @brief build a representation of this Distribution so that it can be printed to a file and be
	 * 	later analysed by another script
	 *
	 * @return a string that represents this Distribution
	 */
	string toFile() override;

	/**
	 * @param  timestep integer, the timestep at which the mean is asked
	 * @return the mean (double) at the specified timestep
	 */
	double get_mean(int timestep) override;
};

#endif
