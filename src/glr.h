#ifndef GLR_H
#define GLR_H

#include "mabalg.h"
#include "cdt.h"
#include "ucb.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
/*
class GLR: public MABAlgorithm {
private:
  UCB1* ucb;
  vector<vector<double>> rewards;
  vector<int> changepoints;
  int M, mod;
  double alpha;
  ofstream output_file;
  vector<vector<int>> past_k;
  ArmPull ucb_epsilon_greedy(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls);
  void find_changepoints(int arm_pulled);
  void update_UCB(int arm_pulled);
public:
  GLR(string name, MAB& mab, int M, int mod, double alpha, boost::mt19937& rng);
	ArmPull run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) override;
  void reset() override;
};*/

#endif
