#ifndef META_ALGORITHMS_H
#define META_ALGORITHMS_H

#include "mabalg.h"
#include "cdt.h"
#include <string>
#include <vector>

class Round_Algorithm: public MABAlgorithm {
private:
  MABAlgorithm* sub_alg;
  vector<bool> arms_pulled;
public:
  Round_Algorithm(string name, int num_of_arms, string sub_alg_line, boost::mt19937& rng);
  int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset() override;
};

class Algorithm_With_Uniform_Exploration: public MABAlgorithm {
private:
	double alpha; // Exploration parameter
	MABAlgorithm* sub_alg;
public:
	Algorithm_With_Uniform_Exploration(string name, int num_of_arms, string sub_alg_line, double alpha, boost::mt19937& rng);
	int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset() override;
};

class CD_Algorithm: public MABAlgorithm {
private:
  vector<CDT*> cdts;
  MABAlgorithm* sub_alg;
  bool use_history;
  int timestep;

  vector<vector<double>> collected_rewards;
  //vector<int> chosen_arms;
  //vector<int> last_resets_global;

  //void update_history_UCB(vector<vector<double>>& all_pulls, int timestep, int arm_reset, CDT_Result cdt_result);
public:
  CD_Algorithm(string name, int num_of_arms, string cdt_line, string sub_alg_line, bool use_history, boost::mt19937& rng);
  int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset() override;
};

class ADAPT_EVE: public MABAlgorithm {
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
	ADAPT_EVE(string name, int num_of_arms, int meta_duration, string cdt_line, string sub_alg_line, boost::mt19937& rng);
  int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset() override;
};

#endif
