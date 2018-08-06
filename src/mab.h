/**
 *  @file    mab.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 */

#ifndef MAB_H
#define MAB_H

#include <vector>

#include "mabalg.h"
#include "distribution.h"

using namespace std;

class MABAlgorithm;
class Distribution;

/**
 * @brief enum class representing the type of regret to consider among Stochastic and Adversarial regret.
 */
enum class RegretType { Stochastic, Adversarial};

/**
 * @brief Class representing a MAB setting, which is a collection of arms.
 */
class MAB {
public:
	/**
	 * @return a vector containing the ids of all the currently available actions
	 */
	virtual vector<int> get_available_actions() = 0;

	/**
	 * @param  action integer, the arm to pull
	 * @return        the reward obtained pulling the specified arm
	 */
	virtual double observe_reward(int action) = 0;
};

/**
 * @brief In order to test different algorithms with the same data, they must be able to obtain the same
 * reward when pulling an arm in the same timestep. The MABExperiment class generate all the rewards of
 * the arms up to the time horizon, before the algorithm starts.
 */
class MABExperiment : public MAB {
private:
	int cur_timestep = 0;
	vector<vector<double>> pulls;
public:
	vector<Distribution*> arms;

	/**
	 * @param arms vector<Distribution*>&, the vector of arms for this MAB setting
	 */
	MABExperiment(vector<Distribution*>& arms);

	/**
	 * @brief Generate the pulls for each arm up to the specified time horizon.
	 *
	 * @param timesteps integer, the time horizon
	 */
	vector<vector<double>> generate_pulls(int timesteps);

	/**
	 * @return a vector containing the ids of all the currently available actions
	 */
	vector<int> get_available_actions() override;

	/**
	 * @param  action integer, the arm to pull
	 * @return        the reward obtained pulling the specified arm
	 */
	double observe_reward(int action) override;

	/**
	 * @brief advance the time tick by one. This must be done after allt he algorithms have
	 * pulled the arm in order to advance to the next timestep.
	 */
	void next_step();

	/**
	 * @brief Reset the current timestep and the generated pulls.
	 */
	void reset();
};

#endif
