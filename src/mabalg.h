/**
 *  @file    mabalg.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 */

#ifndef MABALG_H
#define MABALG_H

#include <vector>
#include "mab.h"
#include <string>

using namespace std;

class MAB;

/**
 * @brief Base class of all the algorithms for multi armed bandits.
 */
class MABAlgorithm {
protected:
	int num_of_arms = 0;

public:
	vector<int> num_of_pulls;
	string name;

	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work with
	 */
	MABAlgorithm(string name, int num_of_arms);

	/**
	 * [choose_action description]
	 * @return [description]
	 */
	 /**
 	 * @return an integer representing the chosen arm
 	 */
 	virtual int choose_action() = 0;

 	/**
 	 * @brief Update the internal variables of the algorithm according to the new received reward
 	 *
 	 * @param reward     double, new received reward
 	 * @param pulled_arm integer, represents the pulled arm
 	 */
 	virtual void receive_reward(double reward, int pulled_arm);

 	/**
 	 * @brief resets the internal variables of the algorithm
 	 * @param action integer, specifies which variables to reset. If in (0, num_of_arms - 1), then it resets
 	 * 	only the info related to such arm. If == -1, resets all the arms.
 	 */
 	virtual void reset(int action = -1);
};


#endif
