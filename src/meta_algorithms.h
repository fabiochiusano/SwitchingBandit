/**
 *  @file    meta_algorithms.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 *
 *  @brief Contains all the algotihms that can be classified as meta-algorithms since they
 *  use other MABAlgorithm as sub-algorithms.
 */

#ifndef META_ALGORITHMS_H
#define META_ALGORITHMS_H

#include "mabalg.h"
#include "cdt.h"
#include <string>
#include <vector>

/**
 * @brief base class of all the meta-algorithms
 */
class MetaAlgorithm: public MABAlgorithm {
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 */
	MetaAlgorithm(string name, int num_of_arms);
};

/**
 * @brief Meta-algorithm which selects each arm once before making its sub-algorithm run.
 */
class Round_Algorithm: public MetaAlgorithm {
private:
  MABAlgorithm* sub_alg;
  vector<bool> arms_pulled;
public:
  /**
   * @param name         string, id of the algorithm
   * @param num_of_arms  integer, number of arms the algorithm will work with
   * @param sub_alg_line string, specification of the sub-algorithm
   * @param rng          boost::mt19937&, random number generator
   * @param mab 				 MAB*, required for setting optimal parameters
   */
  Round_Algorithm(string name, int num_of_arms, string sub_alg_line, boost::mt19937& rng, MAB* mab);

	/**
 	 * @return an integer representing the chosen arm
 	 */
 	virtual int choose_action() override;

 	/**
 	 * @brief Update the internal variables of the algorithm according to the new received reward
 	 *
 	 * @param reward     double, new received reward
 	 * @param pulled_arm integer, represents the pulled arm
 	 */
 	virtual void receive_reward(double reward, int pulled_arm) override;

 	/**
 	 * @brief resets the internal variables of the algorithm
 	 *
 	 * @param action integer, specifies which variables to reset. If in (0, num_of_arms - 1), then it resets
 	 * 	only the info related to such arm. If == -1, resets all the arms.
 	 */
 	virtual void reset(int action = -1) override;
};

/**
 * @brief Meta-algorithm that couples a sub-algorithm with uniform exploration over the arms.
 */
class Algorithm_With_Uniform_Exploration: public MetaAlgorithm {
private:
	double alpha; // Exploration parameter
	MABAlgorithm* sub_alg;
public:
  /**
   * @param name         string, id of the algorithm
   * @param num_of_arms  integer, number of arms the algorithm will work with
   * @param sub_alg_line string, specification of the sub-algorithm
   * @param alpha        double in (0,1), exploration parameter
   * @param rng          boost::mt19937&, random number generator
   */
	Algorithm_With_Uniform_Exploration(string name, int num_of_arms, string sub_alg_line, double alpha, boost::mt19937& rng, MAB* mab);

  /**
 	 * @return an integer representing the chosen arm
 	 */
 	virtual int choose_action() override;

 	/**
 	 * @brief Update the internal variables of the algorithm according to the new received reward
 	 *
 	 * @param reward     double, new received reward
 	 * @param pulled_arm integer, represents the pulled arm
 	 */
 	virtual void receive_reward(double reward, int pulled_arm) override;

 	/**
 	 * @brief resets the internal variables of the algorithm
 	 *
 	 * @param action integer, specifies which variables to reset. If in (0, num_of_arms - 1), then it resets
 	 * 	only the info related to such arm. If == -1, resets all the arms.
 	 */
 	virtual void reset(int action = -1) override;
};

/**
 * @brief Meta-algorithm that couples a sub-algorithm with a change detection test for each arm. When a CDT
 * signals an alarm, the info about the corresponding arm is reset on the sub-algorithm.
 */
class CD_Algorithm: public MetaAlgorithm {
private:
  vector<CDT*> cdts;
  MABAlgorithm* sub_alg;
  bool use_history;
  int max_history;
  int M;
  int timestep;

  vector<int> till_M;
  vector<vector<double>> collected_rewards;
public:
  /**
   * @param name         string, id of the algorithm
   * @param num_of_arms  integer, number of arms the algorithm will work with
   * @param M            integer, number of timesteps required by the CDT to initialize itself
   * @param cdt_line     string, specification of the CDT
   * @param sub_alg_line string, specification of the sub-algorithm
   * @param use_history  boolean, if true the sub-algorithm is not completely reset, and the sample history after
   *  the estimated change point is kept in the sub-algorithm
   * @param max_history  integer, maximum amount of sample history that the algorithm can use
   * @param rng          boost::mt19937&, random number generator
   */
  CD_Algorithm(string name, int num_of_arms, int M, string cdt_line, string sub_alg_line, bool use_history, int max_history, boost::mt19937& rng, MAB* mab);

  /**
 	 * @return an integer representing the chosen arm
 	 */
 	virtual int choose_action() override;

