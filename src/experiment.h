/**
 *  @file    experiment.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 */

#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <vector>
#include "mab.h"

/**
  *  @brief Class that represent an experiment. An experiment is composed by a MAB setting
  *  (i.e. a set of arms), a set of MABAlgorithm and a RegretType, which specifies the type of
  *  regret we are insterest into.
  */
class Experiment {
private:
  MAB* mab;
  RegretType mabtype;
  vector<MABAlgorithm*> algs;
public:
  string name;
  int num_simulations;
  int timesteps;
  int seed;

  /**
   *   @brief Constructor for Experiment
   *
   *   @param name: string, used to identify the experiment
   *   @param num_simulations: int, represents the number of times the experiment is run
   *    with a different seed
   *   @param timestep: int, the time horizon of the experiment
   *   @param seed: int, the seed with which the first simulation of the experiment starts.
   *   @return an instance of Experiment
   */
  Experiment(string name, int num_simulations, int timesteps, int seed);

  /**
   *   @brief Set the MAB of this experiment according to the builder pattern
   *
   *   @param mab: MAB used by the experiment.
   *   @return nothing
   */
  void set_mab(MAB* mab);

  /**
   *   @brief Set the RegretType of this experiment according to the builder pattern
   *
   *   @param mabtype: the RegretType of this experiment
   *   @return nothing
   */
  void set_mab_type(RegretType mabtype);

  /**
   *   @brief Add a new MABAlgorithm to this experiment according to the builder pattern
   *
   *   @param alg: MABAlgorithm that will be added to the experiment
   *   @return nothing
   */
  void add_alg(MABAlgorithm* alg);

  /**
   *   @brief Runs the experiment. It requires the MAB, MABAlgorithms and RegretType to be already set.
   *    It creates a StatisticManager and uses it to store data about the experiment in some files
   *    in the temp directory.
   *
   *   @return nothing
   */
  void run();
};

#endif
