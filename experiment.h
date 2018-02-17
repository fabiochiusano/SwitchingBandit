#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <vector>
#include "mab.h"

class Experiment {
private:
  MAB* mab;
  vector<MABAlgorithm*> algs;
public:
  int num_simulations;
  int timesteps;
  int seed;

  Experiment(int num_simulations, int timesteps, int seed);
  void set_mab(MAB* mab);
  void add_alg(MABAlgorithm* alg);
  void run();
};

#endif