 	/**
 	 * @brief Update the internal variables of the algorithm according to the new received reward
 	 *
 	 * @param reward     double, new received reward
 	 * @param pulled_arm integer, represents the pulled arm
 	 */
 	virtual void receive_reward(double reward, int pulled_arm) override;

 	/**
 	 * @brief resets the internal variables of the algorithm
 	 *
 	 * @param action integer, specifies which variables to reset. If in (0, num_of_arms - 1), then it resets
 	 * 	only the info related to such arm. If == -1, resets all the arms.
 	 */
 	virtual void reset(int action = -1) override;
};

/**
 * @brief Meta-algorithm that couples a sub-algorithm with a CDT for each arm. When a CDT signals an alarm,
 * the algorithm instantiate a meta-bandit setting: a new mab with two choices is created, where one choice is using the
 * sub-algorithm completely reset and the other choice is using the old sub-algorithm. After a certain number of
 * timesteps, the choice with highest mean reward is selected as the new mab and the CDTs start monitoring from scratch.
 */
class ADAPT_EVE: public MetaAlgorithm {
private:
  vector<CDT*> cdts;
  MABAlgorithm *core_sub_alg, *other_sub_alg, *meta_alg;
  boost::mt19937* rng;
  //string sub_alg_line;
  int saved_meta_action;

  double meta_duration;
  bool is_meta;
  double t_meta;

  int timestep;
public:
  /**
   * @param name          string, id of the algorithm
   * @param num_of_arms   integer, number of arms the algorithm will work with
   * @param meta_duration integer, number of timesteps for the duration of the meta-bandit
   * @param cdt_line      string, specification of the CDT
   * @param sub_alg_line  string, specification of the sub-algorithm
   * @param rng           boost::mt19937&, random number generator
   */
	ADAPT_EVE(string name, int num_of_arms, int meta_duration, string cdt_line, string sub_alg_line, boost::mt19937& rng, MAB* mab);

  /**
 	 * @return an integer representing the chosen arm
 	 */
 	virtual int choose_action() override;

 	/**
 	 * @brief Update the internal variables of the algorithm according to the new received reward
 	 *
 	 * @param reward     double, new received reward
 	 * @param pulled_arm integer, represents the pulled arm
 	 */
 	virtual void receive_reward(double reward, int pulled_arm) override;

 	/**
 	 * @brief resets the internal variables of the algorithm
 	 *
 	 * @param action integer, specifies which variables to reset. If in (0, num_of_arms - 1), then it resets
 	 * 	only the info related to such arm. If == -1, resets all the arms.
 	 */
 	virtual void reset(int action = -1) override;
};

/**
 * @brief Meta-algorithm that selects at each timestep the sample history for its sub-algorithm using the
 * Generalized Likelihood Ratio method.
 */
class GLR: public MetaAlgorithm {
private:
  MABAlgorithm* sub_alg;
  vector<vector<double>> rewards;
  vector<int> changepoints;
  int M;
  int max_history;
  vector<vector<int>> past_k;

  /**
   * @brief finds the most probable changepoint for the specified arm
   *
   * @param arm_pulled integer, arm id for which finding the most probable changepoint
   */
  void find_changepoints(int arm_pulled);

  /**
   * @brief Update the sample history of the specified arm in the internal variables of the sub-algorithm
   *
   * @param arm_pulled integer, arm id for which updating the internal variables
   */
  void update_sub_alg(int arm_pulled);

  /**
   * @brief Store the new reward and delete the oldest reward if the maximum amount of history has been reached
   *
   * @param new_reward double, new reward received
   * @param arm_pulled integer, arm pulled
   */
  void cycle_reward(double new_reward, int arm_pulled);
public:
  /**
   * @param name         string, id of the algorithm
   * @param num_of_arms  integer, number of arms the algorithm will work with
   * @param M            integer, number of timesteps required by the CDT to initialize itself
   * @param max_history  integer, maximum amount of the latest samples that the algorithm scans
   *  for the most probable checkpoint
   * @param sub_alg_line string, specification of the sub-algorithm
   * @param rng          boost::mt19937&, random number generator
   */
  GLR(string name, int num_of_arms, int M, int max_history, string sub_alg_line, boost::mt19937& rng, MAB* mab);

  /**
 	 * @return an integer representing the chosen arm
 	 */
 	virtual int choose_action() override;

 	/**
 	 * @brief Update the internal variables of the algorithm according to the new received reward
 	 *
 	 * @param reward     double, new received reward
 	 * @param pulled_arm integer, represents the pulled arm
 	 */
 	virtual void receive_reward(double reward, int pulled_arm) override;

 	/**
 	 * @brief resets the internal variables of the algorithm
 	 *
 	 * @param action integer, specifies which variables to reset. If in (0, num_of_arms - 1), then it resets
 	 * 	only the info related to such arm. If == -1, resets all the arms.
 	 */
 	virtual void reset(int action = -1) override;
};

#endif
