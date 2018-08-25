/**
 *  @file    exp.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 *
 * @brief Contains the algorithms based on EXP3, which are: EXP3, EXP3_S and REXP3.
 */

#ifndef EXP_H
#define EXP_H

#include "mabalg.h"
#include "utils.h"

/**
 * @brief base class of the EXP algorithms
 */
class EXP: public MABAlgorithm {
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 */
	EXP(string name, int num_of_arms);
};

/**
 * @brief EXP3 algorithm
 */
class EXP3: public EXP {
private:
	double beta;
	vector<double> ws;
public:
	/**
	 * @brief constructor of EXP3
	 *
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 * @param beta				double, exploration parameter (between 0 and 1)
	 */
	EXP3(string name, int num_of_arms, double beta);

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
 * @brief EXP3_S algorithm
 */
class EXP3_S: public EXP {
private:
	double beta, alpha;
	vector<double> ws;
public:
	/**
	 * @brief constructor of EXP3_S
	 *
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 * @param beta        double, exploration parameter (between 0 and 1) and also exponential update parameter
	 * @param alpha       double, probability update parameter
	 */
	EXP3_S(string name, int num_of_arms, double beta, double alpha);

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
 * @brief REXP3 algorithm
 */
class REXP3: public EXP {
private:
	double beta;
	int window_size;
	vector<double> ws;
public:
	/**
	 * @brief constructor of REXP3
	 *
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 * @param beta        double, exploration parameter (between 0 and 1) and also exponential update parameter
	 * @param window_size integer, window size
	 */
	REXP3(string name, int num_of_arms, double beta, int window_size);

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
