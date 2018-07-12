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
	void reset(int action = -1) override;
};

class Algorithm_With_Uniform_Exploration: public MABAlgorithm {
private:
	double alpha; // Exploration parameter
	MABAlgorithm* sub_alg;
public:
	Algorithm_With_Uniform_Exploration(string name, int num_of_arms, string sub_alg_line, double alpha, boost::mt19937& rng);
	int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset(int action = -1) override;
};

class CD_Algorithm: public MABAlgorithm {
private:
  vector<CDT*> cdts;
  MABAlgorithm* sub_alg;
  bool use_history;
  int M;
  int timestep;

  vector<int> till_M;
  vector<vector<double>> collected_rewards;
public:
  CD_Algorithm(string name, int num_of_arms, int M, string cdt_line, string sub_alg_line, bool use_history, boost::mt19937& rng);
  int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
	void reset(int action = -1) override;
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
	void reset(int action = -1) override;
};

class GLR: public MABAlgorithm {
private:
  MABAlgorithm* sub_alg;
  vector<vector<double>> rewards;
  vector<int> changepoints;
  int M;
  vector<vector<int>> past_k;

  void find_changepoints(int arm_pulled);
  void update_sub_alg(int arm_pulled);
public:
  GLR(string name, int num_of_arms, int M, string sub_alg_line, boost::mt19937& rng);
  int choose_action() override;
	void receive_reward(double reward, int pulled_arm) override;
  void reset(int action = -1) override;
};

#endif
