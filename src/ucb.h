/**
 *  @file    ucb.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 *
 * @brief Contains the algorithms based on UCB, which are: UCB1,
 * UCBT, D_UCB, SW_UCB.
 */

#ifndef UCB_H
#define UCB_H

#include "mabalg.h"

/**
 * @brief base class of the UCB algorithms
 */
class UCB: public MABAlgorithm {
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 */
	UCB(string name, int num_of_arms);
};

/**
 * @brief UCB1 algorithm
 */
class UCB1: public UCB {
public:
	vector<double> means;

	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 */
	UCB1(string name, int num_of_arms);

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
 * @brief UCBT algorithm
 */
class UCBT: public UCB {
private:
	vector<double> means;
	vector<double> variances;
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 */
	UCBT(string name, int num_of_arms);

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
 * @brief discounted UCB algorithm
 */
class D_UCB: public UCB {
private:
	vector<double> means, ns;
	double B, gamma, epsilon;
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 * @param gamma       double in (0,1), discount factor
	 * @param B           double, confidence parameter
	 * @param epsilon     double in (1,inf), confidence parameter
	 */
	D_UCB(string name, int num_of_arms, double gamma, double B, double epsilon);

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
 * @brief UCB algorithm with a sliding window
 */
class SW_UCB: public UCB {
private:
	vector<vector<double>> windowed_arm_pulls_values;
	vector<vector<int>> windowed_arm_pulls;
	double B, epsilon;
	int tau;
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 * @param tau         integer, window size
	 * @param B           double, confidence parameter
	 * @param epsilon     double in (1,inf), confidence parameter
	 */
	SW_UCB(string name, int num_of_arms, int tau, double B, double epsilon);

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
