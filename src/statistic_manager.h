#ifndef STATISTICMANAGER_H
#define STATISTICMANAGER_H

#include <vector>
#include "mab.h"
#include "mabalg.h"

class StatisticManager {
private:
  string name;

  MAB* mab;
  RegretType mabtype;
  vector<MABAlgorithm*> algs;

  vector<int> best_arm;
  vector<vector<double>> regrets; // regret[algorithm][timestep]
  vector<vector<double>> pulls; // pulls[timestep][arm]

  vector<vector<double>> rewards; // rewards[algorithm][timestep]
  vector<vector<int>> chosen_arm; // chosen_arm[algorithm][timestep]
public:
  StatisticManager(string name, MAB* mab, vector<MABAlgorithm*>& algs);

  void set_mab_type(RegretType mabtype);
  void analyze_pulls(vector<vector<double>>& pulls); // only if MABExperiment
  void update(int arm_to_pull, double reward, int alg_index, int timestep);
  void reset();

  void write_distributions();
  void write_regrets(int sim); // only if MABEXperiment
  void write_rewards(int sim);
  void write_chosen_arm(int sim);
};

#endif
