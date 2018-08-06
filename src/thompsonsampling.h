/**
 *  @file    exp.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 *
 * @brief Contains the algorithms based on Thompson Sampling, which are: ThompsonSamplingBernoulli,
 * ThompsonSamplingGaussian, GlobalCTS, PerArmCTS.
 */

#ifndef THOMPSONSAMPLING_H
#define THOMPSONSAMPLING_H

#include "mabalg.h"
#include "utils.h"

/**
 * @brief base class of all the Thompson Sampling based algorithms
 */
class ThompsonSampling: public MABAlgorithm {
protected:
	boost::mt19937* rng;
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 * @param rng         boost::mt19937&, random number generator
	 */
	ThompsonSampling(string name, int num_of_arms, boost::mt19937& rng);
};

/**
 * @brief Thompson Sampling for bernoulli arms
 */
class ThompsonSamplingBernoulli: public ThompsonSampling {
private:
	vector<int> alphas, betas;
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 * @param rng         boost::mt19937&, random number generator
	 */
	ThompsonSamplingBernoulli(string name, int num_of_arms, boost::mt19937& rng);

	/**
	 * @return an integer representing the chosen arm
	 */
	int choose_action() override;

	/**
	 * @brief Update the internal variables of the algorithm according to the new received reward
	 *
	 * @param reward     double, new received reward
	 * @param pulled_arm integer, represents the pulled arm
	 */
	void receive_reward(double reward, int pulled_arm) override;

	/**
	 * @brief resets the internal variables of the algorithm
	 * @param action integer, specifies which variables to reset. If in (0, num_of_arms - 1), then it resets
	 * 	only the info related to such arm. If == -1, resets all the arms.
	 */
	void reset(int action = -1) override;
};

/**
 * @brief Thompson Sampling for gaussian arms
 */
class ThompsonSamplingGaussian: public ThompsonSampling {
private:
	vector<double> means;
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 * @param rng         boost::mt19937&, random number generator
	 */
	ThompsonSamplingGaussian(string name, int num_of_arms, boost::mt19937& rng);

	/**
	 * @return an integer representing the chosen arm
	 */
	int choose_action() override;

	/**
	 * @brief Update the internal variables of the algorithm according to the new received reward
	 *
	 * @param reward     double, new received reward
	 * @param pulled_arm integer, represents the pulled arm
	 */
	void receive_reward(double reward, int pulled_arm) override;

	/**
	 * @brief resets the internal variables of the algorithm
	 * @param action integer, specifies which variables to reset. If in (0, num_of_arms - 1), then it resets
	 * 	only the info related to such arm. If == -1, resets all the arms.
	 */
	void reset(int action = -1) override;
};

/**
 * @brief Thompson Sampling with prior on runlength distribution. Distribution changes are assumed to
 * happen globally (all the arms change at the same timestep)
 */
class GlobalCTS: public ThompsonSampling {
private:
	double gamma;
	vector<double> runlength_distribution; // runlength_distribution[runlength]
	vector<vector<double>> alphas; // alphas[arm][runlength]
	vector<vector<double>> betas; // betas[arm][runlength]
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 * @param rng         boost::mt19937&, random number generator
	 * @param gamma       double, parameter of the prior of the geometric runlength distribution
	 */
	GlobalCTS(string name, int num_of_arms, boost::mt19937& rng, double gamma);

	/**
	 * @return an integer representing the chosen arm
	 */
	int choose_action() override;

	/**
	 * @brief Update the internal variables of the algorithm according to the new received reward
	 *
	 * @param reward     double, new received reward
	 * @param pulled_arm integer, represents the pulled arm
	 */
	void receive_reward(double reward, int pulled_arm) override;

	/**
	 * @brief resets the internal variables of the algorithm
	 * @param action integer, specifies which variables to reset. If in (0, num_of_arms - 1), then it resets
	 * 	only the info related to such arm. If == -1, resets all the arms.
	 */
	void reset(int action = -1) override;
};

/**
 * @brief Thompson Sampling with prior on runlength distribution. Distribution changes are assumed to
 * happen locally (all arms are not assumed to change at the same timestep)
 */
class PerArmCTS: public ThompsonSampling {
private:
	double gamma;
	vector<vector<double>> runlength_distribution; // runlength_distribution[arm][runlength]
	vector<vector<double>> alphas; // alphas[arm][runlength]
	vector<vector<double>> betas; // betas[arm][runlength]
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 * @param rng         boost::mt19937&, random number generator
	 * @param gamma       double, parameter of the prior of the geometric runlength distribution
	 */
	PerArmCTS(string name, int num_of_arms, boost::mt19937& rng, double gamma);

	/**
	 * @return an integer representing the chosen arm
	 */
	int choose_action() override;

	/**
	 * @brief Update the internal variables of the algorithm according to the new received reward
	 *
	 * @param reward     double, new received reward
	 * @param pulled_arm integer, represents the pulled arm
	 */
	void receive_reward(double reward, int pulled_arm) override;

	/**
	 * @brief resets the internal variables of the algorithm
	 * @param action integer, specifies which variables to reset. If in (0, num_of_arms - 1), then it resets
	 * 	only the info related to such arm. If == -1, resets all the arms.
	 */
	void reset(int action = -1) override;
};

#endif
