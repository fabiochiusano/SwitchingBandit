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
enum class RegretType { Stochastic, Adversarial };

/**
 * @brief  Class representing a MAB setting, which is a collection of arms. In order to test different
 * algorithms with the same data, they must be able to obtain the same
 * reward when pulling an arm in the same timestep. This class generate all the rewards of
 * the arms up to the time horizon, before the algorithm starts.
 */
class MAB {
private:
	int cur_timestep = 0;
	vector<vector<double>> pulls;
public:
	vector<Distribution*> arms;
	int timesteps;

	/**
	 * @param arms vector<Distribution*>&, the vector of arms for this MAB setting
	 */
	MAB(vector<Distribution*>& arms, int timesteps);

	/**
	 * @brief Generate the pulls for each arm up to the specified time horizon.
	 *
	 * @param timesteps integer, the time horizon
	 */
	vector<vector<double>> generate_pulls();

	/**
	 * @return a vector containing the ids of all the currently available actions
	 */
	vector<int> get_available_actions();

	/**
	 * @param  action integer, the arm to pull
	 * @return        the reward obtained pulling the specified arm
	 */
	double observe_reward(int action);

	/**
	 * @brief advance the time tick by one. This must be done after allt he algorithms have
	 * pulled the arm in order to advance to the next timestep.
	 */
	void next_step();

	/**
	 * @brief Reset the current timestep and the generated pulls.
	 */
	void reset();

	/**
	 * @param  arm integer, the id of the requested arm
	 * @return     integer, the number of changepoints of the requested arm. If arm == -1, then returns the
	 * 	global number of changepoints
	 */
	int get_num_changepoints(int arm);

	/**
	 * @return integer, number of arms
	 */
	int get_num_of_arms();

	/**
	 * @brief Assuming a NonStationaryAbruptDistribution on each arm, it computes the smallest difference in the
	 * mean of an arm after a change point.
	 *
	 * @param  arm integer, the id of the arm to analyse
	 * @return     double, the smallest difference in the mean of the specified arm after a change point
	 */
	double get_min_change(int arm);

	/**
	 * @brief Assuming a NonStationaryAbruptDistribution on each arm, it computes the smallest interval
	 * between changepoints.
	 *
	 * @param  arm integer, the id of the arm to analyse
	 * @return     integer, the smallest interval between changepoints
	 */
	int get_shortest_interval_between_changepoints(int arm);

	/**
	 * @param  arm       integer, the id of the arm to analyse
	 * @param  epsilon   double, the smallest difference in the mean of the specified arm after a change point
	 * @param  M         integer, the smallest interval between changepoints
	 * @return           double, the minimal non-trivial gap between expectation and closest grid point
	 */
	double get_minimal_grid_gap_bernoulli(int arm, double epsilon, int M);
};

#endif
