#ifndef MABSTATISTICMANAGER_H
#define MABSTATISTICMANAGER_H

#include <vector>
#include "mab.h"
#include "mabalg.h"

class MABStatisticManager {
private:
  MAB* mab;
  vector<MABAlgorithm*> algs;

  vector<int> best_arm;
  vector<vector<double>> regrets; // regret[arm][timestep]
public:
  MABStatisticManager(MAB* mab, vector<MABAlgorithm*>* algs);

  void analyze_pulls(vector<vector<double>>& pulls);
  void update(ArmPull armpull, int alg_index, int timestep, vector<double> pulls);
  void reset();

  void write_distributions();
  void write_regrets(bool should_append);
};

#endif
