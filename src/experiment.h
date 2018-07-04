#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <vector>
#include "mab.h"

class Experiment {
private:
  MABExperiment* mab;
  RegretType mabtype;
  vector<MABAlgorithm*> algs;
public:
  string name;
  int num_simulations;
  int timesteps;
  int seed;

  Experiment(string name, int num_simulations, int timesteps, int seed);
  void set_mab(MABExperiment* mab);
  void set_mab_type(RegretType mabtype);
  void add_alg(MABAlgorithm* alg);
  void run();
};

#endif
