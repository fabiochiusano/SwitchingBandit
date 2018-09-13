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
#include <set>

/**
 * @brief base class of all the meta-algorithms
 */
class MetaAlgorithm: public MABAlgorithm {
protected:
	MABAlgorithm* sub_alg;
public:
	/**
	 * @param name        string, id of the algorithm
	 * @param num_of_arms integer, number of arms the algorithm will work on
	 */
	MetaAlgorithm(string name, int num_of_arms);

	MABAlgorithm* get_sub_alg();
};

/**
 * @brief Meta-algorithm which selects each arm once before making its sub-algorithm run.
 */
class Round_Algorithm: public MetaAlgorithm {
private:
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
  vector<CDT*> cdts_up, cdts_down, sr_cdts_up, sr_cdts_down;
	vector<int> sr_change_estimates;
  bool use_history;
  int max_history;
  int M;
  int timestep;
	bool smart_resets;

  vector<int> till_M;
  vector<vector<double>> collected_rewards, sr_collected_rewards;
	vector<bool> last_resets_up;
	set<int> to_be_reset;
	//int num_alarms_up, num_alarms_down;

	void reset_arm(int pulled_arm);
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
  CD_Algorithm(string name, int num_of_arms, int M, string cdt_line, string sub_alg_line, bool use_history, int max_history, bool smart_resets, boost::mt19937& rng, MAB* mab);

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
  MABAlgorithm *other_sub_alg, *meta_alg;
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

/*
class GLR: public MetaAlgorithm {
private:
  vector<vector<double>> rewards;
  vector<int> changepoints;
  int M;
  int max_history;
  vector<vector<int>> past_k;

  void find_changepoints(int arm_pulled);

  void update_sub_alg(int arm_pulled);

  void cycle_reward(double new_reward, int arm_pulled);
public:

  GLR(string name, int num_of_arms, int M, int max_history, string sub_alg_line, boost::mt19937& rng, MAB* mab);

 	virtual int choose_action() override;

 	virtual void receive_reward(double reward, int pulled_arm) override;

 	virtual void reset(int action = -1) override;
};
*/

#endif
