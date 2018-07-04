#include "experiment.h"
#include "statistic_manager.h"

Experiment::Experiment(string name, int num_simulations, int timesteps, int seed) {
  this->name = name;
  this->num_simulations = num_simulations;
  this->timesteps = timesteps;
  this->seed = seed;
}

void Experiment::set_mab(MABExperiment* mab) {
  this->mab = mab;
}

void Experiment::set_mab_type(RegretType mabtype) {
  this->mabtype = mabtype;
}

void Experiment::add_alg(MABAlgorithm* alg) {
  this->algs.push_back(alg);
}

void Experiment::run() {
  StatisticManager stat_manager(this->name, this->mab, this->algs);

  stat_manager.write_distributions();

  for (int cur_simulation = 1; cur_simulation <= this->num_simulations; cur_simulation++) {
    // Should generate all the pulls because they are needed in advice in an adversary environment
    vector<vector<double>> all_pulls = this->mab->generate_pulls(this->timesteps); // all_pulls[timestep][arm]
    stat_manager.set_mab_type(this->mabtype);
    stat_manager.analyze_pulls(all_pulls);

		for (int timestep = 0; timestep < this->timesteps; timestep++) {
      vector<double> pulls = all_pulls[timestep];
      for (int alg_index = 0; alg_index < this->algs.size(); alg_index++) {
        int arm_to_pull = this->algs[alg_index]->choose_action();
        double reward = this->mab->observe_reward(arm_to_pull);
        this->algs[alg_index]->receive_reward(reward, arm_to_pull);

        stat_manager.update(arm_to_pull, reward, alg_index, timestep);
      }
      this->mab->next_step();
		}

		stat_manager.write_regrets(cur_simulation);
    stat_manager.write_rewards(cur_simulation);
    stat_manager.write_chosen_arm(cur_simulation);

    for (auto alg : this->algs) {
  		alg->reset();
  	}
    this->mab->reset();
    stat_manager.reset();
	}
}
