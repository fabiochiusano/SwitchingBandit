#ifndef GLR_H
#define GLR_H

#include "mabalg.h"
#include "cdt.h"
#include "ucb.h"
#include <string>
#include <vector>
#include <iostream>
/*
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
*/
#endif
